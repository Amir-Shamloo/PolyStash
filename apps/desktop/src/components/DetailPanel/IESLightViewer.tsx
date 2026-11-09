import { Canvas, useThree } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei';
import { Suspense, useRef, useState, useCallback, useEffect, useMemo } from 'react';
import * as THREE from 'three';
import clsx from 'clsx';
import { convertFileSrc, invoke } from '@tauri-apps/api/core';
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

const LIGHT_POS = new THREE.Vector3(0, 4.2, -1.8);

export function createPhotometricSurfaceMaterial(
  iesTexture: THREE.Texture,
  baseColorHex: number = 0x525866
): THREE.MeshStandardMaterial {
  const mat = new THREE.MeshStandardMaterial({
    color: baseColorHex,
    roughness: 0.75,
    metalness: 0.08,
  });

  mat.onBeforeCompile = (shader) => {
    shader.uniforms.uIesMap = { value: iesTexture };
    shader.uniforms.uLightPos = { value: LIGHT_POS };
    shader.vertexShader = shader.vertexShader
      .replace(
        '#include <common>',
        '#include <common>\nvarying vec3 vWorldPos;'
      )
      .replace(
        '#include <worldpos_vertex>',
        '#include <worldpos_vertex>\nvWorldPos = (modelMatrix * vec4(transformed, 1.0)).xyz;'
      );
    shader.fragmentShader = shader.fragmentShader
      .replace(
        '#include <common>',
        '#include <common>\nuniform sampler2D uIesMap;\nuniform vec3 uLightPos;\nvarying vec3 vWorldPos;'
      )
      .replace(
        '#include <dithering_fragment>',
        `vec3 toFrag = vWorldPos - uLightPos;
        float dist = length(toFrag);
        vec3 dir = normalize(toFrag);
        float cosTheta = clamp(-dir.y, -1.0, 1.0);
        float theta = acos(cosTheta) / 3.14159265;
        float iesProfile = texture2D(uIesMap, vec2(theta, 0.5)).r;
        float attenuation = 22.0 / (1.0 + 0.18 * dist * dist);
        vec3 iesGlow = vec3(1.0, 0.96, 0.88) * iesProfile * attenuation;
        gl_FragColor.rgb += diffuseColor.rgb * iesGlow;
        #include <dithering_fragment>`
      );
  };
  mat.customProgramCacheKey = () => `ies-photometric-${baseColorHex}`;
  return mat;
}

export function createLightConeMaterial(iesTexture: THREE.Texture): THREE.ShaderMaterial {
  return new THREE.ShaderMaterial({
    transparent: true,
    depthWrite: false,
    side: THREE.DoubleSide,
    blending: THREE.AdditiveBlending,
    uniforms: {
      uIesMap: { value: iesTexture },
    },
    vertexShader: `
      varying vec2 vUv;
      varying vec3 vPos;
      void main() {
        vUv = uv;
        vPos = position;
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
      }
    `,
    fragmentShader: `
      uniform sampler2D uIesMap;
      varying vec2 vUv;
      varying vec3 vPos;
      void main() {
        float distFromTop = 1.0 - vUv.y;
        float angleCoord = clamp(distFromTop * 0.55, 0.0, 1.0);
        float iesIntensity = texture2D(uIesMap, vec2(angleCoord, 0.5)).r;
        float verticalFade = pow(1.0 - distFromTop, 1.4);
        float alpha = clamp(iesIntensity * verticalFade * 0.45, 0.0, 0.65);
        gl_FragColor = vec4(1.0, 0.95, 0.84, alpha);
      }
    `,
  });
}

function IESPhotometricScene({ texture }: { texture: THREE.Texture }) {
  const { scene } = useThree();
  const spotLightRef = useRef<THREE.SpotLight>(null);

  const groundMat = useMemo(() => createPhotometricSurfaceMaterial(texture, 0x525866), [texture]);
  const wallMat = useMemo(() => createPhotometricSurfaceMaterial(texture, 0x3e4451), [texture]);
  const coneMat = useMemo(() => createLightConeMaterial(texture), [texture]);

  useEffect(() => {
    const light = spotLightRef.current;
    if (!light) return;
    light.map = texture;
    const iesLight = light as THREE.SpotLight & { iesMap?: THREE.Texture };
    iesLight.iesMap = texture;
    light.target.position.set(0, 0, -1.8);
    scene.add(light.target);
    light.target.updateMatrixWorld();
    return () => {
      scene.remove(light.target);
    };
  }, [texture, scene]);

  return (
    <>
      <ambientLight intensity={0.22} color="#dce4f2" />
      <spotLight
        ref={spotLightRef}
        position={[LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z]}
        angle={Math.PI / 2.2}
        penumbra={0.5}
        intensity={45}
        distance={24}
        castShadow
      />
      <mesh position={[LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z]}>
        <sphereGeometry args={[0.09, 20, 20]} />
        <meshBasicMaterial color="#fffbeb" />
      </mesh>
      <mesh position={[LIGHT_POS.x, LIGHT_POS.y - 2.1, LIGHT_POS.z]}>
        <coneGeometry args={[2.6, 4.2, 48, 1, true]} />
        <primitive object={coneMat} attach="material" />
      </mesh>
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, 0, 0]} receiveShadow>
        <planeGeometry args={[18, 18]} />
        <primitive object={groundMat} attach="material" />
      </mesh>
      <mesh position={[0, 3.5, -3.2]} receiveShadow>
        <planeGeometry args={[18, 9]} />
        <primitive object={wallMat} attach="material" />
      </mesh>
    </>
  );
}

export async function resolveIesSourceUrl(filePath: string, ext: string): Promise<string> {
  if (ext !== 'ldt') {
    return convertFileSrc(filePath);
  }
  try {
    const convertedPath = await invoke<string>('convert_ldt_to_ies', { ldtPath: filePath });
    if (convertedPath) return convertFileSrc(convertedPath);
  } catch {
    try {
      const convertedAlt = await invoke<string>('convert_ldt_to_ies', { path: filePath });
      if (convertedAlt) return convertFileSrc(convertedAlt);
    } catch {
      const siblingIes = filePath.replace(/\.ldt$/i, '.ies');
      return convertFileSrc(siblingIes);
    }
  }
  return convertFileSrc(filePath.replace(/\.ldt$/i, '.ies'));
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

    resolveIesSourceUrl(filePath, cleanExt).then((resolvedUrl) => {
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
      controlsRef.current.target.set(0, 1.6, -1.2);
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
          camera={{ position: [0, 2.8, 5.5], fov: 48 }}
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
              target={[0, 1.6, -1.2]}
              makeDefault
            />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}
