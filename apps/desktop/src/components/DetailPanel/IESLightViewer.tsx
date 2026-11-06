import { Canvas } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei';
import { Suspense, useRef, useState, useCallback, useEffect } from 'react';
import * as THREE from 'three';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import { IESLoader } from 'three/examples/jsm/loaders/IESLoader.js';
import { IconSun, IconRefresh, IconCameraReset } from '../Icons';
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

interface IESToolbarProps {
  autoRotate: boolean;
  isFullscreen?: boolean;
  onToggleRotate: () => void;
  onResetCamera: () => void;
  onToggleFullscreen?: () => void;
}

function IESToolbar({
  autoRotate,
  isFullscreen = false,
  onToggleRotate,
  onResetCamera,
  onToggleFullscreen,
}: IESToolbarProps) {
  return (
    <div className={styles.toolbar} role="toolbar" aria-label="IES Light Viewer Controls">
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

function IESPhotometricScene({ texture }: { texture: THREE.Texture }) {
  const spotLightRef = useRef<THREE.SpotLight>(null);

  useEffect(() => {
    if (spotLightRef.current) {
      const light = spotLightRef.current;
      light.map = texture;
      const iesLight = light as THREE.SpotLight & { iesMap?: THREE.Texture };
      iesLight.iesMap = texture;
      light.target.position.set(0, 0, 0);
      light.target.updateMatrixWorld();
    }
  }, [texture]);

  return (
    <>
      <ambientLight intensity={0.25} color="#ffffff" />
      <spotLight
        ref={spotLightRef}
        position={[0, 4, 1.5]}
        angle={Math.PI / 3}
        penumbra={0.4}
        intensity={80}
        distance={20}
        castShadow
      />
      <mesh position={[0, 4, 1.5]}>
        <sphereGeometry args={[0.08, 16, 16]} />
        <meshBasicMaterial color="#ffffff" />
      </mesh>
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, 0, 0]} receiveShadow>
        <planeGeometry args={[12, 12]} />
        <meshStandardMaterial color="#2a2e37" roughness={0.7} metalness={0.1} />
      </mesh>
      <mesh position={[0, 3, -3]} receiveShadow>
        <planeGeometry args={[12, 8]} />
        <meshStandardMaterial color="#20242c" roughness={0.8} metalness={0.1} />
      </mesh>
    </>
  );
}

export interface IESLightViewerProps {
  filePath: string;
  extension?: string;
  isFullscreen?: boolean;
  onToggleFullscreen?: () => void;
}

export function IESLightViewer({
  filePath,
  extension,
  isFullscreen,
  onToggleFullscreen,
}: IESLightViewerProps) {
  const [autoRotate, setAutoRotate] = useState(false);
  const [texture, setTexture] = useState<THREE.Texture | null>(null);
  const [errorMsg, setErrorMsg] = useState<string | null>(null);
  const controlsRef = useRef<OrbitHandle | null>(null);

  const cleanExt = (extension || filePath.split('.').pop() || '').toLowerCase();
  const fileUrl = convertFileSrc(filePath);

  useEffect(() => {
    let isCancelled = false;
    setTexture(null);
    setErrorMsg(null);

    const loader = new IESLoader();
    loader.load(
      fileUrl,
      (tex) => {
        if (!isCancelled) {
          setTexture(tex);
        }
      },
      undefined,
      () => {
        if (!isCancelled) {
          const msg = cleanExt === 'ldt'
            ? 'Unable to parse LDT photometric data'
            : 'Unable to parse IES photometric data';
          setErrorMsg(msg);
        }
      }
    );

    return () => {
      isCancelled = true;
    };
  }, [fileUrl, cleanExt]);

  const handleResetCamera = useCallback(() => {
    if (controlsRef.current) {
      controlsRef.current.target.set(0, 1.5, 0);
      controlsRef.current.reset();
      controlsRef.current.update();
    }
  }, []);

  if (errorMsg) {
    return (
      <div className={styles.fallback} style={{ flexDirection: 'column', gap: 8 }}>
        <IconSun size={36} style={{ color: 'hsl(45 95% 50%)' }} />
        <span style={{ fontSize: '12px' }}>{errorMsg}</span>
        <span style={{ fontSize: '11px', color: 'var(--color-text-muted)' }}>
          .{cleanExt.toUpperCase()}
        </span>
      </div>
    );
  }

  return (
    <div className={styles.viewerContainer}>
      <IESToolbar
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
          shadows
          camera={{ position: [0, 3, 5], fov: 50 }}
          gl={{
            toneMapping: THREE.ACESFilmicToneMapping,
            toneMappingExposure: 1.0,
            outputColorSpace: THREE.SRGBColorSpace,
            antialias: true,
          }}
        >
          <Suspense fallback={null}>
            {texture && <IESPhotometricScene texture={texture} />}
            <OrbitControls
              ref={controlsRef as never}
              enableDamping
              autoRotate={autoRotate}
              autoRotateSpeed={0.5}
              target={[0, 1.5, 0]}
              makeDefault
            />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}
