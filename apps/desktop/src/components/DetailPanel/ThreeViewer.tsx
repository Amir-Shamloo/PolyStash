import { Canvas, useLoader, useThree } from '@react-three/fiber';
import { useGLTF, useFBX, Stage, Environment, ContactShadows, OrbitControls } from '@react-three/drei';
import { Suspense, Component, useMemo, useRef, useState, useCallback, useEffect, type ReactNode } from 'react';
import * as THREE from 'three';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
import { ColladaLoader } from 'three/examples/jsm/loaders/ColladaLoader.js';
import { PLYLoader } from 'three/examples/jsm/loaders/PLYLoader.js';
import { USDZLoader } from 'three/examples/jsm/loaders/USDZLoader.js';
import { VRMLLoader } from 'three/examples/jsm/loaders/VRMLLoader.js';
import { IconSun, IconGrid, IconRefresh, IconCameraReset } from '../Icons';
import styles from './ThreeViewer.module.css';

interface ErrorBoundaryProps {
  fallback: ReactNode;
  children: ReactNode;
}

interface ErrorBoundaryState {
  hasError: boolean;
}

export class ThreeErrorBoundary extends Component<ErrorBoundaryProps, ErrorBoundaryState> {
  constructor(props: ErrorBoundaryProps) {
    super(props);
    this.state = { hasError: false };
  }

  static getDerivedStateFromError() {
    return { hasError: true };
  }

  componentDidCatch(error: unknown, errorInfo: unknown) {
    console.error('ThreeViewer render error:', error, errorInfo);
  }

  render() {
    if (this.state.hasError) {
      return this.props.fallback;
    }
    return this.props.children;
  }
}

export function useOBJ(url: string) {
  return useLoader(OBJLoader, url);
}

export function useSTL(url: string) {
  return useLoader(STLLoader, url);
}

export function useCollada(url: string) {
  return useLoader(ColladaLoader, url);
}

export function usePLY(url: string) {
  return useLoader(PLYLoader, url);
}

export function useUSDZ(url: string) {
  return useLoader(USDZLoader, url);
}

export function useVRML(url: string) {
  return useLoader(VRMLLoader, url);
}

function GltfModel({ url }: { url: string }) {
  const { scene } = useGLTF(url);
  return <primitive object={scene} />;
}

function FbxModel({ url }: { url: string }) {
  const fbx = useFBX(url);
  return <primitive object={fbx} />;
}

function ObjModel({ url }: { url: string }) {
  const obj = useOBJ(url);
  return <primitive object={obj} />;
}

function StlModel({ url }: { url: string }) {
  const geometry = useSTL(url);
  useMemo(() => {
    if (geometry && !geometry.attributes.normal) {
      geometry.computeVertexNormals();
    }
  }, [geometry]);

  return (
    <mesh geometry={geometry}>
      <meshStandardMaterial color="#b0b8c4" roughness={0.4} metalness={0.2} />
    </mesh>
  );
}

function ColladaModel({ url }: { url: string }) {
  const collada = useCollada(url);
  return <primitive object={collada.scene} />;
}

function PlyModel({ url }: { url: string }) {
  const geometry = usePLY(url);
  useMemo(() => {
    if (geometry && !geometry.attributes.normal) {
      geometry.computeVertexNormals();
    }
  }, [geometry]);

  return (
    <mesh geometry={geometry}>
      <meshStandardMaterial color="#b0b8c4" roughness={0.4} metalness={0.2} />
    </mesh>
  );
}

function UsdzModel({ url }: { url: string }) {
  const group = useUSDZ(url);
  return <primitive object={group} />;
}

function VrmlModel({ url }: { url: string }) {
  const vrml = useVRML(url);
  return <primitive object={vrml} />;
}

function SbsarShaderBall() {
  return (
    <mesh castShadow receiveShadow>
      <sphereGeometry args={[1, 64, 32]} />
      <meshStandardMaterial color="#8899aa" roughness={0.35} metalness={0.15} />
    </mesh>
  );
}

export function getExtension(path: string): string {
  const clean = path.split('?')[0].split('#')[0];
  const lastDot = clean.lastIndexOf('.');
  return lastDot !== -1 ? clean.slice(lastDot + 1).toLowerCase() : '';
}

export const MESH_EXTENSIONS = new Set([
  'gltf', 'glb', 'fbx', 'obj', 'stl', 'dae',
  'ply', 'usdz', 'x3d', 'wrl'
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

type ModelRenderer = (fileUrl: string) => ReactNode;

const MODEL_RENDERERS: Record<string, ModelRenderer> = {
  sbsar: () => <SbsarShaderBall />,
  fbx: (url) => <FbxModel url={url} />,
  obj: (url) => <ObjModel url={url} />,
  stl: (url) => <StlModel url={url} />,
  dae: (url) => <ColladaModel url={url} />,
  ply: (url) => <PlyModel url={url} />,
  usdz: (url) => <UsdzModel url={url} />,
  wrl: (url) => <VrmlModel url={url} />,
  x3d: (url) => <VrmlModel url={url} />,
  gltf: (url) => <GltfModel url={url} />,
  glb: (url) => <GltfModel url={url} />,
};

function Model({ url, extension }: { url: string; extension?: string }) {
  const fileUrl = convertFileSrc(url);
  const ext = (extension || getExtension(url)).toLowerCase();
  const render = MODEL_RENDERERS[ext] ?? MODEL_RENDERERS.gltf;
  return <>{render(fileUrl)}</>;
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

const HDRI_PRESETS = ['city', 'studio', 'sunset', 'warehouse', 'apartment', 'forest'] as const;
type HdriPreset = (typeof HDRI_PRESETS)[number];

interface OrbitHandle {
  target: THREE.Vector3;
  reset: () => void;
  update: () => void;
}

interface ToolbarProps {
  hdriEnabled: boolean;
  hdriPreset: HdriPreset;
  wireframe: boolean;
  autoRotate: boolean;
  onToggleHdri: () => void;
  onChangePreset: (p: HdriPreset) => void;
  onToggleWireframe: () => void;
  onToggleRotate: () => void;
  onResetCamera: () => void;
}

function ViewerToolbar({
  hdriEnabled,
  hdriPreset,
  wireframe,
  autoRotate,
  onToggleHdri,
  onChangePreset,
  onToggleWireframe,
  onToggleRotate,
  onResetCamera,
}: ToolbarProps) {
  return (
    <div className={styles.toolbar} role="toolbar" aria-label="3D Viewer Controls">
      {/* Control 1: HDRI on/off + preset switch */}
      <div className={styles.toolbarGroup} role="group" aria-label="Environment lighting">
        <button
          type="button"
          className={clsx(styles.toolbarBtn, hdriEnabled && styles.toolbarBtnActive)}
          onClick={onToggleHdri}
          aria-label={hdriEnabled ? 'Disable HDRI environment' : 'Enable HDRI environment'}
          aria-pressed={hdriEnabled}
          title={hdriEnabled ? 'HDRI Environment: ON' : 'HDRI Environment: OFF'}
        >
          <IconSun size={13} />
        </button>
        {hdriEnabled && (
          <select
            className={styles.toolbarSelect}
            value={hdriPreset}
            onChange={(e) => onChangePreset(e.target.value as HdriPreset)}
            aria-label="HDRI environment preset"
            title="HDRI preset"
          >
            {HDRI_PRESETS.map((p) => (
              <option key={p} value={p}>
                {p.charAt(0).toUpperCase() + p.slice(1)}
              </option>
            ))}
          </select>
        )}
      </div>

      {/* Control 2: Wireframe toggle */}
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

      {/* Control 3: Auto-rotate toggle */}
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

      {/* Control 4: Reset camera */}
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

export interface ThreeViewerProps {
  modelPath: string;
  extension?: string;
}

export function ThreeViewer({ modelPath, extension }: ThreeViewerProps) {
  const [hdriEnabled, setHdriEnabled] = useState(true);
  const [hdriPreset, setHdriPreset] = useState<HdriPreset>('city');
  const [wireframe, setWireframe] = useState(false);
  const [autoRotate, setAutoRotate] = useState(false);
  const controlsRef = useRef<OrbitHandle | null>(null);

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
        hdriEnabled={hdriEnabled}
        hdriPreset={hdriPreset}
        wireframe={wireframe}
        autoRotate={autoRotate}
        onToggleHdri={() => setHdriEnabled((v) => !v)}
        onChangePreset={setHdriPreset}
        onToggleWireframe={() => setWireframe((v) => !v)}
        onToggleRotate={() => setAutoRotate((v) => !v)}
        onResetCamera={handleResetCamera}
      />

      <ThreeErrorBoundary
        key={modelPath}
        fallback={
          <div className={styles.fallback}>
            Unable to render 3D preview
          </div>
        }
      >
        <Canvas shadows dpr={[1, 2]} camera={{ fov: 50 }}>
          <Suspense fallback={null}>
            <Stage adjustCamera intensity={0.5} shadows={false} environment={null}>
              <Model url={modelPath} extension={extension} />
            </Stage>
            {hdriEnabled && <Environment preset={hdriPreset} />}
            <ContactShadows opacity={0.6} scale={10} blur={1.5} far={10} resolution={256} color="#000000" />
            <OrbitControls ref={controlsRef as any} enableDamping autoRotate={autoRotate} makeDefault />
            <WireframeManager wireframe={wireframe} />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}
