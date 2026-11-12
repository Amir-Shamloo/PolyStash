import * as THREE from 'three';

export interface ModelStats {
  triangles: number;
  vertices: number;
  meshCount: number;
  name: string;
}

export const DEFAULT_CLAY_COLOR = 0xdcdcdc;
export const DEFAULT_CLAY_ROUGHNESS = 0.52;
export const DEFAULT_CLAY_METALNESS = 0.08;

export const NON_MATERIAL_EXTENSIONS = new Set([
  'obj', 'stl', 'dxf', 'dae', 'ply', '3ds', 'wrl', 'fbx',
]);

export const Z_UP_EXTENSIONS = new Set(['3ds']);

/**
 * Centering helper:
 * Computes bounding box of the object. If center is not at (0, 0, 0),
 * shifts root object position so bbox center aligns with (0, 0, 0).
 */
export function centerObject(obj: THREE.Object3D): THREE.Box3 {
  obj.updateMatrixWorld(true);
  const box = new THREE.Box3().setFromObject(obj);
  if (box.isEmpty()) return box;

  const center = new THREE.Vector3();
  box.getCenter(center);

  if (center.lengthSq() > 1e-8) {
    obj.position.sub(center);
    obj.updateMatrixWorld(true);
  }

  return new THREE.Box3().setFromObject(obj);
}

/**
 * Explicit per-format up-axis rule: only 3ds is Z-up (rotate -PI/2 about X);
 * gltf, glb, fbx, obj, dae, ply, usdz, wrl are Y-up (no rotation).
 */
export function isZUpFormat(extension?: string): boolean {
  if (!extension) return false;
  return Z_UP_EXTENSIONS.has(extension.toLowerCase().replace(/^\./, ''));
}

/**
 * Auto-frames a PerspectiveCamera onto an object using 3/4 isometric perspective
 * with a 1.2x safety margin around the bounding box.
 */
export function frameCameraAuto(
  camera: THREE.PerspectiveCamera,
  object: THREE.Object3D,
  flipYZ?: boolean | string
): THREE.Box3 {
  const shouldRotate = typeof flipYZ === 'string' ? isZUpFormat(flipYZ) : Boolean(flipYZ);
  if (shouldRotate) object.rotation.x = -Math.PI / 2;
  const box = centerObject(object);
  const size = box.getSize(new THREE.Vector3());

  const maxDim = Math.max(size.x, size.y, size.z) || 1;
  const fovRad = (camera.fov * Math.PI) / 180;
  const dist = Math.abs(maxDim / (2 * Math.tan(fovRad / 2))) * 1.2;

  camera.near = Math.max(0.01, dist * 0.01);
  camera.far = Math.max(1000, dist * 20);
  camera.position.set(dist * 0.8, dist * 0.6, dist);
  camera.lookAt(0, 0, 0);
  camera.updateProjectionMatrix();
  return box;
}

export function ensureGeometryNormals(geometry: THREE.BufferGeometry): void {
  const normalAttr = geometry.attributes.normal;
  if (!normalAttr || normalAttr.count === 0) {
    try {
      geometry.computeVertexNormals();
    } catch (err) {
      console.warn('Unable to compute vertex normals:', err);
    }
  }
}

export function applyDarkBackfaceShader(material: THREE.Material): void {
  material.side = THREE.DoubleSide;
  material.onBeforeCompile = (shader) => {
    // three r154+ renamed output_fragment to opaque_fragment; inject before it.
    shader.fragmentShader = shader.fragmentShader.replace(
      '#include <opaque_fragment>',
      `if (!gl_FrontFacing) { diffuseColor.rgb = vec3(0.04, 0.04, 0.05); }
#include <opaque_fragment>`
    );
  };
  material.customProgramCacheKey = () => 'polystash-dark-backface';
}

export function createDefaultClayMaterial(): THREE.MeshStandardMaterial {
  const mat = new THREE.MeshStandardMaterial({
    color: DEFAULT_CLAY_COLOR,
    roughness: DEFAULT_CLAY_ROUGHNESS,
    metalness: DEFAULT_CLAY_METALNESS,
    side: THREE.DoubleSide,
  });
  applyDarkBackfaceShader(mat);
  return mat;
}

const TEXTURE_MAP_KEYS = ['map', 'normalMap', 'roughnessMap', 'metalnessMap', 'emissiveMap'];

function hasMaterialTextures(material: THREE.Material): boolean {
  const record = material as unknown as Record<string, unknown>;
  return TEXTURE_MAP_KEYS.some((key) => Boolean(record[key]));
}

function isPureBlackOrWhite(color: THREE.Color): boolean {
  const sum = color.r + color.g + color.b;
  return sum === 0 || sum >= 2.97;
}

function isDefaultColorMaterial(material: THREE.Material): boolean {
  if (!('color' in material) || !(material.color instanceof THREE.Color)) {
    return false;
  }
  return isPureBlackOrWhite(material.color);
}

function applyClayParameters(material: THREE.Material): void {
  if ('color' in material && material.color instanceof THREE.Color) {
    material.color.setHex(DEFAULT_CLAY_COLOR);
  }
  if ('roughness' in material) {
    (material as THREE.MeshStandardMaterial).roughness = DEFAULT_CLAY_ROUGHNESS;
  }
  if ('metalness' in material) {
    (material as THREE.MeshStandardMaterial).metalness = DEFAULT_CLAY_METALNESS;
  }
}

function isStandardOrPhysical(material: THREE.Material): boolean {
  return (
    material instanceof THREE.MeshStandardMaterial ||
    material instanceof THREE.MeshPhysicalMaterial
  );
}

function shouldApplyClayFinish(
  material: THREE.Material,
  hasTex: boolean,
  forceClayIfUnstyled: boolean
): boolean {
  if (hasTex) return false;
  if (forceClayIfUnstyled) return true;
  return isDefaultColorMaterial(material);
}

function sanitizeSingleMaterial(
  material: THREE.Material | null | undefined,
  forceClayIfUnstyled: boolean
): THREE.Material {
  if (!material) return createDefaultClayMaterial();
  const hasTex = hasMaterialTextures(material);

  if (!hasTex && !isStandardOrPhysical(material)) {
    return createDefaultClayMaterial();
  }

  if (shouldApplyClayFinish(material, hasTex, forceClayIfUnstyled)) {
    applyClayParameters(material);
  }

  applyDarkBackfaceShader(material);
  material.needsUpdate = true;
  return material;
}

function sanitizeMeshMaterials(mesh: THREE.Mesh, isNonMatFormat: boolean): void {
  if (!mesh.material) {
    mesh.material = createDefaultClayMaterial();
    return;
  }
  if (Array.isArray(mesh.material)) {
    mesh.material = mesh.material.map((m) => sanitizeSingleMaterial(m, isNonMatFormat));
  } else {
    mesh.material = sanitizeSingleMaterial(mesh.material, isNonMatFormat);
  }
}

/**
 * Traverses geometry & materials to ensure industrial-grade rendering:
 * - Updates world matrices so hierarchical pivots (e.g. 3DS) are preserved.
 * - Computes missing vertex normals.
 * - Replaces missing or unstyled materials with #dcdcdc clay material (roughness 0.52, metalness 0.08).
 * - Renders backfaces dark instead of hollow (DoubleSide + gl_FrontFacing check).
 */
export function sanitizeModelMaterialsAndNormals(
  root: THREE.Object3D,
  extension?: string
): void {
  root.updateMatrixWorld(true);
  const isNonMatFormat = NON_MATERIAL_EXTENSIONS.has(extension?.toLowerCase() ?? '');

  root.traverse((child) => {
    child.visible = true;
    if (!(child as THREE.Mesh).isMesh) return;
    const mesh = child as THREE.Mesh;
    mesh.frustumCulled = false;

    if (mesh.geometry) {
      ensureGeometryNormals(mesh.geometry);
    }
    sanitizeMeshMaterials(mesh, isNonMatFormat);
  });
}

function accumulateMeshStats(
  mesh: THREE.Mesh,
  stats: { triangles: number; vertices: number; meshCount: number }
): void {
  stats.meshCount++;
  const geom = mesh.geometry;
  const pos = geom?.attributes.position;
  if (!geom || !pos) return;

  stats.vertices += pos.count;
  const count = geom.index ? geom.index.count : pos.count;
  stats.triangles += Math.floor(count / 3);
}

/**
 * Computes geometry stats (polys, vertices, meshes) by traversing the scene graph.
 */
export function computeModelStats(root: THREE.Object3D): ModelStats {
  const stats = { triangles: 0, vertices: 0, meshCount: 0 };

  root.traverse((child) => {
    if ((child as THREE.Mesh).isMesh) {
      accumulateMeshStats(child as THREE.Mesh, stats);
    }
  });

  return {
    triangles: stats.triangles,
    vertices: stats.vertices,
    meshCount: stats.meshCount,
    name: root.name || 'Model',
  };
}

const DIRECT_LIGHT_CALL =
  'RE_Direct( directLight, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, material, reflectedLight );';

const KEY_LIGHT_ONLY_CALL = [
  '{',
  '\tPhysicalMaterial specMaterial = material;',
  '\tif ( i > 0 ) specMaterial.specularColor = vec3( 0.0 );',
  '\tRE_Direct( directLight, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, specMaterial, reflectedLight );',
  '}',
].join('\n');

const DIRECTIONAL_LOOP_BLOCK = new RegExp(
  '(getDirectionalLightInfo\\( directionalLight, directLight \\);[\\s\\S]*?' +
    DIRECT_LIGHT_CALL.replace(/[()]/g, '\\$&') +
    ')'
);

/**
 * three.js has no per-light specular control; the studio wants a single key-light
 * highlight. Zeroing specularColor for every directional light after the first keeps
 * their diffuse shading and removes their reflections.
 * If the shader chunk text changes and nothing matches, the material renders normally.
 */
export function applySingleKeyLightSpecularToMaterial(material: THREE.Material | undefined): void {
  const standard = material as THREE.MeshStandardMaterial | undefined;
  if (!standard?.isMeshStandardMaterial || standard.userData.singleKeyLightSpecular) return;
  standard.userData.singleKeyLightSpecular = true;
  standard.onBeforeCompile = (shader) => {
    shader.fragmentShader = shader.fragmentShader.replace(DIRECTIONAL_LOOP_BLOCK, (_m, block: string) =>
      block.replace(DIRECT_LIGHT_CALL, KEY_LIGHT_ONLY_CALL)
    );
  };
  standard.needsUpdate = true;
}

export function applySingleKeyLightSpecular(root: THREE.Object3D): void {
  root.traverse((object) => {
    const mesh = object as THREE.Mesh;
    if (!mesh.isMesh) return;
    const materials = Array.isArray(mesh.material) ? mesh.material : [mesh.material];
    materials.forEach(applySingleKeyLightSpecularToMaterial);
  });
}
