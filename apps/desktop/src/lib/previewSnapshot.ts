import * as THREE from 'three';
import { convertFileSrc } from '@tauri-apps/api/core';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';
import { FBXLoader } from 'three/examples/jsm/loaders/FBXLoader.js';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
import { ColladaLoader } from 'three/examples/jsm/loaders/ColladaLoader.js';
import { PLYLoader } from 'three/examples/jsm/loaders/PLYLoader.js';
import { USDZLoader } from 'three/examples/jsm/loaders/USDZLoader.js';
import { TDSLoader } from 'three/examples/jsm/loaders/TDSLoader.js';
import { VRMLLoader } from 'three/examples/jsm/loaders/VRMLLoader.js';
import { IESLoader } from 'three/examples/jsm/loaders/IESLoader.js';
import { useState, useEffect, useRef } from 'react';
import type { Asset } from '../types';
import { loadGltfResolvingUris } from '../components/DetailPanel/ThreeModelLoaders';
import {
  centerObject,
  createDefaultClayMaterial,
  ensureGeometryNormals,
  frameCameraAuto,
  isLikelyZUp,
  sanitizeModelMaterialsAndNormals,
} from '../components/DetailPanel/ThreeViewerUtils';
import {
  createLightConeMaterial,
  createPhotometricSurfaceMaterial,
  resolveIesSourceUrl,
} from '../components/DetailPanel/IESLightViewer';

const snapshotUrlCache = new Map<string, string>();
const failedSnapshotAssets = new Set<string>();

export const SNAPSHOT_ELIGIBLE_EXTENSIONS = new Set([
  'sbsar', '3ds', 'obj', 'fbx', 'stl', 'ply', 'dae',
  'usdz', 'gltf', 'glb', 'wrl', 'ies', 'ldt',
  'c4d', 'max', 'rvt', 'rfa', 'skp', '3dm',
]);

const NATIVE_LOADER_EXTENSIONS = new Set([
  'sbsar', '3ds', 'obj', 'fbx', 'stl', 'ply', 'dae',
  'usdz', 'gltf', 'glb', 'wrl', 'ies', 'ldt',
]);

export interface SnapshotOptions {
  flipYZ?: boolean;
  resolution?: number;
}

interface SnapshotJobSpec {
  assetId: string;
  targetPath: string;
  extension: string;
}

function createPbrShaderBall(): THREE.Object3D {
  const geometry = new THREE.SphereGeometry(1, 64, 32);
  const material = new THREE.MeshStandardMaterial({
    color: 0x8899aa,
    roughness: 0.35,
    metalness: 0.15,
  });
  return new THREE.Mesh(geometry, material);
}

async function createIesSnapshotGroup(job: { rawPath: string; ext: string }): Promise<THREE.Object3D> {
  const resolvedUrl = await resolveIesSourceUrl(job.rawPath, job.ext);
  const texture = await new IESLoader().loadAsync(resolvedUrl);
  const group = new THREE.Group();

  const ground = new THREE.Mesh(
    new THREE.PlaneGeometry(14, 14),
    createPhotometricSurfaceMaterial(texture, 0x525866)
  );
  ground.rotation.x = -Math.PI / 2;
  group.add(ground);

  const wall = new THREE.Mesh(
    new THREE.PlaneGeometry(14, 8),
    createPhotometricSurfaceMaterial(texture, 0x3e4451)
  );
  wall.position.set(0, 3.2, -3.0);
  group.add(wall);

  const cone = new THREE.Mesh(
    new THREE.ConeGeometry(2.4, 4.0, 48, 1, true),
    createLightConeMaterial(texture)
  );
  cone.position.set(0, 2.1, -1.8);
  group.add(cone);

  return group;
}

async function loadMeshGeometry(loader: STLLoader | PLYLoader, url: string): Promise<THREE.Object3D> {
  const geom = await loader.loadAsync(url);
  ensureGeometryNormals(geom);
  return new THREE.Mesh(geom, createDefaultClayMaterial());
}

type LoaderFn = (ctx: { url: string; rawPath: string }) => Promise<THREE.Object3D>;

const LOADER_DISPATCH: Record<string, LoaderFn> = {
  sbsar: async () => createPbrShaderBall(),
  ies: ({ rawPath }) => createIesSnapshotGroup({ rawPath, ext: 'ies' }),
  ldt: ({ rawPath }) => createIesSnapshotGroup({ rawPath, ext: 'ldt' }),
  '3ds': ({ url }) => new TDSLoader().loadAsync(url),
  obj: ({ url }) => new OBJLoader().loadAsync(url),
  fbx: ({ url }) => new FBXLoader().loadAsync(url),
  stl: ({ url }) => loadMeshGeometry(new STLLoader(), url),
  ply: ({ url }) => loadMeshGeometry(new PLYLoader(), url),
  dae: async ({ url }) => (await new ColladaLoader().loadAsync(url)).scene,
  usdz: ({ url }) => new USDZLoader().loadAsync(url),
  wrl: ({ url }) => new VRMLLoader().loadAsync(url),
  gltf: ({ url }) => loadGltfResolvingUris(url),
  glb: async ({ url }) => (await new GLTFLoader().loadAsync(url)).scene,
};

function disposeSceneHierarchy(scene: THREE.Scene, renderer: THREE.WebGLRenderer): void {
  scene.traverse((obj) => {
    if (obj instanceof THREE.Mesh) {
      obj.geometry?.dispose();
      const mats = Array.isArray(obj.material) ? obj.material : [obj.material];
      mats.forEach((m) => m?.dispose());
    }
  });
  renderer.dispose();
  renderer.forceContextLoss();
}

function createSnapshotRenderer(canvas: HTMLCanvasElement, size: { width: number; height: number }): THREE.WebGLRenderer {
  const renderer = new THREE.WebGLRenderer({
    canvas,
    preserveDrawingBuffer: true,
    alpha: true,
    antialias: true,
  });
  renderer.setSize(size.width, size.height);
  renderer.toneMapping = THREE.ACESFilmicToneMapping;
  renderer.outputColorSpace = THREE.SRGBColorSpace;
  return renderer;
}

function setupSnapshotSceneLights(scene: THREE.Scene): void {
  scene.add(new THREE.AmbientLight(0xffffff, 0.7));
  const dirLight = new THREE.DirectionalLight(0xfff6ed, 1.65);
  dirLight.position.set(5, 8, 6);
  scene.add(dirLight);
  const fillLight = new THREE.DirectionalLight(0xe3edfc, 0.75);
  fillLight.position.set(-5, 3, -3);
  scene.add(fillLight);
  const rimLight = new THREE.DirectionalLight(0xffffff, 0.85);
  rimLight.position.set(0, 5, -6);
  scene.add(rimLight);
}

function prepareSnapshotSceneObject(params: {
  modelObj: THREE.Object3D;
  cleanExt: string;
  camera: THREE.PerspectiveCamera;
  explicitFlipYZ?: boolean;
}): void {
  const { modelObj, cleanExt, camera, explicitFlipYZ } = params;
  if (cleanExt === 'ies' || cleanExt === 'ldt') {
    camera.position.set(0, 2.8, 5.2);
    camera.lookAt(0, 1.4, -1.5);
    camera.updateProjectionMatrix();
    return;
  }

  sanitizeModelMaterialsAndNormals(modelObj, cleanExt);
  const initialBox = centerObject(modelObj);
  const shouldFlip = explicitFlipYZ ?? isLikelyZUp(initialBox);
  frameCameraAuto(camera, modelObj, shouldFlip);
}

/**
 * Capture a single-frame offscreen render of a 3D model, photometric light, or shader ball.
 * Never throws — returns null on failure.
 */
export async function captureModelSnapshot(
  url: string,
  extension: string,
  options?: SnapshotOptions
): Promise<Blob | null> {
  let renderer: THREE.WebGLRenderer | null = null;
  const scene = new THREE.Scene();
  const cleanExt = extension.toLowerCase();
  const resolution = options?.resolution ?? (cleanExt === 'sbsar' ? 1080 : 512);

  try {
    const canvas = document.createElement('canvas');
    canvas.width = resolution;
    canvas.height = resolution;

    renderer = createSnapshotRenderer(canvas, { width: resolution, height: resolution });
    setupSnapshotSceneLights(scene);

    const camera = new THREE.PerspectiveCamera(45, 1, 0.1, 1000);
    const fileUrl = /^(http|asset:|blob:)/.test(url) ? url : convertFileSrc(url);
    const loader = LOADER_DISPATCH[cleanExt] ?? LOADER_DISPATCH.gltf;
    const modelObj = await loader({ url: fileUrl, rawPath: url });

    scene.add(modelObj);
    prepareSnapshotSceneObject({
      modelObj,
      cleanExt,
      camera,
      explicitFlipYZ: options?.flipYZ,
    });

    scene.updateMatrixWorld(true);
    renderer.render(scene, camera);

    return await new Promise<Blob | null>((resolve) => {
      canvas.toBlob((b) => resolve(b), 'image/png');
    });
  } catch (err) {
    console.warn(`Snapshot capture failed for ${url} (${extension}):`, err);
    return null;
  } finally {
    if (renderer) {
      disposeSceneHierarchy(scene, renderer);
    }
  }
}

function hasRasterThumbnail(asset: Asset): boolean {
  return Boolean(asset.thumbnailPath && !asset.thumbnailPath.endsWith('.glb'));
}

function hasConvertedGlbModel(asset: Asset): boolean {
  return Boolean(asset.metadata?.preview_model) || Boolean(asset.thumbnailPath?.endsWith('.glb'));
}

export function isEligibleForSnapshot(asset: Asset | null | undefined): boolean {
  if (!asset || hasRasterThumbnail(asset)) return false;
  const ext = asset.extension.toLowerCase();
  if (!SNAPSHOT_ELIGIBLE_EXTENSIONS.has(ext)) return false;
  return NATIVE_LOADER_EXTENSIONS.has(ext) || hasConvertedGlbModel(asset);
}

function buildSnapshotJobSpec(asset: Asset): SnapshotJobSpec {
  const previewModel = asset.metadata?.preview_model as string | undefined;
  if (previewModel) {
    const pExt = previewModel.split('.').pop()?.toLowerCase() || 'glb';
    return { assetId: asset.id, targetPath: previewModel, extension: pExt };
  }
  if (asset.thumbnailPath?.endsWith('.glb')) {
    return { assetId: asset.id, targetPath: asset.thumbnailPath, extension: 'glb' };
  }
  return { assetId: asset.id, targetPath: asset.filePath, extension: asset.extension.toLowerCase() };
}

function canRunSnapshotJob(asset: Asset | null | undefined): asset is Asset {
  if (!asset || !isEligibleForSnapshot(asset)) return false;
  return !snapshotUrlCache.has(asset.id) && !failedSnapshotAssets.has(asset.id);
}

// Browsers cap live WebGL contexts (~16); snapshots run offscreen one after
// another so a wall of cards can never exhaust the limit at once.
const MAX_CONCURRENT_SNAPSHOTS = 2;
let activeSnapshots = 0;
const waitingForSlot: (() => void)[] = [];

function releaseSnapshotSlot(): void {
  activeSnapshots -= 1;
  waitingForSlot.shift()?.();
}

function acquireSnapshotSlot(): Promise<() => void> {
  if (activeSnapshots < MAX_CONCURRENT_SNAPSHOTS) {
    activeSnapshots += 1;
    return Promise.resolve(releaseSnapshotSlot);
  }
  return new Promise((resolve) => {
    waitingForSlot.push(() => {
      activeSnapshots += 1;
      resolve(releaseSnapshotSlot);
    });
  });
}

function executeSnapshotJob(
  job: SnapshotJobSpec,
  callbacks: { onDone: (url: string | null) => void }
): () => void {
  let isMounted = true;
  let released = false;
  let release: (() => void) | null = null;
  const releaseOnce = () => {
    if (!released) {
      released = true;
      release?.();
    }
  };

  acquireSnapshotSlot().then((rel) => {
    if (released) {
      rel();
      return;
    }
    release = rel;
    captureModelSnapshot(job.targetPath, job.extension).then((blob) => {
      releaseOnce();
      if (!isMounted) return;
      if (blob) {
        const url = URL.createObjectURL(blob);
        snapshotUrlCache.set(job.assetId, url);
        callbacks.onDone(url);
      } else {
        failedSnapshotAssets.add(job.assetId);
        callbacks.onDone(null);
      }
    });
  });

  return () => {
    isMounted = false;
    releaseOnce();
  };
}

function useViewportEntry(
  ref: React.RefObject<HTMLElement>,
  enabled: boolean
): boolean {
  const [isVisible, setIsVisible] = useState(false);

  useEffect(() => {
    if (!enabled || isVisible) return;
    const el = ref.current;
    if (!el || typeof IntersectionObserver === 'undefined') {
      setIsVisible(true);
      return;
    }
    const observer = new IntersectionObserver(
      (entries) => {
        if (entries.some((e) => e.isIntersecting)) {
          setIsVisible(true);
          observer.disconnect();
        }
      },
      { rootMargin: '100px' }
    );
    observer.observe(el);
    return () => observer.disconnect();
  }, [ref, enabled, isVisible]);

  return isVisible;
}

export interface LazySnapshotState {
  ref: React.RefObject<HTMLDivElement>;
  snapshotUrl: string | null;
  isGenerating: boolean;
}

function getCachedSnapshot(assetId: string | undefined): string | null {
  if (!assetId) return null;
  return snapshotUrlCache.get(assetId) ?? null;
}

/**
 * Viewport-based lazy 3JS thumbnail hook using IntersectionObserver (rootMargin: "100px").
 * Generates snapshot ONLY when card enters viewport.
 */
export function useLazyModelSnapshot(asset: Asset | null | undefined): LazySnapshotState {
  const ref = useRef<HTMLDivElement>(null);
  const assetId = asset?.id;
  const eligible = isEligibleForSnapshot(asset);

  const [snapshotUrl, setSnapshotUrl] = useState<string | null>(() => getCachedSnapshot(assetId));
  const [isGenerating, setIsGenerating] = useState(false);
  const isVisible = useViewportEntry(ref, eligible && !snapshotUrl);

  useEffect(() => {
    const cached = getCachedSnapshot(assetId);
    if (cached) {
      setSnapshotUrl(cached);
      return;
    }
    if (!isVisible || !canRunSnapshotJob(asset)) return;

    setIsGenerating(true);
    return executeSnapshotJob(buildSnapshotJobSpec(asset), {
      onDone: (url) => {
        setIsGenerating(false);
        if (url) setSnapshotUrl(url);
      },
    });
  }, [assetId, isVisible, asset]);

  return { ref, snapshotUrl, isGenerating };
}

/**
 * Hook to retrieve an in-memory snapshot blob URL for an asset (used in DetailPanel / QuickLook).
 */
export function useModelSnapshot(asset: Asset | null | undefined): string | null {
  const assetId = asset?.id;
  const [snapshotUrl, setSnapshotUrl] = useState<string | null>(() => getCachedSnapshot(assetId));

  useEffect(() => {
    const cached = getCachedSnapshot(assetId);
    if (cached) {
      setSnapshotUrl(cached);
      return;
    }
    if (!canRunSnapshotJob(asset)) return;

    return executeSnapshotJob(buildSnapshotJobSpec(asset), {
      onDone: (url) => {
        if (url) setSnapshotUrl(url);
      },
    });
  }, [assetId, asset]);

  return snapshotUrl;
}
