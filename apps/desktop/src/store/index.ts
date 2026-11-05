import { create } from 'zustand';
import { persist } from 'zustand/middleware';
import type {
  Asset,
  Library,
  Collection,
  Tag,
  SearchQuery,
  FilterSpec,
  SortField,
  SortOrder,
} from '../types';
import { api } from '../api';

// ── Folder Sentinels & Types ──────────────────────────────────────────────
export interface ActiveFolder {
  libraryId: string | null;
  relativePath: string;
}

export function buildFolderId(active: ActiveFolder): string {
  if (!active.libraryId) {
    return 'root';
  }
  if (!active.relativePath) {
    return `lib:${active.libraryId}`;
  }
  return `dir:${active.libraryId}:${active.relativePath}`;
}

export function parseFolderId(id: string): ActiveFolder {
  if (id === 'root') {
    return { libraryId: null, relativePath: '' };
  }
  if (id.startsWith('lib:')) {
    return { libraryId: id.slice(4), relativePath: '' };
  }
  if (id.startsWith('dir:')) {
    const rest = id.slice(4);
    const colonIdx = rest.indexOf(':');
    if (colonIdx === -1) {
      return { libraryId: rest, relativePath: '' };
    }
    return {
      libraryId: rest.slice(0, colonIdx),
      relativePath: rest.slice(colonIdx + 1),
    };
  }
  return { libraryId: null, relativePath: '' };
}

export function folderToDirectoryPrefix(folder: ActiveFolder | null): string | undefined {
  if (!folder || folder.libraryId === null) {
    return undefined;
  }
  return folder.relativePath;
}

export function folderToSearchQuery(
  folder: ActiveFolder | null,
  currentFilter: FilterSpec = {}
): Partial<SearchQuery> {
  const directoryPrefix = folderToDirectoryPrefix(folder);
  const nextFilter: FilterSpec = { ...currentFilter };
  if (directoryPrefix === undefined) {
    delete nextFilter.directoryPrefix;
  } else {
    nextFilter.directoryPrefix = directoryPrefix;
  }
  return {
    libraryId: folder?.libraryId ?? undefined,
    libraryIds: undefined,
    collectionId: undefined,
    filter: nextFilter,
  };
}

// ── UI State ──────────────────────────────────────────────────────────────
interface UIState {
  viewMode: 'grid' | 'list';
  gridSize: 'sm' | 'md' | 'lg';
  sidebarOpen: boolean;
  detailPanelOpen: boolean;
  selectedAssetIds: Set<string>;
  focusedAssetId: string | null;
  activeLibraryId: string | null;
  activeCollectionId: string | null;
  activeFolder: ActiveFolder | null;
  folderTreeVersion: number;

  setViewMode: (mode: 'grid' | 'list') => void;
  setGridSize: (size: 'sm' | 'md' | 'lg') => void;
  toggleSidebar: () => void;
  toggleDetailPanel: () => void;
  selectAsset: (id: string, multi?: boolean) => void;
  selectAll: (ids: string[]) => void;
  clearSelection: () => void;
  focusAsset: (id: string | null) => void;
  selectedLibraryIds: string[];
  setSelectedLibraryIds: (ids: string[]) => void;
  toggleSelectedLibrary: (id: string, multi?: boolean) => void;
  clearLibrarySelection: () => void;
  setActiveLibrary: (id: string | null) => void;
  setActiveCollection: (id: string | null) => void;
  setActiveFolder: (folder: ActiveFolder | null) => void;
  refreshFolderTree: () => void;
}

export const useUIStore = create<UIState>()(
  persist(
    (set) => ({
      viewMode: 'grid',
      gridSize: 'md',
      sidebarOpen: true,
      detailPanelOpen: true,
      selectedAssetIds: new Set(),
      focusedAssetId: null,
      activeLibraryId: null,
      selectedLibraryIds: [],
      activeCollectionId: null,
      activeFolder: null,
      folderTreeVersion: 0,

      setViewMode: (viewMode) => set({ viewMode }),
      setGridSize: (gridSize) => set({ gridSize }),
      toggleSidebar: () => set((s) => ({ sidebarOpen: !s.sidebarOpen })),
      toggleDetailPanel: () => set((s) => ({ detailPanelOpen: !s.detailPanelOpen })),

      selectAsset: (id, multi = false) =>
        set((s) => {
          if (multi) {
            const next = new Set(s.selectedAssetIds);
            if (next.has(id)) next.delete(id);
            else next.add(id);
            return { selectedAssetIds: next, focusedAssetId: id };
          }
          return { selectedAssetIds: new Set([id]), focusedAssetId: id };
        }),

      selectAll: (ids) =>
        set({ selectedAssetIds: new Set(ids) }),

      clearSelection: () =>
        set({ selectedAssetIds: new Set(), focusedAssetId: null }),

      focusAsset: (id) => set({ focusedAssetId: id }),

      setSelectedLibraryIds: (ids) =>
        set({ selectedLibraryIds: ids, activeLibraryId: ids[0] ?? null, activeCollectionId: null }),

      toggleSelectedLibrary: (id, multi = false) =>
        set((s) => {
          let next: string[];
          if (multi) {
            next = s.selectedLibraryIds.includes(id)
              ? s.selectedLibraryIds.filter((i) => i !== id)
              : [...s.selectedLibraryIds, id];
          } else {
            next = s.selectedLibraryIds.length === 1 && s.selectedLibraryIds[0] === id ? [] : [id];
          }
          return {
            selectedLibraryIds: next,
            activeLibraryId: next.length === 1 ? next[0] : null,
            activeCollectionId: null,
          };
        }),

      clearLibrarySelection: () =>
        set({ selectedLibraryIds: [], activeLibraryId: null, activeFolder: null }),

      setActiveLibrary: (id) =>
        set({ activeLibraryId: id, selectedLibraryIds: id ? [id] : [], activeCollectionId: null }),
      setActiveCollection: (id) =>
        set({ activeCollectionId: id, activeLibraryId: null, selectedLibraryIds: [], activeFolder: null }),
      setActiveFolder: (activeFolder) => {
        set({
          activeFolder,
          activeLibraryId: activeFolder?.libraryId ?? null,
          selectedLibraryIds: activeFolder?.libraryId ? [activeFolder.libraryId] : [],
          activeCollectionId: null,
        });
        const currentFilter = useAssetStore.getState().query.filter;
        useAssetStore.getState().setQuery(folderToSearchQuery(activeFolder, currentFilter));
      },
      refreshFolderTree: () =>
        set((s) => ({ folderTreeVersion: s.folderTreeVersion + 1 })),
    }),
    {
      name: 'polystash-ui',
      partialize: (s) => ({
        viewMode: s.viewMode,
        gridSize: s.gridSize,
        sidebarOpen: s.sidebarOpen,
        detailPanelOpen: s.detailPanelOpen,
      }),
    }
  )
);

// ── Library Store ─────────────────────────────────────────────────────────
interface LibraryStore {
  libraries: Library[];
  loading: boolean;
  fetchLibraries: () => Promise<void>;
  addLibrary: (name: string, rootPaths: string[]) => Promise<void>;
  removeLibrary: (id: string) => Promise<void>;
}

export const useLibraryStore = create<LibraryStore>()((set) => ({
  libraries: [],
  loading: false,

  fetchLibraries: async () => {
    set({ loading: true });
    try {
      const libraries = await api.listLibraries();
      set({ libraries });
    } finally {
      set({ loading: false });
    }
  },

  addLibrary: async (name, rootPaths) => {
    const lib = await api.addLibrary({ name, rootPaths });
    set((s) => ({ libraries: [...s.libraries, lib] }));
    useUIStore.getState().refreshFolderTree();
  },

  removeLibrary: async (id) => {
    await api.removeLibrary(id);
    set((s) => ({ libraries: s.libraries.filter((l) => l.id !== id) }));
    useUIStore.getState().refreshFolderTree();
  },
}));

// ── Asset / Search Store ──────────────────────────────────────────────────
interface AssetStore {
  assets: Asset[];
  total: number;
  page: number;
  pageSize: number;
  loading: boolean;
  query: SearchQuery;
  assetMap: Map<string, Asset>;

  setQuery: (q: Partial<SearchQuery>) => void;
  setFilter: (f: Partial<FilterSpec>) => void;
  replaceFilter: (f: FilterSpec, collectionId?: string) => void;
  resetFilter: () => void;
  clearFilter: (key: keyof FilterSpec) => void;
  setSort: (field: SortField, order?: SortOrder) => void;
  setPage: (page: number) => void;
  search: () => Promise<void>;
  patchAsset: (id: string, patch: Partial<Asset>) => Promise<void>;
  deleteAsset: (id: string, deleteFile?: boolean) => Promise<void>;
  deleteAssets: (ids: string[], deleteFile?: boolean) => Promise<void>;
  renameAsset: (id: string, newName: string) => Promise<Asset>;
  updateAssetPreview: (id: string, thumbnailPath?: string, previewModel?: string) => void;
  setPreviewStatus: (id: string, status: Asset['previewStatus']) => void;
  getById: (id: string) => Asset | undefined;
}

const defaultQuery: SearchQuery = {
  text: '',
  filter: {},
  sort: { field: 'date', order: 'desc' },
  includeSubcollections: true,
};

export const useAssetStore = create<AssetStore>()((set, get) => ({
  assets: [],
  total: 0,
  page: 0,
  pageSize: 100,
  loading: false,
  query: defaultQuery,
  assetMap: new Map(),

  setQuery: (q) => set((s) => ({ query: { ...s.query, ...q }, page: 0 })),

  setFilter: (f) =>
    set((s) => ({
      query: { ...s.query, filter: { ...s.query.filter, ...f } },
      page: 0,
    })),

  replaceFilter: (f, collectionId) =>
    set((s) => ({
      query: { ...s.query, collectionId, filter: f },
      page: 0,
    })),

  resetFilter: () =>
    set((s) => ({
      query: { ...s.query, collectionId: undefined, filter: {} },
      page: 0,
    })),

  clearFilter: (key) =>
    set((s) => {
      const filter = { ...s.query.filter };
      delete filter[key];
      return { query: { ...s.query, filter }, page: 0 };
    }),

  setSort: (field, order) =>
    set((s) => {
      const currentOrder = s.query.sort.order;
      return {
        query: {
          ...s.query,
          sort: {
            field,
            order: order ?? (field === s.query.sort.field && currentOrder === 'desc' ? 'asc' : 'desc'),
          },
        },
        page: 0,
      };
    }),

  setPage: (page) => set({ page }),

  search: async () => {
    const { query, page, pageSize } = get();
    set({ loading: true });
    try {
      const result = await api.searchAssets({ query, page, pageSize });
      const assetMap = new Map(result.assets.map((a) => [a.id, a]));
      set({ assets: result.assets, total: result.total, assetMap });
    } finally {
      set({ loading: false });
    }
  },

  patchAsset: async (id, patch) => {
    const updated = await api.updateAsset(id, patch);
    set((s) => ({
      assets: s.assets.map((a) => (a.id === id ? updated : a)),
      assetMap: new Map(s.assetMap).set(id, updated),
    }));
  },

  deleteAsset: async (id, deleteFile = true) => {
    await api.deleteAsset(id, deleteFile);
    set((s) => {
      const assets = s.assets.filter((a) => a.id !== id);
      const assetMap = new Map(s.assetMap);
      assetMap.delete(id);
      return { assets, total: Math.max(0, s.total - 1), assetMap };
    });
    const ui = useUIStore.getState();
    if (ui.focusedAssetId === id) {
      ui.focusAsset(null);
    }
    if (ui.selectedAssetIds.has(id)) {
      const next = new Set(ui.selectedAssetIds);
      next.delete(id);
      ui.selectAll(Array.from(next));
    }
  },

  deleteAssets: async (ids, deleteFile = true) => {
    await api.deleteAssets(ids, deleteFile);
    const idSet = new Set(ids);
    set((s) => {
      const assets = s.assets.filter((a) => !idSet.has(a.id));
      const assetMap = new Map(s.assetMap);
      for (const id of ids) assetMap.delete(id);
      return { assets, total: Math.max(0, s.total - ids.length), assetMap };
    });
    const ui = useUIStore.getState();
    if (ui.focusedAssetId && idSet.has(ui.focusedAssetId)) {
      ui.focusAsset(null);
    }
    const next = new Set(Array.from(ui.selectedAssetIds).filter((id) => !idSet.has(id)));
    ui.selectAll(Array.from(next));
  },

  renameAsset: async (id, newName) => {
    const updated = await api.renameAsset(id, newName);
    set((s) => ({
      assets: s.assets.map((a) => (a.id === id ? updated : a)),
      assetMap: new Map(s.assetMap).set(id, updated),
    }));
    return updated;
  },

  updateAssetPreview: (id, thumbnailPath, previewModel) => {
    set((s) => {
      const asset = s.assetMap.get(id);
      if (!asset) return s;
      const updated: Asset = {
        ...asset,
        thumbnailPath: thumbnailPath || asset.thumbnailPath,
        previewStatus: 'done',
        metadata: previewModel ? { ...asset.metadata, preview_model: previewModel } : asset.metadata,
      };
      return {
        assets: s.assets.map((a) => (a.id === id ? updated : a)),
        assetMap: new Map(s.assetMap).set(id, updated),
      };
    });
  },

  setPreviewStatus: (id, status) => {
    set((s) => {
      const asset = s.assetMap.get(id);
      if (!asset) return s;
      const updated: Asset = { ...asset, previewStatus: status };
      return {
        assets: s.assets.map((a) => (a.id === id ? updated : a)),
        assetMap: new Map(s.assetMap).set(id, updated),
      };
    });
  },

  getById: (id) => get().assetMap.get(id),
}));

// ── Tag Store ─────────────────────────────────────────────────────────────
interface TagStore {
  tags: Tag[];
  tagMap: Map<string, Tag>;
  fetchTags: () => Promise<void>;
  createTag: (name: string, parentId?: string) => Promise<void>;
  deleteTag: (id: string) => Promise<void>;
}

export const useTagStore = create<TagStore>()((set) => ({
  tags: [],
  tagMap: new Map(),

  fetchTags: async () => {
    const tags = await api.listTags();
    set({ tags, tagMap: new Map(tags.map((t) => [t.id, t])) });
  },

  createTag: async (name, parentId) => {
    const tag = await api.createTag(name, parentId);
    set((s) => ({
      tags: [...s.tags, tag],
      tagMap: new Map(s.tagMap).set(tag.id, tag),
    }));
  },

  deleteTag: async (id) => {
    await api.deleteTag(id);
    set((s) => {
      const tagMap = new Map(s.tagMap);
      tagMap.delete(id);
      return { tags: s.tags.filter((t) => t.id !== id), tagMap };
    });
  },
}));

// ── Collection Store ──────────────────────────────────────────────────────
interface CollectionStore {
  collections: Collection[];
  fetchCollections: () => Promise<void>;
  createCollection: (
    name: string,
    description?: string,
    parentId?: string,
  ) => Promise<void>;
  addToCollection: (collectionId: string, assetIds: string[]) => Promise<void>;
  removeFromCollection: (collectionId: string, assetIds: string[]) => Promise<void>;
  deleteCollection: (id: string) => Promise<void>;
}

export const useCollectionStore = create<CollectionStore>()((set) => ({
  collections: [],

  fetchCollections: async () => {
    const collections = await api.listCollections();
    set({ collections });
  },

  createCollection: async (name, description, parentId) => {
    const col = await api.createCollection(name, description, parentId);
    set((s) => ({ collections: [...s.collections, col] }));
  },

  addToCollection: async (collectionId, assetIds) => {
    await api.addToCollection(collectionId, assetIds);
    set((s) => ({
      collections: s.collections.map((c) =>
        c.id === collectionId
          ? { ...c, assetIds: [...new Set([...c.assetIds, ...assetIds])] }
          : c
      ),
    }));
    useAssetStore.setState((s) => {
      const assets = s.assets.map((a) => {
        if (assetIds.includes(a.id)) {
          const cur = a.collections ?? [];
          return { ...a, collections: [...new Set([...cur, collectionId])] };
        }
        return a;
      });
      const assetMap = new Map(s.assetMap);
      for (const aid of assetIds) {
        const a = assetMap.get(aid);
        if (a) {
          const cur = a.collections ?? [];
          assetMap.set(aid, { ...a, collections: [...new Set([...cur, collectionId])] });
        }
      }
      return { assets, assetMap };
    });
  },

  removeFromCollection: async (collectionId, assetIds) => {
    await api.removeFromCollection(collectionId, assetIds);
    set((s) => ({
      collections: s.collections.map((c) =>
        c.id === collectionId
          ? { ...c, assetIds: c.assetIds.filter((id) => !assetIds.includes(id)) }
          : c
      ),
    }));
    useAssetStore.setState((s) => {
      const assets = s.assets.map((a) => {
        if (assetIds.includes(a.id)) {
          const cur = a.collections ?? [];
          return { ...a, collections: cur.filter((cid) => cid !== collectionId) };
        }
        return a;
      });
      const assetMap = new Map(s.assetMap);
      for (const aid of assetIds) {
        const a = assetMap.get(aid);
        if (a) {
          const cur = a.collections ?? [];
          assetMap.set(aid, { ...a, collections: cur.filter((cid) => cid !== collectionId) });
        }
      }
      return { assets, assetMap };
    });
  },

  deleteCollection: async (id) => {
    await api.deleteCollection(id);
    set((s) => ({ collections: s.collections.filter((c) => c.id !== id) }));
  },
}));
