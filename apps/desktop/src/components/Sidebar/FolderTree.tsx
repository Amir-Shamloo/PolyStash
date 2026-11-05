import { useState, useEffect } from 'react';
import clsx from 'clsx';
import {
  asyncDataLoaderFeature,
  selectionFeature,
  hotkeysCoreFeature,
  type ItemInstance,
} from '@headless-tree/core';
import { useTree } from '@headless-tree/react';
import {
  useLibraryStore,
  useUIStore,
  useAssetStore,
  parseFolderId,
  buildFolderId,
  type ActiveFolder,
} from '../../store';
import { fetchTreeChildren, pruneStaleItems, resolveItemName } from './folderTreeData';
import { IconFolder, IconChevronDown, IconChevronRight } from '../Icons';
import styles from './FolderTree.module.css';

// Module-scoped so expand state survives the `key` remount after an index refresh.
let savedExpandedItems: string[] = [];

interface FolderTreeRowProps {
  item: ItemInstance<string>;
  onSelect: (folder: ActiveFolder) => void;
}

function ChevronIcon({ hasChildren, isLoading, isExpanded }: { hasChildren: boolean; isLoading: boolean; isExpanded: boolean }) {
  if (!hasChildren) return <span className={styles.emptySlot} />;
  if (isLoading) return <span className={styles.spinner} aria-label="Loading" />;
  return (
    <span className={styles.chevron}>
      {isExpanded ? <IconChevronDown size={11} /> : <IconChevronRight size={11} />}
    </span>
  );
}

function FolderTreeRow({ item, onSelect }: FolderTreeRowProps) {
  const itemProps = item.getProps();
  const isExpanded = item.isExpanded();
  const isLoading = item.isLoading();
  const isSelected = item.isSelected();
  const level = item.getItemMeta().level;
  // A folder only gets a chevron when it actually has subdirectories to reveal.
  // headless-tree resolves children lazily, so a not-yet-loaded folder reports
  // zero children; `isFolder()` covers those and avoids hiding the arrow.
  const hasChildren = isLoading || item.isFolder() || item.getChildren().length > 0;

  return (
    <button
      type="button"
      {...itemProps}
      id={`folder-${item.getId().replace(/[:/]/g, '-')}`}
      className={clsx(styles.treeRow, itemProps.className, isSelected && styles.selected)}
      style={{
        paddingLeft: `${level * 14 + 4}px`,
        ...itemProps.style,
      }}
      onClick={(e) => {
        itemProps.onClick?.(e);
        onSelect(parseFolderId(item.getId()));
      }}
      title={item.getItemName()}
    >
      <span
        className={styles.chevronSlot}
        onClick={(e) => {
          e.stopPropagation();
          if (!hasChildren) return;
          if (isExpanded) item.collapse();
          else item.expand();
        }}
      >
        <ChevronIcon hasChildren={hasChildren} isLoading={isLoading} isExpanded={isExpanded} />
      </span>

      <span className={styles.iconSlot}>
        <IconFolder size={13} />
      </span>

      <span className={clsx(styles.name, 'truncate')}>{item.getItemName()}</span>
    </button>
  );
}

export function FolderTree() {
  const libraries = useLibraryStore((s) => s.libraries);
  const activeFolder = useUIStore((s) => s.activeFolder);
  const setActiveFolder = useUIStore((s) => s.setActiveFolder);
  const search = useAssetStore((s) => s.search);

  const [expandedItems, setExpandedItems] = useState<string[]>(() =>
    pruneStaleItems(savedExpandedItems, libraries.map((l) => l.id))
  );
  const [selectedItems, setSelectedItems] = useState<string[]>(() =>
    activeFolder ? [buildFolderId(activeFolder)] : []
  );

  useEffect(() => {
    const target = activeFolder ? buildFolderId(activeFolder) : '';
    setSelectedItems(target ? [target] : []);
  }, [activeFolder]);

  // A removed library must not keep filtering the grid.
  useEffect(() => {
    if (activeFolder && !libraries.some((l) => l.id === activeFolder.libraryId)) {
      setActiveFolder(null);
    }
  }, [libraries, activeFolder, setActiveFolder]);

  const tree = useTree<string>({
    rootItemId: 'root',
    getItemName: (item) => resolveItemName(item.getId(), libraries, item.getItemData()),
    isItemFolder: () => true,
    dataLoader: {
      getItem: (id) => id,
      getChildren: fetchTreeChildren,
    },
    state: { selectedItems, expandedItems },
    setSelectedItems,
    setExpandedItems: (updaterOrValue) => {
      setExpandedItems((prev) => {
        const next = typeof updaterOrValue === 'function' ? updaterOrValue(prev) : updaterOrValue;
        savedExpandedItems = next;
        return next;
      });
    },
    features: [asyncDataLoaderFeature, selectionFeature, hotkeysCoreFeature],
  });

  if (libraries.length === 0) {
    return <p className={styles.empty}>No libraries yet</p>;
  }

  return (
    <div {...tree.getContainerProps()} className={styles.treeContainer}>
      {tree.getItems().map((item) => (
        <FolderTreeRow
          key={item.getId()}
          item={item}
          onSelect={(folder) => {
            setActiveFolder(folder);
            search();
          }}
        />
      ))}
    </div>
  );
}
