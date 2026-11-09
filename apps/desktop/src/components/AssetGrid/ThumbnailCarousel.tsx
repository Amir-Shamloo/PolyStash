import { memo, useState, useCallback, useMemo, useEffect } from 'react';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import type { Asset } from '../../types';
import { IconChevronLeft, IconChevronRight } from '../Icons';
import styles from './ThumbnailCarousel.module.css';

export interface ThumbnailCarouselProps {
  asset: Asset;
  className?: string;
  imgClassName?: string;
  alt?: string;
  loading?: 'lazy' | 'eager';
  snapshotUrl?: string | null;
  fallback?: React.ReactNode;
}

const PROTOCOL_REGEX = /^(blob:|https?:|asset:)/;
const ABSOLUTE_PATH_REGEX = /^([/\\]|[a-zA-Z]:[/\\])/;

const BROWSER_DECODABLE_EXTENSIONS = new Set([
  'jpg', 'jpeg', 'png', 'webp', 'gif', 'svg', 'bmp', 'avif',
]);

interface RelativePathRef {
  baseThumb: string;
  relPath: string;
}

function getSeparator(pathStr: string): string {
  return pathStr.includes('\\') ? '\\' : '/';
}

function resolveFromPolystashRoot(ref: RelativePathRef, polystashIdx: number): string {
  const sep = getSeparator(ref.baseThumb);
  const root = ref.baseThumb.slice(0, polystashIdx);
  const cleanRel = ref.relPath.replace(/^[/\\]+/, '').split(/[/\\]/).join(sep);
  return `${root}${sep}${cleanRel}`;
}

function resolveFromParentDir(ref: RelativePathRef): string {
  const sep = getSeparator(ref.baseThumb);
  const lastSlash = Math.max(ref.baseThumb.lastIndexOf('/'), ref.baseThumb.lastIndexOf('\\'));
  const dir = lastSlash !== -1 ? ref.baseThumb.slice(0, lastSlash) : '';
  const filename = ref.relPath.split(/[/\\]/).pop() || ref.relPath;
  return dir ? `${dir}${sep}${filename}` : ref.relPath;
}

function resolveRelativePath(ref: RelativePathRef): string {
  const polystashIdx = ref.baseThumb.search(/[/\\]\.polystash/);
  if (polystashIdx !== -1) {
    return resolveFromPolystashRoot(ref, polystashIdx);
  }
  return resolveFromParentDir(ref);
}

export function resolveThumbnailUrl(asset: Asset, rawPath?: string): string {
  if (!rawPath) return '';
  if (PROTOCOL_REGEX.test(rawPath)) return rawPath;
  if (ABSOLUTE_PATH_REGEX.test(rawPath)) return convertFileSrc(rawPath);

  const resolved = asset.thumbnailPath
    ? resolveRelativePath({ baseThumb: asset.thumbnailPath, relPath: rawPath })
    : rawPath;
  return convertFileSrc(resolved);
}

function getValidRasterThumbPath(asset: Asset): string | null {
  const clean = asset.thumbnailPath?.trim();
  if (!clean || clean.endsWith('.glb')) return null;
  return clean;
}

function isNonEmptyString(val: unknown): val is string {
  return typeof val === 'string' && val.trim().length > 0;
}

function getMultiThumbs(asset: Asset): string[] {
  const thumbsMeta = asset.metadata?.['polystash.thumbs'];
  if (!Array.isArray(thumbsMeta)) return [];
  const primaryThumb = getValidRasterThumbPath(asset);

  return thumbsMeta
    .filter(isNonEmptyString)
    .map((p, idx) =>
      idx === 0 && primaryThumb ? convertFileSrc(primaryThumb) : resolveThumbnailUrl(asset, p)
    );
}

function getNativeImageUrl(asset: Asset): string | null {
  const ext = asset.extension.toLowerCase();
  if (!BROWSER_DECODABLE_EXTENSIONS.has(ext) || !asset.filePath) return null;
  return convertFileSrc(asset.filePath);
}

function getSingleThumb(asset: Asset, snapshotUrl?: string | null): string | null {
  if (asset.extension.toLowerCase() === 'gif' && asset.filePath) {
    return convertFileSrc(asset.filePath);
  }
  const rasterThumb = getValidRasterThumbPath(asset);
  if (rasterThumb) return convertFileSrc(rasterThumb);
  return getNativeImageUrl(asset) ?? (snapshotUrl || null);
}

export function getAssetThumbnailUrls(asset: Asset, snapshotUrl?: string | null): string[] {
  if (asset.extension.toLowerCase() === 'gif' && asset.filePath) {
    return [convertFileSrc(asset.filePath)];
  }
  const multi = getMultiThumbs(asset);
  if (multi.length > 0) return multi;

  const single = getSingleThumb(asset, snapshotUrl);
  return single ? [single] : [];
}

interface CarouselNavProps {
  total: number;
  current: number;
  onPrev: (e: React.MouseEvent) => void;
  onNext: (e: React.MouseEvent) => void;
  onSelect: (index: number, e: React.MouseEvent) => void;
}

function CarouselControls({ total, current, onPrev, onNext, onSelect }: CarouselNavProps) {
  return (
    <>
      <button
        type="button"
        className={clsx(styles.navBtn, styles.prevBtn)}
        onClick={onPrev}
        aria-label="Previous thumbnail variant"
        title="Previous variant (Left arrow)"
      >
        <IconChevronLeft size={13} />
      </button>

      <button
        type="button"
        className={clsx(styles.navBtn, styles.nextBtn)}
        onClick={onNext}
        aria-label="Next thumbnail variant"
        title="Next variant (Right arrow)"
      >
        <IconChevronRight size={13} />
      </button>

      <div className={styles.dots} role="tablist" aria-label="Thumbnail variant navigation">
        {Array.from({ length: total }).map((_, idx) => (
          <button
            key={idx}
            type="button"
            role="tab"
            aria-selected={idx === current}
            className={clsx(styles.dot, idx === current && styles.dotActive)}
            onClick={(e) => onSelect(idx, e)}
            aria-label={`Thumbnail variant ${idx + 1} of ${total}`}
          />
        ))}
      </div>
    </>
  );
}

interface CarouselBounds {
  curr: number;
  total: number;
}

function stepPrev({ curr, total }: CarouselBounds): number {
  return curr > 0 ? curr - 1 : total - 1;
}

function stepNext({ curr, total }: CarouselBounds): number {
  return curr + 1 < total ? curr + 1 : 0;
}

function getKeyStepDelta(key: string): -1 | 1 | 0 {
  if (key === 'ArrowLeft') return -1;
  if (key === 'ArrowRight') return 1;
  return 0;
}

function useCarouselIndex(bounds: { total: number }) {
  const { total } = bounds;
  const [index, setIndex] = useState(0);

  const prev = useCallback((e: React.MouseEvent) => {
    e.stopPropagation();
    setIndex((curr) => stepPrev({ curr, total }));
  }, [total]);

  const next = useCallback((e: React.MouseEvent) => {
    e.stopPropagation();
    setIndex((curr) => stepNext({ curr, total }));
  }, [total]);

  const select = useCallback((targetIndex: number, e: React.MouseEvent) => {
    e.stopPropagation();
    setIndex(targetIndex);
  }, []);

  const onKeyDown = useCallback((e: React.KeyboardEvent) => {
    const delta = getKeyStepDelta(e.key);
    if (total <= 1 || delta === 0) return;
    e.stopPropagation();
    setIndex((curr) => (delta < 0 ? stepPrev({ curr, total }) : stepNext({ curr, total })));
  }, [total]);

  return { index: index < total ? index : 0, prev, next, select, onKeyDown };
}

function useValidUrls(params: { urls: string[]; asset: Asset; snapshotUrl?: string | null }) {
  const { urls, asset, snapshotUrl } = params;
  const [failedUrls, setFailedUrls] = useState<Set<string>>(() => new Set());

  useEffect(() => {
    setFailedUrls(new Set());
  }, [asset.id, snapshotUrl]);

  const markFailed = useCallback((e: React.SyntheticEvent<HTMLImageElement>) => {
    const failedSrc = e.currentTarget.getAttribute('src') ?? e.currentTarget.src;
    setFailedUrls((prevSet) => new Set(prevSet).add(failedSrc));
  }, []);

  const validUrls = useMemo(
    () => urls.filter((u) => !failedUrls.has(u)),
    [urls, failedUrls]
  );

  return { validUrls, markFailed };
}

export const ThumbnailCarousel = memo(function ThumbnailCarousel({
  asset,
  className,
  imgClassName,
  alt,
  loading = 'lazy',
  snapshotUrl,
  fallback = null,
}: ThumbnailCarouselProps) {
  const urls = useMemo(
    () => getAssetThumbnailUrls(asset, snapshotUrl),
    [asset, snapshotUrl]
  );
  const { validUrls, markFailed } = useValidUrls({ urls, asset, snapshotUrl });
  const { index, prev, next, select, onKeyDown } = useCarouselIndex({ total: validUrls.length });
  const label = alt || asset.fileName;

  if (validUrls.length === 0) {
    return <>{fallback}</>;
  }

  if (validUrls.length === 1) {
    return (
      <img
        src={validUrls[0]}
        alt={label}
        className={clsx(styles.singleImg, imgClassName)}
        loading={loading}
        onError={markFailed}
      />
    );
  }

  return (
    <div
      className={clsx(styles.container, className)}
      tabIndex={0}
      role="region"
      aria-label={`Thumbnail variants for ${asset.fileName}`}
      onKeyDown={onKeyDown}
    >
      <img
        src={validUrls[index]}
        alt={`${label} (preview ${index + 1} of ${validUrls.length})`}
        className={clsx(styles.carouselImg, imgClassName)}
        loading={loading}
        onError={markFailed}
      />

      <CarouselControls
        total={validUrls.length}
        current={index}
        onPrev={prev}
        onNext={next}
        onSelect={select}
      />
    </div>
  );
});
