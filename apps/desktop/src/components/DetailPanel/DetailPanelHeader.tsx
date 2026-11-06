import clsx from 'clsx';
import type { Asset } from '../../types';
import { api } from '../../api';
import {
  IconX,
  IconExternal,
  IconEye,
  IconCopy,
  IconCheck,
  IconEdit,
  IconTrash,
  IconHeart,
  IconHeartFilled,
} from '../Icons';
import styles from './DetailPanel.module.css';

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
  onClosePanel: () => void;
}

function HeaderActionButtons({
  asset,
  onToggleFavorite,
  onStartRename,
  onOpenDelete,
  onClosePanel,
}: HeaderActionsProps) {
  return (
    <div className={styles.headerActions}>
      <button
        id="detail-favorite"
        type="button"
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
        type="button"
        className={styles.actionBtn}
        title="Rename file (F2)"
        aria-label="Rename file"
        onClick={onStartRename}
      >
        <IconEdit size={14} />
      </button>
      <button
        id="detail-open-external"
        type="button"
        className={styles.actionBtn}
        title="Open with default app"
        aria-label="Open with default app"
        onClick={() => api.openWithDefault(asset.filePath).catch(console.error)}
      >
        <IconExternal size={14} />
      </button>
      <button
        id="detail-reveal"
        type="button"
        className={styles.actionBtn}
        title="Reveal in Explorer"
        aria-label="Reveal in Explorer"
        onClick={() => api.revealInExplorer(asset.filePath).catch(console.error)}
      >
        <IconEye size={14} />
      </button>
      <button
        id="detail-copy-path"
        type="button"
        className={styles.actionBtn}
        title="Copy path"
        aria-label="Copy file path"
        onClick={() => navigator.clipboard.writeText(asset.filePath)}
      >
        <IconCopy size={14} />
      </button>
      <button
        id="detail-delete"
        type="button"
        className={clsx(styles.actionBtn, styles.actionBtnDanger)}
        title="Delete file (Move to Recycle Bin)"
        aria-label="Delete file"
        onClick={onOpenDelete}
      >
        <IconTrash size={14} />
      </button>

      <div className={styles.headerDivider} />

      <button
        id="detail-close-btn"
        type="button"
        className={clsx(styles.actionBtn, styles.closePanelBtn)}
        title="Close Detail Panel"
        aria-label="Close Detail Panel"
        onClick={onClosePanel}
      >
        <IconX size={15} />
      </button>
    </div>
  );
}

export interface DetailPanelHeaderProps {
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
  onClosePanel: () => void;
}

export function DetailPanelHeader({
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
  onClosePanel,
}: DetailPanelHeaderProps) {
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
          onClosePanel={onClosePanel}
        />
      </div>

      {nameError && (
        <div className={styles.nameErrorBanner}>
          {nameError}
        </div>
      )}
    </>
  );
}
