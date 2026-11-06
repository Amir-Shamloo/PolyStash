import { Canvas, useLoader, useThree } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei';
import { Suspense, useRef, useState, useCallback, useEffect } from 'react';
import * as THREE from 'three';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import { RGBELoader } from 'three/examples/jsm/loaders/RGBELoader.js';
import { EXRLoader } from 'three/examples/jsm/loaders/EXRLoader.js';
import { IconRefresh, IconCameraReset } from '../Icons';
import { ThreeErrorBoundary } from './ThreeViewer';
import styles from './ThreeViewer.module.css';

interface OrbitHandle {
  target: THREE.Vector3;
  reset: () => void;
  update: () => void;
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

interface PanoramaToolbarProps {
  autoRotate: boolean;
  isFullscreen?: boolean;
  onToggleRotate: () => void;
  onResetCamera: () => void;
  onToggleFullscreen?: () => void;
}

function PanoramaToolbar({
  autoRotate,
  isFullscreen = false,
  onToggleRotate,
  onResetCamera,
  onToggleFullscreen,
}: PanoramaToolbarProps) {
  return (
    <div className={styles.toolbar} role="toolbar" aria-label="Panorama Viewer Controls">
      <button
        type="button"
        className={clsx(styles.toolbarBtn, autoRotate && styles.toolbarBtnActive)}
        onClick={onToggleRotate}
        aria-label="Toggle auto-rotation"
        aria-pressed={autoRotate}
        title="Toggle auto-rotation"
      >
        <IconRefresh size={13} />
      </button>

      <button
        type="button"
        className={styles.toolbarBtn}
        onClick={onResetCamera}
        aria-label="Reset camera and target"
        title="Reset camera and target"
      >
        <IconCameraReset size={13} />
      </button>

      {onToggleFullscreen && (
        <button
          type="button"
          className={styles.toolbarBtn}
          onClick={onToggleFullscreen}
          aria-label={isFullscreen ? 'Exit fullscreen' : 'Enter fullscreen'}
          title={isFullscreen ? 'Exit fullscreen' : 'Fullscreen'}
        >
          {isFullscreen ? <IconMinimize size={13} /> : <IconMaximize size={13} />}
        </button>
      )}
    </div>
  );
}

function EquirectEnvironment({ url, isExr }: { url: string; isExr: boolean }) {
  const { scene } = useThree();
  const loader = isExr ? EXRLoader : RGBELoader;
  const texture = useLoader(loader, url);

  useEffect(() => {
    if (!texture) return;
    texture.mapping = THREE.EquirectangularReflectionMapping;
    scene.background = texture;
    return () => {
      scene.background = null;
    };
  }, [texture, scene]);

  return (
    <mesh scale={[-1, 1, 1]}>
      <sphereGeometry args={[500, 64, 32]} />
      <meshBasicMaterial map={texture} side={THREE.BackSide} />
    </mesh>
  );
}

export interface PanoramaViewerProps {
  filePath: string;
  extension?: string;
  isFullscreen?: boolean;
  onToggleFullscreen?: () => void;
}

export function PanoramaViewer({
  filePath,
  extension,
  isFullscreen,
  onToggleFullscreen,
}: PanoramaViewerProps) {
  const [autoRotate, setAutoRotate] = useState(false);
  const controlsRef = useRef<OrbitHandle | null>(null);

  const cleanExt = (extension || filePath.split('.').pop() || '').toLowerCase();
  const isExr = cleanExt === 'exr';
  const fileUrl = convertFileSrc(filePath);

  const handleResetCamera = useCallback(() => {
    if (controlsRef.current) {
      controlsRef.current.target.set(0, 0, 0);
      controlsRef.current.reset();
      controlsRef.current.update();
    }
  }, []);

  return (
    <div className={styles.viewerContainer}>
      <PanoramaToolbar
        autoRotate={autoRotate}
        isFullscreen={isFullscreen}
        onToggleRotate={() => setAutoRotate((v) => !v)}
        onResetCamera={handleResetCamera}
        onToggleFullscreen={onToggleFullscreen}
      />

      <ThreeErrorBoundary
        key={filePath}
        extension={cleanExt}
      >
        <Canvas
          camera={{ position: [0, 0, 0.1], fov: 75 }}
          gl={{
            toneMapping: THREE.ACESFilmicToneMapping,
            toneMappingExposure: 1.0,
            outputColorSpace: THREE.SRGBColorSpace,
            antialias: true,
          }}
        >
          <Suspense fallback={null}>
            <EquirectEnvironment url={fileUrl} isExr={isExr} />
            <OrbitControls
              ref={controlsRef as never}
              enableZoom
              enablePan={false}
              enableDamping
              autoRotate={autoRotate}
              autoRotateSpeed={0.5}
              rotateSpeed={-0.4}
              makeDefault
            />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}
