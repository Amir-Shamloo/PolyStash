import { useCallback, useEffect, useMemo, useState } from 'react';
import clsx from 'clsx';
import { open } from '@tauri-apps/plugin-dialog';
import { useUIStore, useAssetStore, useLibraryStore, type ActiveFolder } from '../../store';
import { api } from '../../api';
import { AssetCard } from './AssetCard';
import { AssetListRow } from './AssetListRow';
import { FilterBar } from './FilterBar';
import { useVisibleAssetPreviewRequest } from './useVisibleAssetPreviewRequest';
import { IconFolder } from '../Icons';
import type { Asset } from '../../types';
import styles from './AssetGrid.module.css';

const GRID_COLS: Record<'sm' | 'md' | 'lg', number> = {
  sm: 180,
  md: 220,
  lg: 280,
};

const INITIAL_VISIBLE_BATCH = 60;
const BATCH_INCREMENT = 60;

interface VirtualSubfolder {
  id: string;
  name: string;
  libraryId: string;
  relativePath: string;
}

function resolveTargetLibraryId(
  activeFolder: ActiveFolder | null,
  activeLibraryId: string | null,
  libraries: { id: string }[]
): string | null {
  return (
    activeFolder?.libraryId ??
    activeLibraryId ??
    (libraries.length === 1 ? libraries[0].id : null)
  );
}

function buildSubfolderItems(libraryId: string, paths: string[]): VirtualSubfolder[] {
  return paths.map((relPath) => {
    const clean = relPath.replace(/\\/g, '/').replace(/\/+$/, '');
    const name = clean.split('/').pop() || clean;
    return {
      id: `subfolder-${libraryId}-${clean}`,
      name,
      libraryId,
      relativePath: clean,
    };
  });
}

function useSubfolderItems(includeSubfolders: boolean): VirtualSubfolder[] {
  const activeFolder = useUIStore((s) => s.activeFolder);
  const activeLibraryId = useUIStore((s) => s.activeLibraryId);
  const libraries = useLibraryStore((s) => s.libraries);
  const [subfolders, setSubfolders] = useState<VirtualSubfolder[]>([]);

  const targetLibId = resolveTargetLibraryId(activeFolder, activeLibraryId, libraries);
  const parentPath = activeFolder?.relativePath ?? '';

  useEffect(() => {
    if (includeSubfolders || !targetLibId) {
      setSubfolders([]);
      return;
    }
    let cancelled = false;
    api
      .listSubdirectories(targetLibId, parentPath)
      .then((dirs) => {
        if (!cancelled) setSubfolders(buildSubfolderItems(targetLibId, dirs));
      })
      .catch(() => {
        if (!cancelled) setSubfolders([]);
      });
    return () => {
      cancelled = true;
    };
  }, [includeSubfolders, targetLibId, parentPath]);

  return subfolders;
}

function canObserveSentinel(node: HTMLDivElement | null, visible: number, total: number): node is HTMLDivElement {
  if (!node || visible >= total) return false;
  return typeof IntersectionObserver !== 'undefined';
}

function useViewportBatching(totalCount: number) {
  const [visibleCount, setVisibleCount] = useState(INITIAL_VISIBLE_BATCH);
  const [sentinelNode, setSentinelNode] = useState<HTMLDivElement | null>(null);

  useEffect(() => {
    setVisibleCount(INITIAL_VISIBLE_BATCH);
  }, [totalCount]);

  useEffect(() => {
    if (!canObserveSentinel(sentinelNode, visibleCount, totalCount)) return;
    const observer = new IntersectionObserver(
      (entries) => {
        if (entries.some((e) => e.isIntersecting)) {
          setVisibleCount((prev) => Math.min(totalCount, prev + BATCH_INCREMENT));
        }
      },
      { rootMargin: '300px' }
    );
    observer.observe(sentinelNode);
    return () => observer.disconnect();
  }, [sentinelNode, visibleCount, totalCount]);

  return { visibleCount, setSentinelNode };
}

function SubfolderItem({
  folder,
  className,
  iconSize,
  subLabel,
  title,
  onNavigate,
}: {
  folder: VirtualSubfolder;
  className: string;
  iconSize: number;
  subLabel: string;
  title?: string;
  onNavigate: (f: VirtualSubfolder) => void;
}) {
  return (
    <div
      id={folder.id}
      className={className}
      onClick={() => onNavigate(folder)}
      role="button"
      tabIndex={0}
      onKeyDown={(e) => e.key === 'Enter' && onNavigate(folder)}
      title={title}
    >
      <span className={styles.folderIconWrap}>
        <IconFolder size={iconSize} />
      </span>
      <span className={clsx(styles.folderCardName, 'truncate')}>{folder.name}</span>
      <span className={styles.folderCardSub}>{subLabel}</span>
    </div>
  );
}

function filterDisplayAssets(assets: Asset[], includeSubfolders: boolean): Asset[] {
  if (!includeSubfolders) return assets;
  return assets.filter((a) => a.kind !== 'folder');
}

interface GridItemsViewProps {
  viewMode: 'grid' | 'list';
  tileSize: number;
  subfolders: VirtualSubfolder[];
  visibleAssets: Asset[];
  selectedAssetIds: Set<string>;
  onNavigateSubfolder: (f: VirtualSubfolder) => void;
  onAssetClick: (asset: Asset, e: React.MouseEvent) => void;
}

function GridItemsView({
  viewMode,
  tileSize,
  subfolders,
  visibleAssets,
  selectedAssetIds,
  onNavigateSubfolder,
  onAssetClick,
}: GridItemsViewProps) {
  if (viewMode === 'list') {
    return (
      <div className={styles.list}>
        {subfolders.map((folder) => (
          <SubfolderItem
            key={folder.id}
            folder={folder}
            className={styles.folderListRow}
            iconSize={16}
            subLabel="Folder"
            onNavigate={onNavigateSubfolder}
          />
        ))}
        {visibleAssets.map((asset) => (
          <AssetListRow
            key={asset.id}
            asset={asset}
            selected={selectedAssetIds.has(asset.id)}
            onClick={onAssetClick}
          />
        ))}
      </div>
    );
  }

  return (
    <div
      className={styles.grid}
      style={{ '--tile-size': `${tileSize}px` } as React.CSSProperties}
    >
      {subfolders.map((folder) => (
        <SubfolderItem
          key={folder.id}
          folder={folder}
          className={styles.folderCard}
          iconSize={38}
          subLabel="Subfolder"
          title={`Open folder: ${folder.name}`}
          onNavigate={onNavigateSubfolder}
        />
      ))}
      {visibleAssets.map((asset) => (
        <AssetCard
          key={asset.id}
          asset={asset}
          selected={selectedAssetIds.has(asset.id)}
          onClick={onAssetClick}
        />
      ))}
    </div>
  );
}

function formatFooterSummary(displayed: number, total: number): string {
  if (displayed === total) {
    return `${total.toLocaleString()} ${total === 1 ? 'asset' : 'assets'}`;
  }
  return `${displayed.toLocaleString()} of ${total.toLocaleString()} assets`;
}

function useGridInteractions() {
  const { selectAsset, clearSelection, setActiveFolder } = useUIStore();
  const { query, search } = useAssetStore();

  const handleBackdropClick = useCallback(
    (e: React.MouseEvent) => {
      if (e.target === e.currentTarget) clearSelection();
    },
    [clearSelection]
  );

  const handleNavigateSubfolder = useCallback(
    (folder: VirtualSubfolder) => {
      setActiveFolder({ libraryId: folder.libraryId, relativePath: folder.relativePath });
      search();
    },
    [setActiveFolder, search]
  );

  const handleAssetClick = useCallback(
    (asset: Asset, e: React.MouseEvent) => {
      if (asset.kind === 'folder') {
        const libId = query.libraryId ?? useUIStore.getState().activeLibraryId;
        if (libId) {
          setActiveFolder({ libraryId: libId, relativePath: asset.filePath });
          search();
        }
        return;
      }
      selectAsset(asset.id, e.metaKey || e.ctrlKey);
    },
    [selectAsset, query.libraryId, setActiveFolder, search]
  );

  return { handleBackdropClick, handleNavigateSubfolder, handleAssetClick };
}

function LoadingSkeletonGrid({ tileSize }: { tileSize: number }) {
  return (
    <div className={styles.loadingGrid} style={{ '--tile-size': `${tileSize}px` } as React.CSSProperties}>
      {Array.from({ length: 24 }).map((_, i) => (
        <div key={i} className={clsx(styles.skeletonCard, 'skeleton')} />
      ))}
    </div>
  );
}

export function AssetGrid() {
  const { viewMode, gridSize, selectedAssetIds } = useUIStore();
  const { assets, loading, total, query } = useAssetStore();
  const { handleBackdropClick, handleNavigateSubfolder, handleAssetClick } = useGridInteractions();

  const includeSubfolders = query.includeSubfolders !== false && query.filter.includeSubfolders !== false;
  const subfolders = useSubfolderItems(includeSubfolders);
  const displayAssets = useMemo(
    () => filterDisplayAssets(assets, includeSubfolders),
    [assets, includeSubfolders]
  );

  const { visibleCount, setSentinelNode } = useViewportBatching(displayAssets.length);
  const visibleAssets = useMemo(
    () => displayAssets.slice(0, visibleCount),
    [displayAssets, visibleCount]
  );

  const [scrollNode, setScrollNode] = useState<HTMLDivElement | null>(null);
  useVisibleAssetPreviewRequest(scrollNode, visibleAssets);

  const tileSize = GRID_COLS[gridSize];
  const isEmpty = displayAssets.length === 0 && subfolders.length === 0;

  if (isEmpty) {
    return (
      <div className={styles.container}>
        <FilterBar />
        {loading ? <LoadingSkeletonGrid tileSize={tileSize} /> : <EmptyState />}
      </div>
    );
  }

  return (
    <div className={styles.container}>
      <FilterBar />
      <div ref={setScrollNode} className={styles.scrollArea} onClick={handleBackdropClick}>
        <GridItemsView
          viewMode={viewMode}
          tileSize={tileSize}
          subfolders={includeSubfolders ? [] : subfolders}
          visibleAssets={visibleAssets}
          selectedAssetIds={selectedAssetIds}
          onNavigateSubfolder={handleNavigateSubfolder}
          onAssetClick={handleAssetClick}
        />
        {visibleCount < displayAssets.length && (
          <div ref={setSentinelNode} style={{ height: 1, width: '100%' }} aria-hidden />
        )}
      </div>
      <div className={styles.footer}>
        <span className={styles.footerCount}>
          {formatFooterSummary(displayAssets.length, total)}
        </span>
      </div>
    </div>
  );
}

function EmptyState() {
  const libraries = useLibraryStore((s) => s.libraries);
  const addLibrary = useLibraryStore((s) => s.addLibrary);
  const resetFilter = useAssetStore((s) => s.resetFilter);
  const search = useAssetStore((s) => s.search);

  const handleAddLibrary = async () => {
    try {
      const selectedPath = await open({
        directory: true,
        multiple: false,
        title: 'Select Library Folder',
      });
      if (selectedPath && typeof selectedPath === 'string') {
        const name = selectedPath.split(/[/\\]/).filter(Boolean).pop() || 'New Library';
        await addLibrary(name, [selectedPath]);
        search();
      }
    } catch (err) {
      console.error('Failed to add library:', err);
    }
  };

  const handleClearFilters = () => {
    resetFilter();
    search();
  };

  const hasLibraries = libraries.length > 0;

  return (
    <div className={styles.empty}>
      <div className={styles.emptyIcon}>
        <svg viewBox="0 0 80 80" fill="none" xmlns="http://www.w3.org/2000/svg">
          <rect x="10" y="20" width="60" height="48" rx="6"
            fill="hsl(220 13% 16%)" stroke="hsl(220 10% 25%)" strokeWidth="1.5"/>
          <path d="M28 36l8 8 8-8" stroke="hsl(220 85% 50%)" strokeWidth="2"
            strokeLinecap="round" strokeLinejoin="round"/>
          <circle cx="40" cy="12" r="8" fill="hsl(220 85% 50% / 0.15)"
            stroke="hsl(220 85% 50%)" strokeWidth="1.5"/>
          <path d="M40 8v8M36 12h8" stroke="hsl(220 85% 50%)" strokeWidth="1.5"
            strokeLinecap="round"/>
        </svg>
      </div>
      <h2 className={styles.emptyTitle}>
        {hasLibraries ? 'No matching assets found' : 'No libraries yet'}
      </h2>
      <p className={styles.emptyText}>
        {hasLibraries
          ? 'No assets match your current search or filter criteria.'
          : 'Add a library folder to start browsing your 3D assets, materials, textures, and videos.'}
      </p>
      <div className={styles.emptyActions}>
        {hasLibraries && (
          <button
            type="button"
            className={styles.emptyBtn}
            id="empty-clear-filters"
            onClick={handleClearFilters}
          >
            Clear Filters
          </button>
        )}
        <button
          type="button"
          className={hasLibraries ? styles.emptyBtnSecondary : styles.emptyBtn}
          id="empty-add-library"
          onClick={handleAddLibrary}
        >
          + Add Library
        </button>
      </div>
    </div>
  );
}
