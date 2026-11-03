import { Canvas, useLoader } from '@react-three/fiber';
import { useGLTF, useFBX, Stage, PresentationControls } from '@react-three/drei';
import { Suspense, Component, useMemo, type ReactNode } from 'react';
import { convertFileSrc } from '@tauri-apps/api/core';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
import { ColladaLoader } from 'three/examples/jsm/loaders/ColladaLoader.js';

interface ErrorBoundaryProps {
  fallback: ReactNode;
  children: ReactNode;
}

interface ErrorBoundaryState {
  hasError: boolean;
}

class ThreeErrorBoundary extends Component<ErrorBoundaryProps, ErrorBoundaryState> {
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

export function getExtension(path: string): string {
  const clean = path.split('?')[0].split('#')[0];
  const lastDot = clean.lastIndexOf('.');
  return lastDot !== -1 ? clean.slice(lastDot + 1).toLowerCase() : '';
}

export const MESH_EXTENSIONS = new Set(['gltf', 'glb', 'fbx', 'obj', 'stl', 'dae']);

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

function Model({ url, extension }: { url: string; extension?: string }) {
  const fileUrl = convertFileSrc(url);
  const ext = (extension || getExtension(url)).toLowerCase();

  switch (ext) {
    case 'fbx':
      return <FbxModel url={fileUrl} />;
    case 'obj':
      return <ObjModel url={fileUrl} />;
    case 'stl':
      return <StlModel url={fileUrl} />;
    case 'dae':
      return <ColladaModel url={fileUrl} />;
    case 'gltf':
    case 'glb':
    default:
      return <GltfModel url={fileUrl} />;
  }
}

export interface ThreeViewerProps {
  modelPath: string;
  extension?: string;
}

export function ThreeViewer({ modelPath, extension }: ThreeViewerProps) {
  return (
    <ThreeErrorBoundary
      key={modelPath}
      fallback={
        <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'center', height: '100%', color: 'var(--color-text-muted)', fontSize: 13 }}>
          Unable to render 3D preview
        </div>
      }
    >
      <Canvas shadows dpr={[1, 2]} camera={{ fov: 50 }}>
        <Suspense fallback={null}>
          <PresentationControls speed={1.5} global zoom={0.5} polar={[-0.1, Math.PI / 4]}>
            <Stage environment="city" intensity={0.5}>
              <Model url={modelPath} extension={extension} />
            </Stage>
          </PresentationControls>
        </Suspense>
      </Canvas>
    </ThreeErrorBoundary>
  );
}
