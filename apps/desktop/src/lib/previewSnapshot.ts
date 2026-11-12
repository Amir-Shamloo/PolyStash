import * as THREE from 'three';
import { convertFileSrc } from '@tauri-apps/api/core';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';
import { FBXLoader } from 'three/examples/jsm/loaders/FBXLoader.js';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
import { ColladaLoader } from 'three/examples/jsm/loaders/ColladaLoader.js';
import { PLYLoader } from 'three/examples/jsm/loaders/PLYLoader.js';
import { TDSLoader } from 'three/examples/jsm/loaders/TDSLoader.js';
import { VRMLLoader } from 'three/examples/jsm/loaders/VRMLLoader.js';
import { IESLoader } from 'three/examples/jsm/loaders/IESLoader.js';
import { useState, useEffect, useRef } from 'react';
import type { Asset } from '../types';
import {
  USD_EXTENSIONS,
  convertUsdAsset,
  hasUsdConversionFailed,
  markUsdConversionFailed,
  refreshAssetInStore,
} from './usdConversion';
import {
  createGltfLoader,
  createSbsarShaderBallObject,
  extractSbsarTextures,
  loadGltfResolvingUris,
  type SbsarTextures,
} from '../components/DetailPanel/ThreeModelLoaders';
import {
  createDefaultClayMaterial,
  ensureGeometryNormals,
  frameCameraAuto,
  isZUpFormat,
  sanitizeModelMaterialsAndNormals,
} from '../components/DetailPanel/ThreeViewerUtils';
import {
  resolveIesSourceUrl,
  convertIesTextureToGrayscaleRgba,
  buildIesGoboTexture,
  IES_SPOT_ANGLE,
} from '../components/DetailPanel/IESLightViewer';

const snapshotUrlCache = new Map<string, string>();
const failedSnapshotAssets = new Set<string>();

export const SNAPSHOT_ELIGIBLE_EXTENSIONS = new Set([
  'sbsar', '3ds', 'obj', 'fbx', 'stl', 'ply', 'dae',
  'usd', 'usda', 'usdc', 'usdz', 'gltf', 'glb', 'wrl', 'ies', 'ldt',
  'max', 'rvt', 'rfa', 'skp', '3dm', 'c4d',
]);

const NATIVE_LOADER_EXTENSIONS = new Set([
  'sbsar', '3ds', 'obj', 'fbx', 'stl', 'ply', 'dae',
  'gltf', 'glb', 'wrl', 'ies', 'ldt',
]);

export interface SnapshotOptions {
  flipYZ?: boolean;
  resolution?: number;
  sbsarTextures?: SbsarTextures;
}

interface SnapshotJobSpec {
  assetId: string;
  targetPath: string;
  extension: string;
  sbsarTextures?: SbsarTextures;
  /** USD asset still lacking a converted GLB; converted just before the snapshot. */
  usdAsset?: Asset;
}

async function createIesSnapshotGroup(job: { rawPath: string; ext: string }): Promise<THREE.Object3D> {
  const resolvedUrl = await resolveIesSourceUrl(job.rawPath, job.ext);
  const rawTexture = await new IESLoader().loadAsync(resolvedUrl);
  const texture = convertIesTextureToGrayscaleRgba(rawTexture as THREE.DataTexture);
  const group = new THREE.Group();

  const spotLight = new THREE.SpotLight(0xffffff, 220);
  spotLight.position.set(0, 4.2, 0);
  spotLight.angle = IES_SPOT_ANGLE;
  spotLight.penumbra = 0.3;
  spotLight.distance = 28;
  spotLight.decay = 2;
  spotLight.castShadow = true;
  spotLight.map = buildIesGoboTexture(texture);
  spotLight.target.position.set(0, 0, 0);
  group.add(spotLight);
  group.add(spotLight.target);

  const groundMat = new THREE.MeshStandardMaterial({ color: 0x565b66, roughness: 0.72, metalness: 0.05 });
  const ground = new THREE.Mesh(new THREE.PlaneGeometry(60, 60), groundMat);
  ground.rotation.x = -Math.PI * 0.5;
  ground.receiveShadow = true;
  group.add(ground);

  const wallMat = new THREE.MeshStandardMaterial({ color: 0x484d57, roughness: 0.78, metalness: 0.04 });
  const wall = new THREE.Mesh(new THREE.PlaneGeometry(28, 9), wallMat);
  wall.position.set(0, 4.5, -3.6);
  wall.receiveShadow = true;
  group.add(wall);

  const pedMat = new THREE.MeshStandardMaterial({ color: 0x6e7380, roughness: 0.48, metalness: 0.08 });
  const pedestal = new THREE.Mesh(new THREE.CylinderGeometry(0.42, 0.48, 0.9, 32), pedMat);
  pedestal.position.set(0, 0.45, -0.8);
  pedestal.castShadow = true;
  pedestal.receiveShadow = true;
  group.add(pedestal);

  const emitterGroup = new THREE.Group();
  emitterGroup.position.set(0, 4.2, 0);
  const housing = new THREE.Mesh(
    new THREE.CylinderGeometry(0.22, 0.28, 0.36, 32),
    new THREE.MeshStandardMaterial({ color: 0x1f232b, roughness: 0.4, metalness: 0.65 })
  );
  emitterGroup.add(housing);
  const disc = new THREE.Mesh(
    new THREE.CircleGeometry(0.2, 32),
    new THREE.MeshBasicMaterial({ color: 0xffffff, toneMapped: false })
  );
  disc.rotation.x = Math.PI / 2;
  disc.position.y = -0.181;
  emitterGroup.add(disc);
  group.add(emitterGroup);

  return group;
}

async function loadMeshGeometry(loader: STLLoader | PLYLoader, url: string): Promise<THREE.Object3D> {
  const geom = await loader.loadAsync(url);
  ensureGeometryNormals(geom);
  return new THREE.Mesh(geom, createDefaultClayMaterial());
}

interface LoaderContext {
  url: string;
  rawPath: string;
  renderer: THREE.WebGLRenderer;
  sbsarTextures?: SbsarTextures;
}

type LoaderFn = (ctx: LoaderContext) => Promise<THREE.Object3D>;

const LOADER_DISPATCH: Record<string, LoaderFn> = {
  sbsar: ({ sbsarTextures }) => createSbsarShaderBallObject(sbsarTextures),
  ies: ({ rawPath }) => createIesSnapshotGroup({ rawPath, ext: 'ies' }),
  ldt: ({ rawPath }) => createIesSnapshotGroup({ rawPath, ext: 'ldt' }),
  '3ds': ({ url }) => new TDSLoader().loadAsync(url),
  obj: ({ url }) => new OBJLoader().loadAsync(url),
  fbx: ({ url }) => new FBXLoader().loadAsync(url),
  stl: ({ url }) => loadMeshGeometry(new STLLoader(), url),
  ply: ({ url }) => loadMeshGeometry(new PLYLoader(), url),
  dae: async ({ url }) => (await new ColladaLoader().loadAsync(url)).scene,
  wrl: ({ url }) => new VRMLLoader().loadAsync(url),
  gltf: ({ url, renderer }) => loadGltfResolvingUris(url, renderer),
  glb: async ({ url, renderer }) => (await createGltfLoader(renderer).loadAsync(url)).scene,
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
  renderer.shadowMap.enabled = true;
  renderer.toneMapping = THREE.ACESFilmicToneMapping;
  renderer.outputColorSpace = THREE.SRGBColorSpace;
  return renderer;
}

function setupSnapshotSceneLights(scene: THREE.Scene, cleanExt: string): void {
  if (cleanExt === 'ies' || cleanExt === 'ldt') {
    scene.add(new THREE.AmbientLight(0xffffff, 0.18));
    return;
  }
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
    camera.fov = 40;
    camera.position.set(5.5, 3.2, 6.8);
    camera.lookAt(0, 1.5, 0);
    camera.updateProjectionMatrix();
    return;
  }

  sanitizeModelMaterialsAndNormals(modelObj, cleanExt);
  const shouldFlip = explicitFlipYZ ?? isZUpFormat(cleanExt);
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
    setupSnapshotSceneLights(scene, cleanExt);

    const camera = new THREE.PerspectiveCamera(45, 1, 0.1, 1000);
    const fileUrl = /^(http|asset:|blob:)/.test(url) ? url : convertFileSrc(url);
    const loader = LOADER_DISPATCH[cleanExt] ?? LOADER_DISPATCH.gltf;
    const modelObj = await loader({
      url: fileUrl,
      rawPath: url,
      renderer,
      sbsarTextures: options?.sbsarTextures,
    });

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
  if (!asset) return false;
  const ext = asset.extension.toLowerCase();
  if (ext === 'sbsar') {
    // An archive-embedded preview already is a raster thumbnail; only render the ball without one.
    return Boolean(extractSbsarTextures(asset.metadata)) && !hasRasterThumbnail(asset);
  }
  if (hasRasterThumbnail(asset)) return false;
  if (!SNAPSHOT_ELIGIBLE_EXTENSIONS.has(ext)) return false;
  if (needsUsdConversion(asset)) return !hasUsdConversionFailed(asset.id);
  return NATIVE_LOADER_EXTENSIONS.has(ext) || hasConvertedGlbModel(asset);
}

function needsUsdConversion(asset: Asset): boolean {
  return USD_EXTENSIONS.has(asset.extension.toLowerCase()) && !hasConvertedGlbModel(asset);
}

function buildSnapshotJobSpec(asset: Asset): SnapshotJobSpec {
  const sbsarTextures = extractSbsarTextures(asset.metadata);
  const previewModel = asset.metadata?.preview_model as string | undefined;
  if (previewModel) {
    const pExt = previewModel.split('.').pop()?.toLowerCase() || 'glb';
    return { assetId: asset.id, targetPath: previewModel, extension: pExt, sbsarTextures };
  }
  if (asset.thumbnailPath?.endsWith('.glb')) {
    return { assetId: asset.id, targetPath: asset.thumbnailPath, extension: 'glb', sbsarTextures };
  }
  if (needsUsdConversion(asset)) {
    return { assetId: asset.id, targetPath: '', extension: 'glb', usdAsset: asset };
  }
  return {
    assetId: asset.id,
    targetPath: asset.filePath,
    extension: asset.extension.toLowerCase(),
    sbsarTextures,
  };
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

async function captureJobBlob(job: SnapshotJobSpec): Promise<Blob | null> {
  let targetPath = job.targetPath;
  if (job.usdAsset) {
    try {
      targetPath = await convertUsdAsset(job.usdAsset);
    } catch {
      return null;
    }
  }
  return captureModelSnapshot(targetPath, job.extension, { sbsarTextures: job.sbsarTextures });
}

function handleJobBlob(
  job: SnapshotJobSpec,
  blob: Blob | null,
  isMounted: boolean,
  onDone: (url: string | null) => void
) {
  if (!blob) {
    failedSnapshotAssets.add(job.assetId);
    if (job.usdAsset) markUsdConversionFailed(job.assetId);
    if (isMounted) onDone(null);
    return;
  }

  const url = URL.createObjectURL(blob);
  snapshotUrlCache.set(job.assetId, url);
  if (isMounted) onDone(url);
  if (job.usdAsset) void refreshAssetInStore(job.assetId).catch(() => undefined);
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
    captureJobBlob(job).then((blob) => {
      releaseOnce();
      handleJobBlob(job, blob, isMounted, callbacks.onDone);
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
      { rootMargin: '0px', threshold: 0 }
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
 * Viewport-based lazy 3JS thumbnail hook using IntersectionObserver (rootMargin: "0px").
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
    setSnapshotUrl(cached);
    if (cached) return;
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
    setSnapshotUrl(cached);
    if (cached) return;
    if (!canRunSnapshotJob(asset)) return;

    return executeSnapshotJob(buildSnapshotJobSpec(asset), {
      onDone: (url) => {
        if (url) setSnapshotUrl(url);
      },
    });
  }, [assetId, asset]);

  return snapshotUrl;
}
