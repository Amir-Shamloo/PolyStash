import { useMemo, useState } from 'react';
import clsx from 'clsx';
import { IconChevronDown, IconChevronRight } from '../Icons';
import { TagIcon } from './TagIcon';
import {
  loadPredefinedForest,
  resolveTemplateRoots,
  templates,
  toCssColor,
  type GranularityLevel,
  type PredefinedNode,
} from './tagForgeData';
import styles from './PredefinedPanel.module.css';

export interface PredefinedPanelProps {
  onStage: (node: PredefinedNode) => void;
  stagedIds: Set<string>;
}

interface VisibleTreeRow {
  node: PredefinedNode;
  depth: number;
}

const GRANULARITIES: readonly GranularityLevel[] = ['Summary', 'Balanced', 'Comprehensive'];
const DEFAULT_GRANULARITY: GranularityLevel = 'Balanced';
const INDENT_PX = 14;
const ROW_BASE_PAD_PX = 6;

function getForestRoots(
  granularity: GranularityLevel,
  activeTemplate: string | null,
): PredefinedNode[] {
  return activeTemplate
    ? resolveTemplateRoots(activeTemplate, granularity)
    : loadPredefinedForest(granularity);
}

function getRootGuids(roots: PredefinedNode[]): Set<string> {
  return new Set(roots.map((node) => node.guid));
}

function collectTreeGuids(roots: PredefinedNode[]): { all: string[]; expandable: string[] } {
  const all: string[] = [];
  const expandable: string[] = [];
  const walk = (nodes: PredefinedNode[]): void => {
    for (const node of nodes) {
      all.push(node.guid);
      if (node.children.length > 0) {
        expandable.push(node.guid);
        walk(node.children);
      }
    }
  };
  walk(roots);
  return { all, expandable };
}

function flattenVisibleTree(
  roots: PredefinedNode[],
  expandedIds: Set<string>,
): VisibleTreeRow[] {
  const rows: VisibleTreeRow[] = [];
  const walk = (nodes: PredefinedNode[], depth: number): void => {
    for (const node of nodes) {
      rows.push({ node, depth });
      if (node.children.length > 0 && expandedIds.has(node.guid)) {
        walk(node.children, depth + 1);
      }
    }
  };
  walk(roots, 0);
  return rows;
}

function matchesQuery(node: PredefinedNode, needle: string): boolean {
  if (node.name.toLowerCase().includes(needle)) return true;
  return node.aliases.some((alias) => alias.toLowerCase().includes(needle));
}

function searchForest(roots: PredefinedNode[], needle: string): PredefinedNode[] {
  const hits: PredefinedNode[] = [];
  const walk = (nodes: PredefinedNode[]): void => {
    for (const node of nodes) {
      if (matchesQuery(node, needle)) hits.push(node);
      if (node.children.length > 0) walk(node.children);
    }
  };
  walk(roots);
  return hits;
}

function TagSwatch({ node }: { node: PredefinedNode }) {
  return (
    <>
      <TagIcon iconId={node.iconId} size={14} />
      <span
        className={styles.dot}
        style={{ background: node.color ? toCssColor(node.color) : undefined }}
        aria-hidden="true"
      />
    </>
  );
}

function StageButton({
  node,
  staged,
  onStage,
}: {
  node: PredefinedNode;
  staged: boolean;
  onStage: (node: PredefinedNode) => void;
}) {
  return (
    <button
      type="button"
      className={clsx(styles.stageBtn, staged && styles.stageBtnStaged)}
      disabled={staged}
      aria-label={staged ? `${node.name} staged` : `Stage ${node.name}`}
      onClick={() => onStage(node)}
    >
      {staged ? 'Staged' : '→'}
    </button>
  );
}

function TreeRow({
  node,
  depth,
  isExpanded,
  staged,
  onToggle,
  onStage,
}: {
  node: PredefinedNode;
  depth: number;
  isExpanded: boolean;
  staged: boolean;
  onToggle: (guid: string) => void;
  onStage: (node: PredefinedNode) => void;
}) {
  const hasChildren = node.children.length > 0;
  return (
    <div
      role="treeitem"
      aria-level={depth + 1}
      aria-expanded={hasChildren ? isExpanded : undefined}
      className={styles.row}
      style={{ paddingLeft: depth * INDENT_PX + ROW_BASE_PAD_PX }}
    >
      {hasChildren ? (
        <button
          type="button"
          className={styles.chevronBtn}
          aria-label={isExpanded ? `Collapse ${node.name}` : `Expand ${node.name}`}
          onClick={() => onToggle(node.guid)}
        >
          {isExpanded ? <IconChevronDown size={12} /> : <IconChevronRight size={12} />}
        </button>
      ) : (
        <span className={styles.chevronSpacer} aria-hidden="true" />
      )}
      <TagSwatch node={node} />
      <span className={clsx(styles.name, 'truncate')} title={node.name}>
        {node.name}
      </span>
      <StageButton node={node} staged={staged} onStage={onStage} />
    </div>
  );
}

function SearchRow({
  node,
  staged,
  onStage,
}: {
  node: PredefinedNode;
  staged: boolean;
  onStage: (node: PredefinedNode) => void;
}) {
  return (
    <div role="listitem" className={styles.row}>
      <TagSwatch node={node} />
      <div className={styles.searchText}>
        <span className={clsx(styles.name, 'truncate')} title={node.name}>
          {node.name}
        </span>
        <span className={clsx(styles.searchPath, 'truncate')} title={node.path}>
          {node.path}
        </span>
      </div>
      <StageButton node={node} staged={staged} onStage={onStage} />
    </div>
  );
}

export function PredefinedPanel({ onStage, stagedIds }: PredefinedPanelProps) {
  const [query, setQuery] = useState('');
  const [activeTemplate, setActiveTemplate] = useState<string | null>(null);
  const [granularity, setGranularity] = useState<GranularityLevel>(DEFAULT_GRANULARITY);
  const [expandedIds, setExpandedIds] = useState<Set<string>>(() =>
    getRootGuids(getForestRoots(DEFAULT_GRANULARITY, null)),
  );

  const roots = useMemo(
    () => getForestRoots(granularity, activeTemplate),
    [granularity, activeTemplate],
  );

  const { all: allGuids, expandable: expandableGuids } = useMemo(
    () => collectTreeGuids(roots),
    [roots],
  );

  const needle = query.trim().toLowerCase();
  const isSearching = needle.length > 0;

  const searchResults = useMemo(
    () => (isSearching ? searchForest(roots, needle) : []),
    [roots, needle, isSearching],
  );

  const visibleRows = useMemo(
    () => (isSearching ? [] : flattenVisibleTree(roots, expandedIds)),
    [roots, expandedIds, isSearching],
  );

  const allExpanded =
    expandableGuids.length > 0 && expandableGuids.every((id) => expandedIds.has(id));

  const handleTemplateToggle = (name: string) => {
    const nextTemplate = activeTemplate === name ? null : name;
    setActiveTemplate(nextTemplate);
    setExpandedIds(getRootGuids(getForestRoots(granularity, nextTemplate)));
  };

  const handleGranularityChange = (nextGranularity: GranularityLevel) => {
    if (nextGranularity === granularity) return;
    setGranularity(nextGranularity);
    setExpandedIds(getRootGuids(getForestRoots(nextGranularity, activeTemplate)));
  };

  const handleToggleExpandAll = () => {
    setExpandedIds(allExpanded ? new Set() : new Set(allGuids));
  };

  const handleToggleNode = (guid: string) => {
    setExpandedIds((prev) => {
      const next = new Set(prev);
      if (next.has(guid)) next.delete(guid);
      else next.add(guid);
      return next;
    });
  };

  return (
    <section className={styles.panel} aria-label="Predefined tags">
      <header className={styles.header}>
        <h3 className={styles.title}>Tags and Templates</h3>
      </header>

      <input
        className={styles.input}
        type="search"
        placeholder="Search predefined tags"
        aria-label="Search predefined tags"
        value={query}
        onChange={(e) => setQuery(e.target.value)}
      />

      <div className={styles.pills} role="group" aria-label="Templates">
        {templates.map((template) => {
          const active = activeTemplate === template.name;
          return (
            <button
              key={template.name}
              type="button"
              className={clsx(styles.pill, active && styles.pillActive)}
              aria-pressed={active}
              onClick={() => handleTemplateToggle(template.name)}
            >
              {template.name}
            </button>
          );
        })}
      </div>

      <div className={styles.controls}>
        <button
          type="button"
          className={styles.controlBtn}
          disabled={isSearching || expandableGuids.length === 0}
          onClick={handleToggleExpandAll}
        >
          {allExpanded ? 'Collapse All' : 'Expand All'}
        </button>

        <div className={styles.segmented} role="group" aria-label="Granularity">
          {GRANULARITIES.map((level) => {
            const active = granularity === level;
            return (
              <button
                key={level}
                type="button"
                className={clsx(styles.segBtn, active && styles.segBtnActive)}
                aria-pressed={active}
                onClick={() => handleGranularityChange(level)}
              >
                {level}
              </button>
            );
          })}
        </div>
      </div>

      {isSearching ? (
        <div className={styles.treeScroll} role="list" aria-label="Search results">
          {searchResults.length === 0 ? (
            <div className={styles.empty}>No tags found</div>
          ) : (
            searchResults.map((node) => (
              <SearchRow
                key={node.guid}
                node={node}
                staged={stagedIds.has(node.guid)}
                onStage={onStage}
              />
            ))
          )}
        </div>
      ) : (
        <div className={styles.treeScroll} role="tree" aria-label="Predefined tag tree">
          {visibleRows.length === 0 ? (
            <div className={styles.empty}>No tags found</div>
          ) : (
            visibleRows.map(({ node, depth }) => (
              <TreeRow
                key={node.guid}
                node={node}
                depth={depth}
                isExpanded={expandedIds.has(node.guid)}
                staged={stagedIds.has(node.guid)}
                onToggle={handleToggleNode}
                onStage={onStage}
              />
            ))
          )}
        </div>
      )}
    </section>
  );
}
