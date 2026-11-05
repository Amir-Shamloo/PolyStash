import * as THREE from 'three';
import { convertFileSrc } from '@tauri-apps/api/core';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';
import { FBXLoader } from 'three/examples/jsm/loaders/FBXLoader.js';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
import { ColladaLoader } from 'three/examples/jsm/loaders/ColladaLoader.js';
import { PLYLoader } from 'three/examples/jsm/loaders/PLYLoader.js';
import { USDZLoader } from 'three/examples/jsm/loaders/USDZLoader.js';
import { VRMLLoader } from 'three/examples/jsm/loaders/VRMLLoader.js';
import { useState, useEffect } from 'react';
import type { Asset } from '../types';

const snapshotUrlCache = new Map<string, string>();

function createPbrShaderBall(): THREE.Object3D {
  const geometry = new THREE.SphereGeometry(1, 64, 32);
  const material = new THREE.MeshStandardMaterial({
    color: 0x8899aa,
    roughness: 0.35,
    metalness: 0.15,
  });
  return new THREE.Mesh(geometry, material);
}

async function loadMeshGeometry(loader: STLLoader | PLYLoader, url: string): Promise<THREE.Object3D> {
  const geom = await loader.loadAsync(url);
  if (!geom.attributes.normal) geom.computeVertexNormals();
  return new THREE.Mesh(geom, new THREE.MeshStandardMaterial({ color: 0xb0b8c4, roughness: 0.4, metalness: 0.2 }));
}

type LoaderFn = (url: string) => Promise<THREE.Object3D>;

const LOADER_DISPATCH: Record<string, LoaderFn> = {
  sbsar: async () => createPbrShaderBall(),
  obj: (url) => new OBJLoader().loadAsync(url),
  fbx: (url) => new FBXLoader().loadAsync(url),
  stl: (url) => loadMeshGeometry(new STLLoader(), url),
  ply: (url) => loadMeshGeometry(new PLYLoader(), url),
  dae: async (url) => (await new ColladaLoader().loadAsync(url)).scene,
  usdz: (url) => new USDZLoader().loadAsync(url),
  wrl: (url) => new VRMLLoader().loadAsync(url),
  x3d: (url) => new VRMLLoader().loadAsync(url),
  gltf: async (url) => (await new GLTFLoader().loadAsync(url)).scene,
  glb: async (url) => (await new GLTFLoader().loadAsync(url)).scene,
  blend: async (url) => (await new GLTFLoader().loadAsync(url)).scene,
};

async function loadObjectByExtension(fileUrl: string, ext: string): Promise<THREE.Object3D | null> {
  const loader = LOADER_DISPATCH[ext] ?? LOADER_DISPATCH.gltf;
  return await loader(fileUrl);
}

function frameCameraOnObject(camera: THREE.PerspectiveCamera, object: THREE.Object3D): void {
  const box = new THREE.Box3().setFromObject(object);
  const size = box.getSize(new THREE.Vector3());
  const center = box.getCenter(new THREE.Vector3());

  object.position.x -= center.x;
  object.position.y -= center.y;
  object.position.z -= center.z;

  const maxDim = Math.max(size.x, size.y, size.z) || 1;
  const fovRad = (camera.fov * Math.PI) / 180;
  const cameraDistance = Math.abs(maxDim / 2 / Math.tan(fovRad / 2)) * 1.5;

  camera.position.set(cameraDistance * 0.8, cameraDistance * 0.6, cameraDistance);
  camera.lookAt(0, 0, 0);
  camera.updateProjectionMatrix();
}

function disposeSceneHierarchy(scene: THREE.Scene, renderer: THREE.WebGLRenderer): void {
  scene.traverse((obj) => {
    if (obj instanceof THREE.Mesh) {
      obj.geometry?.dispose();
      if (Array.isArray(obj.material)) {
        obj.material.forEach((m) => m.dispose());
      } else if (obj.material) {
        obj.material.dispose();
      }
    }
  });
  renderer.dispose();
  renderer.forceContextLoss();
}

function createSnapshotRenderer(canvas: HTMLCanvasElement): THREE.WebGLRenderer {
  const renderer = new THREE.WebGLRenderer({
    canvas,
    preserveDrawingBuffer: true,
    alpha: true,
    antialias: true,
  });
  renderer.setSize(512, 512);
  renderer.toneMapping = THREE.ACESFilmicToneMapping;
  renderer.outputColorSpace = THREE.SRGBColorSpace;
  return renderer;
}

function setupSnapshotSceneLights(scene: THREE.Scene): void {
  scene.add(new THREE.AmbientLight(0xffffff, 0.8));
  const dirLight = new THREE.DirectionalLight(0xffffff, 1.2);
  dirLight.position.set(5, 10, 7);
  scene.add(dirLight);
  const fillLight = new THREE.DirectionalLight(0xffffff, 0.4);
  fillLight.position.set(-5, -2, -5);
  scene.add(fillLight);
}

/**
 * Capture a single-frame offscreen render of a 3D model or shader ball.
 * Never throws — returns null on failure.
 */
export async function captureModelSnapshot(url: string, extension: string): Promise<Blob | null> {
  let renderer: THREE.WebGLRenderer | null = null;
  const scene = new THREE.Scene();

  try {
    const canvas = document.createElement('canvas');
    canvas.width = 512;
    canvas.height = 512;

    renderer = createSnapshotRenderer(canvas);
    setupSnapshotSceneLights(scene);

    const camera = new THREE.PerspectiveCamera(45, 1, 0.1, 1000);
    const fileUrl = url.startsWith('http') || url.startsWith('asset:') || url.startsWith('blob:')
      ? url
      : convertFileSrc(url);

    const modelObj = await loadObjectByExtension(fileUrl, extension.toLowerCase());
    if (!modelObj) return null;

    scene.add(modelObj);
    frameCameraOnObject(camera, modelObj);

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

function isEligibleForSnapshot(asset: Asset | null | undefined): boolean {
  if (!asset) return false;
  if (asset.thumbnailPath && !asset.thumbnailPath.endsWith('.glb')) return false;
  const ext = asset.extension.toLowerCase();
  return ext === 'blend' || ext === 'sbsar';
}

function executeSnapshotFetch(asset: Asset, onLoaded: (url: string) => void): () => void {
  let isMounted = true;
  const target = (asset.metadata?.preview_model as string) || asset.filePath;
  captureModelSnapshot(target, asset.extension).then((blob) => {
    if (blob && isMounted) {
      const url = URL.createObjectURL(blob);
      snapshotUrlCache.set(asset.id, url);
      onLoaded(url);
    }
  });
  return () => {
    isMounted = false;
  };
}

/**
 * Hook to retrieve an in-memory snapshot blob URL for models (.blend, .sbsar)
 * when no backend thumbnail exists.
 */
export function useModelSnapshot(asset: Asset | null | undefined): string | null {
  const assetId = asset?.id;
  const [snapshotUrl, setSnapshotUrl] = useState<string | null>(() => {
    return assetId ? snapshotUrlCache.get(assetId) ?? null : null;
  });

  useEffect(() => {
    if (!isEligibleForSnapshot(asset)) return;
    const currentAsset = asset!;
    const cached = snapshotUrlCache.get(currentAsset.id);
    if (cached) {
      setSnapshotUrl(cached);
      return;
    }
    return executeSnapshotFetch(currentAsset, setSnapshotUrl);
  }, [assetId, asset?.thumbnailPath, asset?.filePath, asset?.extension]);

  return snapshotUrl;
}
