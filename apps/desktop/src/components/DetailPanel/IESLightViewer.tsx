import { Canvas, useFrame, useThree } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei';
import { Suspense, useRef, useState, useCallback, useEffect } from 'react';
import * as THREE from 'three';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import { parsePhotometry, serializeIES } from '@ies-tools/core';
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

const LIGHT_POS = new THREE.Vector3(6.5, 3, 6.5);

function IESPhotometricScene({ texture }: { texture: THREE.Texture }) {
  const { scene } = useThree();
  const spotLightRef = useRef<THREE.SpotLight>(null);

  useEffect(() => {
    const light = spotLightRef.current;
    if (!light) return;
    light.map = texture;
    const iesLight = light as THREE.SpotLight & { iesMap?: THREE.Texture };
    iesLight.iesMap = texture;
    light.target.position.set(0, 0, 0);
    scene.add(light.target);
    light.target.updateMatrixWorld();
    return () => {
      scene.remove(light.target);
    };
  }, [texture, scene]);

  useFrame(({ clock }) => {
    const light = spotLightRef.current;
    if (!light) return;
    const time = clock.getElapsedTime();
    const t = (Math.sin(time * (Math.PI / 2)) + 1) / 2;
    light.target.position.x = THREE.MathUtils.lerp(LIGHT_POS.x, 0, t);
    light.target.position.z = THREE.MathUtils.lerp(LIGHT_POS.z, 0, t);
    light.target.updateMatrixWorld();
  });

  return (
    <>
      <ambientLight intensity={0.18} color="#ffffff" />
      <spotLight
        ref={spotLightRef}
        color="#FFFFFF"
        intensity={500}
        position={[LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z]}
        angle={Math.PI / 8}
        penumbra={0.7}
        distance={20}
        castShadow
      />
      <mesh rotation={[-Math.PI * 0.5, 0, 0]} receiveShadow>
        <planeGeometry args={[200, 200]} />
        <meshPhongMaterial color={0x999999} />
      </mesh>
      <mesh position={[0, 1, 0]} castShadow receiveShadow>
        <boxGeometry args={[2, 2, 2]} />
        <meshPhongMaterial color={0x999999} />
      </mesh>
    </>
  );
}

export async function resolveIesSourceUrl(filePath: string, ext: string): Promise<string> {
  if (ext !== 'ldt') {
    return convertFileSrc(filePath);
  }
  const text = await fetch(convertFileSrc(filePath)).then((r) => r.text());
  const doc = parsePhotometry(text, 'ldt');
  const { value } = serializeIES(doc);
  return URL.createObjectURL(new Blob([value], { type: 'text/plain' }));
}

function useIesTexture(filePath: string, cleanExt: string) {
  const [texture, setTexture] = useState<THREE.Texture | null>(null);
  const [errorMsg, setErrorMsg] = useState<string | null>(null);

  useEffect(() => {
    let isCancelled = false;
    setTexture(null);
    setErrorMsg(null);

    const failMsg = cleanExt === 'ldt'
      ? 'Unable to parse LDT photometric data'
      : 'Unable to parse IES photometric data';

    resolveIesSourceUrl(filePath, cleanExt)
      .then((resolvedUrl) => {
        if (isCancelled) return;
        new IESLoader().load(
          resolvedUrl,
          (tex) => {
            if (!isCancelled) setTexture(tex);
          },
          undefined,
          () => {
            if (!isCancelled) setErrorMsg(failMsg);
          }
        );
      })
      .catch(() => {
        if (!isCancelled) setErrorMsg(failMsg);
      });

    return () => {
      isCancelled = true;
    };
  }, [filePath, cleanExt]);

  return { texture, errorMsg };
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
  const controlsRef = useRef<OrbitHandle | null>(null);
  const cleanExt = (extension || filePath.split('.').pop() || '').toLowerCase();
  const { texture, errorMsg } = useIesTexture(filePath, cleanExt);

  const handleResetCamera = useCallback(() => {
    if (controlsRef.current) {
      controlsRef.current.target.set(0, 1, 0);
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

      <div className={styles.infoPanel} role="region" aria-label="Photometric Profile Info">
        <div className={styles.infoRow}>
          <span className={styles.infoLabel}>Profile</span>
          <span className={styles.infoValue}>{cleanExt.toUpperCase()} Photometric</span>
        </div>
        <div className={styles.infoRow}>
          <span className={styles.infoLabel}>Distribution</span>
          <span className={styles.infoValue}>IES Candela Curve</span>
        </div>
      </div>

      <ThreeErrorBoundary
        key={filePath}
        extension={cleanExt}
      >
        <Canvas
          shadows
          camera={{ position: [16, 4, 1], fov: 35, near: 0.1, far: 100 }}
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
              enablePan={false}
              minDistance={2}
              maxDistance={50}
              autoRotate={autoRotate}
              autoRotateSpeed={0.5}
              target={[0, 1, 0]}
              makeDefault
            />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}
