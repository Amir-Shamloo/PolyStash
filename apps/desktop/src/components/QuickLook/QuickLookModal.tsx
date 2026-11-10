import { memo, useRef, useState, useEffect, useCallback, type SyntheticEvent } from 'react';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import type { Asset } from '../../types';
import { ThreeViewer, DdsViewer } from '../DetailPanel/ThreeViewer';
import { PanoramaViewer } from '../DetailPanel/PanoramaViewer';
import { IESLightViewer } from '../DetailPanel/IESLightViewer';
import { ThumbnailCarousel } from '../AssetGrid/ThumbnailCarousel';
import { FullscreenPreviewModal } from '../DetailPanel/FullscreenPreviewModal';
import { useModelSnapshot } from '../../lib/previewSnapshot';
import { resolvePreview } from '../../lib/previewRouting';
import { IconStar, IconModel3D, IconImage, IconVideo, IconFileText } from '../Icons';
import styles from './QuickLookModal.module.css';

interface QuickLookModalProps {
  asset: Asset | null;
  isOpen: boolean;
}

const VIDEO_STEPS = [
  { label: '0%', factor: 0.0 },
  { label: '25%', factor: 0.25 },
  { label: '50%', factor: 0.50 },
  { label: '75%', factor: 0.75 },
  { label: '100%', factor: 1.0 },
];

function IconMaximize({ size = 13 }: { size?: number }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
      <path d="M8 3H5a2 2 0 0 0-2 2v3m18 0V5a2 2 0 0 0-2-2h-3m0 18h3a2 2 0 0 0 2-2v-3M3 16v3a2 2 0 0 0 2 2h3" />
    </svg>
  );
}

function formatTime(sec: number): string {
  const m = Math.floor(sec / 60);
  const s = Math.floor(sec % 60);
  return `${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
}

function formatSize(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
  if (bytes < 1024 * 1024 * 1024) return `${(bytes / 1024 / 1024).toFixed(1)} MB`;
  return `${(bytes / 1024 / 1024 / 1024).toFixed(2)} GB`;
}

function seekVideoToStep(video: HTMLVideoElement, factor: number, duration: number): void {
  const seekTarget = factor === 1.0 ? Math.max(0, duration - 1.0) : duration * factor;
  video.currentTime = seekTarget;
  video.play().catch(() => {});
}

function resolveVideoError(mediaError: MediaError | null, filePath: string): string {
  const ext = filePath.split('.').pop()?.toLowerCase() || '';
  const isPotentialAv1 = ext === 'av1' || ext === 'mkv' || ext === 'webm';
  if (mediaError?.code === 4 || isPotentialAv1) {
    return 'This video format requires the AV1 Video Extension to play (Windows).';
  }
  return 'Unable to play video format.';
}

function VideoErrorDisplay({ error }: { error: string }) {
  return (
    <div
      style={{
        position: 'absolute',
        inset: 0,
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        gap: 8,
        background: 'rgba(10, 14, 23, 0.92)',
        zIndex: 10,
        padding: 16,
        textAlign: 'center',
      }}
      role="alert"
    >
      <IconVideo size={32} style={{ color: 'var(--color-brand-500)' }} />
      <span style={{ fontSize: '13px', fontWeight: 600, color: 'var(--color-text-primary)' }}>
        Playback Error
      </span>
      <span style={{ fontSize: '12px', color: 'var(--color-text-secondary)', maxWidth: 320 }}>
        {error}
      </span>
    </div>
  );
}

function VideoTimelineControls({
  activeStep,
  currentTime,
  duration,
  onStepClick,
}: {
  activeStep: number;
  currentTime: number;
  duration: number;
  onStepClick: (idx: number) => void;
}) {
  return (
    <div className={styles.videoTimelineOverlay}>
      <div className={styles.stepPills}>
        {VIDEO_STEPS.map((st, idx) => (
          <button
            key={st.label}
            type="button"
            className={clsx(styles.stepPill, idx === activeStep && styles.stepPillActive)}
            onClick={() => onStepClick(idx)}
            title={`Jump to ${st.label}`}
            aria-label={`Jump to ${st.label}`}
          >
            <span className={styles.stepDot} />
            <span>{st.label}</span>
          </button>
        ))}
      </div>
      <div className={styles.videoTimeBadge}>
        <span>{formatTime(currentTime)} / {formatTime(duration)}</span>
      </div>
    </div>
  );
}

function VideoQuickLook({ filePath }: { filePath: string }) {
  const videoRef = useRef<HTMLVideoElement>(null);
  const [activeStep, setActiveStep] = useState<number>(0);
  const [duration, setDuration] = useState<number>(0);
  const [currentTime, setCurrentTime] = useState<number>(0);
  const [playbackError, setPlaybackError] = useState<string | null>(null);

  const handleLoadedMetadata = useCallback(() => {
    if (videoRef.current) {
      setDuration(videoRef.current.duration);
      videoRef.current.currentTime = 0;
      videoRef.current.play().catch(() => {});
    }
  }, []);

  const handleError = useCallback((e: SyntheticEvent<HTMLVideoElement, Event>) => {
    setPlaybackError(resolveVideoError(e.currentTarget.error, filePath));
  }, [filePath]);

  useEffect(() => {
    if (duration <= 0) return;
    let stepIdx = 0;
    const interval = setInterval(() => {
      stepIdx = (stepIdx + 1) % VIDEO_STEPS.length;
      setActiveStep(stepIdx);
      if (videoRef.current) {
        seekVideoToStep(videoRef.current, VIDEO_STEPS[stepIdx].factor, duration);
      }
    }, 1000);
    return () => clearInterval(interval);
  }, [duration]);

  const handleStepClick = useCallback((idx: number) => {
    if (videoRef.current && duration > 0) {
      setActiveStep(idx);
      seekVideoToStep(videoRef.current, VIDEO_STEPS[idx].factor, duration);
    }
  }, [duration]);

  return (
    <div className={styles.videoWrapper}>
      <video
        ref={videoRef}
        src={convertFileSrc(filePath)}
        autoPlay
        muted
        playsInline
        className={styles.largeVideo}
        onLoadedMetadata={handleLoadedMetadata}
        onError={handleError}
        onTimeUpdate={() => videoRef.current && setCurrentTime(videoRef.current.currentTime)}
      />

      {playbackError && <VideoErrorDisplay error={playbackError} />}

      <VideoTimelineControls
        activeStep={activeStep}
        currentTime={currentTime}
        duration={duration}
        onStepClick={handleStepClick}
      />
    </div>
  );
}

function QuickLookHeader({ asset, onFullscreen }: { asset: Asset; onFullscreen: () => void }) {
  return (
    <div className={styles.header}>
      <div className={styles.headerLeft}>
        <span className={styles.extBadge}>.{asset.extension.toUpperCase()}</span>
        <span className={styles.fileName} title={asset.fileName}>
          {asset.fileName}
        </span>
      </div>
      <div className={styles.headerRight}>
        {asset.colorLabel !== 'none' && (
          <span
            className={styles.colorDot}
            style={{ background: `var(--swatch-${asset.colorLabel})` }}
            title={`Label: ${asset.colorLabel}`}
          />
        )}
        {asset.rating > 0 && (
          <div className={styles.ratingStars}>
            {Array.from({ length: asset.rating }).map((_, i) => (
              <IconStar key={i} size={13} className={styles.starFilled} />
            ))}
          </div>
        )}
        <span className={styles.sizeBadge}>{formatSize(asset.sizeBytes)}</span>
        <button
          type="button"
          onClick={onFullscreen}
          style={{
            display: 'inline-flex',
            alignItems: 'center',
            justifyContent: 'center',
            width: 26,
            height: 26,
            background: 'var(--color-bg-base)',
            border: '1px solid var(--color-border)',
            borderRadius: 'var(--radius-sm)',
            color: 'var(--color-text-secondary)',
            cursor: 'pointer',
          }}
          aria-label="Open fullscreen"
          title="Fullscreen preview"
        >
          <IconMaximize size={13} />
        </button>
      </div>
    </div>
  );
}

const PLACEHOLDER_ICONS: Record<string, React.FC<{ size?: number }>> = {
  '3d_model': IconModel3D,
  video: IconVideo,
  document: IconFileText,
};

function PlaceholderIcon({ kind, ext, message }: { kind: string; ext: string; message?: string }) {
  const Icon = PLACEHOLDER_ICONS[kind] ?? IconImage;

  return (
    <div className={styles.placeholder}>
      <Icon size={64} />
      <p className={styles.placeholderExt}>.{ext.toUpperCase()}</p>
      <p className={styles.placeholderText}>{message ?? 'No preview available'}</p>
    </div>
  );
}

interface ContentProps {
  asset: Asset;
  snapshotUrl: string | null;
  onFullscreen: () => void;
}

function QuickLookPreviewContent({ asset, snapshotUrl, onFullscreen }: ContentProps) {
  const preview = resolvePreview(asset, snapshotUrl);

  switch (preview.type) {
    case 'video':
      return <VideoQuickLook filePath={preview.path} />;
    case 'model':
      return (
        <div className={styles.threeWrapper}>
          <ThreeViewer
            modelPath={preview.path}
            extension={preview.extension}
            sbsarTextures={preview.sbsarTextures}
            onToggleFullscreen={onFullscreen}
          />
        </div>
      );
    case 'panorama':
      return (
        <div className={styles.threeWrapper}>
          <PanoramaViewer filePath={preview.path} extension={preview.extension} onToggleFullscreen={onFullscreen} />
        </div>
      );
    case 'ies':
      return (
        <div className={styles.threeWrapper}>
          <IESLightViewer filePath={preview.path} extension={preview.extension} onToggleFullscreen={onFullscreen} />
        </div>
      );
    case 'dds':
      return (
        <div className={styles.threeWrapper}>
          <DdsViewer filePath={preview.path} onToggleFullscreen={onFullscreen} />
        </div>
      );
    case 'thumbnail':
      return (
        <div className={styles.imgWrapper} style={{ position: 'relative' }}>
          <ThumbnailCarousel
            asset={asset}
            snapshotUrl={snapshotUrl}
            imgClassName={styles.largeImg}
            alt={asset.fileName}
          />
        </div>
      );
    case 'placeholder':
      return <PlaceholderIcon kind={preview.kind} ext={preview.extension} message={preview.message} />;
  }
}

export const QuickLookModal = memo(function QuickLookModal({
  asset,
  isOpen,
}: QuickLookModalProps) {
  const [isFullscreen, setIsFullscreen] = useState(false);
  const snapshotUrl = useModelSnapshot(asset);

  const handleOpenFullscreen = useCallback(() => setIsFullscreen(true), []);
  const handleCloseFullscreen = useCallback(() => setIsFullscreen(false), []);

  if (!isOpen || !asset) return null;

  return (
    <>
      <div className={styles.overlay} aria-modal="true" role="dialog">
        <div className={styles.container}>
          <QuickLookHeader asset={asset} onFullscreen={handleOpenFullscreen} />

          <div className={styles.previewContent}>
            <QuickLookPreviewContent
              asset={asset}
              snapshotUrl={snapshotUrl}
              onFullscreen={handleOpenFullscreen}
            />
          </div>

          <div className={styles.footer}>
            <span className={styles.filePath} title={asset.filePath}>
              {asset.filePath}
            </span>
            <div className={styles.hintBadge}>
              <kbd className={styles.kbd}>Space</kbd>
              <span>Release to close</span>
            </div>
          </div>
        </div>
      </div>

      {isFullscreen && (
        <FullscreenPreviewModal
          asset={asset}
          onClose={handleCloseFullscreen}
          snapshotUrl={snapshotUrl}
        />
      )}
    </>
  );
});
