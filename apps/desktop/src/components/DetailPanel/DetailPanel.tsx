import { useCallback, useState, useEffect } from 'react';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import { useUIStore, useAssetStore, useTagStore, useCollectionStore } from '../../store';
import type { Asset, ColorLabel, Collection, AssetKind } from '../../types';
import {
  IconStar,
  IconX,
  IconExternal,
  IconEye,
  IconInfo,
  IconCopy,
  IconCheck,
  IconEdit,
  IconTrash,
  IconHeart,
  IconHeartFilled,
  IconModel3D,
  IconImage,
  IconVideo,
  IconFileText,
} from '../Icons';
import { ConfirmDeleteDialog } from '../ConfirmDeleteDialog';
import { api } from '../../api';
import { ThreeViewer, getModelPath } from './ThreeViewer';
import { ThumbnailCarousel, getAssetThumbnailUrls } from '../AssetGrid/ThumbnailCarousel';
import { useModelSnapshot } from '../../lib/previewSnapshot';
import styles from './DetailPanel.module.css';

const COLOR_LABELS: ColorLabel[] = [
  'red', 'orange', 'yellow', 'green', 'teal', 'blue', 'purple', 'pink',
];

const KIND_COLOR: Record<AssetKind, string> = {
  '3d_model': 'hsl(220 85% 55%)',
  material: 'hsl(268 72% 60%)',
  texture: 'hsl(178 64% 46%)',
  hdri: 'hsl( 38 92% 56%)',
  ies: 'hsl( 45 95% 50%)',
  video: 'hsl(  4 80% 55%)',
  image: 'hsl(140 64% 46%)',
  document: 'hsl(213 84% 55%)',
  other: 'hsl(220  8% 50%)',
};

const KIND_ICON: Record<AssetKind, React.FC<{ size?: number; style?: React.CSSProperties }>> = {
  '3d_model': IconModel3D,
  material: IconImage,
  texture: IconImage,
  hdri: IconImage,
  ies: IconImage,
  video: IconVideo,
  image: IconImage,
  document: IconFileText,
  other: IconImage,
};

interface NameEditorProps {
  isEditing: boolean;
  name: string;
  onStart: () => void;
  onCancel: () => void;
  onSave: () => void;
  onChange: (v: string) => void;
}

function NameEditor({ isEditing, name, onStart, onCancel, onSave, onChange }: NameEditorProps) {
  if (!isEditing) {
    return (
      <p
        className={clsx(styles.fileName, 'truncate')}
        title="Double-click or click edit icon to rename"
        onDoubleClick={onStart}
        style={{ cursor: 'pointer' }}
      >
        {name}
      </p>
    );
  }

  return (
    <form
      className={styles.nameEditForm}
      onSubmit={(e) => {
        e.preventDefault();
        onSave();
      }}
    >
      <input
        type="text"
        autoFocus
        className={styles.nameInput}
        value={name}
        onChange={(e) => onChange(e.target.value)}
        onKeyDown={(e) => {
          if (e.key === 'Escape') onCancel();
        }}
      />
      <button
        type="submit"
        className={clsx(styles.nameActionBtn, styles.nameActionBtnConfirm)}
        title="Save name"
        aria-label="Save name"
      >
        <IconCheck size={13} />
      </button>
      <button
        type="button"
        className={clsx(styles.nameActionBtn, styles.nameActionBtnCancel)}
        onClick={onCancel}
        title="Cancel"
        aria-label="Cancel rename"
      >
        <IconX size={13} />
      </button>
    </form>
  );
}

interface HeaderActionsProps {
  asset: Asset;
  onToggleFavorite: () => void;
  onStartRename: () => void;
  onOpenDelete: () => void;
}

function HeaderActionButtons({ asset, onToggleFavorite, onStartRename, onOpenDelete }: HeaderActionsProps) {
  return (
    <div className={styles.headerActions}>
      <button
        id="detail-favorite"
        className={clsx(styles.actionBtn, asset.isFavorite && styles.actionBtnFavorite)}
        title={asset.isFavorite ? 'Remove from favorites' : 'Add to favorites'}
        aria-label={asset.isFavorite ? 'Remove from favorites' : 'Add to favorites'}
        onClick={onToggleFavorite}
      >
        {asset.isFavorite ? (
          <IconHeartFilled size={14} className={styles.favoriteHeartFilled} />
        ) : (
          <IconHeart size={14} />
        )}
      </button>
      <button
        id="detail-rename"
        className={styles.actionBtn}
        title="Rename file (F2)"
        aria-label="Rename file"
        onClick={onStartRename}
      >
        <IconEdit size={14} />
      </button>
      <button
        id="detail-open-external"
        className={styles.actionBtn}
        title="Open with default app"
        aria-label="Open with default app"
        onClick={() => api.openWithDefault(asset.filePath).catch(console.error)}
      >
        <IconExternal size={14} />
      </button>
      <button
        id="detail-reveal"
        className={styles.actionBtn}
        title="Reveal in Explorer"
        aria-label="Reveal in Explorer"
        onClick={() => api.revealInExplorer(asset.filePath).catch(console.error)}
      >
        <IconEye size={14} />
      </button>
      <button
        id="detail-copy-path"
        className={styles.actionBtn}
        title="Copy path"
        aria-label="Copy file path"
        onClick={() => navigator.clipboard.writeText(asset.filePath)}
      >
        <IconCopy size={14} />
      </button>
      <button
        id="detail-delete"
        className={clsx(styles.actionBtn, styles.actionBtnDanger)}
        title="Delete file (Move to Recycle Bin)"
        aria-label="Delete file"
        onClick={onOpenDelete}
      >
        <IconTrash size={14} />
      </button>
    </div>
  );
}

interface DetailHeaderProps {
  asset: Asset;
  isEditingName: boolean;
  editingName: string;
  nameError: string | null;
  onStartRename: () => void;
  onCancelRename: () => void;
  onSaveRename: () => void;
  onEditingNameChange: (v: string) => void;
  onToggleFavorite: () => void;
  onOpenDelete: () => void;
}

function DetailPanelHeader({
  asset,
  isEditingName,
  editingName,
  nameError,
  onStartRename,
  onCancelRename,
  onSaveRename,
  onEditingNameChange,
  onToggleFavorite,
  onOpenDelete,
}: DetailHeaderProps) {
  return (
    <>
      <div className={styles.header}>
        <NameEditor
          isEditing={isEditingName}
          name={editingName}
          onStart={onStartRename}
          onCancel={onCancelRename}
          onSave={onSaveRename}
          onChange={onEditingNameChange}
        />
        <HeaderActionButtons
          asset={asset}
          onToggleFavorite={onToggleFavorite}
          onStartRename={onStartRename}
          onOpenDelete={onOpenDelete}
        />
      </div>

      {nameError && (
        <div style={{ color: '#ef4444', fontSize: '11px', padding: '2px 16px', background: 'rgba(239, 68, 68, 0.1)' }}>
          {nameError}
        </div>
      )}
    </>
  );
}

interface DetailPreviewProps {
  asset: Asset;
  modelPath?: string;
  snapshotUrl?: string | null;
}

function renderPreviewBody(asset: Asset, modelPath?: string, snapshotUrl?: string | null) {
  const thumbs = getAssetThumbnailUrls(asset, snapshotUrl);
  if (thumbs.length > 0) {
    return (
      <ThumbnailCarousel
        asset={asset}
        snapshotUrl={snapshotUrl}
        imgClassName={styles.thumbImg}
        alt={asset.fileName}
      />
    );
  }

  if (asset.kind === 'video' && asset.filePath) {
    return (
      <video
        src={convertFileSrc(asset.filePath)}
        preload="metadata"
        muted
        playsInline
        controls
        className={styles.thumbImg}
        style={{ objectFit: 'contain' }}
      />
    );
  }

  if (modelPath) {
    return <ThreeViewer modelPath={modelPath} />;
  }

  const KindIcon = KIND_ICON[asset.kind] ?? IconImage;
  const kindColor = KIND_COLOR[asset.kind] ?? 'var(--color-text-disabled)';
  return (
    <div className={styles.thumbPlaceholder}>
      <KindIcon size={36} style={{ color: kindColor }} />
      <span className={styles.thumbExt} style={{ color: kindColor }}>
        .{asset.extension.toUpperCase()}
      </span>
    </div>
  );
}

function DetailPanelPreviewArea({ asset, modelPath, snapshotUrl }: DetailPreviewProps) {
  return (
    <div className={styles.thumbArea}>
      {renderPreviewBody(asset, modelPath, snapshotUrl)}
    </div>
  );
}

function useAssetMutations(asset: Asset | null) {
  const patchAsset = useAssetStore((s) => s.patchAsset);
  const deleteAsset = useAssetStore((s) => s.deleteAsset);
  const renameAsset = useAssetStore((s) => s.renameAsset);

  const setRating = useCallback(
    (rating: number) => asset && patchAsset(asset.id, { rating }),
    [asset, patchAsset]
  );

  const setColorLabel = useCallback(
    (label: ColorLabel) => asset && patchAsset(asset.id, { colorLabel: label === asset.colorLabel ? 'none' : label }),
    [asset, patchAsset]
  );

  const toggleFavorite = useCallback(
    () => asset && patchAsset(asset.id, { isFavorite: !asset.isFavorite }),
    [asset, patchAsset]
  );

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

type TabKey = 'info' | 'tags' | 'collections' | 'preview';

interface TabContentProps {
  tab: TabKey;
  asset: Asset;
  modelPath?: string;
  snapshotUrl?: string | null;
  onRatingChange: (r: number) => void;
  onColorLabelChange: (l: ColorLabel) => void;
  onFavoriteChange: () => void;
  onToggleTag: (id: string) => void;
}

function DetailTabContent({
  tab,
  asset,
  modelPath,
  snapshotUrl,
  onRatingChange,
  onColorLabelChange,
  onFavoriteChange,
  onToggleTag,
}: TabContentProps) {
  const tags = useTagStore((s) => s.tags);
  const collections = useCollectionStore((s) => s.collections);
  const addToCollection = useCollectionStore((s) => s.addToCollection);
  const removeFromCollection = useCollectionStore((s) => s.removeFromCollection);

  if (tab === 'tags') {
    return <TagsTab asset={asset} tags={tags} onToggle={onToggleTag} />;
  }
  if (tab === 'collections') {
    return (
      <CollectionsTab
        asset={asset}
        collections={collections}
        onAdd={async (colId) => {
          await addToCollection(colId, [asset.id]);
          useCollectionStore.getState().fetchCollections();
        }}
        onRemove={async (colId) => {
          await removeFromCollection(colId, [asset.id]);
          useCollectionStore.getState().fetchCollections();
        }}
      />
    );
  }
  if (tab === 'preview') {
    return <PreviewTab asset={asset} modelPath={modelPath} snapshotUrl={snapshotUrl} />;
  }
  return (
    <InfoTab
      asset={asset}
      onRatingChange={onRatingChange}
      onColorLabelChange={onColorLabelChange}
      onFavoriteChange={onFavoriteChange}
    />
  );
}

export function DetailPanel() {
  const focusedAssetId = useUIStore((s) => s.focusedAssetId);
  const asset = useAssetStore((s) =>
    focusedAssetId ? s.assets.find((a) => a.id === focusedAssetId) ?? s.assetMap.get(focusedAssetId) ?? null : null
  );
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

  const modelPath = getModelPath(asset);

  return (
    <aside className={styles.panel} id="detail-panel">
      <DetailPanelPreviewArea asset={asset} modelPath={modelPath} snapshotUrl={snapshotUrl} />

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
      />

      <div className={styles.tabs}>
        {(['info', 'tags', 'collections', 'preview'] as const).map((t) => (
          <button
            key={t}
            id={`detail-tab-${t}`}
            className={clsx(styles.tab, tab === t && styles.tabActive)}
            onClick={() => setTab(t)}
          >
            {t.charAt(0).toUpperCase() + t.slice(1)}
          </button>
        ))}
      </div>

      <div className={styles.tabContent}>
        <DetailTabContent
          tab={tab}
          asset={asset}
          modelPath={modelPath}
          snapshotUrl={snapshotUrl}
          onRatingChange={mutations.setRating}
          onColorLabelChange={mutations.setColorLabel}
          onFavoriteChange={mutations.toggleFavorite}
          onToggleTag={mutations.toggleTag}
        />
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

function StarRatingRow({ rating, onChange }: { rating: number; onChange: (r: number) => void }) {
  const [hoverRating, setHoverRating] = useState(0);

  return (
    <div className={styles.starRow}>
      {Array.from({ length: 5 }).map((_, i) => {
        const n = i + 1;
        return (
          <button
            key={n}
            id={`detail-star-${n}`}
            className={clsx(
              styles.starBtn,
              (hoverRating || rating) >= n && styles.starOn
            )}
            onMouseEnter={() => setHoverRating(n)}
            onMouseLeave={() => setHoverRating(0)}
            onClick={() => onChange(rating === n ? 0 : n)}
            aria-label={`Rate ${n} star${n > 1 ? 's' : ''}`}
          >
            <IconStar size={16} />
          </button>
        );
      })}
    </div>
  );
}

function ColorLabelRow({ active, onChange }: { active: ColorLabel; onChange: (l: ColorLabel) => void }) {
  return (
    <div className={styles.colorRow}>
      <button
        className={clsx(styles.colorNoneBtn, active === 'none' && styles.colorNoneActive)}
        onClick={() => onChange('none')}
        title="No label"
        aria-label="Clear color label"
      >
        <IconX size={10} />
      </button>
      {COLOR_LABELS.map((label) => (
        <button
          key={label}
          id={`detail-color-${label}`}
          className={clsx(styles.colorDot, active === label && styles.colorDotActive)}
          style={{ background: `var(--swatch-${label})` }}
          title={label}
          aria-label={`Color label ${label}`}
          onClick={() => onChange(label)}
        />
      ))}
    </div>
  );
}

function MetadataEntries({ metadata }: { metadata: Record<string, unknown> }) {
  const entries = Object.entries(metadata);
  if (entries.length === 0) return null;

  return (
    <>
      <div className={styles.metaDivider} />
      <label className={clsx(styles.infoLabel, styles.metaHeader)}>Metadata</label>
      <div />
      {entries.map(([k, v]) => (
        <div key={`meta-${k}`} style={{ display: 'contents' }}>
          <label className={clsx(styles.infoLabel, styles.metaKey)}>{k}</label>
          <span className={clsx(styles.infoValue, styles.mono)}>
            {typeof v === 'object' ? JSON.stringify(v) : String(v)}
          </span>
        </div>
      ))}
    </>
  );
}

// ── Info Tab ──────────────────────────────────────────────────────────────
function InfoTab({
  asset, onRatingChange, onColorLabelChange, onFavoriteChange,
}: {
  asset: Asset;
  onRatingChange: (r: number) => void;
  onColorLabelChange: (l: ColorLabel) => void;
  onFavoriteChange: () => void;
}) {
  return (
    <div className={styles.infoGrid}>
      <label className={styles.infoLabel}>Favorite</label>
      <div className={styles.favoriteRow}>
        <button
          id="detail-info-favorite"
          type="button"
          className={clsx(styles.infoFavoriteBtn, asset.isFavorite && styles.infoFavoriteActive)}
          onClick={onFavoriteChange}
        >
          {asset.isFavorite ? (
            <>
              <IconHeartFilled size={14} className={styles.favoriteHeartFilled} />
              <span>In Favorites</span>
            </>
          ) : (
            <>
              <IconHeart size={14} />
              <span>Add to Favorites</span>
            </>
          )}
        </button>
      </div>

      <label className={styles.infoLabel}>Rating</label>
      <StarRatingRow rating={asset.rating} onChange={onRatingChange} />

      <label className={styles.infoLabel}>Label</label>
      <ColorLabelRow active={asset.colorLabel} onChange={onColorLabelChange} />

      <label className={styles.infoLabel}>Path</label>
      <span
        className={clsx(styles.infoValue, styles.mono, 'truncate')}
        title={`${asset.filePath} (Click to reveal in Explorer)`}
        style={{ cursor: 'pointer' }}
        onClick={() => api.revealInExplorer(asset.filePath).catch(console.error)}
      >
        {asset.filePath}
      </span>

      <label className={styles.infoLabel}>Format</label>
      <span className={styles.infoValue}>.{asset.extension.toUpperCase()}</span>

      <label className={styles.infoLabel}>Kind</label>
      <span className={styles.infoValue}>{asset.kind.replace('_', ' ')}</span>

      <label className={styles.infoLabel}>Size</label>
      <span className={clsx(styles.infoValue, styles.mono)}>
        {(asset.sizeBytes / 1024 / 1024).toFixed(2)} MB
      </span>

      <label className={styles.infoLabel}>Modified</label>
      <span className={styles.infoValue}>
        {new Date(asset.modifiedAt).toLocaleDateString()}
      </span>

      <label className={styles.infoLabel}>Added</label>
      <span className={styles.infoValue}>
        {new Date(asset.indexedAt).toLocaleDateString()}
      </span>

      <MetadataEntries metadata={asset.metadata} />
    </div>
  );
}

// ── Tags Tab ──────────────────────────────────────────────────────────────
function TagsTab({
  asset, tags, onToggle,
}: {
  asset: Asset;
  tags: import('../../types').Tag[];
  onToggle: (id: string) => void;
}) {
  const assigned = tags.filter((t) => asset.tags.includes(t.id));
  const unassigned = tags.filter((t) => !asset.tags.includes(t.id));

  return (
    <div className={styles.tagsSection}>
      {tags.length === 0 && (
        <p className={styles.noTags}>No tags yet. Create tags in the sidebar.</p>
      )}

      {assigned.length > 0 && (
        <>
          <p className={styles.tagsGroupLabel}>Assigned</p>
          <div className={styles.tagsGrid}>
            {assigned.map((tag) => (
              <button
                key={tag.id}
                id={`detail-tag-${tag.id}`}
                className={clsx(styles.tagPill, styles.tagActive)}
                onClick={() => onToggle(tag.id)}
                title="Click to remove"
              >
                <span
                  className={styles.tagDot}
                  style={{ background: tag.color ? `var(--swatch-${tag.color})` : 'var(--color-text-muted)' }}
                />
                <span className={styles.tagPillName}>{tag.name}</span>
                <IconCheck size={11} className={styles.tagCheckIcon} />
              </button>
            ))}
          </div>
        </>
      )}

      {unassigned.length > 0 && (
        <>
          <p className={styles.tagsGroupLabel}>{assigned.length > 0 ? 'Available' : 'All Tags'}</p>
          <div className={styles.tagsGrid}>
            {unassigned.map((tag) => (
              <button
                key={tag.id}
                id={`detail-tag-${tag.id}`}
                className={styles.tagPill}
                onClick={() => onToggle(tag.id)}
                title="Click to assign"
              >
                <span
                  className={styles.tagDot}
                  style={{ background: tag.color ? `var(--swatch-${tag.color})` : 'var(--color-text-muted)' }}
                />
                <span className={styles.tagPillName}>{tag.name}</span>
              </button>
            ))}
          </div>
        </>
      )}
    </div>
  );
}

// ── Collections Tab ───────────────────────────────────────────────────────
function CollectionsTab({
  asset, collections, onAdd, onRemove,
}: {
  asset: Asset;
  collections: Collection[];
  onAdd: (collectionId: string) => void;
  onRemove: (collectionId: string) => void;
}) {
  const getCollectionPath = (col: Collection): string => {
    const parts = [col.name];
    let curr = col;
    while (curr.parentId) {
      const parent = collections.find((c: Collection) => c.id === curr.parentId);
      if (!parent) break;
      parts.unshift(parent.name);
      curr = parent;
    }
    return parts.join(' / ');
  };

  return (
    <div className={styles.tagsGrid}>
      {collections.length === 0 && (
        <p className={styles.noTags}>No collections yet. Create collections in the sidebar.</p>
      )}
      {collections.map((col: Collection) => {
        const isMember = (col.assetIds && col.assetIds.includes(asset.id)) || (asset.collections?.includes(col.id) ?? false);
        const pathName = getCollectionPath(col);
        return (
          <button
            key={col.id}
            id={`detail-col-${col.id}`}
            className={clsx(styles.tagPill, isMember && styles.tagActive)}
            onClick={() => (isMember ? onRemove(col.id) : onAdd(col.id))}
            title={pathName}
          >
            <span className={styles.tagDot} style={{ background: 'var(--color-accent)' }} />
            {pathName}
            <span className={styles.tagCount}>({col.assetIds.length})</span>
          </button>
        );
      })}
    </div>
  );
}

// ── Preview Tab ───────────────────────────────────────────────────────────
function PreviewTab({
  asset,
  modelPath,
  snapshotUrl,
}: {
  asset: Asset;
  modelPath?: string;
  snapshotUrl?: string | null;
}) {
  const thumbs = getAssetThumbnailUrls(asset, snapshotUrl);
  const KindIcon = KIND_ICON[asset.kind] ?? IconImage;
  const kindColor = KIND_COLOR[asset.kind] ?? 'var(--color-text-disabled)';

  if (modelPath) {
    return (
      <div className={styles.previewArea}>
        <div className={styles.previewViewerWrapper}>
          <ThreeViewer modelPath={modelPath} />
        </div>
      </div>
    );
  }

  if (asset.kind === 'video' && asset.filePath) {
    return (
      <div className={styles.previewArea}>
        <div className={styles.previewViewerWrapper}>
          <video
            src={convertFileSrc(asset.filePath)}
            controls
            autoPlay
            muted
            playsInline
            className={styles.previewImg}
            style={{ width: '100%', height: '100%', objectFit: 'contain', background: '#000' }}
          />
        </div>
      </div>
    );
  }

  if (thumbs.length > 0) {
    return (
      <div className={styles.previewArea}>
        <div className={styles.previewViewerWrapper}>
          <ThumbnailCarousel
            asset={asset}
            snapshotUrl={snapshotUrl}
            imgClassName={styles.previewImg}
            alt={asset.fileName}
          />
        </div>
      </div>
    );
  }

  return (
    <div className={styles.previewArea}>
      <div className={styles.previewEmpty}>
        <div className={styles.emptyStatusIcon}>
          <KindIcon size={32} style={{ color: kindColor }} />
        </div>
        <p className={styles.emptyStatusTitle}>
          .{asset.extension.toUpperCase()}
        </p>
        <p className={styles.emptyStatusSub}>
          No preview available
        </p>
      </div>
    </div>
  );
}
