import { useCallback, useState } from 'react';
import clsx from 'clsx';
import { useUIStore, useAssetStore } from '../../store';
import { api } from '../../api';
import {
  IconGrid,
  IconList,
  IconSearch,
  IconRefresh,
} from '../Icons';
import { AssetTypeDropdown } from './AssetTypeDropdown';
import styles from './Toolbar.module.css';

interface ViewControlsProps {
  viewMode: 'grid' | 'list';
  gridSize: 'sm' | 'md' | 'lg';
  onSetViewMode: (mode: 'grid' | 'list') => void;
  onSetGridSize: (size: 'sm' | 'md' | 'lg') => void;
}

function ViewControls({ viewMode, gridSize, onSetViewMode, onSetGridSize }: ViewControlsProps) {
  return (
    <>
      {viewMode === 'grid' && (
        <div className={styles.buttonGroup} role="group" aria-label="Grid size">
          {(['sm', 'md', 'lg'] as const).map((size) => (
            <button
              key={size}
              id={`grid-size-${size}`}
              type="button"
              className={clsx(styles.iconBtn, gridSize === size && styles.active)}
              onClick={() => onSetGridSize(size)}
              title={`Grid size: ${size.toUpperCase()}`}
              aria-label={`Grid size ${size}`}
            >
              <span
                className={styles.gridSizeDot}
                style={{ fontSize: size === 'sm' ? 8 : size === 'md' ? 10 : 12 }}
              >
                ⬛
              </span>
            </button>
          ))}
        </div>
      )}

      <div className={styles.buttonGroup} role="group" aria-label="View mode">
        <button
          id="view-grid"
          type="button"
          className={clsx(styles.iconBtn, viewMode === 'grid' && styles.active)}
          onClick={() => onSetViewMode('grid')}
          title="Grid view"
          aria-label="Grid view"
        >
          <IconGrid />
        </button>
        <button
          id="view-list"
          type="button"
          className={clsx(styles.iconBtn, viewMode === 'list' && styles.active)}
          onClick={() => onSetViewMode('list')}
          title="List view"
          aria-label="List view"
        >
          <IconList />
        </button>
      </div>
    </>
  );
}

export function Toolbar() {
  const { viewMode, gridSize, setViewMode, setGridSize } = useUIStore();
  const { query, setQuery, search, loading } = useAssetStore();
  const [isRefreshing, setIsRefreshing] = useState(false);

  const handleRefresh = async () => {
    setIsRefreshing(true);
    try {
      await api.refreshAllLibraries();
      await search();
    } catch (err) {
      console.error('Failed to refresh libraries:', err);
    } finally {
      setIsRefreshing(false);
    }
  };

  const handleSearch = useCallback(
    (e: React.ChangeEvent<HTMLInputElement>) => {
      setQuery({ text: e.target.value });
      search();
    },
    [setQuery, search]
  );

  return (
    <header className={styles.toolbar} data-tauri-drag-region>
      {/* App brand: Logo + Name */}
      <div className={styles.brand}>
        <div className={styles.logo}>
          <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M12 2L4 6v12l8 4 8-4V6L12 2Z" fill="hsl(220 85% 50%)" opacity="0.9" />
            <path d="M12 2v18M4 6l8 4 8-4" stroke="hsl(220 85% 70%)" strokeWidth="1.5" strokeLinecap="round" />
          </svg>
        </div>
        <span className={styles.appName}>PolyStash</span>
      </div>

      {/* Global Search Bar */}
      <div className={styles.searchSection}>
        <div className={styles.searchWrap}>
          <IconSearch className={styles.searchIcon} />
          <input
            id="toolbar-search"
            type="search"
            placeholder="Search assets, tags, metadata…"
            className={styles.searchInput}
            value={query.text}
            onChange={handleSearch}
            aria-label="Search assets"
          />
          {loading && <div className={clsx(styles.spinner, 'animate-spin')} />}
        </div>
      </div>

      {/* Center/Right: Creative Asset Type Dropdown Filter */}
      <div className={styles.typeFilterSection}>
        <AssetTypeDropdown />
      </div>

      {/* Controls: Grid Size, View Mode, Refresh */}
      <div className={styles.controls}>
        <ViewControls
          viewMode={viewMode}
          gridSize={gridSize}
          onSetViewMode={setViewMode}
          onSetGridSize={setGridSize}
        />

        <div className={styles.divider} />

        <button
          id="refresh-library"
          type="button"
          className={styles.iconBtn}
          onClick={handleRefresh}
          title="Refresh and scan libraries (F5)"
          aria-label="Refresh and scan libraries"
        >
          <IconRefresh className={loading || isRefreshing ? 'animate-spin' : undefined} />
        </button>
      </div>
    </header>
  );
}
