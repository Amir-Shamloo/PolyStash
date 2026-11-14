import { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import type { KeyboardEvent as ReactKeyboardEvent } from 'react';
import clsx from 'clsx';
import { api } from '../../api';
import { useTagStore, useUIStore } from '../../store';
import type { Tag } from '../../types';
import { IconX } from '../Icons';
import { PredefinedPanel } from './PredefinedPanel';
import { StagedPanel } from './StagedPanel';
import { InspectorPanel } from './InspectorPanel';
import {
  applyColor,
  applyIcon,
  collectIds,
  createCustomTag,
  duplicateTag,
  findTag,
  flattenTree,
  insertTag,
  moveTag,
  nestTags,
  removeTag,
  renameTag,
  stagePredefined,
} from './tagForgeModel';
import type { ForgeTag, PredefinedNode } from './tagForgeModel';
import styles from './TagForgeModal.module.css';

export interface TagForgeModalProps {
  onSaved?: () => void;
}

interface HistoryState {
  past: ForgeTag[][];
  present: ForgeTag[];
  future: ForgeTag[][];
}

interface ShortcutEvent {
  key: string;
  ctrlKey: boolean;
  metaKey: boolean;
  altKey: boolean;
  shiftKey: boolean;
  target: EventTarget | null;
}

type ShortcutAction = 'escape' | 'undo' | 'redo';

const HISTORY_LIMIT = 100;
const EMPTY_HISTORY: HistoryState = { past: [], present: [], future: [] };
const EDITABLE_TAGS = new Set(['input', 'textarea', 'select']);

function isEditableElement(target: EventTarget | null): boolean {
  if (!(target instanceof HTMLElement)) return false;
  if (target.isContentEditable) return true;
  return EDITABLE_TAGS.has(target.tagName.toLowerCase());
}

function isEditingShortcutTarget(target: EventTarget | null): boolean {
  if (isEditableElement(target)) return true;
  return isEditableElement(document.activeElement);
}

function hasPrimaryModifier(e: ShortcutEvent): boolean {
  if (e.altKey) return false;
  return e.ctrlKey || e.metaKey;
}

function resolveShortcutAction(e: ShortcutEvent): ShortcutAction | null {
  if (e.key === 'Escape') return 'escape';
  if (isEditingShortcutTarget(e.target)) return null;
  if (!hasPrimaryModifier(e)) return null;
  const key = e.key.toLowerCase();
  if (key === 'y') return 'redo';
  if (key !== 'z') return null;
  return e.shiftKey ? 'redo' : 'undo';
}

function pushHistory(history: HistoryState, next: ForgeTag[]): HistoryState {
  const past = [...history.past, history.present];
  const capped = past.length > HISTORY_LIMIT ? past.slice(past.length - HISTORY_LIMIT) : past;
  return { past: capped, present: next, future: [] };
}

function stepUndo(history: HistoryState): HistoryState {
  if (history.past.length === 0) return history;
  const previous = history.past[history.past.length - 1];
  return {
    past: history.past.slice(0, -1),
    present: previous,
    future: [history.present, ...history.future],
  };
}

function stepRedo(history: HistoryState): HistoryState {
  if (history.future.length === 0) return history;
  const [next, ...rest] = history.future;
  return {
    past: [...history.past, history.present],
    present: next,
    future: rest,
  };
}

function countMissingIds(baseline: Set<string>, current: Set<string>): number {
  let missing = 0;
  for (const id of baseline) {
    if (!current.has(id)) missing += 1;
  }
  return missing;
}

function confirmRemovalIfNeeded(baselineIds: Set<string>, forest: ForgeTag[]): boolean {
  const removedCount = countMissingIds(baselineIds, collectIds(forest));
  if (removedCount === 0) return true;
  const noun = removedCount === 1 ? 'tag' : 'tags';
  return window.confirm(`Saving will remove ${removedCount} existing ${noun}. Continue?`);
}

function toTagPayload(nodes: ForgeTag[]): Tag[] {
  return nodes.map(({ id, name, parentId, color, iconId }) => ({
    id,
    name,
    parentId,
    color,
    iconId,
  }));
}

function formatError(err: unknown): string {
  return err instanceof Error ? err.message : String(err);
}

function useTagForgeShortcuts(
  isOpen: boolean,
  handlers: { onEscape: () => void; onUndo: () => void; onRedo: () => void },
) {
  const { onEscape, onUndo, onRedo } = handlers;

  const dispatchShortcut = useCallback(
    (action: ShortcutAction | null): boolean => {
      if (!action) return false;
      if (action === 'escape') onEscape();
      else if (action === 'undo') onUndo();
      else onRedo();
      return true;
    },
    [onEscape, onUndo, onRedo],
  );

  useEffect(() => {
    if (!isOpen) return;
    const onWindowKeyDown = (e: KeyboardEvent) => {
      if (e.defaultPrevented) return;
      if (dispatchShortcut(resolveShortcutAction(e))) {
        e.preventDefault();
      }
    };
    window.addEventListener('keydown', onWindowKeyDown);
    return () => window.removeEventListener('keydown', onWindowKeyDown);
  }, [isOpen, dispatchShortcut]);

  return useCallback(
    (e: ReactKeyboardEvent<HTMLDivElement>) => {
      if (dispatchShortcut(resolveShortcutAction(e))) {
        e.preventDefault();
      }
      e.stopPropagation();
    },
    [dispatchShortcut],
  );
}

function TagForgeHeader({
  saving,
  onClose,
}: {
  saving: boolean;
  onClose: () => void;
}) {
  return (
    <header className={styles.header}>
      <div className={styles.titleGroup}>
        <h2 id="tag-forge-title" className={styles.title}>
          Tag Forge
        </h2>
        <p className={styles.subtitle}>
          Browse predefined taxonomies, organize your tag tree, and customize icons and colors.
        </p>
      </div>
      <button
        type="button"
        className={styles.closeBtn}
        onClick={onClose}
        disabled={saving}
        aria-label="Close Tag Forge"
        title="Close"
      >
        <IconX size={14} />
      </button>
    </header>
  );
}

function FooterStatus({ error, loading }: { error: string | null; loading: boolean }) {
  if (error) {
    return (
      <span className={styles.errorText} role="alert">
        {error}
      </span>
    );
  }
  if (loading) {
    return <span className={styles.loadingText}>Loading workspace tags…</span>;
  }
  return null;
}

function TagForgeFooter({
  error,
  loading,
  saving,
  canSave,
  onCancel,
  onSave,
}: {
  error: string | null;
  loading: boolean;
  saving: boolean;
  canSave: boolean;
  onCancel: () => void;
  onSave: () => void;
}) {
  return (
    <footer className={styles.footer}>
      <div className={styles.statusArea}>
        <FooterStatus error={error} loading={loading} />
      </div>
      <div className={styles.actions}>
        <button
          type="button"
          className={clsx(styles.btn, styles.cancelBtn)}
          onClick={onCancel}
          disabled={saving}
        >
          Cancel
        </button>
        <button
          type="button"
          className={clsx(styles.btn, styles.saveBtn)}
          onClick={onSave}
          disabled={!canSave}
        >
          {saving ? 'Saving…' : 'Save Tags'}
        </button>
      </div>
    </footer>
  );
}

function useTagForgeSession(tagForgeOpen: boolean) {
  const [history, setHistory] = useState<HistoryState>(EMPTY_HISTORY);
  const [selectedId, setSelectedId] = useState<string | null>(null);
  const [autoRenameId, setAutoRenameId] = useState<string | null>(null);
  const [applyToSubtags, setApplyToSubtags] = useState(false);
  const [baselineIds, setBaselineIds] = useState<Set<string>>(() => new Set());
  const [loading, setLoading] = useState(false);
  const [saving, setSaving] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const dialogRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!tagForgeOpen) return;
    let cancelled = false;

    setHistory(EMPTY_HISTORY);
    setSelectedId(null);
    setAutoRenameId(null);
    setApplyToSubtags(false);
    setBaselineIds(new Set());
    setError(null);
    setSaving(false);
    setLoading(true);
    dialogRef.current?.focus();

    api
      .listTags()
      .then((tags) => {
        if (cancelled) return;
        const forest = nestTags(tags);
        setHistory({ past: [], present: forest, future: [] });
        setBaselineIds(collectIds(forest));
        setLoading(false);
      })
      .catch((err) => {
        if (cancelled) return;
        setError(formatError(err));
        setLoading(false);
      });

    return () => {
      cancelled = true;
    };
  }, [tagForgeOpen]);

  return {
    dialogRef,
    history,
    setHistory,
    selectedId,
    setSelectedId,
    autoRenameId,
    setAutoRenameId,
    applyToSubtags,
    setApplyToSubtags,
    baselineIds,
    loading,
    saving,
    setSaving,
    error,
    setError,
  };
}

export function TagForgeModal({ onSaved }: TagForgeModalProps) {
  const tagForgeOpen = useUIStore((s) => s.tagForgeOpen);
  const setTagForgeOpen = useUIStore((s) => s.setTagForgeOpen);
  const saveTagTree = useTagStore((s) => s.saveTagTree);
  const session = useTagForgeSession(tagForgeOpen);
  const { history, setHistory } = session;

  const canUndo = history.past.length > 0;
  const canRedo = history.future.length > 0;
  const stagedIds = useMemo(() => collectIds(history.present), [history.present]);
  const selected = useMemo(
    () => (session.selectedId ? findTag(history.present, session.selectedId) : null),
    [history.present, session.selectedId],
  );
  const effectiveSelectedId = selected ? selected.id : null;

  const commit = useCallback(
    (next: ForgeTag[]) => {
      setHistory((prev) => pushHistory(prev, next));
    },
    [setHistory],
  );
  const handleUndo = useCallback(() => setHistory(stepUndo), [setHistory]);
  const handleRedo = useCallback(() => setHistory(stepRedo), [setHistory]);

  const handleRequestClose = useCallback(() => {
    if (session.saving) return;
    if (canUndo && !window.confirm('Discard unsaved Tag Forge changes?')) return;
    setTagForgeOpen(false);
  }, [session.saving, canUndo, setTagForgeOpen]);

  const handleOverlayKeyDown = useTagForgeShortcuts(tagForgeOpen, {
    onEscape: handleRequestClose,
    onUndo: handleUndo,
    onRedo: handleRedo,
  });

  const handleStage = (node: PredefinedNode) => {
    if (stagedIds.has(node.guid)) return;
    commit(
      insertTag(
        history.present,
        effectiveSelectedId,
        stagePredefined(node, effectiveSelectedId, stagedIds),
      ),
    );
    session.setSelectedId(node.guid);
    session.setAutoRenameId(null);
  };

  const handleRemove = (id: string) => {
    const next = removeTag(history.present, id);
    commit(next);
    if (effectiveSelectedId && !findTag(next, effectiveSelectedId)) {
      session.setSelectedId(null);
    }
  };

  const handleMove = (id: string, newParentId: string | null, index: number) => {
    const next = moveTag(history.present, id, newParentId, index);
    if (next !== history.present) commit(next);
  };

  const handleAddRoot = () => {
    const created = createCustomTag(null);
    commit(insertTag(history.present, null, created));
    session.setSelectedId(created.id);
    session.setAutoRenameId(created.id);
  };

  const handleApplyIcon = (iconId: string | null) => {
    if (!effectiveSelectedId) return;
    commit(applyIcon(history.present, effectiveSelectedId, iconId, session.applyToSubtags));
  };

  const handleApplyColor = (color: string | null) => {
    if (!effectiveSelectedId) return;
    commit(applyColor(history.present, effectiveSelectedId, color, session.applyToSubtags));
  };

  const handleSave = async () => {
    if (session.saving || !canUndo) return;
    if (!confirmRemovalIfNeeded(session.baselineIds, history.present)) return;

    session.setSaving(true);
    session.setError(null);
    try {
      await saveTagTree(toTagPayload(flattenTree(history.present)));
      onSaved?.();
      setTagForgeOpen(false);
    } catch (err) {
      session.setError(formatError(err));
    } finally {
      session.setSaving(false);
    }
  };

  if (!tagForgeOpen) return null;

  return (
    <div className={styles.overlay} onClick={handleRequestClose} onKeyDown={handleOverlayKeyDown}>
      <div
        ref={session.dialogRef}
        className={styles.dialog}
        role="dialog"
        aria-modal="true"
        aria-labelledby="tag-forge-title"
        tabIndex={-1}
        onClick={(e) => e.stopPropagation()}
      >
        <TagForgeHeader saving={session.saving} onClose={handleRequestClose} />
        <div className={styles.body}>
          <div className={styles.column}>
            <PredefinedPanel onStage={handleStage} stagedIds={stagedIds} />
          </div>
          <div className={styles.column}>
            <StagedPanel
              forest={history.present}
              selectedId={effectiveSelectedId}
              autoRenameId={session.autoRenameId}
              onSelect={(id) => {
                session.setSelectedId(id);
                session.setAutoRenameId(null);
              }}
              onRename={(id, name) => commit(renameTag(history.present, id, name))}
              onRemove={handleRemove}
              onMove={handleMove}
              onAddRoot={handleAddRoot}
              onDuplicate={(id) => commit(duplicateTag(history.present, id))}
              onUndo={handleUndo}
              onRedo={handleRedo}
              canUndo={canUndo}
              canRedo={canRedo}
            />
          </div>
          <div className={styles.column}>
            <InspectorPanel
              selected={selected}
              applyToSubtags={session.applyToSubtags}
              onToggleApplyToSubtags={session.setApplyToSubtags}
              onApplyIcon={handleApplyIcon}
              onApplyColor={handleApplyColor}
            />
          </div>
        </div>
        <TagForgeFooter
          error={session.error}
          loading={session.loading}
          saving={session.saving}
          canSave={!session.saving && canUndo}
          onCancel={handleRequestClose}
          onSave={handleSave}
        />
      </div>
    </div>
  );
}

export default TagForgeModal;
