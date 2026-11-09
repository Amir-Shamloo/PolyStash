// ── Asset types ─────────────────────────────────────────────────────────
export type AssetKind =
  | '3d_model'
  | 'material'
  | 'texture'
  | 'hdri'
  | 'ies'
  | 'video'
  | 'image'
  | 'document'
  | 'folder'
  | 'other';

const EXTENSION_KIND_MAP: Record<string, AssetKind> = {
  // 3D CAD & Geometry Models
  fbx: '3d_model', obj: '3d_model', '3ds': '3d_model', dae: '3d_model',
  gltf: '3d_model', glb: '3d_model', skp: '3d_model', blend: '3d_model',
  c4d: '3d_model', '3dm': '3d_model', rfa: '3d_model', rvt: '3d_model',
  dwg: '3d_model', dxf: '3d_model', stl: '3d_model', usd: '3d_model',
  usdz: '3d_model', max: '3d_model', ply: '3d_model', x3d: '3d_model', wrl: '3d_model',
  // Materials & Shaders
  mat: 'material', vrmat: 'material', sbsar: 'material', sbs: 'material', mdl: 'material',
  // Textures & Surface Maps
  jpg: 'texture', jpeg: 'texture', png: 'texture', tga: 'texture',
  tif: 'texture', tiff: 'texture', bmp: 'texture', dds: 'texture',
  ktx: 'texture', ktx2: 'texture', webp: 'texture', avif: 'texture',
  // Environment & HDRI
  hdr: 'hdri', exr: 'hdri', hdri: 'hdri',
  // Photometric Lights
  ies: 'ies', ldt: 'ies',
  // Video & Motion
  mp4: 'video', mov: 'video', avi: 'video', mkv: 'video', webm: 'video', m4v: 'video',
  // Layered, Vector & Graphics
  psd: 'image', psb: 'image', ai: 'image', svg: 'image', gif: 'image',
  // Documents & Text
  pdf: 'document', txt: 'document', md: 'document', rtf: 'document', docx: 'document',
  // Point caches & Other
  abc: 'other',
};

/**
 * Authoritative mapping from lowercase file extension to AssetKind.
 * Matches backend AssetKind::from_extension rules.
 */
export function kindFromExtension(ext: string): AssetKind {
  const cleanExt = ext.toLowerCase().replace(/^\./, '').trim();
  return EXTENSION_KIND_MAP[cleanExt] ?? 'other';
}

export type ColorLabel =
  | 'none'
  | 'red'
  | 'orange'
  | 'yellow'
  | 'green'
  | 'teal'
  | 'blue'
  | 'purple'
  | 'pink';

export interface Tag {
  id: string;
  name: string;
  color?: ColorLabel;
  parentId?: string;
}

export interface AssetMeta {
  /** Format-specific extracted metadata (renderer, poly count, resolution…) */
  [key: string]: unknown;
}

export interface Asset {
  id: string;
  /** Absolute path on the local filesystem */
  filePath: string;
  fileName: string;
  /** File extension, lowercase, no dot */
  extension: string;
  kind: AssetKind;
  /** Size in bytes */
  sizeBytes: number;
  /** Last-modified timestamp (ms) */
  modifiedAt: number;
  /** When it was added to the library */
  indexedAt: number;
  /** Star rating 0–5 */
  rating: number;
  colorLabel: ColorLabel;
  isFavorite: boolean;
  description: string;
  tags: string[];       // tag IDs
  collections?: string[]; // collection IDs
  /** Relative path to cached thumbnail (inside DAM cache dir) */
  thumbnailPath?: string;
  /** Whether a preview job is pending/running */
  previewStatus: 'none' | 'pending' | 'generating' | 'done' | 'error';
  metadata: AssetMeta;
}

export interface Library {
  id: string;
  name: string;
  /** Root paths this library watches */
  rootPaths: string[];
  createdAt: number;
  assetCount: number;
}

export interface Collection {
  id: string;
  name: string;
  description: string;
  parentId?: string;
  assetIds: string[];
  createdAt: number;
}

// ── Search & Filter ──────────────────────────────────────────────────────
export type SortField = 'name' | 'date' | 'size' | 'rating' | 'kind' | 'favorite';
export type SortOrder = 'asc' | 'desc';

export interface FilterSpec {
  kinds?: AssetKind[];
  tags?: string[];
  rating?: { min: number; max: number };
  colorLabels?: ColorLabel[];
  isFavorite?: boolean;
  dateRange?: { from?: number; to?: number };
  sizeRange?: { min?: number; max?: number };
  extensions?: string[];
  previewStatus?: Asset['previewStatus'];
  unorganized?: boolean;
  directory?: string;
  directoryPrefix?: string;
  namePrefix?: string;
  nameSuffix?: string;
  tagLogic?: 'and' | 'or' | 'and_or' | 'none';
  collectionId?: string;
  collectionIds?: string[];
  collectionLogic?: 'and' | 'or' | 'and_or' | 'none';
  includeSubcollections?: boolean;
  includeSubfolders?: boolean;
}

export interface SearchQuery {
  text: string;
  filter: FilterSpec;
  sort: { field: SortField; order: SortOrder };
  libraryId?: string;
  libraryIds?: string[];
  collectionId?: string;
  includeSubcollections?: boolean;
  includeSubfolders?: boolean;
}

// ── Preview job ──────────────────────────────────────────────────────────
export interface PreviewJob {
  id: string;
  assetId: string;
  status: 'queued' | 'running' | 'done' | 'error';
  progress?: number; // 0–100
  errorMessage?: string;
  createdAt: number;
}

// ── IPC / Command interfaces (Tauri invoke payloads) ─────────────────────
export interface AddLibraryPayload {
  name: string;
  rootPaths: string[];
}

export interface SearchPayload {
  query: SearchQuery;
  page: number;
  pageSize: number;
}

export interface SearchResult {
  assets: Asset[];
  total: number;
  page: number;
  pageSize: number;
}
