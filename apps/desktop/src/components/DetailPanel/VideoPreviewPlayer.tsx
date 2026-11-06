import { useState, useCallback, type SyntheticEvent } from 'react';
import { convertFileSrc } from '@tauri-apps/api/core';
import { IconVideo } from '../Icons';
import styles from './VideoPreviewPlayer.module.css';

export interface VideoPreviewPlayerProps {
  filePath: string;
  className?: string;
  controls?: boolean;
  autoPlay?: boolean;
  muted?: boolean;
  playsInline?: boolean;
  isFullscreen?: boolean;
  onToggleFullscreen?: () => void;
}

function IconMaximize({ size = 13 }: { size?: number }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
      <path d="M8 3H5a2 2 0 0 0-2 2v3m18 0V5a2 2 0 0 0-2-2h-3m0 18h3a2 2 0 0 0 2-2v-3M3 16v3a2 2 0 0 0 2 2h3" />
    </svg>
  );
}

function IconMinimize({ size = 13 }: { size?: number }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
      <path d="M4 14h6m0 0v6m0-6L3 21m17-7h-6m0 0v6m0-6l7 7M14 10V4m0 6h6m-6 0l7-7M10 10V4m0 6H4m6 0L3 3" />
    </svg>
  );
}

function getFileExtension(path: string): string {
  const clean = path.split('?')[0].split('#')[0];
  const lastDot = clean.lastIndexOf('.');
  return lastDot !== -1 ? clean.slice(lastDot + 1).toLowerCase() : '';
}

function resolveVideoErrorMessage(mediaError: MediaError | null, ext: string): string {
  const isPotentialAv1 = ext === 'av1' || ext === 'mkv' || ext === 'webm';
  if (mediaError?.code === 4 || isPotentialAv1) {
    return 'This video format requires the AV1 Video Extension to play (Windows).';
  }
  return 'Unable to play video format.';
}

export function VideoPreviewPlayer({
  filePath,
  className,
  controls = true,
  autoPlay = false,
  muted = true,
  playsInline = true,
  isFullscreen = false,
  onToggleFullscreen,
}: VideoPreviewPlayerProps) {
  const [playbackError, setPlaybackError] = useState<string | null>(null);

  const handleError = useCallback((e: SyntheticEvent<HTMLVideoElement, Event>) => {
    const mediaError = e.currentTarget.error;
    const ext = getFileExtension(filePath);
    setPlaybackError(resolveVideoErrorMessage(mediaError, ext));
  }, [filePath]);

  return (
    <div className={className ? `${styles.videoContainer} ${className}` : styles.videoContainer}>
      {onToggleFullscreen && (
        <div className={styles.videoToolbar} role="toolbar" aria-label="Video Controls">
          <button
            type="button"
            className={styles.toolbarBtn}
            onClick={onToggleFullscreen}
            aria-label={isFullscreen ? 'Exit fullscreen' : 'Enter fullscreen'}
            title={isFullscreen ? 'Exit fullscreen' : 'Fullscreen'}
          >
            {isFullscreen ? <IconMinimize size={13} /> : <IconMaximize size={13} />}
          </button>
        </div>
      )}

      <video
        src={convertFileSrc(filePath)}
        controls={controls}
        autoPlay={autoPlay}
        muted={muted}
        playsInline={playsInline}
        onError={handleError}
        className={styles.videoElement}
      />

      {playbackError && (
        <div className={styles.errorOverlay} role="alert">
          <IconVideo size={28} style={{ color: 'var(--color-brand-500)' }} />
          <span className={styles.errorTitle}>Playback Error</span>
          <span className={styles.errorMessage}>{playbackError}</span>
          <span className={styles.errorHint}>Codec support required</span>
        </div>
      )}
    </div>
  );
}
