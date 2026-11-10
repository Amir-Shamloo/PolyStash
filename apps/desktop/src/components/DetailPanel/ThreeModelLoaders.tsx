import { useMemo, useEffect, useState, type ReactNode } from 'react';
import { useLoader, useThree } from '@react-three/fiber';
import { useFBX } from '@react-three/drei';
import { convertFileSrc } from '@tauri-apps/api/core';
import * as THREE from 'three';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';
import { DRACOLoader } from 'three/examples/jsm/loaders/DRACOLoader.js';
import { KTX2Loader } from 'three/examples/jsm/loaders/KTX2Loader.js';
import { MeshoptDecoder } from 'three/examples/jsm/libs/meshopt_decoder.module.js';
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

export type SbsarTextures = Partial<
  Record<'baseColor' | 'normal' | 'roughness' | 'metallic' | 'height' | 'opacity', string>
>;

export interface ModelLoadProps {
  url: string;
  extension?: string;
  sbsarTextures?: SbsarTextures;
  onLoaded?: (obj: THREE.Object3D) => void;
}

const DIRECT_URL_REGEX = /^(blob:|https?:|asset:)/;

export function ensureConvertedModelUrl(rawUrl: string): string {
  if (!rawUrl) return '';
  return DIRECT_URL_REGEX.test(rawUrl) ? rawUrl : convertFileSrc(rawUrl);
}

let sharedDracoLoader: DRACOLoader | null = null;
let sharedKtx2Loader: KTX2Loader | null = null;

export function createGltfLoader(renderer?: THREE.WebGLRenderer): GLTFLoader {
  if (!sharedDracoLoader) {
    sharedDracoLoader = new DRACOLoader().setDecoderPath('/draco/gltf/');
  }
  if (!sharedKtx2Loader) {
    sharedKtx2Loader = new KTX2Loader().setTranscoderPath('/basis/');
  }
  if (renderer) {
    sharedKtx2Loader.detectSupport(renderer);
  }
  return new GLTFLoader()
    .setDRACOLoader(sharedDracoLoader)
    .setKTX2Loader(sharedKtx2Loader)
    .setMeshoptDecoder(MeshoptDecoder);
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

function normalizePathSegments(fullPath: string): string {
  const isWinDrive = /^[a-zA-Z]:\//.test(fullPath);
  const prefix = isWinDrive ? fullPath.slice(0, 2) : '';
  const rest = isWinDrive ? fullPath.slice(2) : fullPath;
  const stack: string[] = [];
  for (const part of rest.split('/')) {
    if (!part || part === '.') continue;
    if (part === '..') stack.pop();
    else stack.push(part);
  }
  return `${prefix}/${stack.join('/')}`;
}

function extractDirFromConvertedUrl(safeUrl: string): string {
  let decoded = safeUrl;
  try {
    decoded = decodeURIComponent(safeUrl);
  } catch {}
  const pathPart = decoded
    .replace(/^https?:\/\/asset\.localhost\/?/i, '')
    .replace(/^asset:\/\/localhost\/?/i, '')
    .replace(/^asset:\/+/i, '');
  const normalized = /^[a-zA-Z]:[/\\]/.test(pathPart)
    ? pathPart.replace(/\\/g, '/')
    : `/${pathPart.replace(/^\/+/, '')}`;
  const lastSlash = normalized.lastIndexOf('/');
  return lastSlash !== -1 ? normalized.slice(0, lastSlash + 1) : '';
}

interface GltfResourceItem {
  uri?: unknown;
}

function resolveCleanedUri(clean: string, baseDirPath: string): string | null {
  if (/^file:/i.test(clean)) {
    const rawFile = clean.replace(/^file:\/\/\/?/, '');
    const filePath = /^[a-zA-Z]:\//.test(rawFile) ? rawFile : `/${rawFile}`;
    return convertFileSrc(normalizePathSegments(filePath));
  }
  if (/^([a-zA-Z]:\/|\/)/.test(clean)) {
    return convertFileSrc(normalizePathSegments(clean));
  }
  const isDotPath = clean.includes('..') || clean.startsWith('./');
  if (baseDirPath && isDotPath) {
    return convertFileSrc(normalizePathSegments(`${baseDirPath}${clean}`));
  }
  return null;
}

function resolveGltfResourceUri(item: GltfResourceItem, baseDirPath: string): unknown {
  const { uri } = item;
  if (typeof uri !== 'string' || uri === '') return uri;
  if (/^(blob:|data:|https?:|asset:)/i.test(uri)) return uri;

  let decoded = uri;
  try {
    decoded = decodeURIComponent(uri);
  } catch {}
  return resolveCleanedUri(decoded.replace(/\\/g, '/'), baseDirPath) ?? uri;
}

/**
 * Load a .gltf JSON, rewriting buffer/image URIs that desktop exporters write
 * as absolute paths, file:/// URLs, URL-encoded paths, or ../ relative paths.
 */
export async function loadGltfResolvingUris(
  safeUrl: string,
  renderer?: THREE.WebGLRenderer
): Promise<THREE.Object3D> {
  const res = await fetch(safeUrl);
  if (!res.ok) throw new Error(`HTTP ${res.status} loading ${safeUrl}`);
  const json = await res.json();
  const base = safeUrl.replace(/\/[^/]*$/, '/');
  const baseDirPath = extractDirFromConvertedUrl(safeUrl);
  for (const b of json.buffers ?? []) b.uri = resolveGltfResourceUri(b, baseDirPath);
  for (const i of json.images ?? []) i.uri = resolveGltfResourceUri(i, baseDirPath);
  const loader = createGltfLoader(renderer);
  return new Promise((resolve, reject) => {
    loader.parse(
      JSON.stringify(json),
      base,
      (g: { scene: THREE.Object3D }) => resolve(g.scene),
      reject
    );
  });
}

export async function loadGltfOrGlb(
  safeUrl: string,
  extension?: string,
  renderer?: THREE.WebGLRenderer
): Promise<THREE.Object3D> {
  const ext = extension?.toLowerCase();
  const isGlb = ext === 'glb' || safeUrl.toLowerCase().split('?')[0].endsWith('.glb');
  if (isGlb) {
    const gltf = await createGltfLoader(renderer).loadAsync(safeUrl);
    return gltf.scene;
  }
  return loadGltfResolvingUris(safeUrl, renderer);
}

export function GltfModel({ url, extension, onLoaded }: ModelLoadProps) {
  const gl = useThree((s) => s.gl);
  const safeUrl = ensureConvertedModelUrl(url);
  const ext = (extension || 'gltf').toLowerCase();
  const [scene, setScene] = useState<THREE.Object3D | null>(null);

  useEffect(() => {
    let cancelled = false;
    loadGltfOrGlb(safeUrl, ext, gl)
      .then((obj) => {
        if (!cancelled) setScene(obj);
      })
      .catch((err) => {
        console.warn('GLTF/GLB load failed:', err);
        if (!cancelled) setScene(null);
      });
    return () => {
      cancelled = true;
    };
  }, [safeUrl, ext, gl]);

  if (!scene) return null;
  return <ModelObjectWrapper object={scene} extension={ext} onLoaded={onLoaded} />;
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

function isPlainObject(val: unknown): val is Record<string, unknown> {
  if (!val || typeof val !== 'object') return false;
  return !Array.isArray(val);
}

export function extractSbsarTextures(
  metadata?: Record<string, unknown>
): SbsarTextures | undefined {
  const raw = metadata?.sbsar_textures;
  if (!isPlainObject(raw)) return undefined;
  const entries = Object.entries(raw).filter(
    ([, val]) => typeof val === 'string' && val.trim().length > 0
  );
  if (entries.length === 0) return undefined;
  return Object.fromEntries(entries) as SbsarTextures;
}

interface SbsarTexSpec {
  loader: THREE.TextureLoader;
  rawPath?: string;
  srgb: boolean;
}

function loadSbsarTexture({ loader, rawPath, srgb }: SbsarTexSpec): THREE.Texture | null {
  if (!rawPath) return null;
  const tex = loader.load(ensureConvertedModelUrl(rawPath));
  tex.colorSpace = srgb ? THREE.SRGBColorSpace : THREE.LinearSRGBColorSpace;
  return tex;
}

async function loadSbsarTextureAsync({
  loader,
  rawPath,
  srgb,
}: SbsarTexSpec): Promise<THREE.Texture | null> {
  if (!rawPath) return null;
  try {
    const tex = await loader.loadAsync(ensureConvertedModelUrl(rawPath));
    tex.colorSpace = srgb ? THREE.SRGBColorSpace : THREE.LinearSRGBColorSpace;
    return tex;
  } catch {
    return null;
  }
}

interface LoadedSbsarMaps {
  baseColor: THREE.Texture | null;
  normal: THREE.Texture | null;
  roughness: THREE.Texture | null;
  metallic: THREE.Texture | null;
  height: THREE.Texture | null;
  opacity: THREE.Texture | null;
}

function buildSbsarMaterialFromMaps(maps: LoadedSbsarMaps): THREE.MeshStandardMaterial {
  const hasAny = Object.values(maps).some(Boolean);
  if (!hasAny) {
    return new THREE.MeshStandardMaterial({
      color: 0x8899aa,
      roughness: 0.35,
      metalness: 0.15,
    });
  }
  return new THREE.MeshStandardMaterial({
    color: maps.baseColor ? 0xffffff : 0x8899aa,
    map: maps.baseColor,
    normalMap: maps.normal,
    roughnessMap: maps.roughness,
    roughness: maps.roughness ? 1.0 : 0.35,
    metalnessMap: maps.metallic,
    metalness: maps.metallic ? 1.0 : 0.15,
    displacementMap: maps.height,
    displacementScale: maps.height ? 0.05 : 0,
    alphaMap: maps.opacity,
    transparent: Boolean(maps.opacity),
  });
}

export function createSbsarMaterialSync(textures?: SbsarTextures): THREE.MeshStandardMaterial {
  if (!textures) {
    return new THREE.MeshStandardMaterial({
      color: 0x8899aa,
      roughness: 0.35,
      metalness: 0.15,
    });
  }
  const loader = new THREE.TextureLoader();
  return buildSbsarMaterialFromMaps({
    baseColor: loadSbsarTexture({ loader, rawPath: textures.baseColor, srgb: true }),
    normal: loadSbsarTexture({ loader, rawPath: textures.normal, srgb: false }),
    roughness: loadSbsarTexture({ loader, rawPath: textures.roughness, srgb: false }),
    metallic: loadSbsarTexture({ loader, rawPath: textures.metallic, srgb: false }),
    height: loadSbsarTexture({ loader, rawPath: textures.height, srgb: false }),
    opacity: loadSbsarTexture({ loader, rawPath: textures.opacity, srgb: false }),
  });
}

export async function createSbsarShaderBallObject(
  textures?: SbsarTextures
): Promise<THREE.Mesh> {
  const geom = new THREE.SphereGeometry(1, 64, 32);
  if (!textures) {
    const mat = createSbsarMaterialSync();
    return new THREE.Mesh(geom, mat);
  }
  const loader = new THREE.TextureLoader();
  const [baseColor, normal, roughness, metallic, height, opacity] = await Promise.all([
    loadSbsarTextureAsync({ loader, rawPath: textures.baseColor, srgb: true }),
    loadSbsarTextureAsync({ loader, rawPath: textures.normal, srgb: false }),
    loadSbsarTextureAsync({ loader, rawPath: textures.roughness, srgb: false }),
    loadSbsarTextureAsync({ loader, rawPath: textures.metallic, srgb: false }),
    loadSbsarTextureAsync({ loader, rawPath: textures.height, srgb: false }),
    loadSbsarTextureAsync({ loader, rawPath: textures.opacity, srgb: false }),
  ]);
  const mat = buildSbsarMaterialFromMaps({
    baseColor,
    normal,
    roughness,
    metallic,
    height,
    opacity,
  });
  return new THREE.Mesh(geom, mat);
}

export function SbsarShaderBall({
  sbsarTextures,
  onLoaded,
}: {
  sbsarTextures?: SbsarTextures;
  onLoaded?: (obj: THREE.Object3D) => void;
}) {
  const { baseColor, normal, roughness, metallic, height, opacity } = sbsarTextures ?? {};
  const mesh = useMemo(() => {
    const geom = new THREE.SphereGeometry(1, 64, 32);
    const mat = createSbsarMaterialSync({
      baseColor,
      normal,
      roughness,
      metallic,
      height,
      opacity,
    });
    return new THREE.Mesh(geom, mat);
  }, [baseColor, normal, roughness, metallic, height, opacity]);

  useEffect(() => {
    onLoaded?.(mesh);
  }, [mesh, onLoaded]);

  return <primitive object={mesh} />;
}

type ModelRenderer = (props: ModelLoadProps) => ReactNode;

export const MODEL_RENDERERS: Record<string, ModelRenderer> = {
  sbsar: ({ sbsarTextures, onLoaded }) => (
    <SbsarShaderBall sbsarTextures={sbsarTextures} onLoaded={onLoaded} />
  ),
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
