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

interface IESToolbarProps {
  autoRotate: boolean;
  onToggleRotate: () => void;
  onResetCamera: () => void;
}

function IESToolbar({ autoRotate, onToggleRotate, onResetCamera }: IESToolbarProps) {
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
      <ambientLight intensity={0.15} />
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
}

export function IESLightViewer({ filePath, extension }: IESLightViewerProps) {
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
        onToggleRotate={() => setAutoRotate((v) => !v)}
        onResetCamera={handleResetCamera}
      />

      <ThreeErrorBoundary
        key={filePath}
        fallback={
          <div className={styles.fallback}>
            Unable to render photometric light preview
          </div>
        }
      >
        <Canvas
          shadows
          camera={{ position: [0, 3, 5], fov: 50 }}
          gl={{ toneMapping: THREE.ACESFilmicToneMapping }}
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
