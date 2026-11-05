import { memo, useState, useCallback, useMemo } from 'react';
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

function isDirectUrl(path: string): boolean {
  return PROTOCOL_REGEX.test(path);
}

function isAbsolutePath(path: string): boolean {
  return ABSOLUTE_PATH_REGEX.test(path);
}

function resolveRelativePath(baseThumb: string, relPath: string): string {
  const sep = baseThumb.includes('\\') ? '\\' : '/';
  const polystashIdx = baseThumb.search(/[/\\]\.polystash/);
  if (polystashIdx !== -1) {
    const root = baseThumb.slice(0, polystashIdx);
    const cleanRel = relPath.replace(/^[/\\]+/, '').split(/[/\\]/).join(sep);
    return `${root}${sep}${cleanRel}`;
  }

  const lastSlash = Math.max(baseThumb.lastIndexOf('/'), baseThumb.lastIndexOf('\\'));
  const dir = lastSlash !== -1 ? baseThumb.slice(0, lastSlash) : '';
  const filename = relPath.split(/[/\\]/).pop() || relPath;
  return dir ? `${dir}${sep}${filename}` : relPath;
}

export function resolveThumbnailUrl(asset: Asset, rawPath?: string): string {
  if (!rawPath) return '';
  if (isDirectUrl(rawPath)) return rawPath;
  if (isAbsolutePath(rawPath)) return convertFileSrc(rawPath);

  const resolved = asset.thumbnailPath
    ? resolveRelativePath(asset.thumbnailPath, rawPath)
    : rawPath;
  return convertFileSrc(resolved);
}

function getMultiThumbs(asset: Asset): string[] | null {
  const thumbsMeta = asset.metadata?.['polystash.thumbs'];
  if (!Array.isArray(thumbsMeta) || thumbsMeta.length === 0) return null;

  return thumbsMeta
    .filter((p): p is string => typeof p === 'string' && p.trim().length > 0)
    .map((p, idx) => {
      const isPrimary = idx === 0 && Boolean(asset.thumbnailPath) && !asset.thumbnailPath?.endsWith('.glb');
      return isPrimary ? convertFileSrc(asset.thumbnailPath!) : resolveThumbnailUrl(asset, p);
    });
}

function getSingleThumb(asset: Asset, snapshotUrl?: string | null): string | null {
  if (asset.thumbnailPath && !asset.thumbnailPath.endsWith('.glb')) {
    return convertFileSrc(asset.thumbnailPath);
  }
  const isImageKind = asset.kind === 'image' || asset.kind === 'texture';
  if (isImageKind && asset.filePath) {
    return convertFileSrc(asset.filePath);
  }
  return snapshotUrl || null;
}

export function getAssetThumbnailUrls(asset: Asset, snapshotUrl?: string | null): string[] {
  const multi = getMultiThumbs(asset);
  if (multi && multi.length > 0) return multi;

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

function stepPrev(curr: number, total: number): number {
  return curr > 0 ? curr - 1 : total - 1;
}

function stepNext(curr: number, total: number): number {
  return curr + 1 < total ? curr + 1 : 0;
}

function useCarouselIndex(total: number) {
  const [index, setIndex] = useState(0);

  const prev = useCallback((e: React.MouseEvent) => {
    e.stopPropagation();
    setIndex((curr) => stepPrev(curr, total));
  }, [total]);

  const next = useCallback((e: React.MouseEvent) => {
    e.stopPropagation();
    setIndex((curr) => stepNext(curr, total));
  }, [total]);

  const select = useCallback((targetIndex: number, e: React.MouseEvent) => {
    e.stopPropagation();
    setIndex(targetIndex);
  }, []);

  const onKeyDown = useCallback((e: React.KeyboardEvent) => {
    if (total <= 1) return;
    if (e.key === 'ArrowLeft') {
      e.stopPropagation();
      setIndex((curr) => stepPrev(curr, total));
    } else if (e.key === 'ArrowRight') {
      e.stopPropagation();
      setIndex((curr) => stepNext(curr, total));
    }
  }, [total]);

  return { index: index < total ? index : 0, prev, next, select, onKeyDown };
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
  const { index, prev, next, select, onKeyDown } = useCarouselIndex(urls.length);

  if (urls.length === 0) {
    return <>{fallback}</>;
  }

  if (urls.length === 1) {
    return (
      <img
        src={urls[0]}
        alt={alt || asset.fileName}
        className={clsx(styles.singleImg, imgClassName)}
        loading={loading}
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
        src={urls[index]}
        alt={`${alt || asset.fileName} (preview ${index + 1} of ${urls.length})`}
        className={clsx(styles.carouselImg, imgClassName)}
        loading={loading}
      />

      <CarouselControls
        total={urls.length}
        current={index}
        onPrev={prev}
        onNext={next}
        onSelect={select}
      />
    </div>
  );
});
