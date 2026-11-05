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

interface PanoramaToolbarProps {
  autoRotate: boolean;
  onToggleRotate: () => void;
  onResetCamera: () => void;
}

function PanoramaToolbar({ autoRotate, onToggleRotate, onResetCamera }: PanoramaToolbarProps) {
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
}

export function PanoramaViewer({ filePath, extension }: PanoramaViewerProps) {
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
        onToggleRotate={() => setAutoRotate((v) => !v)}
        onResetCamera={handleResetCamera}
      />

      <ThreeErrorBoundary
        key={filePath}
        fallback={
          <div className={styles.fallback}>
            Unable to render 360° panorama
          </div>
        }
      >
        <Canvas
          camera={{ position: [0, 0, 0.1], fov: 75 }}
          gl={{ toneMapping: THREE.ACESFilmicToneMapping, toneMappingExposure: 1.0 }}
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
