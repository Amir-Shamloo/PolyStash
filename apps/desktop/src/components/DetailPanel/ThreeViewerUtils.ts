import * as THREE from 'three';

export interface ModelStats {
  triangles: number;
  vertices: number;
  meshCount: number;
  name: string;
}

/**
 * Centering helper:
 * Computes bounding box of the object. If center is not at (0, 0, 0),
 * shifts root object position so bbox center aligns with (0, 0, 0).
 * If already centered within a small epsilon, no-op.
 */
export function centerObject(obj: THREE.Object3D): THREE.Box3 {
  obj.updateMatrixWorld(true);
  const box = new THREE.Box3().setFromObject(obj);
  if (box.isEmpty()) return box;

  const center = new THREE.Vector3();
  box.getCenter(center);

  if (center.lengthSq() > 1e-8) {
    obj.position.x -= center.x;
    obj.position.y -= center.y;
    obj.position.z -= center.z;
    obj.updateMatrixWorld(true);
  }

  return new THREE.Box3().setFromObject(obj);
}

function ensureGeometryNormals(geometry: THREE.BufferGeometry): void {
  if (!geometry.attributes.normal) {
    try {
      geometry.computeVertexNormals();
    } catch (err) {
      console.warn('Unable to compute vertex normals:', err);
    }
  }
}

function repairMaterialColor(material: THREE.Material): void {
  if ('color' in material && material.color instanceof THREE.Color) {
    const isPitchBlack = material.color.r === 0 && material.color.g === 0 && material.color.b === 0;
    const hasTexture = 'map' in material && Boolean((material as { map?: THREE.Texture }).map);
    if (isPitchBlack && !hasTexture) {
      material.color.setHex(0xb0b8c4);
    }
  }
  material.needsUpdate = true;
}

/**
 * Traverses geometry & materials to prevent absolute black rendering:
 * - Computes missing vertex normals.
 * - Replaces missing materials with shaded MeshStandardMaterial.
 * - Brightens pure black unlit materials with no textures.
 */
export function sanitizeModelMaterialsAndNormals(root: THREE.Object3D): void {
  root.traverse((child) => {
    if (!(child as THREE.Mesh).isMesh) return;
    const mesh = child as THREE.Mesh;

    if (mesh.geometry) {
      ensureGeometryNormals(mesh.geometry);
    }

    if (!mesh.material) {
      mesh.material = new THREE.MeshStandardMaterial({
        color: 0xb0b8c4,
        roughness: 0.4,
        metalness: 0.2,
      });
      return;
    }

    const materials = Array.isArray(mesh.material) ? mesh.material : [mesh.material];
    materials.forEach(repairMaterialColor);
  });
}

function accumulateMeshStats(
  mesh: THREE.Mesh,
  stats: { triangles: number; vertices: number; meshCount: number }
): void {
  stats.meshCount++;
  const geom = mesh.geometry;
  if (!geom) return;

  const pos = geom.attributes.position;
  if (!pos) return;

  stats.vertices += pos.count;
  if (geom.index) {
    stats.triangles += Math.floor(geom.index.count / 3);
  } else {
    stats.triangles += Math.floor(pos.count / 3);
  }
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
