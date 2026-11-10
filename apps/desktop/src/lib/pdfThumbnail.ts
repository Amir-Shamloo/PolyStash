import { useEffect, useState } from 'react';
import { convertFileSrc } from '@tauri-apps/api/core';
import { getDocument, GlobalWorkerOptions } from 'pdfjs-dist';
import pdfWorkerUrl from 'pdfjs-dist/build/pdf.worker.min.mjs?url';
import type { Asset } from '../types';

GlobalWorkerOptions.workerSrc = pdfWorkerUrl;

const PDF_THUMB_SIZE = 512;
const pdfThumbnailCache = new Map<string, string>();
const pdfFailedAssets = new Set<string>();
const pdfInFlight = new Map<string, Promise<string | null>>();

function hasRasterThumbnail(asset: Asset): boolean {
  const clean = asset.thumbnailPath?.trim();
  return Boolean(clean && !clean.endsWith('.glb'));
}

function isPdfWithoutRasterThumb(asset: Asset | null | undefined): asset is Asset {
  if (!asset || !asset.filePath) return false;
  if (asset.extension.toLowerCase() !== 'pdf') return false;
  return !hasRasterThumbnail(asset);
}

function canvasToBlobUrl(canvas: HTMLCanvasElement): Promise<string | null> {
  return new Promise((resolve) => {
    canvas.toBlob((blob) => {
      resolve(blob ? URL.createObjectURL(blob) : null);
    }, 'image/png');
  });
}

async function renderPdfFirstPage(filePath: string): Promise<string | null> {
  const loadingTask = getDocument(convertFileSrc(filePath));
  const pdf = await loadingTask.promise;
  try {
    const page = await pdf.getPage(1);
    const baseViewport = page.getViewport({ scale: 1 });
    const longestEdge = Math.max(baseViewport.width, baseViewport.height, 1);
    const viewport = page.getViewport({ scale: PDF_THUMB_SIZE / longestEdge });

    const canvas = document.createElement('canvas');
    canvas.width = Math.max(1, Math.round(viewport.width));
    canvas.height = Math.max(1, Math.round(viewport.height));

    const ctx = canvas.getContext('2d');
    if (!ctx) return null;

    ctx.fillStyle = '#ffffff';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    const renderParams = { canvasContext: ctx, viewport, canvas };
    await page.render(renderParams as Parameters<typeof page.render>[0]).promise;
    return await canvasToBlobUrl(canvas);
  } finally {
    void pdf.destroy();
  }
}

function loadOrQueuePdfThumbnail(asset: Asset): Promise<string | null> {
  const cached = pdfThumbnailCache.get(asset.id);
  if (cached) return Promise.resolve(cached);
  if (pdfFailedAssets.has(asset.id)) return Promise.resolve(null);

  const existing = pdfInFlight.get(asset.id);
  if (existing) return existing;

  const task = renderPdfFirstPage(asset.filePath)
    .then((url) => {
      if (url) {
        pdfThumbnailCache.set(asset.id, url);
      } else {
        pdfFailedAssets.add(asset.id);
      }
      return url;
    })
    .catch((err) => {
      console.warn('[pdfThumbnail] Failed to render PDF page 1 for', asset.fileName, err);
      pdfFailedAssets.add(asset.id);
      return null;
    })
    .finally(() => {
      pdfInFlight.delete(asset.id);
    });

  pdfInFlight.set(asset.id, task);
  return task;
}

export function usePdfPageThumbnail(asset: Asset | null | undefined): string | null {
  const eligible = isPdfWithoutRasterThumb(asset);
  const assetId = asset?.id ?? '';

  const [thumbUrl, setThumbUrl] = useState<string | null>(() =>
    eligible && asset ? pdfThumbnailCache.get(asset.id) ?? null : null
  );

  useEffect(() => {
    if (!eligible || !asset) {
      setThumbUrl(null);
      return;
    }

    const cached = pdfThumbnailCache.get(asset.id);
    if (cached) {
      setThumbUrl(cached);
      return;
    }

    let cancelled = false;
    void loadOrQueuePdfThumbnail(asset).then((url) => {
      if (!cancelled) setThumbUrl(url);
    });

    return () => {
      cancelled = true;
    };
  }, [eligible, assetId, asset]);

  return thumbUrl;
}
