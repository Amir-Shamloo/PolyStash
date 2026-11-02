import { memo } from 'react';
import { useUIStore } from '../../store';
import {
  IconGrid, IconList, IconSidebar,
  IconPanel, IconRefresh, IconPlus,
} from '../Icons';
import styles from './Toolbar.module.css';
import clsx from 'clsx';

const GRID_SIZES = ['sm', 'md', 'lg'] as const;
const DOT_SIZES = { sm: 8, md: 10, lg: 12 } as const;

export const ViewModeControls = memo(function ViewModeControls() {
  const {
    viewMode,
    gridSize,
    sidebarOpen,
    detailPanelOpen,
    setViewMode,
    setGridSize,
    toggleSidebar,
    toggleDetailPanel,
  } = useUIStore();

  return (
    <>
      {viewMode === 'grid' && (
        <div className={styles.buttonGroup}>
          {GRID_SIZES.map((size) => (
            <button
              key={size}
              id={`grid-size-${size}`}
              className={clsx(styles.iconBtn, gridSize === size && styles.active)}
              onClick={() => setGridSize(size)}
              title={`Grid size: ${size}`}
            >
              <span className={styles.gridSizeDot} style={{ fontSize: DOT_SIZES[size] }}>
                ⬛
              </span>
            </button>
          ))}
        </div>
      )}

      <div className={styles.buttonGroup}>
        <button
          id="view-grid"
          className={clsx(styles.iconBtn, viewMode === 'grid' && styles.active)}
          onClick={() => setViewMode('grid')}
          title="Grid view"
        >
          <IconGrid />
        </button>
        <button
          id="view-list"
          className={clsx(styles.iconBtn, viewMode === 'list' && styles.active)}
          onClick={() => setViewMode('list')}
          title="List view"
        >
          <IconList />
        </button>
      </div>

      <div className={styles.divider} />

      <button
        id="toggle-sidebar"
        className={clsx(styles.iconBtn, !sidebarOpen && styles.dimmed)}
        onClick={toggleSidebar}
        title="Toggle sidebar"
      >
        <IconSidebar />
      </button>
      <button
        id="toggle-detail"
        className={clsx(styles.iconBtn, !detailPanelOpen && styles.dimmed)}
        onClick={toggleDetailPanel}
        title="Toggle detail panel"
      >
        <IconPanel />
      </button>
    </>
  );
});

interface ToolbarControlsProps {
  onRefresh: () => void;
  onAddLibrary: () => void;
  isRefreshing: boolean;
  loading: boolean;
}

export const ToolbarControls = memo(function ToolbarControls({
  onRefresh,
  onAddLibrary,
  isRefreshing,
  loading,
}: ToolbarControlsProps) {
  const isSpinning = loading || isRefreshing;

  return (
    <div className={styles.controls}>
      <ViewModeControls />
      <div className={styles.divider} />

      <button
        id="refresh-library"
        className={styles.iconBtn}
        onClick={onRefresh}
        title="Refresh and scan libraries"
      >
        <IconRefresh className={isSpinning ? 'animate-spin' : undefined} />
      </button>

      <button
        id="add-library"
        className={styles.primaryBtn}
        title="Add Library"
        onClick={onAddLibrary}
      >
        <IconPlus />
        <span>Add Library</span>
      </button>
    </div>
  );
});
