import { Canvas, useLoader, useThree } from '@react-three/fiber';
import { Stage, Environment, ContactShadows, OrbitControls } from '@react-three/drei';
import { Suspense, Component, useRef, useState, useCallback, useEffect, type ReactNode } from 'react';
import * as THREE from 'three';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import { DDSLoader } from 'three/examples/jsm/loaders/DDSLoader.js';
import { IconGrid, IconRefresh, IconCameraReset, IconInfo } from '../Icons';
import { computeModelStats, type ModelStats } from './ThreeViewerUtils';
import { MODEL_RENDERERS } from './ThreeModelLoaders';
import styles from './ThreeViewer.module.css';

interface ErrorBoundaryProps {
  fallback?: ReactNode;
  children: ReactNode;
  extension?: string;
}

interface ErrorBoundaryState {
  hasError: boolean;
  errorMessage?: string;
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

function resolveFallbackHint(extension?: string, errorMessage?: string): string {
  const ext = extension?.toUpperCase() || '3D';
  if (!errorMessage) {
    return `Unable to render ${ext} preview`;
  }
  if (errorMessage.includes('Crate') || errorMessage.includes('.usdc')) {
    return 'Binary USD (USDC crate) is not supported. Use .USDZ archive.';
  }
  if (errorMessage.includes('zip') || errorMessage.includes('fflate')) {
    return `Invalid archive format for ${ext}. Ensure file is a valid package.`;
  }
  return `Unable to render ${ext} preview`;
}

export class ThreeErrorBoundary extends Component<ErrorBoundaryProps, ErrorBoundaryState> {
  constructor(props: ErrorBoundaryProps) {
    super(props);
    this.state = { hasError: false };
  }

  static getDerivedStateFromError(error: unknown): ErrorBoundaryState {
    const msg = error instanceof Error ? error.message : String(error);
    return { hasError: true, errorMessage: msg };
  }

  componentDidCatch(error: unknown, errorInfo: unknown) {
    console.error('ThreeViewer render error:', error, errorInfo);
  }

  render() {
    if (!this.state.hasError) {
      return this.props.children;
    }
    if (this.props.fallback) {
      return this.props.fallback;
    }
    const hint = resolveFallbackHint(this.props.extension, this.state.errorMessage);
    return (
      <div className={styles.fallback} role="alert">
        <span className={styles.fallbackTitle}>{hint}</span>
        {this.state.errorMessage && (
          <span className={styles.fallbackDetail}>{this.state.errorMessage}</span>
        )}
      </div>
    );
  }
}

export function getExtension(path: string): string {
  const clean = path.split('?')[0].split('#')[0];
  const lastDot = clean.lastIndexOf('.');
  return lastDot !== -1 ? clean.slice(lastDot + 1).toLowerCase() : '';
}

export const MESH_EXTENSIONS = new Set([
  'gltf', 'glb', 'fbx', 'obj', 'stl', 'dae',
  '3ds', 'ply', 'usd', 'usdz', 'wrl',
]);

export function getModelPath(asset: {
  filePath: string;
  extension: string;
  thumbnailPath?: string;
  metadata?: Record<string, unknown>;
}): string | undefined {
  return (
    (asset.metadata?.preview_model as string | undefined) ||
    (MESH_EXTENSIONS.has(asset.extension.toLowerCase())
      ? asset.filePath
      : asset.thumbnailPath?.endsWith('.glb')
      ? asset.thumbnailPath
      : undefined)
  );
}

export function Model({
  url,
  extension,
  onStatsReady,
}: {
  url: string;
  extension?: string;
  onStatsReady?: (stats: ModelStats) => void;
}) {
  const fileUrl = convertFileSrc(url);
  const ext = (extension || getExtension(url)).toLowerCase();
  const render = MODEL_RENDERERS[ext] ?? MODEL_RENDERERS.gltf;

  const handleLoaded = useCallback(
    (obj: THREE.Object3D) => {
      if (onStatsReady) {
        onStatsReady(computeModelStats(obj));
      }
    },
    [onStatsReady]
  );

  return <>{render({ url: fileUrl, onLoaded: handleLoaded })}</>;
}

function WireframeManager({ wireframe }: { wireframe: boolean }) {
  const { scene } = useThree();

  useEffect(() => {
    const apply = () => {
      scene.traverse((obj) => {
        if ((obj as THREE.Mesh).isMesh) {
          const mesh = obj as THREE.Mesh;
          const mats = Array.isArray(mesh.material) ? mesh.material : [mesh.material];
          mats.forEach((mat) => {
            (mat as THREE.Material & { wireframe?: boolean }).wireframe = wireframe;
          });
        }
      });
    };

    apply();
    const timer = setTimeout(apply, 80);
    return () => clearTimeout(timer);
  }, [scene, wireframe]);

  return null;
}

function StudioLighting() {
  return (
    <>
      <ambientLight intensity={0.65} color="#ffffff" />
      <directionalLight
        position={[6, 9, 6]}
        intensity={1.75}
        color="#fff6ed"
        castShadow
        shadow-mapSize={[1024, 1024]}
        shadow-bias={-0.0001}
      />
      <directionalLight
        position={[-6, 4, -3]}
        intensity={0.75}
        color="#e3edfc"
      />
      <directionalLight
        position={[0, 6, -8]}
        intensity={0.9}
        color="#f8fafc"
      />
    </>
  );
}

interface OrbitHandle {
  target: THREE.Vector3;
  reset: () => void;
  update: () => void;
}

interface ToolbarProps {
  wireframe: boolean;
  autoRotate: boolean;
  showInfo: boolean;
  isFullscreen?: boolean;
  onToggleWireframe: () => void;
  onToggleRotate: () => void;
  onToggleInfo: () => void;
  onResetCamera: () => void;
  onToggleFullscreen?: () => void;
}

function ViewerToolbar({
  wireframe,
  autoRotate,
  showInfo,
  isFullscreen = false,
  onToggleWireframe,
  onToggleRotate,
  onToggleInfo,
  onResetCamera,
  onToggleFullscreen,
}: ToolbarProps) {
  return (
    <div className={styles.toolbar} role="toolbar" aria-label="3D Viewer Controls">
      <button
        type="button"
        className={clsx(styles.toolbarBtn, wireframe && styles.toolbarBtnActive)}
        onClick={onToggleWireframe}
        aria-label="Toggle wireframe mode"
        aria-pressed={wireframe}
        title="Toggle wireframe mode"
      >
        <IconGrid size={13} />
      </button>

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
        className={clsx(styles.toolbarBtn, showInfo && styles.toolbarBtnActive)}
        onClick={onToggleInfo}
        aria-label="Toggle model information"
        aria-pressed={showInfo}
        title="Model information"
      >
        <IconInfo size={13} />
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

export interface ThreeViewerProps {
  modelPath: string;
  extension?: string;
  isFullscreen?: boolean;
  onToggleFullscreen?: () => void;
}

export function ThreeViewer({
  modelPath,
  extension,
  isFullscreen,
  onToggleFullscreen,
}: ThreeViewerProps) {
  const [wireframe, setWireframe] = useState(false);
  const [autoRotate, setAutoRotate] = useState(false);
  const [showInfo, setShowInfo] = useState(false);
  const [modelStats, setModelStats] = useState<ModelStats | null>(null);
  const controlsRef = useRef<OrbitHandle | null>(null);

  const cleanExt = (extension || getExtension(modelPath)).toLowerCase();

  const handleResetCamera = useCallback(() => {
    if (controlsRef.current) {
      controlsRef.current.target.set(0, 0, 0);
      controlsRef.current.reset();
      controlsRef.current.update();
    }
  }, []);

  return (
    <div className={styles.viewerContainer}>
      <ViewerToolbar
        wireframe={wireframe}
        autoRotate={autoRotate}
        showInfo={showInfo}
        isFullscreen={isFullscreen}
        onToggleWireframe={() => setWireframe((v) => !v)}
        onToggleRotate={() => setAutoRotate((v) => !v)}
        onToggleInfo={() => setShowInfo((v) => !v)}
        onResetCamera={handleResetCamera}
        onToggleFullscreen={onToggleFullscreen}
      />

      {showInfo && modelStats && (
        <div className={styles.infoPanel} role="region" aria-label="3D Model Statistics">
          <div className={styles.infoRow}>
            <span className={styles.infoLabel}>Meshes</span>
            <span className={styles.infoValue}>{modelStats.meshCount.toLocaleString()}</span>
          </div>
          <div className={styles.infoRow}>
            <span className={styles.infoLabel}>Polys</span>
            <span className={styles.infoValue}>{modelStats.triangles.toLocaleString()}</span>
          </div>
          <div className={styles.infoRow}>
            <span className={styles.infoLabel}>Vertices</span>
            <span className={styles.infoValue}>{modelStats.vertices.toLocaleString()}</span>
          </div>
        </div>
      )}

      <ThreeErrorBoundary
        key={modelPath}
        extension={cleanExt}
      >
        <Canvas
          shadows
          dpr={[1, 2]}
          camera={{ fov: 50, position: [0, 0, 5] }}
          gl={{
            toneMapping: THREE.ACESFilmicToneMapping,
            toneMappingExposure: 1.0,
            outputColorSpace: THREE.SRGBColorSpace,
            antialias: true,
          }}
        >
          <Suspense fallback={null}>
            <StudioLighting />
            <Stage adjustCamera intensity={0.4} shadows={false} environment={null}>
              <Model url={modelPath} extension={cleanExt} onStatsReady={setModelStats} />
            </Stage>
            <Environment preset="city" />
            <ContactShadows opacity={0.6} scale={10} blur={1.5} far={10} resolution={256} color="#000000" />
            <OrbitControls ref={controlsRef as never} enableDamping autoRotate={autoRotate} makeDefault />
            <WireframeManager wireframe={wireframe} />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}

function DdsPlane({ url }: { url: string }) {
  const texture = useLoader(DDSLoader, url);
  return (
    <mesh>
      <planeGeometry args={[2, 2]} />
      <meshBasicMaterial map={texture} toneMapped={false} />
    </mesh>
  );
}

export function DdsViewer({
  filePath,
  isFullscreen,
  onToggleFullscreen,
}: {
  filePath: string;
  isFullscreen?: boolean;
  onToggleFullscreen?: () => void;
}) {
  const fileUrl = convertFileSrc(filePath);
  return (
    <div className={styles.viewerContainer}>
      {onToggleFullscreen && (
        <div className={styles.toolbar} role="toolbar" aria-label="DDS Controls">
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
      <ThreeErrorBoundary
        key={filePath}
        extension="DDS"
      >
        <Canvas
          camera={{ position: [0, 0, 2.5], fov: 45 }}
          gl={{
            toneMapping: THREE.ACESFilmicToneMapping,
            outputColorSpace: THREE.SRGBColorSpace,
            antialias: true,
          }}
        >
          <Suspense fallback={null}>
            <DdsPlane url={fileUrl} />
            <OrbitControls enableRotate={false} enablePan enableZoom makeDefault />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}
