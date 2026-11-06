import { useCallback, useState, useEffect } from 'react';
import clsx from 'clsx';
import { useUIStore, useAssetStore } from '../../store';
import type { Asset, ColorLabel } from '../../types';
import { IconInfo } from '../Icons';
import { ConfirmDeleteDialog } from '../ConfirmDeleteDialog';
import { DetailPreviewArea } from './DetailPreviewArea';
import { DetailPanelHeader } from './DetailPanelHeader';
import { useModelSnapshot } from '../../lib/previewSnapshot';
import { InfoTab } from './InfoTab';
import { OrganizeTab } from './OrganizeTab';
import styles from './DetailPanel.module.css';

function useAssetMutations(asset: Asset | null) {
  const patchAsset = useAssetStore((s) => s.patchAsset);
  const deleteAsset = useAssetStore((s) => s.deleteAsset);
  const renameAsset = useAssetStore((s) => s.renameAsset);

  const setRating = useCallback(
    (rating: number) => {
      if (asset) patchAsset(asset.id, { rating });
    },
    [asset, patchAsset]
  );

  const setColorLabel = useCallback(
    (label: ColorLabel) => {
      if (asset) {
        patchAsset(asset.id, { colorLabel: label === asset.colorLabel ? 'none' : label });
      }
    },
    [asset, patchAsset]
  );

  const toggleFavorite = useCallback(() => {
    if (asset) patchAsset(asset.id, { isFavorite: !asset.isFavorite });
  }, [asset, patchAsset]);

  const toggleTag = useCallback(
    (tagId: string) => {
      if (!asset) return;
      const next = asset.tags.includes(tagId)
        ? asset.tags.filter((t) => t !== tagId)
        : [...asset.tags, tagId];
      patchAsset(asset.id, { tags: next });
    },
    [asset, patchAsset]
  );

  return { setRating, setColorLabel, toggleFavorite, toggleTag, deleteAsset, renameAsset };
}

type TabKey = 'info' | 'organize';

export function DetailPanel() {
  const focusedAssetId = useUIStore((s) => s.focusedAssetId);
  const setDetailPanelOpen = useUIStore((s) => s.setDetailPanelOpen);
  const asset = useAssetStore((s) => {
    if (!focusedAssetId) return null;
    return s.assets.find((a) => a.id === focusedAssetId) ?? s.assetMap.get(focusedAssetId) ?? null;
  });

  const [tab, setTab] = useState<TabKey>('info');
  const [isEditingName, setIsEditingName] = useState(false);
  const [editingName, setEditingName] = useState('');
  const [nameError, setNameError] = useState<string | null>(null);
  const [isDeleteDialogOpen, setIsDeleteDialogOpen] = useState(false);

  const snapshotUrl = useModelSnapshot(asset);
  const mutations = useAssetMutations(asset);

  useEffect(() => {
    setIsEditingName(false);
    setNameError(null);
    if (asset) setEditingName(asset.fileName);
  }, [asset?.id]);

  const handleStartRename = () => {
    if (!asset) return;
    setIsEditingName(true);
    setEditingName(asset.fileName);
    setNameError(null);
  };

  const handleSaveRename = async () => {
    if (!asset) return;
    const trimmed = editingName.trim();
    if (!trimmed || trimmed === asset.fileName) {
      setIsEditingName(false);
      setNameError(null);
      return;
    }
    try {
      await mutations.renameAsset(asset.id, trimmed);
      setIsEditingName(false);
      setNameError(null);
    } catch (err: unknown) {
      const msg = err instanceof Error ? err.message : String(err);
      setNameError(msg);
    }
  };

  const handleConfirmDelete = async () => {
    if (!asset) return;
    setIsDeleteDialogOpen(false);
    try {
      await mutations.deleteAsset(asset.id, true);
    } catch (err) {
      console.error('Failed to delete asset:', err);
    }
  };

  if (!asset) {
    return (
      <aside className={styles.panel} id="detail-panel">
        <div className={styles.empty}>
          <IconInfo size={32} />
          <p>Select an asset to see details</p>
        </div>
      </aside>
    );
  }

  return (
    <aside className={styles.panel} id="detail-panel">
      {/* Always-visible preview area at top */}
      <div className={styles.thumbArea}>
        <DetailPreviewArea
          asset={asset}
          snapshotUrl={snapshotUrl}
          imgClassName={styles.thumbImg}
        />
      </div>

      {/* Header with name editor, action buttons, and close (X) button */}
      <DetailPanelHeader
        asset={asset}
        isEditingName={isEditingName}
        editingName={editingName}
        nameError={nameError}
        onStartRename={handleStartRename}
        onCancelRename={() => {
          setIsEditingName(false);
          setNameError(null);
        }}
        onSaveRename={handleSaveRename}
        onEditingNameChange={(v) => {
          setEditingName(v);
          if (nameError) setNameError(null);
        }}
        onToggleFavorite={mutations.toggleFavorite}
        onOpenDelete={() => setIsDeleteDialogOpen(true)}
        onClosePanel={() => setDetailPanelOpen(false)}
      />

      {/* Exactly TWO tabs: Info and Organize */}
      <div className={styles.tabs} role="tablist" aria-label="Asset detail tabs">
        <button
          id="detail-tab-info"
          type="button"
          role="tab"
          aria-selected={tab === 'info'}
          className={clsx(styles.tab, tab === 'info' && styles.tabActive)}
          onClick={() => setTab('info')}
        >
          Info
        </button>
        <button
          id="detail-tab-organize"
          type="button"
          role="tab"
          aria-selected={tab === 'organize'}
          className={clsx(styles.tab, tab === 'organize' && styles.tabActive)}
          onClick={() => setTab('organize')}
        >
          Organize
        </button>
      </div>

      <div className={styles.tabContent} role="tabpanel">
        {tab === 'info' ? (
          <InfoTab asset={asset} />
        ) : (
          <OrganizeTab
            asset={asset}
            onRatingChange={mutations.setRating}
            onColorLabelChange={mutations.setColorLabel}
            onFavoriteChange={mutations.toggleFavorite}
            onToggleTag={mutations.toggleTag}
          />
        )}
      </div>

      <ConfirmDeleteDialog
        isOpen={isDeleteDialogOpen}
        itemType="asset"
        itemName={asset.fileName}
        onConfirm={handleConfirmDelete}
        onCancel={() => setIsDeleteDialogOpen(false)}
      />
    </aside>
  );
}
