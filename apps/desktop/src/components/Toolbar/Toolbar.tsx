import { useState } from 'react';
import { useAssetStore, useLibraryStore } from '../../store';
import { api } from '../../api';
import { open } from '@tauri-apps/plugin-dialog';
import { ToolbarSearch } from './ToolbarSearch';
import { ToolbarFilter } from './ToolbarFilter';
import { ToolbarControls } from './ToolbarControls';
import styles from './Toolbar.module.css';

function Brand() {
  return (
    <div className={styles.brand}>
      <div className={styles.logo}>
        <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
          <path d="M12 2L4 6v12l8 4 8-4V6L12 2Z" fill="hsl(220 85% 50%)" opacity="0.9" />
          <path d="M12 2v18M4 6l8 4 8-4" stroke="hsl(220 85% 70%)" strokeWidth="1.5" strokeLinecap="round" />
        </svg>
      </div>
      <span className={styles.appName}>PolyStash</span>
    </div>
  );
}

export function Toolbar() {
  const { search, loading } = useAssetStore();
  const { addLibrary } = useLibraryStore();
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

  const handleAddLibrary = async () => {
    try {
      const selectedPath = await open({
        directory: true,
        multiple: false,
        title: 'Select Library Folder',
      });
      if (selectedPath && typeof selectedPath === 'string') {
        const name = selectedPath.split(/[/\\]/).pop() || 'New Library';
        await addLibrary(name, [selectedPath]);
      }
    } catch (err) {
      console.error('Failed to add library:', err);
    }
  };

  return (
    <header className={styles.toolbar} data-tauri-drag-region>
      <Brand />
      <ToolbarSearch />
      <ToolbarFilter />
      <ToolbarControls
        onRefresh={handleRefresh}
        onAddLibrary={handleAddLibrary}
        isRefreshing={isRefreshing}
        loading={loading}
      />
    </header>
  );
}
