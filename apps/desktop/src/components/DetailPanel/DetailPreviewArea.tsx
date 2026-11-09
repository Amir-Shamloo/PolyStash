import { useState, useCallback, useRef } from 'react';
import type { Asset, AssetKind } from '../../types';
import { resolvePreview } from '../../lib/previewRouting';
import { useAssetStore } from '../../store';
import type { ModelStats } from './ThreeViewerUtils';
import { ThreeViewer, DdsViewer } from './ThreeViewer';
import { PanoramaViewer } from './PanoramaViewer';
import { IESLightViewer } from './IESLightViewer';
import { VideoPreviewPlayer } from './VideoPreviewPlayer';
import { ThumbnailCarousel } from '../AssetGrid/ThumbnailCarousel';
import { FullscreenPreviewModal } from './FullscreenPreviewModal';
import { IconModel3D, IconImage, IconVideo, IconFileText, IconFolder } from '../Icons';
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

const KIND_ICON: Record<AssetKind, React.FC<{ size?: number; style?: React.CSSProperties }>> = {
  '3d_model': IconModel3D,
  material: IconImage,
  texture: IconImage,
  hdri: IconImage,
  ies: IconImage,
  video: IconVideo,
  image: IconImage,
  document: IconFileText,
  folder: IconFolder,
  other: IconImage,
};

const KIND_COLOR: Record<AssetKind, string> = {
  '3d_model': 'hsl(220 85% 55%)',
  material: 'hsl(268 72% 60%)',
  texture: 'hsl(178 64% 46%)',
  hdri: 'hsl( 38 92% 56%)',
  ies: 'hsl( 45 95% 50%)',
  video: 'hsl(  4 80% 55%)',
  image: 'hsl(140 64% 46%)',
  document: 'hsl(213 84% 55%)',
  folder: 'hsl( 38 92% 56%)',
  other: 'hsl(220  8% 50%)',
};

function PlaceholderView({ kind, ext, message }: { kind: AssetKind; ext: string; message?: string }) {
  const KindIcon = KIND_ICON[kind] ?? IconImage;
  const color = KIND_COLOR[kind] ?? 'var(--color-text-disabled)';
  return (
    <div
      style={{
        width: '100%',
        height: '100%',
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        gap: '8px',
      }}
    >
      <KindIcon size={36} style={{ color }} />
      <span style={{ fontSize: '24px', fontWeight: 700, fontFamily: 'var(--font-mono)', color }}>
        .{ext.toUpperCase()}
      </span>
      {message && (
        <span style={{ fontSize: '11px', color: 'var(--color-text-muted)', textAlign: 'center', maxWidth: '80%' }}>
          {message}
        </span>
      )}
    </div>
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

  switch (preview.type) {
    case 'video':
      return <VideoPreviewPlayer filePath={preview.path} onToggleFullscreen={onFullscreen} />;
    case 'model':
      return (
        <ThreeViewer
          modelPath={preview.path}
          extension={preview.extension}
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
        snapshotUrl={snapshotUrl}
        imgClassName={imgClassName}
        onFullscreen={handleOpenFullscreen}
        onModelStats={handleModelStats}
      />
      {isFullscreen && (
        <FullscreenPreviewModal
          asset={asset}
          onClose={handleCloseFullscreen}
          snapshotUrl={snapshotUrl}
        />
      )}
    </div>
  );
}
