import { useEffect, useCallback } from 'react';
import { convertFileSrc } from '@tauri-apps/api/core';
import type { Asset } from '../../types';
import { resolvePreview } from '../../lib/previewRouting';
import { UsdConvertView } from './UsdConvertView';
import { IconX } from '../Icons';
import { ThreeViewer, DdsViewer } from './ThreeViewer';
import { PanoramaViewer } from './PanoramaViewer';
import { IESLightViewer } from './IESLightViewer';
import { VideoPreviewPlayer } from './VideoPreviewPlayer';
import styles from './FullscreenPreviewModal.module.css';

export interface FullscreenPreviewModalProps {
  asset: Asset;
  onClose: () => void;
  snapshotUrl?: string | null;
}

const RASTER_TEXTURE_EXTENSIONS = new Set([
  'jpg', 'jpeg', 'png', 'webp', 'bmp', 'tga', 'tif', 'tiff', 'avif',
]);

function TextureFullscreenViewer({ asset, fallbackUrl }: { asset: Asset; fallbackUrl?: string }) {
  const ext = asset.extension.toLowerCase();
  const isRaster = RASTER_TEXTURE_EXTENSIONS.has(ext);
  const sourceUrl = isRaster && asset.filePath ? convertFileSrc(asset.filePath) : (fallbackUrl || '');

  return (
    <div className={styles.textureContainer}>
      <img
        src={sourceUrl}
        alt={asset.fileName}
        className={styles.textureImage}
      />
    </div>
  );
}

function FullscreenContent({
  asset,
  onClose,
  snapshotUrl,
}: {
  asset: Asset;
  onClose: () => void;
  snapshotUrl?: string | null;
}) {
  const preview = resolvePreview(asset, snapshotUrl);

  switch (preview.type) {
    case 'model':
      return (
        <ThreeViewer
          modelPath={preview.path}
          extension={preview.extension}
          sbsarTextures={preview.sbsarTextures}
          isFullscreen
          onToggleFullscreen={onClose}
        />
      );
    case 'panorama':
      return (
        <PanoramaViewer
          filePath={preview.path}
          extension={preview.extension}
          isFullscreen
          onToggleFullscreen={onClose}
        />
      );
    case 'ies':
      return (
        <IESLightViewer
          filePath={preview.path}
          extension={preview.extension}
          isFullscreen
          onToggleFullscreen={onClose}
        />
      );
    case 'dds':
      return (
        <DdsViewer
          filePath={preview.path}
          isFullscreen
          onToggleFullscreen={onClose}
        />
      );
    case 'video':
      return (
        <VideoPreviewPlayer
          filePath={preview.path}
          controls
          autoPlay
          isFullscreen
          onToggleFullscreen={onClose}
        />
      );
    case 'thumbnail':
      return <TextureFullscreenViewer asset={asset} fallbackUrl={preview.urls[0]} />;
    case 'usd-convert':
      return <UsdConvertView asset={preview.asset} fallback={null} />;
    case 'placeholder':
      return null;
  }
}

export function FullscreenPreviewModal({
  asset,
  onClose,
  snapshotUrl,
}: FullscreenPreviewModalProps) {
  const handleKeyDown = useCallback(
    (e: KeyboardEvent) => {
      if (e.key === 'Escape') {
        onClose();
      }
    },
    [onClose]
  );

  useEffect(() => {
    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [handleKeyDown]);

  return (
    <div
      className={styles.overlay}
      role="dialog"
      aria-modal="true"
      aria-label={`Fullscreen preview: ${asset.fileName}`}
    >
      <div className={styles.header}>
        <div className={styles.headerLeft}>
          <span className={styles.extBadge}>.{asset.extension.toUpperCase()}</span>
          <span className={styles.fileName} title={asset.fileName}>
            {asset.fileName}
          </span>
        </div>
        <div className={styles.headerRight}>
          <span className={styles.shortcutHint}>Esc to exit</span>
          <button
            type="button"
            className={styles.closeBtn}
            onClick={onClose}
            aria-label="Close fullscreen preview"
            title="Close (Esc)"
          >
            <IconX size={15} />
          </button>
        </div>
      </div>

      <div className={styles.body}>
        <FullscreenContent asset={asset} onClose={onClose} snapshotUrl={snapshotUrl} />
      </div>
    </div>
  );
}
