import { useEffect, useMemo, useRef, useState } from 'react';
import type { DragEvent, KeyboardEvent, MouseEvent } from 'react';
import clsx from 'clsx';
import { IconChevronDown, IconChevronRight, IconCopy, IconPlus, IconTrash } from '../Icons';
import { TagIcon } from './TagIcon';
import { toCssColor } from './tagForgeData';
import { collectIds, findParentId, findTag } from './tagForgeModel';
import type { ForgeTag } from './tagForgeModel';
import styles from './StagedPanel.module.css';

export interface StagedPanelProps {
  forest: ForgeTag[];
  selectedId: string | null;
  autoRenameId?: string | null;
  onSelect: (id: string | null) => void;
  onRename: (id: string, name: string) => void;
  onRemove: (id: string) => void;
  onMove: (id: string, newParentId: string | null, index: number) => void;
  onAddRoot: () => void;
  onDuplicate: (id: string) => void;
  onUndo: () => void;
  onRedo: () => void;
  canUndo: boolean;
  canRedo: boolean;
}

type DropZone = 'before' | 'inside' | 'after';

interface DropTarget {
  id: string;
  zone: DropZone;
}

interface VisibleRow {
  node: ForgeTag;
  depth: number;
  parentId: string | null;
  indexInParent: number;
  ancestorPath: string;
}

const EDITABLE_TAGS = new Set(['INPUT', 'TEXTAREA', 'SELECT']);

const DROP_ZONE_CLASS: Record<DropZone, string> = {
  before: styles.dropBefore,
  inside: styles.dropInside,
  after: styles.dropAfter,
};

function IconUndo() {
  return (
    <svg width={12} height={12} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden="true">
      <path d="M3 7v6h6" />
      <path d="M21 17a9 9 0 0 0-9-9 9 9 0 0 0-6 2.3L3 13" />
    </svg>
  );
}

function IconRedo() {
  return (
    <svg width={12} height={12} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden="true">
      <path d="M21 7v6h-6" />
      <path d="M3 17a9 9 0 0 1 9-9 9 9 0 0 1 6 2.3L21 13" />
    </svg>
  );
}

function isEditableTarget(target: EventTarget | null): boolean {
  if (!(target instanceof HTMLElement)) return false;
  return EDITABLE_TAGS.has(target.tagName) || target.isContentEditable;
}

function isDeleteEvent(e: KeyboardEvent): boolean {
  return e.key === 'Delete' || e.key === 'Backspace';
}

function collectExpandableIds(roots: ForgeTag[]): string[] {
  const ids: string[] = [];
  for (const node of roots) {
    if (node.children.length > 0) {
      ids.push(node.id, ...collectExpandableIds(node.children));
    }
  }
  return ids;
}

function flattenVisibleTree(ctx: {
  nodes: ForgeTag[];
  expandedIds: Set<string>;
  depth?: number;
  parentId?: string | null;
}): VisibleRow[] {
  const { nodes, expandedIds, depth = 0, parentId = null } = ctx;
  const rows: VisibleRow[] = [];
  nodes.forEach((node, indexInParent) => {
    rows.push({ node, depth, parentId, indexInParent, ancestorPath: '' });
    if (node.children.length > 0 && expandedIds.has(node.id)) {
      rows.push(
        ...flattenVisibleTree({
          nodes: node.children,
          expandedIds,
          depth: depth + 1,
          parentId: node.id,
        }),
      );
    }
  });
  return rows;
}

function collectSearchMatches(ctx: {
  nodes: ForgeTag[];
  query: string;
  ancestors?: string[];
  parentId?: string | null;
}): VisibleRow[] {
  const { nodes, query, ancestors = [], parentId = null } = ctx;
  const rows: VisibleRow[] = [];
  nodes.forEach((node, indexInParent) => {
    if (node.name.toLowerCase().includes(query)) {
      rows.push({
        node,
        depth: 0,
        parentId,
        indexInParent,
        ancestorPath: ancestors.join(' / '),
      });
    }
    if (node.children.length > 0) {
      rows.push(
        ...collectSearchMatches({
          nodes: node.children,
          query,
          ancestors: [...ancestors, node.name],
          parentId: node.id,
        }),
      );
    }
  });
  return rows;
}

function isSelfOrDescendant(ctx: {
  roots: ForgeTag[];
  sourceId: string;
  candidateId: string;
}): boolean {
  const { roots, sourceId, candidateId } = ctx;
  if (sourceId === candidateId) return true;
  const source = findTag(roots, sourceId);
  return source ? findTag(source.children, candidateId) !== null : false;
}

function canDropOnTarget(ctx: {
  roots: ForgeTag[];
  dragId: string | null;
  targetId: string;
  isSearchMode: boolean;
}): boolean {
  const { roots, dragId, targetId, isSearchMode } = ctx;
  if (isSearchMode || !dragId) return false;
  if (!findTag(roots, dragId)) return false;
  return !isSelfOrDescendant({ roots, sourceId: dragId, candidateId: targetId });
}

function computeDropZone(e: DragEvent<HTMLDivElement>): DropZone {
  const rect = e.currentTarget.getBoundingClientRect();
  const ratio = rect.height > 0 ? (e.clientY - rect.top) / rect.height : 0.5;
  if (ratio < 0.25) return 'before';
  if (ratio > 0.75) return 'after';
  return 'inside';
}

function resolveDropMove(ctx: {
  roots: ForgeTag[];
  dragId: string;
  targetRow: VisibleRow;
  zone: DropZone;
}): { parentId: string | null; index: number } {
  const { roots, dragId, targetRow, zone } = ctx;
  if (zone === 'inside') {
    return { parentId: targetRow.node.id, index: targetRow.node.children.length };
  }
  const rawIndex = targetRow.indexInParent + (zone === 'after' ? 1 : 0);
  const dragParentId = findParentId(roots, dragId);
  if (dragParentId !== targetRow.parentId) {
    return { parentId: targetRow.parentId, index: rawIndex };
  }
  const siblings = dragParentId === null ? roots : (findTag(roots, dragParentId)?.children ?? []);
  const dragIndex = siblings.findIndex((s) => s.id === dragId);
  // moveTag removes the dragged node before inserting at `index`, so forward moves shift by -1.
  const index = dragIndex !== -1 && dragIndex < rawIndex ? rawIndex - 1 : rawIndex;
  return { parentId: targetRow.parentId, index };
}

function buildRowClassName(flags: {
  isSelected: boolean;
  isDragging: boolean;
  dropZone: DropZone | null;
}): string {
  return clsx(
    styles.row,
    flags.isSelected && styles.rowSelected,
    flags.isDragging && styles.rowDragging,
    flags.dropZone && DROP_ZONE_CLASS[flags.dropZone],
  );
}

function handleRowKeyDown(ctx: {
  e: KeyboardEvent<HTMLDivElement>;
  node: ForgeTag;
  onStartRename: (node: ForgeTag) => void;
  onSelect: (id: string) => void;
}) {
  const { e, node, onStartRename, onSelect } = ctx;
  if (isEditableTarget(e.target)) return;
  if (e.key === 'F2') {
    e.preventDefault();
    e.stopPropagation();
    onStartRename(node);
  } else if (e.key === 'Enter') {
    e.preventDefault();
    onSelect(node.id);
  }
}

interface StagedPanelHeaderProps {
  search: string;
  canUndo: boolean;
  canRedo: boolean;
  canExpandToggle: boolean;
  allExpanded: boolean;
  onSearchChange: (value: string) => void;
  onUndo: () => void;
  onRedo: () => void;
  onToggleExpandAll: () => void;
  onAddRoot: () => void;
}

function StagedPanelHeader({
  search,
  canUndo,
  canRedo,
  canExpandToggle,
  allExpanded,
  onSearchChange,
  onUndo,
  onRedo,
  onToggleExpandAll,
  onAddRoot,
}: StagedPanelHeaderProps) {
  return (
    <header className={styles.header}>
      <div className={styles.headerTop}>
        <h3 className={styles.title}>Preview</h3>
      </div>
      <div className={styles.toolbar}>
        <button type="button" className={styles.toolBtn} disabled={!canUndo} title="Undo" onClick={onUndo}>
          <IconUndo />
          Undo
        </button>
        <button type="button" className={styles.toolBtn} disabled={!canRedo} title="Redo" onClick={onRedo}>
          <IconRedo />
          Redo
        </button>
        <button type="button" className={styles.toolBtn} disabled={!canExpandToggle} onClick={onToggleExpandAll}>
          {allExpanded ? 'Collapse All' : 'Expand All'}
        </button>
        <button type="button" className={clsx(styles.toolBtn, styles.addBtn)} onClick={onAddRoot}>
          <IconPlus size={12} />
          Add Tag
        </button>
      </div>
      <input
        type="search"
        className={styles.searchInput}
        placeholder="Search staged tags"
        aria-label="Search staged tags"
        value={search}
        onChange={(e) => onSearchChange(e.target.value)}
      />
    </header>
  );
}

function RowChevron({
  hasChildren,
  isExpanded,
  nodeName,
  onToggle,
}: {
  hasChildren: boolean;
  isExpanded: boolean;
  nodeName: string;
  onToggle: () => void;
}) {
  return (
    <span className={styles.chevronSlot}>
      {hasChildren && (
        <button
          type="button"
          tabIndex={-1}
          className={styles.chevronBtn}
          aria-label={isExpanded ? `Collapse ${nodeName}` : `Expand ${nodeName}`}
          onClick={(e) => {
            e.stopPropagation();
            onToggle();
          }}
        >
          {isExpanded ? <IconChevronDown size={11} /> : <IconChevronRight size={11} />}
        </button>
      )}
    </span>
  );
}

function RenameInput({
  nodeName,
  draftName,
  onChange,
  onCommit,
  onCancel,
}: {
  nodeName: string;
  draftName: string;
  onChange: (value: string) => void;
  onCommit: () => void;
  onCancel: () => void;
}) {
  const inputRef = useRef<HTMLInputElement | null>(null);
  const cancellingRef = useRef(false);

  useEffect(() => {
    cancellingRef.current = false;
    inputRef.current?.focus();
    inputRef.current?.select();
  }, []);

  const handleKeyDown = (e: KeyboardEvent<HTMLInputElement>) => {
    if (e.key === 'Enter') {
      e.preventDefault();
      e.stopPropagation();
      onCommit();
    } else if (e.key === 'Escape') {
      e.preventDefault();
      e.stopPropagation();
      cancellingRef.current = true;
      onCancel();
    }
  };

  const handleBlur = () => {
    if (cancellingRef.current) {
      cancellingRef.current = false;
      return;
    }
    onCommit();
  };

  return (
    <input
      ref={inputRef}
      type="text"
      className={styles.renameInput}
      value={draftName}
      aria-label={`Rename ${nodeName}`}
      onClick={(e) => e.stopPropagation()}
      onDoubleClick={(e) => e.stopPropagation()}
      onChange={(e) => onChange(e.target.value)}
      onKeyDown={handleKeyDown}
      onBlur={handleBlur}
    />
  );
}

function RowLabel({
  node,
  ancestorPath,
  isEditing,
  draftName,
  onDraftNameChange,
  onStartRename,
  onCommitRename,
  onCancelRename,
}: {
  node: ForgeTag;
  ancestorPath: string;
  isEditing: boolean;
  draftName: string;
  onDraftNameChange: (value: string) => void;
  onStartRename: (node: ForgeTag) => void;
  onCommitRename: (node: ForgeTag) => void;
  onCancelRename: () => void;
}) {
  if (isEditing) {
    return (
      <RenameInput
        nodeName={node.name}
        draftName={draftName}
        onChange={onDraftNameChange}
        onCommit={() => onCommitRename(node)}
        onCancel={onCancelRename}
      />
    );
  }
  return (
    <span
      className={styles.nameArea}
      onDoubleClick={(e) => {
        e.stopPropagation();
        onStartRename(node);
      }}
    >
      <span className={styles.name} title={node.name}>
        {node.name}
      </span>
      {ancestorPath && (
        <span className={styles.ancestorPath} title={ancestorPath}>
          {ancestorPath}
        </span>
      )}
    </span>
  );
}

function RowActions({
  nodeName,
  onDuplicate,
  onRemove,
}: {
  nodeName: string;
  onDuplicate: () => void;
  onRemove: () => void;
}) {
  return (
    <span className={styles.rowActions}>
      <button
        type="button"
        className={styles.iconBtn}
        title={`Duplicate ${nodeName}`}
        aria-label={`Duplicate ${nodeName}`}
        onClick={(e) => {
          e.stopPropagation();
          onDuplicate();
        }}
      >
        <IconCopy size={12} />
      </button>
      <button
        type="button"
        className={clsx(styles.iconBtn, styles.deleteBtn)}
        title={`Delete ${nodeName}`}
        aria-label={`Delete ${nodeName}`}
        onClick={(e) => {
          e.stopPropagation();
          onRemove();
        }}
      >
        <IconTrash size={12} />
      </button>
    </span>
  );
}

interface StagedRowProps {
  row: VisibleRow;
  isSelected: boolean;
  isExpanded: boolean;
  isEditing: boolean;
  isSearchMode: boolean;
  isDragging: boolean;
  dropZone: DropZone | null;
  draftName: string;
  onDraftNameChange: (value: string) => void;
  onSelect: (id: string) => void;
  onToggleExpand: (node: ForgeTag) => void;
  onStartRename: (node: ForgeTag) => void;
  onCommitRename: (node: ForgeTag) => void;
  onCancelRename: () => void;
  onDuplicate: (id: string) => void;
  onRemove: (id: string) => void;
  onDragStart: (e: DragEvent<HTMLDivElement>, node: ForgeTag) => void;
  onDragOver: (e: DragEvent<HTMLDivElement>, node: ForgeTag) => void;
  onDrop: (e: DragEvent<HTMLDivElement>, row: VisibleRow) => void;
  onDragEnd: () => void;
}

function StagedRow(props: StagedRowProps) {
  const { row, isSelected, isExpanded, isEditing, isSearchMode, isDragging, dropZone } = props;
  const { node, depth, ancestorPath } = row;
  const hasChildren = !isSearchMode && node.children.length > 0;
  const dotStyle = node.color ? { background: toCssColor(node.color) } : undefined;

  return (
    <div
      role={isSearchMode ? 'listitem' : 'treeitem'}
      aria-selected={isSelected}
      aria-expanded={hasChildren ? isExpanded : undefined}
      tabIndex={0}
      draggable={!isSearchMode && !isEditing}
      className={buildRowClassName({ isSelected, isDragging, dropZone })}
      style={{ paddingLeft: `${depth * 16 + 6}px` }}
      onClick={(e: MouseEvent<HTMLDivElement>) => {
        e.stopPropagation();
        props.onSelect(node.id);
      }}
      onKeyDown={(e) =>
        handleRowKeyDown({ e, node, onStartRename: props.onStartRename, onSelect: props.onSelect })
      }
      onDragStart={(e) => props.onDragStart(e, node)}
      onDragOver={(e) => props.onDragOver(e, node)}
      onDrop={(e) => props.onDrop(e, row)}
      onDragEnd={props.onDragEnd}
    >
      {!isSearchMode && (
        <RowChevron
          hasChildren={hasChildren}
          isExpanded={isExpanded}
          nodeName={node.name}
          onToggle={() => props.onToggleExpand(node)}
        />
      )}
      <TagIcon iconId={node.iconId} size={15} />
      <span className={styles.colorDot} style={dotStyle} aria-hidden="true" />
      <RowLabel
        node={node}
        ancestorPath={ancestorPath}
        isEditing={isEditing}
        draftName={props.draftName}
        onDraftNameChange={props.onDraftNameChange}
        onStartRename={props.onStartRename}
        onCommitRename={props.onCommitRename}
        onCancelRename={props.onCancelRename}
      />
      <RowActions
        nodeName={node.name}
        onDuplicate={() => props.onDuplicate(node.id)}
        onRemove={() => props.onRemove(node.id)}
      />
    </div>
  );
}

function useStagedPanelState(ctx: { forest: ForgeTag[]; autoRenameId?: string | null }) {
  const { forest, autoRenameId } = ctx;
  const [expandedIds, setExpandedIds] = useState<Set<string>>(
    () => new Set(forest.map((n) => n.id)),
  );
  const [editingId, setEditingId] = useState<string | null>(null);
  const [draftName, setDraftName] = useState('');
  const knownIdsRef = useRef<Set<string>>(collectIds(forest));
  const appliedAutoRenameRef = useRef<string | null>(null);

  // Auto-expand newly introduced root nodes without overriding existing collapse choices.
  useEffect(() => {
    const newRootIds = forest.filter((n) => !knownIdsRef.current.has(n.id)).map((n) => n.id);
    knownIdsRef.current = collectIds(forest);
    if (newRootIds.length > 0) {
      setExpandedIds((prev) => new Set([...prev, ...newRootIds]));
    }
    if (editingId !== null && !findTag(forest, editingId)) {
      setEditingId(null);
    }
  }, [forest, editingId]);

  useEffect(() => {
    if (!autoRenameId) {
      appliedAutoRenameRef.current = null;
      return;
    }
    if (appliedAutoRenameRef.current === autoRenameId) return;
    const target = findTag(forest, autoRenameId);
    if (!target) return;
    appliedAutoRenameRef.current = autoRenameId;
    setEditingId(autoRenameId);
    setDraftName(target.name);
  }, [autoRenameId, forest]);

  return { expandedIds, setExpandedIds, editingId, setEditingId, draftName, setDraftName };
}

export function StagedPanel(props: StagedPanelProps) {
  const { forest, selectedId, autoRenameId, onSelect, onRename, onRemove, onMove } = props;
  const [search, setSearch] = useState('');
  const [dragId, setDragId] = useState<string | null>(null);
  const [dropTarget, setDropTarget] = useState<DropTarget | null>(null);
  const state = useStagedPanelState({ forest, autoRenameId });

  const trimmedQuery = search.trim().toLowerCase();
  const isSearchMode = trimmedQuery.length > 0;
  const expandableIds = useMemo(() => collectExpandableIds(forest), [forest]);
  const allExpanded =
    expandableIds.length > 0 && expandableIds.every((id) => state.expandedIds.has(id));

  const rows = useMemo(
    () =>
      isSearchMode
        ? collectSearchMatches({ nodes: forest, query: trimmedQuery })
        : flattenVisibleTree({ nodes: forest, expandedIds: state.expandedIds }),
    [forest, state.expandedIds, isSearchMode, trimmedQuery],
  );

  const toggleExpand = (node: ForgeTag) => {
    state.setExpandedIds((prev) => {
      const next = new Set(prev);
      if (next.has(node.id)) next.delete(node.id);
      else next.add(node.id);
      return next;
    });
  };

  const commitRename = (node: ForgeTag) => {
    if (state.editingId !== node.id) return;
    state.setEditingId(null);
    const trimmed = state.draftName.trim();
    if (trimmed !== '' && trimmed !== node.name) {
      onRename(node.id, trimmed);
    }
  };

  const clearDragState = () => {
    setDragId(null);
    setDropTarget(null);
  };

  const handleDragStart = (e: DragEvent<HTMLDivElement>, node: ForgeTag) => {
    if (isSearchMode || state.editingId === node.id) {
      e.preventDefault();
      return;
    }
    e.dataTransfer.effectAllowed = 'move';
    e.dataTransfer.setData('text/plain', node.id);
    setDragId(node.id);
  };

  const handleDragOver = (e: DragEvent<HTMLDivElement>, target: ForgeTag) => {
    if (!canDropOnTarget({ roots: forest, dragId, targetId: target.id, isSearchMode })) {
      if (dropTarget !== null) setDropTarget(null);
      return;
    }
    const zone = computeDropZone(e);
    e.preventDefault();
    e.dataTransfer.dropEffect = 'move';
    setDropTarget((prev) =>
      prev && prev.id === target.id && prev.zone === zone ? prev : { id: target.id, zone },
    );
  };

  const handleDrop = (e: DragEvent<HTMLDivElement>, targetRow: VisibleRow) => {
    e.preventDefault();
    const activeDragId = dragId || e.dataTransfer.getData('text/plain');
    const targetId = targetRow.node.id;
    const allowed = canDropOnTarget({
      roots: forest,
      dragId: activeDragId,
      targetId,
      isSearchMode,
    });
    const zone =
      dropTarget && dropTarget.id === targetId ? dropTarget.zone : computeDropZone(e);
    clearDragState();
    if (!allowed || !activeDragId) return;
    const move = resolveDropMove({ roots: forest, dragId: activeDragId, targetRow, zone });
    onMove(activeDragId, move.parentId, move.index);
  };

  const handlePanelKeyDown = (e: KeyboardEvent<HTMLElement>) => {
    if (isEditableTarget(e.target) || state.editingId !== null) return;
    if (selectedId === null || !isDeleteEvent(e)) return;
    e.stopPropagation();
    e.preventDefault();
    onRemove(selectedId);
  };

  return (
    <section className={styles.panel} aria-label="Staged tags" onKeyDown={handlePanelKeyDown}>
      <StagedPanelHeader
        search={search}
        canUndo={props.canUndo}
        canRedo={props.canRedo}
        canExpandToggle={expandableIds.length > 0}
        allExpanded={allExpanded}
        onSearchChange={setSearch}
        onUndo={props.onUndo}
        onRedo={props.onRedo}
        onToggleExpandAll={() =>
          state.setExpandedIds(allExpanded ? new Set() : collectIds(forest))
        }
        onAddRoot={props.onAddRoot}
      />
      <div
        className={styles.tree}
        role={isSearchMode ? 'list' : 'tree'}
        aria-label="Staged tag hierarchy"
        onClick={() => onSelect(null)}
        onDragOver={(e) => {
          if (e.target === e.currentTarget && dropTarget !== null) setDropTarget(null);
        }}
        onDragLeave={(e) => {
          if (!e.currentTarget.contains(e.relatedTarget as Node | null)) setDropTarget(null);
        }}
      >
        {rows.length === 0 ? (
          <div className={styles.empty}>
            {forest.length === 0 ? 'No tags staged' : 'No matching tags'}
          </div>
        ) : (
          rows.map((row) => (
            <StagedRow
              key={row.node.id}
              row={row}
              isSelected={selectedId === row.node.id}
              isExpanded={state.expandedIds.has(row.node.id)}
              isEditing={state.editingId === row.node.id}
              isSearchMode={isSearchMode}
              isDragging={dragId === row.node.id}
              dropZone={dropTarget?.id === row.node.id ? dropTarget.zone : null}
              draftName={state.draftName}
              onDraftNameChange={state.setDraftName}
              onSelect={onSelect}
              onToggleExpand={toggleExpand}
              onStartRename={(node) => {
                state.setEditingId(node.id);
                state.setDraftName(node.name);
              }}
              onCommitRename={commitRename}
              onCancelRename={() => state.setEditingId(null)}
              onDuplicate={props.onDuplicate}
              onRemove={onRemove}
              onDragStart={handleDragStart}
              onDragOver={handleDragOver}
              onDrop={handleDrop}
              onDragEnd={clearDragState}
            />
          ))
        )}
      </div>
    </section>
  );
}
