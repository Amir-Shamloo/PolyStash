import * as THREE from 'three';
import { PXR, type Pxr, type UsdTimeCode } from '@openusd-wasm/pxr';
import { extractPackageEntries, findPackageRootLayer } from '@openusd-wasm/utils';
import { GLTFExporter } from 'three/examples/jsm/exporters/GLTFExporter.js';
import { convertFileSrc } from '@tauri-apps/api/core';

type Dict = Record<string, unknown>;
type Disposable = { delete?: () => void } | null | undefined;

interface ShaderInfo { path: string; id: string | null; inputs: Dict }
interface UvInfo { uvPrimvar?: string; uvTransform?: Dict | null }
interface MaterialInfo { path: string; shaders: ShaderInfo[]; textureInfo?: UvInfo }
interface TriGeometry {
  positions: ArrayLike<number>;
  pointIndices?: ArrayLike<number>;
  normals?: ArrayLike<number>;
  uvs?: ArrayLike<number>;
  displayColor?: number[] | null;
}

interface GeometryQuery {
  time: UsdTimeCode;
  uv?: UvInfo;
}

export const USD_EXTENSIONS = new Set(['usd', 'usda', 'usdc', 'usdz']);

const ANALYTIC_PRIMS = new Set(['Cube', 'Sphere', 'Cylinder', 'Cone', 'Capsule', 'Plane']);
const WORK_DIR = '/polystash_usd';

let pxrPromise: Promise<Pxr> | null = null;
let loadCounter = 0;

function getPxr(): Promise<Pxr> {
  pxrPromise ??= new PXR().load().catch((err: unknown) => {
    pxrPromise = null;
    throw err;
  });
  return pxrPromise;
}

function free(...values: Disposable[]): void {
  for (const v of values) {
    if (v && typeof v.delete === 'function') v.delete();
  }
}

function asDict(value: unknown): Dict | null {
  return value && typeof value === 'object' && !Array.isArray(value) ? (value as Dict) : null;
}

function inputOf(shader: ShaderInfo, name: string): unknown {
  return shader.inputs[`inputs:${name}`];
}

function connectedShaderPath(shader: ShaderInfo, name: string): { path: string; output: string } | null {
  const key = `inputs:${name}`;
  let raw: unknown = shader.inputs[`${key}.resolvedConnect`] ?? shader.inputs[`${key}.connect`];
  if (Array.isArray(raw)) raw = raw[0];
  const rec = asDict(raw);
  if (rec) raw = rec.path ?? rec.connection ?? rec.target;
  if (typeof raw !== 'string' || !raw) return null;
  const idx = raw.lastIndexOf('.outputs:');
  return idx >= 0
    ? { path: raw.slice(0, idx), output: raw.slice(idx + 9) }
    : { path: raw, output: 'rgb' };
}

function numberOf(value: unknown, fallback: number): number {
  return typeof value === 'number' && Number.isFinite(value) ? value : fallback;
}

function colorOf(value: unknown, fallback: THREE.Color): THREE.Color {
  if (Array.isArray(value) && value.length >= 3) {
    return new THREE.Color().setRGB(Number(value[0]), Number(value[1]), Number(value[2]), THREE.LinearSRGBColorSpace);
  }
  return fallback;
}

function normalizePath(p: string): string {
  const inner = /\[([^\]]+)\]\s*$/.exec(p);
  if (inner) p = inner[1];
  return p.replace(/^\.\//, '').replace(/^@|@$/g, '');
}

function findPackageBytes(entries: Map<string, Uint8Array>, assetPath: string): Uint8Array | null {
  const wanted = normalizePath(assetPath).toLowerCase();
  const base = wanted.split('/').pop() ?? wanted;
  let fallback: Uint8Array | null = null;
  for (const [key, bytes] of entries) {
    const k = key.toLowerCase();
    if (k === wanted || k.endsWith(`/${wanted}`)) return bytes;
    if (k.split('/').pop() === base) fallback = bytes;
  }
  return fallback;
}

interface ConversionContext {
  sourceDir: string;
  entries: Map<string, Uint8Array> | null;
  textures: Map<string, Promise<THREE.Texture | null>>;
}

async function readTextureBytes(ctx: ConversionContext, assetPath: string): Promise<Uint8Array | null> {
  const fromPackage = ctx.entries ? findPackageBytes(ctx.entries, assetPath) : null;
  if (fromPackage) return fromPackage;
  if (!ctx.sourceDir || /^[a-z]+:/i.test(assetPath)) return null;
  try {
    const res = await fetch(convertFileSrc(`${ctx.sourceDir}/${normalizePath(assetPath)}`));
    return res.ok ? new Uint8Array(await res.arrayBuffer()) : null;
  } catch {
    return null;
  }
}

const MAX_TEXTURE_SIZE = 2048;

/** Decodes an image and caps its longest side, since the GLB export re-encodes every texture. */
async function decodeTextureBitmap(bytes: Uint8Array): Promise<ImageBitmap> {
  const full = await createImageBitmap(new Blob([bytes as BlobPart]), { imageOrientation: 'none' });
  const longest = Math.max(full.width, full.height);
  if (longest <= MAX_TEXTURE_SIZE) return full;
  const scale = MAX_TEXTURE_SIZE / longest;
  const small = await createImageBitmap(full, {
    resizeWidth: Math.round(full.width * scale),
    resizeHeight: Math.round(full.height * scale),
    resizeQuality: 'medium',
    imageOrientation: 'none',
  });
  full.close();
  return small;
}

function loadTexture(ctx: ConversionContext, shader: ShaderInfo, srgb: boolean): Promise<THREE.Texture | null> {
  const file = asDict(inputOf(shader, 'file'));
  const assetPath = (file?.resolvedPath ?? file?.path) as string | undefined;
  if (!assetPath) return Promise.resolve(null);
  const key = `${srgb}:${assetPath}`;
  const cached = ctx.textures.get(key);
  if (cached) return cached;
  const task = (async () => {
    const bytes = await readTextureBytes(ctx, assetPath);
    if (!bytes) return null;
    try {
      const bitmap = await decodeTextureBitmap(bytes);
      const tex = new THREE.Texture(bitmap as unknown as HTMLImageElement);
      tex.flipY = false;
      tex.colorSpace = srgb ? THREE.SRGBColorSpace : THREE.NoColorSpace;
      tex.wrapS = wrapMode(inputOf(shader, 'wrapS'));
      tex.wrapT = wrapMode(inputOf(shader, 'wrapT'));
      tex.needsUpdate = true;
      return tex;
    } catch {
      return null;
    }
  })();
  ctx.textures.set(key, task);
  return task;
}

function wrapMode(value: unknown): THREE.Wrapping {
  if (value === 'clamp') return THREE.ClampToEdgeWrapping;
  if (value === 'mirror') return THREE.MirroredRepeatWrapping;
  return THREE.RepeatWrapping;
}

interface ShaderLookup {
  ctx: ConversionContext;
  shaders: Map<string, ShaderInfo>;
  surface: ShaderInfo;
}

type TextureSlot = THREE.Texture | null;

interface MaterialRequest {
  info: MaterialInfo | undefined;
  displayColor: number[] | null | undefined;
  doubleSided: boolean;
}

const DEFAULT_GRAY = (): THREE.Color => new THREE.Color(0.8, 0.8, 0.8);

async function textureForSlot(lookup: ShaderLookup, slot: string, srgb: boolean): Promise<TextureSlot> {
  const link = connectedShaderPath(lookup.surface, slot);
  const source = link ? lookup.shaders.get(link.path) : undefined;
  return source ? loadTexture(lookup.ctx, source, srgb) : null;
}

function constantOrFallback(shader: ShaderInfo, slot: string, link: unknown, fallback: number): number {
  return link ? fallback : numberOf(inputOf(shader, slot), fallback);
}

function applyConstants(mat: THREE.MeshStandardMaterial, surface: ShaderInfo): void {
  mat.color = colorOf(inputOf(surface, 'diffuseColor'), mat.color);
  mat.roughness = constantOrFallback(surface, 'roughness', connectedShaderPath(surface, 'roughness'), 0.6);
  mat.metalness = constantOrFallback(surface, 'metallic', connectedShaderPath(surface, 'metallic'), 0);
  const opacity = numberOf(inputOf(surface, 'opacity'), 1);
  if (opacity < 1) {
    mat.opacity = opacity;
    mat.transparent = true;
  }
  const emissive = inputOf(surface, 'emissiveColor');
  if (emissive) mat.emissive = colorOf(emissive, mat.emissive);
}

interface MaterialTextures {
  map: TextureSlot;
  normalMap: TextureSlot;
  roughnessMap: TextureSlot;
  metalnessMap: TextureSlot;
  emissiveMap: TextureSlot;
  aoMap: TextureSlot;
}

async function loadMaterialTextures(lookup: ShaderLookup): Promise<MaterialTextures> {
  const [map, normalMap, roughnessMap, metalnessMap, emissiveMap, aoMap] = await Promise.all([
    textureForSlot(lookup, 'diffuseColor', true),
    textureForSlot(lookup, 'normal', false),
    textureForSlot(lookup, 'roughness', false),
    textureForSlot(lookup, 'metallic', false),
    textureForSlot(lookup, 'emissiveColor', true),
    textureForSlot(lookup, 'occlusion', false),
  ]);
  return { map, normalMap, roughnessMap, metalnessMap, emissiveMap, aoMap };
}

function applyColorTextures(mat: THREE.MeshStandardMaterial, t: MaterialTextures): void {
  if (t.map) {
    mat.map = t.map;
    mat.color.set(0xffffff);
  }
  if (t.emissiveMap) {
    mat.emissiveMap = t.emissiveMap;
    mat.emissive.set(0xffffff);
  }
}

function applyDataTextures(mat: THREE.MeshStandardMaterial, t: MaterialTextures): void {
  if (t.normalMap) mat.normalMap = t.normalMap;
  if (t.aoMap) mat.aoMap = t.aoMap;
  if (t.roughnessMap) {
    mat.roughnessMap = t.roughnessMap;
    mat.roughness = 1;
  }
  if (t.metalnessMap) {
    mat.metalnessMap = t.metalnessMap;
    mat.metalness = 1;
  }
}

async function buildMaterial(ctx: ConversionContext, request: MaterialRequest): Promise<THREE.MeshStandardMaterial> {
  const { info, displayColor, doubleSided } = request;
  const mat = new THREE.MeshStandardMaterial({
    color: displayColor ? colorOf(displayColor, DEFAULT_GRAY()) : DEFAULT_GRAY(),
    roughness: 0.6,
    metalness: 0,
    side: doubleSided ? THREE.DoubleSide : THREE.FrontSide,
  });
  const surface = info?.shaders.find((s) => s.id === 'UsdPreviewSurface');
  if (!info || !surface) return mat;

  mat.name = info.path.split('/').pop() ?? '';
  applyConstants(mat, surface);
  const shaders = new Map(info.shaders.map((s) => [s.path, s]));
  const textures = await loadMaterialTextures({ ctx, shaders, surface });
  applyColorTextures(mat, textures);
  applyDataTextures(mat, textures);
  return mat;
}

function flipV(uvs: ArrayLike<number>): number[] {
  const out = Array.from(uvs);
  for (let i = 1; i < out.length; i += 2) out[i] = 1 - out[i];
  return out;
}

function applyIndices(geo: THREE.BufferGeometry, data: TriGeometry): void {
  const vertexCount = geo.getAttribute('position').count;
  const indices = data.pointIndices;
  // Equal lengths mean the arrays are already expanded per corner; indexing
  // them anyway scrambles the mesh.
  if (indices?.length && indices.length !== vertexCount) {
    geo.setIndex(Array.from(indices));
  }
}

function buildGeometry(data: TriGeometry) {
  const geo = new THREE.BufferGeometry();
  geo.setAttribute('position', new THREE.Float32BufferAttribute(Array.from(data.positions), 3));
  if (data.normals?.length) geo.setAttribute('normal', new THREE.Float32BufferAttribute(Array.from(data.normals), 3));
  if (data.uvs?.length) geo.setAttribute('uv', new THREE.Float32BufferAttribute(flipV(data.uvs), 2));
  applyIndices(geo, data);
  if (!geo.getAttribute('normal')) geo.computeVertexNormals();
  return geo;
}

function materialBindingPath(pxr: Pxr, prim: unknown): string | null {
  const binding = (pxr.UsdShade as unknown as { ComputeBoundMaterialPath: (p: unknown) => Dict | null })
    .ComputeBoundMaterialPath(prim);
  const path = binding?.resolvedTargetPath || binding?.materialPath;
  return typeof path === 'string' && path ? path : null;
}

function attributeOf(prim: any, name: string, time: unknown): unknown {
  const attr = prim.GetAttribute(name);
  try {
    return attr?.IsValid?.() ? attr.Get(time) : null;
  } finally {
    free(attr);
  }
}

function computeGeometry(pxr: Pxr, prim: any, typeName: string, query: GeometryQuery): TriGeometry {
  const Schema = (pxr.UsdGeom as any)[typeName];
  const schema = new Schema(prim);
  try {
    return typeName === 'Mesh'
      ? schema.ComputeTriangulatedGeometry(
          query.time,
          query.uv?.uvPrimvar || 'primvars:st',
          query.uv?.uvTransform ?? null
        )
      : schema.ComputeTriangulatedGeometry(query.time);
  } finally {
    free(schema);
  }
}

function localMatrix(pxr: Pxr, prim: any, time: UsdTimeCode): THREE.Matrix4 | null {
  if (!prim.IsA('Xformable')) return null;
  const xf = new pxr.UsdGeom.Xformable(prim);
  try {
    const t = xf.GetLocalTransformation(time) as { matrix?: ArrayLike<number> } | null;
    const m = t?.matrix;
    return m && m.length === 16 ? new THREE.Matrix4().fromArray(Array.from(m)) : null;
  } finally {
    free(xf);
  }
}

function tryOpenStage(pxr: Pxr, id: string) {
  const stage = pxr.Usd.Stage.Open(id);
  if (stage?.IsValid?.() !== false && stage) return stage;
  free(stage);
  return null;
}

function openStage(pxr: Pxr, filePath: string, extension: string) {
  const root = extension === 'usdz' ? findPackageRootLayer(pxr, filePath) : null;
  const candidates = root ? [`${filePath}[${root}]`, filePath] : [filePath];
  for (const id of candidates) {
    const stage = tryOpenStage(pxr, id);
    if (stage) return stage;
  }
  throw new Error(`Failed to open USD stage ${filePath}`);
}

interface SceneState {
  pxr: Pxr;
  ctx: ConversionContext;
  time: UsdTimeCode;
  materialInfos: Map<string, MaterialInfo>;
  objects: Map<string, THREE.Object3D>;
  root: THREE.Group;
}

function collectMaterialInfos(pxr: Pxr, prims: any[], materialTime: unknown): Map<string, MaterialInfo> {
  const infos = new Map<string, MaterialInfo>();
  for (const prim of prims) {
    if (String(prim.GetTypeName()) !== 'Material') continue;
    const info = (pxr.UsdShade as any).ComputeMaterialInfo(prim, materialTime) as MaterialInfo;
    infos.set(String(info.path), info);
  }
  return infos;
}

async function createMesh(state: SceneState, prim: any, typeName: string): Promise<THREE.Mesh> {
  const { pxr } = state;
  const boundPath = materialBindingPath(pxr, prim);
  const info = boundPath ? state.materialInfos.get(boundPath) : undefined;
  const geom = computeGeometry(pxr, prim, typeName, { time: state.time, uv: info?.textureInfo });
  const material = await buildMaterial(state.ctx, {
    info,
    displayColor: geom.displayColor,
    doubleSided: Boolean(attributeOf(prim, 'doubleSided', state.time)),
  });
  return new THREE.Mesh(buildGeometry(geom), material);
}

async function createObject(state: SceneState, prim: any): Promise<THREE.Object3D> {
  const typeName = String(prim.GetTypeName());
  const isGeometry = typeName === 'Mesh' || ANALYTIC_PRIMS.has(typeName);
  const object: THREE.Object3D = isGeometry ? await createMesh(state, prim, typeName) : new THREE.Group();
  object.name = String(prim.GetName());
  object.visible = attributeOf(prim, 'visibility', state.time) !== 'invisible';
  applyLocalMatrix(object, localMatrix(state.pxr, prim, state.time));
  return object;
}

function applyLocalMatrix(object: THREE.Object3D, matrix: THREE.Matrix4 | null): void {
  if (!matrix) return;
  object.matrixAutoUpdate = false;
  object.matrix.copy(matrix);
  object.matrix.decompose(object.position, object.quaternion, object.scale);
  object.matrixAutoUpdate = true;
}

function parentPathOf(prim: any): string | null {
  const parent = prim.GetParent();
  const path = parent?.IsValid?.() && !parent.IsPseudoRoot?.() ? String(parent.GetPath()) : null;
  free(parent);
  return path;
}

function attachToParent(state: SceneState, prim: any, object: THREE.Object3D): void {
  const parentPath = parentPathOf(prim);
  const container = parentPath ? state.objects.get(parentPath) : state.root;
  container?.add(object);
}

async function buildScene(pxr: Pxr, stage: any, ctx: ConversionContext): Promise<THREE.Group> {
  const time = pxr.Usd.TimeCode.Default();
  const materialTime = new pxr.Usd.TimeCode(stage.GetStartTimeCode());
  const prims: any[] = stage.TraverseInstanceProxies();

  try {
    const state: SceneState = {
      pxr,
      ctx,
      time,
      materialInfos: collectMaterialInfos(pxr, prims, materialTime),
      objects: new Map(),
      root: new THREE.Group(),
    };
    for (const prim of prims) {
      const object = await createObject(state, prim);
      state.objects.set(String(prim.GetPath()), object);
      attachToParent(state, prim, object);
    }
    if (String(pxr.UsdGeom.GetStageUpAxis(stage)) === 'Z') state.root.rotation.x = -Math.PI / 2;
    return state.root;
  } finally {
    free(...prims, materialTime, time);
  }
}

function exportGlb(scene: THREE.Object3D): Promise<ArrayBuffer> {
  return new GLTFExporter().parseAsync(scene, { binary: true }) as Promise<ArrayBuffer>;
}

function bytesToBase64(buffer: ArrayBuffer): string {
  const bytes = new Uint8Array(buffer);
  let binary = '';
  const chunk = 0x8000;
  for (let i = 0; i < bytes.length; i += chunk) {
    binary += String.fromCharCode(...bytes.subarray(i, i + chunk));
  }
  return btoa(binary);
}

/** Converts a USD/USDA/USDC/USDZ file into base64-encoded GLB bytes. */
export async function convertUsdToGlbBase64(filePath: string, extension: string): Promise<string> {
  const pxr = await getPxr();
  const res = await fetch(convertFileSrc(filePath));
  if (!res.ok) throw new Error(`Cannot read ${filePath}`);
  const input = new Uint8Array(await res.arrayBuffer());

  const ext = extension.toLowerCase();
  const dir = `${WORK_DIR}/${++loadCounter}`;
  const fsPath = `${dir}/model.${ext}`;
  if (!pxr.FS.exists(WORK_DIR)) pxr.FS.createDir(WORK_DIR);
  pxr.FS.createDir(dir);
  pxr.FS.writeFile(fsPath, input);

  let stage: any = null;
  try {
    const entries = ext === 'usdz' ? extractPackageEntries(pxr, fsPath, `${dir}/.packages`) : null;
    stage = openStage(pxr, fsPath, ext);
    const ctx: ConversionContext = {
      sourceDir: filePath.replace(/[\\/][^\\/]*$/, ''),
      entries,
      textures: new Map(),
    };
    const scene = await buildScene(pxr, stage, ctx);
    return bytesToBase64(await exportGlb(scene));
  } finally {
    free(stage);
    try {
      pxr.FS.deleteFile(fsPath);
    } catch {
      /* best-effort cleanup of the virtual filesystem */
    }
  }
}
