import type { Asset, AssetKind } from '../types';
import { getModelPath, MESH_EXTENSIONS } from '../components/DetailPanel/ThreeViewer';
import { extractSbsarTextures, type SbsarTextures } from '../components/DetailPanel/ThreeModelLoaders';
import { USD_EXTENSIONS } from './usdToGlb';
import { getAssetThumbnailUrls } from '../components/AssetGrid/ThumbnailCarousel';

export const VIDEO_EXTENSIONS = new Set(['mp4', 'webm', 'mov', 'mkv', 'avi', 'm4v']);
export const PANORAMA_EXTENSIONS = new Set(['hdr', 'exr', 'hdri']);
export const LIGHT_EXTENSIONS = new Set(['ies', 'ldt']);
export const GPU_TEXTURE_EXTENSIONS = new Set(['dds', 'ktx', 'ktx2']);
export const CATEGORY_D_EXTENSIONS = new Set(['sbs', 'mdl', 'mat', 'abc']);
export const NO_PREVIEW_EXTENSIONS = new Set(['abc', 'x3d', 'avif', 'ztl', 'zpr', 'zbr']);
const MODEL_NO_PREVIEW_EXTENSIONS = new Set(['abc', 'x3d', 'ztl', 'zpr', 'zbr']);

export type PreviewView =
  | { type: 'video'; path: string }
  | { type: 'model'; path: string; extension: string; sbsarTextures?: SbsarTextures }
  | { type: 'usd-convert'; asset: Asset }
  | { type: 'panorama'; path: string; extension: string }
  | { type: 'ies'; path: string; extension: string }
  | { type: 'dds'; path: string }
  | { type: 'thumbnail'; urls: string[] }
  | { type: 'placeholder'; kind: AssetKind; extension: string; message?: string };

function resolveVideo(asset: Asset): PreviewView | null {
  if (!asset.filePath) return null;
  const ext = asset.extension.toLowerCase();
  if (asset.kind === 'video' || VIDEO_EXTENSIONS.has(ext)) {
    return { type: 'video', path: asset.filePath };
  }
  return null;
}

function resolveNoPreview(asset: Asset, snapshotUrl?: string | null): PreviewView | null {
  const ext = asset.extension.toLowerCase();
  if (MODEL_NO_PREVIEW_EXTENSIONS.has(ext)) {
    return {
      type: 'placeholder',
      kind: asset.kind,
      extension: ext,
      message: '3D scene preview unavailable',
    };
  }
  if (ext === 'avif') {
    const thumbs = getAssetThumbnailUrls(asset, snapshotUrl);
    if (thumbs.length > 0) {
      return { type: 'thumbnail', urls: thumbs };
    }
    return {
      type: 'placeholder',
      kind: asset.kind,
      extension: ext,
      message: 'AVIF preview unavailable without thumbnail',
    };
  }
  return null;
}

function resolveUsd(asset: Asset): PreviewView | null {
  const ext = asset.extension.toLowerCase();
  if (!USD_EXTENSIONS.has(ext)) return null;
  const cached = asset.metadata?.preview_model;
  if (typeof cached === 'string' && cached.toLowerCase().endsWith('.glb')) {
    return { type: 'model', path: cached, extension: 'glb' };
  }
  return { type: 'usd-convert', asset };
}

function resolveModel(asset: Asset): PreviewView | null {
  const ext = asset.extension.toLowerCase();
  const sbsarTextures = ext === 'sbsar' ? extractSbsarTextures(asset.metadata) : undefined;
  const directPath = getModelPath(asset);
  if (directPath) {
    // Converted scenes (for example C4D through Cineware) publish a GLB as the
    // model preview; tell the viewer to use the glTF loader and Y-up framing.
    const modelExt = directPath.toLowerCase().endsWith('.glb') ? 'glb' : ext;
    return { type: 'model', path: directPath, extension: modelExt, sbsarTextures };
  }
  if (MESH_EXTENSIONS.has(ext) || ext === 'sbsar') {
    return { type: 'model', path: asset.filePath, extension: ext, sbsarTextures };
  }
  return null;
}

function resolveEnvironment(asset: Asset): PreviewView | null {
  if (!asset.filePath) return null;
  const ext = asset.extension.toLowerCase();
  if (asset.kind === 'hdri' || PANORAMA_EXTENSIONS.has(ext)) {
    return { type: 'panorama', path: asset.filePath, extension: ext };
  }
  if (asset.kind === 'ies' || LIGHT_EXTENSIONS.has(ext)) {
    return { type: 'ies', path: asset.filePath, extension: ext };
  }
  return null;
}

function resolveGpuTexture(asset: Asset): PreviewView | null {
  const ext = asset.extension.toLowerCase();
  if (ext === 'dds') {
    const thumb = asset.thumbnailPath;
    const hasRasterThumb = Boolean(thumb && !thumb.endsWith('.glb'));
    if (!hasRasterThumb && asset.filePath) {
      return { type: 'dds', path: asset.filePath };
    }
  }
  if (ext === 'ktx' || ext === 'ktx2') {
    return {
      type: 'placeholder',
      kind: asset.kind,
      extension: ext,
      message: 'GPU texture container (KTX/KTX2)',
    };
  }
  return null;
}

function resolveThumbnail(asset: Asset, snapshotUrl?: string | null): PreviewView | null {
  const thumbs = getAssetThumbnailUrls(asset, snapshotUrl);
  if (thumbs.length > 0) {
    return { type: 'thumbnail', urls: thumbs };
  }
  return null;
}

function resolveFallback(asset: Asset): PreviewView {
  return {
    type: 'placeholder',
    kind: asset.kind,
    extension: asset.extension.toLowerCase(),
  };
}

export function resolvePreview(
  asset: Asset,
  snapshotUrl?: string | null
): PreviewView {
  return (
    resolveVideo(asset) ??
    resolveNoPreview(asset, snapshotUrl) ??
    resolveUsd(asset) ??
    resolveModel(asset) ??
    resolveEnvironment(asset) ??
    resolveGpuTexture(asset) ??
    resolveThumbnail(asset, snapshotUrl) ??
    resolveFallback(asset)
  );
}
