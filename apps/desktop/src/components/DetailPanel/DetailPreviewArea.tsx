import { useState, useCallback, useMemo, useRef } from 'react';
import type { Asset } from '../../types';
import { resolvePreview } from '../../lib/previewRouting';
import { usePdfPageThumbnail } from '../../lib/pdfThumbnail';
import { useAssetStore } from '../../store';
import type { ModelStats } from './ThreeViewerUtils';
import { ThreeViewer, DdsViewer } from './ThreeViewer';
import { PanoramaViewer } from './PanoramaViewer';
import { UsdConvertView } from './UsdConvertView';
import { IESLightViewer } from './IESLightViewer';
import { VideoPreviewPlayer } from './VideoPreviewPlayer';
import { ThumbnailCarousel } from '../AssetGrid/ThumbnailCarousel';
import { FullscreenPreviewModal } from './FullscreenPreviewModal';
import { PlaceholderView } from './PlaceholderView';
import styles from './DetailPreviewArea.module.css';

export interface DetailPreviewAreaProps {
  asset: Asset;
  snapshotUrl?: string | null;
  imgClassName?: string;
  isTab?: boolean;
}

function IconMaximize({ size = 13 }: { size?: number }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
      <path d="M8 3H5a2 2 0 0 0-2 2v3m18 0V5a2 2 0 0 0-2-2h-3m0 18h3a2 2 0 0 0 2-2v-3M3 16v3a2 2 0 0 0 2 2h3" />
    </svg>
  );
}

function PreviewBody({
  asset,
  snapshotUrl,
  imgClassName,
  onFullscreen,
  onModelStats,
}: {
  asset: Asset;
  snapshotUrl?: string | null;
  imgClassName?: string;
  onFullscreen: () => void;
  onModelStats?: (stats: ModelStats) => void;
}) {
  const preview = resolvePreview(asset, snapshotUrl);
  const thumbnailFallback = useMemo(
    () => <PlaceholderView kind={asset.kind} ext={asset.extension.toLowerCase()} />,
    [asset.kind, asset.extension]
  );

  switch (preview.type) {
    case 'video':
      return <VideoPreviewPlayer filePath={preview.path} onToggleFullscreen={onFullscreen} />;
    case 'model':
      return (
        <ThreeViewer
          modelPath={preview.path}
          extension={preview.extension}
          sbsarTextures={preview.sbsarTextures}
          onToggleFullscreen={onFullscreen}
          onStats={onModelStats}
        />
      );
    case 'panorama':
      return <PanoramaViewer filePath={preview.path} extension={preview.extension} onToggleFullscreen={onFullscreen} />;
    case 'ies':
      return <IESLightViewer filePath={preview.path} extension={preview.extension} onToggleFullscreen={onFullscreen} />;
    case 'dds':
      return <DdsViewer filePath={preview.path} onToggleFullscreen={onFullscreen} />;
    case 'thumbnail':
      return (
        <>
          <ThumbnailCarousel
            asset={asset}
            snapshotUrl={snapshotUrl}
            imgClassName={imgClassName}
            alt={asset.fileName}
            fallback={thumbnailFallback}
          />
          <button
            type="button"
            className={styles.fullscreenBtn}
            onClick={onFullscreen}
            aria-label="View fullscreen image"
            title="Fullscreen image"
          >
            <IconMaximize size={13} />
          </button>
        </>
      );
    case 'usd-convert':
      return <UsdConvertView asset={preview.asset} fallback={thumbnailFallback} />;
    case 'placeholder':
      return <PlaceholderView kind={preview.kind} ext={preview.extension} message={preview.message} />;
  }
}

export function DetailPreviewArea({
  asset,
  snapshotUrl,
  imgClassName,
}: DetailPreviewAreaProps) {
  const [isFullscreen, setIsFullscreen] = useState(false);
  const patchAsset = useAssetStore((s) => s.patchAsset);
  const pdfThumbUrl = usePdfPageThumbnail(asset);
  const effectiveSnapshotUrl = snapshotUrl ?? pdfThumbUrl;
  const assetRef = useRef(asset);
  assetRef.current = asset;
  const handleOpenFullscreen = useCallback(() => setIsFullscreen(true), []);
  const handleCloseFullscreen = useCallback(() => setIsFullscreen(false), []);

  // One-shot: mesh stats land in technical metadata the first time a model loads.
  const handleModelStats = useCallback(
    (stats: ModelStats) => {
      const current = assetRef.current;
      if (!current || 'meshes' in (current.metadata ?? {})) return;
      void patchAsset(current.id, {
        metadata: {
          ...current.metadata,
          meshes: stats.meshCount,
          triangles: stats.triangles,
          vertices: stats.vertices,
        },
      });
    },
    [patchAsset]
  );

  return (
    <div className={styles.container}>
      <PreviewBody
        asset={asset}
        snapshotUrl={effectiveSnapshotUrl}
        imgClassName={imgClassName}
        onFullscreen={handleOpenFullscreen}
        onModelStats={handleModelStats}
      />
      {isFullscreen && (
        <FullscreenPreviewModal
          asset={asset}
          onClose={handleCloseFullscreen}
          snapshotUrl={effectiveSnapshotUrl}
        />
      )}
    </div>
  );
}
