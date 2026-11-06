import { useMemo, useEffect, type ReactNode } from 'react';
import { useLoader } from '@react-three/fiber';
import { useGLTF, useFBX } from '@react-three/drei';
import * as THREE from 'three';
import { OBJLoader } from 'three/examples/jsm/loaders/OBJLoader.js';
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader.js';
import { ColladaLoader } from 'three/examples/jsm/loaders/ColladaLoader.js';
import { PLYLoader } from 'three/examples/jsm/loaders/PLYLoader.js';
import { USDZLoader } from 'three/examples/jsm/loaders/USDZLoader.js';
import { TDSLoader } from 'three/examples/jsm/loaders/TDSLoader.js';
import { VRMLLoader } from 'three/examples/jsm/loaders/VRMLLoader.js';
import {
  centerObject,
  sanitizeModelMaterialsAndNormals,
} from './ThreeViewerUtils';

export interface ModelLoadProps {
  url: string;
  onLoaded?: (obj: THREE.Object3D) => void;
}

function ModelObjectWrapper({
  object,
  onLoaded,
}: {
  object: THREE.Object3D;
  onLoaded?: (obj: THREE.Object3D) => void;
}) {
  const preparedObject = useMemo(() => {
    centerObject(object);
    sanitizeModelMaterialsAndNormals(object);
    return object;
  }, [object]);

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
    if (!geometry.attributes.normal) {
      geometry.computeVertexNormals();
    }
    geometry.center();
    const material = new THREE.MeshStandardMaterial({
      color: 0xb0b8c4,
      roughness: 0.4,
      metalness: 0.2,
    });
    return new THREE.Mesh(geometry, material);
  }, [geometry]);

  useEffect(() => {
    onLoaded?.(mesh);
  }, [mesh, onLoaded]);

  return <primitive object={mesh} />;
}

export function GltfModel({ url, onLoaded }: ModelLoadProps) {
  const { scene } = useGLTF(url);
  const cloned = useMemo(() => scene.clone(true), [scene]);
  return <ModelObjectWrapper object={cloned} onLoaded={onLoaded} />;
}

export function FbxModel({ url, onLoaded }: ModelLoadProps) {
  const fbx = useFBX(url);
  return <ModelObjectWrapper object={fbx} onLoaded={onLoaded} />;
}

export function ObjModel({ url, onLoaded }: ModelLoadProps) {
  const obj = useLoader(OBJLoader, url);
  return <ModelObjectWrapper object={obj} onLoaded={onLoaded} />;
}

export function StlModel({ url, onLoaded }: ModelLoadProps) {
  const geometry = useLoader(STLLoader, url);
  return <BufferGeometryModel geometry={geometry} onLoaded={onLoaded} />;
}

export function ColladaModel({ url, onLoaded }: ModelLoadProps) {
  const collada = useLoader(ColladaLoader, url);
  return <ModelObjectWrapper object={collada.scene} onLoaded={onLoaded} />;
}

export function PlyModel({ url, onLoaded }: ModelLoadProps) {
  const geometry = useLoader(PLYLoader, url);
  return <BufferGeometryModel geometry={geometry} onLoaded={onLoaded} />;
}

export function UsdzModel({ url, onLoaded }: ModelLoadProps) {
  const group = useLoader(USDZLoader, url);
  return <ModelObjectWrapper object={group} onLoaded={onLoaded} />;
}

export function TdsModel({ url, onLoaded }: ModelLoadProps) {
  const group = useLoader(TDSLoader, url);
  return <ModelObjectWrapper object={group} onLoaded={onLoaded} />;
}

export function VrmlModel({ url, onLoaded }: ModelLoadProps) {
  const vrml = useLoader(VRMLLoader, url);
  return <ModelObjectWrapper object={vrml} onLoaded={onLoaded} />;
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
  usd: (props) => <UsdzModel {...props} />,
  usdz: (props) => <UsdzModel {...props} />,
  wrl: (props) => <VrmlModel {...props} />,
  gltf: (props) => <GltfModel {...props} />,
  glb: (props) => <GltfModel {...props} />,
};
