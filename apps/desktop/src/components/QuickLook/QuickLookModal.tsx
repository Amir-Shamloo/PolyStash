import { memo, useRef, useState, useEffect, useCallback } from 'react';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import type { Asset } from '../../types';
import { ThreeViewer, getModelPath } from '../DetailPanel/ThreeViewer';
import { ThumbnailCarousel, getAssetThumbnailUrls } from '../AssetGrid/ThumbnailCarousel';
import { useModelSnapshot } from '../../lib/previewSnapshot';
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

function seekVideoToStep(video: HTMLVideoElement, factor: number, duration: number) {
  const seekTarget = factor === 1.0 ? Math.max(0, duration - 1.0) : duration * factor;
  video.currentTime = seekTarget;
  video.play().catch(() => {});
}

function VideoQuickLook({ filePath }: { filePath: string }) {
  const videoRef = useRef<HTMLVideoElement>(null);
  const [activeStep, setActiveStep] = useState<number>(0);
  const [duration, setDuration] = useState<number>(0);
  const [currentTime, setCurrentTime] = useState<number>(0);

  const handleLoadedMetadata = () => {
    if (videoRef.current) {
      setDuration(videoRef.current.duration);
      videoRef.current.currentTime = 0;
      videoRef.current.play().catch(() => {});
    }
  };

  useEffect(() => {
    if (!duration || duration <= 0) return;

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
        onTimeUpdate={() => videoRef.current && setCurrentTime(videoRef.current.currentTime)}
      />
      <div className={styles.videoTimelineOverlay}>
        <div className={styles.stepPills}>
          {VIDEO_STEPS.map((st, idx) => (
            <button
              key={st.label}
              type="button"
              className={clsx(styles.stepPill, idx === activeStep && styles.stepPillActive)}
              onClick={() => handleStepClick(idx)}
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
    </div>
  );
}

function QuickLookHeader({ asset }: { asset: Asset }) {
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
      </div>
    </div>
  );
}

function PlaceholderIcon({ kind, ext }: { kind: string; ext: string }) {
  const Icon = kind === '3d_model' ? IconModel3D
    : kind === 'video' ? IconVideo
    : kind === 'document' ? IconFileText
    : IconImage;

  return (
    <div className={styles.placeholder}>
      <Icon size={64} />
      <p className={styles.placeholderExt}>.{ext.toUpperCase()}</p>
      <p className={styles.placeholderText}>No preview available</p>
    </div>
  );
}

interface ContentProps {
  asset: Asset;
  modelPath?: string;
  snapshotUrl: string | null;
}

function QuickLookPreviewContent({ asset, modelPath, snapshotUrl }: ContentProps) {
  if (modelPath) {
    return (
      <div className={styles.threeWrapper}>
        <ThreeViewer modelPath={modelPath} />
      </div>
    );
  }

  const isVideo =
    asset.kind === 'video' ||
    ['mp4', 'webm', 'mov', 'mkv', 'avi'].includes(asset.extension.toLowerCase());

  if (isVideo && asset.filePath) {
    return <VideoQuickLook filePath={asset.filePath} />;
  }

  const thumbs = getAssetThumbnailUrls(asset, snapshotUrl);
  if (thumbs.length > 0) {
    return (
      <div className={styles.imgWrapper}>
        <ThumbnailCarousel
          asset={asset}
          snapshotUrl={snapshotUrl}
          imgClassName={styles.largeImg}
          alt={asset.fileName}
        />
      </div>
    );
  }

  return <PlaceholderIcon kind={asset.kind} ext={asset.extension} />;
}

export const QuickLookModal = memo(function QuickLookModal({
  asset,
  isOpen,
}: QuickLookModalProps) {
  const snapshotUrl = useModelSnapshot(asset);

  if (!isOpen || !asset) return null;

  const modelPath = getModelPath(asset);

  return (
    <div className={styles.overlay} aria-modal="true" role="dialog">
      <div className={styles.container}>
        <QuickLookHeader asset={asset} />

        <div className={styles.previewContent}>
          <QuickLookPreviewContent
            asset={asset}
            modelPath={modelPath}
            snapshotUrl={snapshotUrl}
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
  );
});
