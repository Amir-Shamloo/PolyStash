import { useMemo, useEffect, useState, type ReactNode } from 'react';
import { useLoader } from '@react-three/fiber';
import { useFBX } from '@react-three/drei';
import { convertFileSrc } from '@tauri-apps/api/core';
import * as THREE from 'three';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
import { ColladaLoader } from 'three/examples/jsm/loaders/ColladaLoader.js';
import { PLYLoader } from 'three/examples/jsm/loaders/PLYLoader.js';
import { USDZLoader } from 'three/examples/jsm/loaders/USDZLoader.js';
import { TDSLoader } from 'three/examples/jsm/loaders/TDSLoader.js';
import { VRMLLoader } from 'three/examples/jsm/loaders/VRMLLoader.js';
import {
  centerObject,
  createDefaultClayMaterial,
  ensureGeometryNormals,
  sanitizeModelMaterialsAndNormals,
} from './ThreeViewerUtils';

export interface ModelLoadProps {
  url: string;
  extension?: string;
  onLoaded?: (obj: THREE.Object3D) => void;
}

const DIRECT_URL_REGEX = /^(blob:|https?:|asset:)/;

export function ensureConvertedModelUrl(rawUrl: string): string {
  if (!rawUrl) return '';
  return DIRECT_URL_REGEX.test(rawUrl) ? rawUrl : convertFileSrc(rawUrl);
}

function ModelObjectWrapper({
  object,
  extension,
  onLoaded,
}: {
  object: THREE.Object3D;
  extension?: string;
  onLoaded?: (obj: THREE.Object3D) => void;
}) {
  const preparedObject = useMemo(() => {
    const root = new THREE.Group();
    const cloned = object.clone(true);
    cloned.updateMatrixWorld(true);
    root.add(cloned);
    root.updateMatrixWorld(true);
    sanitizeModelMaterialsAndNormals(root, extension);
    centerObject(root);
    return root;
  }, [object, extension]);

  useEffect(() => {
    onLoaded?.(preparedObject);
  }, [preparedObject, onLoaded]);

  return <primitive object={preparedObject} />;
}

function BufferGeometryModel({
  geometry,
  onLoaded,
}: {
  geometry: THREE.BufferGeometry;
  onLoaded?: (obj: THREE.Object3D) => void;
}) {
  const mesh = useMemo(() => {
    const geom = geometry.clone();
    ensureGeometryNormals(geom);
    geom.center();
    const material = createDefaultClayMaterial();
    const m = new THREE.Mesh(geom, material);
    m.frustumCulled = false;
    return m;
  }, [geometry]);

  useEffect(() => {
    onLoaded?.(mesh);
  }, [mesh, onLoaded]);

  return <primitive object={mesh} />;
}

function resolveGltfResourceUri(uri: unknown): unknown {
  if (typeof uri !== 'string' || uri === '') return uri;
  if (/^(blob:|data:|https?:|asset:)/i.test(uri)) return uri;
  if (/^file:/i.test(uri)) return convertFileSrc(uri.replace(/^file:\/\//, ''));
  if (/^[a-zA-Z]:[/\\]/.test(uri)) return convertFileSrc(uri.replace(/\\/g, '/'));
  return uri;
}

/**
 * Load a .gltf JSON, rewriting buffer/image URIs that desktop exporters write
 * as absolute Windows paths or file:/// URLs (both un-fetchable in the webview).
 * Relative URIs resolve against the .gltf's own directory via parse()'s path.
 * shortcut: no DRACO/KTX2 support in this parse path, add loaders if such files appear.
 */
export async function loadGltfResolvingUris(safeUrl: string): Promise<THREE.Group> {
  const res = await fetch(safeUrl);
  if (!res.ok) throw new Error(`HTTP ${res.status} loading ${safeUrl}`);
  const json = await res.json();
  const base = safeUrl.replace(/\/[^/]*$/, '/');
  for (const b of json.buffers ?? []) b.uri = resolveGltfResourceUri(b.uri);
  for (const i of json.images ?? []) i.uri = resolveGltfResourceUri(i.uri);
  return new Promise((resolve, reject) => {
    new GLTFLoader().parse(
      JSON.stringify(json),
      base,
      (g: { scene: THREE.Object3D }) => resolve(g.scene),
      reject
    );
  });
}

export function GltfModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const [scene, setScene] = useState<THREE.Object3D | null>(null);

  useEffect(() => {
    let cancelled = false;
    loadGltfResolvingUris(safeUrl)
      .then((obj) => {
        if (!cancelled) setScene(obj);
      })
      .catch((err) => {
        console.warn('GLTF load failed:', err);
        if (!cancelled) setScene(null);
      });
    return () => {
      cancelled = true;
    };
  }, [safeUrl]);

  if (!scene) return null;
  return <ModelObjectWrapper object={scene} extension="gltf" onLoaded={onLoaded} />;
}

export function FbxModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const fbx = useFBX(safeUrl);
  return <ModelObjectWrapper object={fbx} extension="fbx" onLoaded={onLoaded} />;
}

export function ObjModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const obj = useLoader(OBJLoader, safeUrl);
  return <ModelObjectWrapper object={obj} extension="obj" onLoaded={onLoaded} />;
}

export function StlModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const geometry = useLoader(STLLoader, safeUrl);
  return <BufferGeometryModel geometry={geometry} onLoaded={onLoaded} />;
}

export function ColladaModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const collada = useLoader(ColladaLoader, safeUrl);
  return <ModelObjectWrapper object={collada.scene} extension="dae" onLoaded={onLoaded} />;
}

export function PlyModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const geometry = useLoader(PLYLoader, safeUrl);
  return <BufferGeometryModel geometry={geometry} onLoaded={onLoaded} />;
}

export function UsdzModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const group = useLoader(USDZLoader, safeUrl);
  return <ModelObjectWrapper object={group} extension="usdz" onLoaded={onLoaded} />;
}

export function TdsModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const group = useLoader(TDSLoader, safeUrl);
  return <ModelObjectWrapper object={group} extension="3ds" onLoaded={onLoaded} />;
}

export function VrmlModel({ url, onLoaded }: ModelLoadProps) {
  const safeUrl = ensureConvertedModelUrl(url);
  const vrml = useLoader(VRMLLoader, safeUrl);
  return <ModelObjectWrapper object={vrml} extension="wrl" onLoaded={onLoaded} />;
}

export function SbsarShaderBall({ onLoaded }: { onLoaded?: (obj: THREE.Object3D) => void }) {
  const mesh = useMemo(() => {
    const geom = new THREE.SphereGeometry(1, 64, 32);
    const mat = new THREE.MeshStandardMaterial({
      color: 0x8899aa,
      roughness: 0.35,
      metalness: 0.15,
    });
    return new THREE.Mesh(geom, mat);
  }, []);

  useEffect(() => {
    onLoaded?.(mesh);
  }, [mesh, onLoaded]);

  return <primitive object={mesh} />;
}

type ModelRenderer = (props: ModelLoadProps) => ReactNode;

export const MODEL_RENDERERS: Record<string, ModelRenderer> = {
  sbsar: ({ onLoaded }) => <SbsarShaderBall onLoaded={onLoaded} />,
  '3ds': (props) => <TdsModel {...props} />,
  fbx: (props) => <FbxModel {...props} />,
  obj: (props) => <ObjModel {...props} />,
  stl: (props) => <StlModel {...props} />,
  dae: (props) => <ColladaModel {...props} />,
  ply: (props) => <PlyModel {...props} />,
  usdz: (props) => <UsdzModel {...props} />,
  wrl: (props) => <VrmlModel {...props} />,
  gltf: (props) => <GltfModel {...props} />,
  glb: (props) => <GltfModel {...props} />,
};
