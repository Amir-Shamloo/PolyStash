import { useState, useRef, useMemo, useCallback } from 'react';
import clsx from 'clsx';
import type { AssetKind } from '../../types';
import { useAssetStore } from '../../store';
import { useOutsideDismiss } from './useOutsideDismiss';
import styles from './AssetTypeDropdown.module.css';

interface KindDefinition {
  kind: AssetKind;
  label: string;
  badge: string;
  formats: string[];
  color: string;
  icon: (props: { size?: number }) => JSX.Element;
}

const KIND_DEFS: KindDefinition[] = [
  {
    kind: '3d_model',
    label: '3D Models',
    badge: '3D',
    formats: ['max', '3dm', 'rvt', 'rfa', 'blend', 'fbx', 'obj', 'gltf', 'glb', 'c4d', 'skp', 'usd', 'usdz', 'stl', 'ply', '3ds', 'dae', 'dwg', 'dxf', 'wrl', 'x3d'],
    color: '#3b82f6',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z" />
        <polyline points="3.27 6.96 12 12.01 20.73 6.96" />
        <line x1="12" y1="22.08" x2="12" y2="12" />
      </svg>
    ),
  },
  {
    kind: 'material',
    label: 'Materials',
    badge: 'MAT',
    formats: ['vrmat', 'mat', 'sbsar', 'sbs', 'mdl'],
    color: '#a855f7',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <circle cx="12" cy="12" r="9" />
        <path d="M12 3a9 9 0 0 1 9 9 9 9 0 0 1-9 9" fill="currentColor" fillOpacity={0.2} />
        <circle cx="9" cy="9" r="2" fill="currentColor" />
      </svg>
    ),
  },
  {
    kind: 'texture',
    label: 'Textures',
    badge: 'TEX',
    formats: ['jpg', 'jpeg', 'png', 'tga', 'tif', 'tiff', 'bmp', 'dds', 'ktx', 'ktx2', 'webp', 'avif'],
    color: '#06b6d4',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <rect x="3" y="3" width="18" height="18" rx="2" />
        <path d="M3 9h18M9 21V9M15 21V9M3 15h18" strokeOpacity={0.7} />
      </svg>
    ),
  },
  {
    kind: 'hdri',
    label: 'HDRI Environments',
    badge: 'HDR',
    formats: ['hdr', 'exr', 'hdri'],
    color: '#f59e0b',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <circle cx="12" cy="12" r="5" />
        <path d="M12 1v2M12 21v2M4.22 4.22l1.42 1.42M18.36 18.36l1.42 1.42M1 12h2M21 12h2M4.22 19.78l1.42-1.42M18.36 5.64l1.42-1.42" />
      </svg>
    ),
  },
  {
    kind: 'ies',
    label: 'IES Light Profiles',
    badge: 'IES',
    formats: ['ies', 'ldt'],
    color: '#eab308',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <path d="M12 2v6M9 8h6l3 10H6L9 8z" />
        <path d="M8 22h8" />
      </svg>
    ),
  },
  {
    kind: 'video',
    label: 'Videos',
    badge: 'VID',
    formats: ['mp4', 'mov', 'avi', 'mkv', 'webm', 'm4v'],
    color: '#ef4444',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <rect x="2" y="4" width="20" height="16" rx="2" />
        <polygon points="10 8 16 12 10 16 10 8" fill="currentColor" />
      </svg>
    ),
  },
  {
    kind: 'image',
    label: 'Images & Vectors',
    badge: 'IMG',
    formats: ['psd', 'psb', 'ai', 'svg', 'gif'],
    color: '#10b981',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <rect x="3" y="3" width="18" height="18" rx="2" />
        <circle cx="8.5" cy="8.5" r="1.5" />
        <polyline points="21 15 16 10 5 21" />
      </svg>
    ),
  },
  {
    kind: 'document',
    label: 'Documents',
    badge: 'DOC',
    formats: ['pdf', 'docx', 'txt', 'md', 'rtf'],
    color: '#6366f1',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z" />
        <polyline points="14 2 14 8 20 8" />
        <line x1="16" y1="13" x2="8" y2="13" />
        <line x1="16" y1="17" x2="8" y2="17" />
        <line x1="10" y1="9" x2="8" y2="9" />
      </svg>
    ),
  },
  {
    kind: 'other',
    label: 'Other Assets',
    badge: 'ETC',
    formats: ['abc', 'zip'],
    color: '#94a3b8',
    icon: ({ size = 16 }) => (
      <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={1.75} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
        <rect x="3" y="3" width="18" height="18" rx="2" />
        <circle cx="8" cy="12" r="1" fill="currentColor" />
        <circle cx="12" cy="12" r="1" fill="currentColor" />
        <circle cx="16" cy="12" r="1" fill="currentColor" />
      </svg>
    ),
  },
];

const ALL_FORMATS = new Set(KIND_DEFS.flatMap((d) => d.formats));

function isKindSelected(
  def: KindDefinition,
  selectedKinds: AssetKind[],
  selectedExts: string[]
): 'all' | 'partial' | 'none' {
  if (selectedKinds.includes(def.kind)) return 'all';
  const matchingCount = def.formats.filter((ext) => selectedExts.includes(ext)).length;
  if (matchingCount === 0) return 'none';
  if (matchingCount === def.formats.length) return 'all';
  return 'partial';
}

function isFormatSelected(
  ext: string,
  kind: AssetKind,
  selectedKinds: AssetKind[],
  selectedExts: string[]
): boolean {
  return selectedKinds.includes(kind) || selectedExts.includes(ext);
}

interface FormatChipsProps {
  def: KindDefinition;
  visibleFormats: string[];
  selectedKinds: AssetKind[];
  selectedExts: string[];
  onToggleFormat: (ext: string, def: KindDefinition) => void;
}

function FormatChipsList({
  def,
  visibleFormats,
  selectedKinds,
  selectedExts,
  onToggleFormat,
}: FormatChipsProps) {
  return (
    <div className={styles.formatsGrid} role="group" aria-label={`${def.label} formats`}>
      {visibleFormats.map((ext) => {
        const active = isFormatSelected(ext, def.kind, selectedKinds, selectedExts);
        return (
          <button
            key={ext}
            type="button"
            id={`asset-format-item-${ext}`}
            className={clsx(styles.formatChipBtn, active && styles.formatChipActive)}
            onClick={() => onToggleFormat(ext, def)}
            aria-pressed={active}
          >
            <span>.{ext}</span>
          </button>
        );
      })}
    </div>
  );
}

function KindCheckbox({
  selectionState,
  onToggle,
}: {
  selectionState: 'all' | 'partial' | 'none';
  onToggle: () => void;
}) {
  const isChecked = selectionState === 'all';
  const isPartial = selectionState === 'partial';

  return (
    <label className={styles.checkboxWrap} onClick={(e) => e.stopPropagation()}>
      <input
        type="checkbox"
        className={styles.checkboxInput}
        checked={isChecked}
        onChange={onToggle}
      />
      <span
        className={clsx(
          styles.checkboxCustom,
          isChecked && styles.checkboxChecked,
          isPartial && styles.checkboxPartial
        )}
      >
        {isChecked && (
          <svg width={10} height={10} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={3} strokeLinecap="round" strokeLinejoin="round">
            <polyline points="20 6 9 17 4 12" />
          </svg>
        )}
        {isPartial && (
          <svg width={10} height={10} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={3} strokeLinecap="round">
            <line x1="5" y1="12" x2="19" y2="12" />
          </svg>
        )}
      </span>
    </label>
  );
}

function ExpandButton({
  label,
  isExpanded,
  onClick,
}: {
  label: string;
  isExpanded: boolean;
  onClick: (e: React.MouseEvent) => void;
}) {
  const ariaLabel = isExpanded ? `Collapse ${label}` : `Expand ${label}`;
  const title = isExpanded ? 'Hide formats' : 'Show individual formats';

  return (
    <button
      type="button"
      className={clsx(styles.expandBtn, isExpanded && styles.expandBtnRotated)}
      onClick={onClick}
      aria-label={ariaLabel}
      title={title}
    >
      <svg width={11} height={11} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2.5} strokeLinecap="round" strokeLinejoin="round">
        <polyline points="9 18 15 12 9 6" />
      </svg>
    </button>
  );
}

interface ItemRowProps {
  def: KindDefinition;
  visibleFormats: string[];
  selectionState: 'all' | 'partial' | 'none';
  isExpanded: boolean;
  selectedKinds: AssetKind[];
  selectedExts: string[];
  onToggleExpand: (kind: AssetKind) => void;
  onSelectKindAll: (def: KindDefinition) => void;
  onSelectKindNone: (def: KindDefinition) => void;
  onToggleKind: (def: KindDefinition) => void;
  onToggleFormat: (ext: string, def: KindDefinition) => void;
}

function AssetTypeItemRow({
  def,
  visibleFormats,
  selectionState,
  isExpanded,
  selectedKinds,
  selectedExts,
  onToggleExpand,
  onSelectKindAll,
  onSelectKindNone,
  onToggleKind,
  onToggleFormat,
}: ItemRowProps) {
  const IconComp = def.icon;
  const isActive = selectionState !== 'none';

  const handleKeyDown = (e: React.KeyboardEvent) => {
    if (e.key === ' ' || e.key === 'Enter') {
      e.preventDefault();
      onToggleKind(def);
    }
  };

  return (
    <div className={styles.kindSection}>
      <div
        id={`asset-type-item-${def.kind}`}
        className={clsx(styles.itemRow, isActive && styles.itemRowChecked)}
        onClick={() => onToggleKind(def)}
        role="option"
        aria-selected={selectionState === 'all'}
        tabIndex={0}
        onKeyDown={handleKeyDown}
      >
        <ExpandButton
          label={def.label}
          isExpanded={isExpanded}
          onClick={(e) => {
            e.stopPropagation();
            onToggleExpand(def.kind);
          }}
        />
        <KindCheckbox selectionState={selectionState} onToggle={() => onToggleKind(def)} />

        <div
          className={styles.iconBadge}
          style={{ color: def.color, background: `${def.color}18`, borderColor: `${def.color}35` }}
        >
          <IconComp size={15} />
        </div>

        <div className={styles.itemMeta}>
          <div className={styles.itemTitleRow}>
            <span className={styles.itemLabel}>{def.label}</span>
            <div className={styles.kindActions} onClick={(e) => e.stopPropagation()}>
              <button type="button" className={styles.miniBtn} onClick={() => onSelectKindAll(def)}>
                All
              </button>
              <button type="button" className={styles.miniBtn} onClick={() => onSelectKindNone(def)}>
                None
              </button>
              <span className={styles.itemBadge} style={{ color: def.color, borderColor: `${def.color}40` }}>
                {def.badge}
              </span>
            </div>
          </div>
          <span className={styles.itemFormats} title={def.formats.join(', ')}>
            {def.formats.join(', ')}
          </span>
        </div>
      </div>

      {isExpanded && (
        <FormatChipsList
          def={def}
          visibleFormats={visibleFormats}
          selectedKinds={selectedKinds}
          selectedExts={selectedExts}
          onToggleFormat={onToggleFormat}
        />
      )}
    </div>
  );
}

function getFilteredDefs(term: string): { def: KindDefinition; visibleFormats: string[] }[] {
  const q = term.trim().toLowerCase().replace(/^\./, '');
  if (!q) {
    return KIND_DEFS.map((def) => ({ def, visibleFormats: def.formats }));
  }
  const results: { def: KindDefinition; visibleFormats: string[] }[] = [];
  for (const def of KIND_DEFS) {
    const labelMatch = def.label.toLowerCase().includes(q) || def.badge.toLowerCase().includes(q);
    const matchingFormats = def.formats.filter((f) => f.includes(q));
    if (labelMatch || matchingFormats.length > 0) {
      results.push({
        def,
        visibleFormats: matchingFormats.length > 0 ? matchingFormats : def.formats,
      });
    }
  }
  return results;
}

interface PopoverProps {
  selectedKinds: AssetKind[];
  selectedExts: string[];
  isAllSelected: boolean;
  onToggleKind: (def: KindDefinition) => void;
  onSelectKindAll: (def: KindDefinition) => void;
  onSelectKindNone: (def: KindDefinition) => void;
  onToggleFormat: (ext: string, def: KindDefinition) => void;
  onSelectAll: () => void;
  onSelectNone: () => void;
}

function AssetTypePopover({
  selectedKinds,
  selectedExts,
  isAllSelected,
  onToggleKind,
  onSelectKindAll,
  onSelectKindNone,
  onToggleFormat,
  onSelectAll,
  onSelectNone,
}: PopoverProps) {
  const [searchTerm, setSearchTerm] = useState('');
  const [expandedKinds, setExpandedKinds] = useState<Record<string, boolean>>({});

  const filteredItems = useMemo(() => getFilteredDefs(searchTerm), [searchTerm]);
  const hasSearch = searchTerm.trim().length > 0;

  const toggleExpand = useCallback((kind: AssetKind) => {
    setExpandedKinds((prev) => ({ ...prev, [kind]: !prev[kind] }));
  }, []);

  return (
    <div className={styles.popover} role="dialog" aria-label="Asset type filters">
      <div className={styles.popoverHeader}>
        <span className={styles.headerTitle}>Filter by Asset Type</span>
        <div className={styles.headerActions}>
          <button
            type="button"
            id="asset-type-all-btn"
            className={clsx(styles.allBtn, isAllSelected && styles.allBtnActive)}
            onClick={onSelectAll}
          >
            All
          </button>
          <button
            type="button"
            id="asset-type-none-btn"
            className={styles.allBtn}
            onClick={onSelectNone}
          >
            None
          </button>
        </div>
      </div>

      <div className={styles.searchBox}>
        <svg width={13} height={13} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" className={styles.searchBoxIcon} aria-hidden>
          <circle cx="11" cy="11" r="8" />
          <line x1="21" y1="21" x2="16.65" y2="16.65" />
        </svg>
        <input
          type="text"
          autoFocus
          placeholder="Search types & extensions…"
          className={styles.searchBoxInput}
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
        />
        {searchTerm && (
          <button
            type="button"
            className={styles.searchClearBtn}
            onClick={() => setSearchTerm('')}
            title="Clear type search"
            aria-label="Clear type search"
          >
            ✕
          </button>
        )}
      </div>

      <div className={styles.list} role="listbox" aria-multiselectable="true">
        {filteredItems.map(({ def, visibleFormats }) => (
          <AssetTypeItemRow
            key={def.kind}
            def={def}
            visibleFormats={visibleFormats}
            selectionState={isKindSelected(def, selectedKinds, selectedExts)}
            isExpanded={hasSearch || Boolean(expandedKinds[def.kind])}
            selectedKinds={selectedKinds}
            selectedExts={selectedExts}
            onToggleExpand={toggleExpand}
            onSelectKindAll={onSelectKindAll}
            onSelectKindNone={onSelectKindNone}
            onToggleKind={onToggleKind}
            onToggleFormat={onToggleFormat}
          />
        ))}
        {filteredItems.length === 0 && (
          <p className={styles.noMatches}>No asset types match &ldquo;{searchTerm}&rdquo;</p>
        )}
      </div>

      <div className={styles.popoverFooter}>
        <span className={styles.footerSummary}>
          {isAllSelected
            ? 'Showing all formats'
            : `${selectedKinds.length} group(s), ${selectedExts.length} format(s)`}
        </span>
        {!isAllSelected && (
          <button type="button" className={styles.resetBtn} onClick={onSelectAll} title="Reset to all types">
            Reset
          </button>
        )}
      </div>
    </div>
  );
}

function getTriggerInfo(selectedKinds: AssetKind[], selectedExts: string[]): { label: string; count: number } {
  const totalSelected = selectedKinds.length + selectedExts.length;
  if (totalSelected === 0) return { label: 'All Types', count: 0 };
  if (selectedKinds.length === 1 && selectedExts.length === 0) {
    const match = KIND_DEFS.find((d) => d.kind === selectedKinds[0]);
    return { label: match ? match.label : '1 Type', count: 1 };
  }
  if (selectedKinds.length === 0 && selectedExts.length === 1) {
    return { label: `.${selectedExts[0].toUpperCase()}`, count: 1 };
  }
  return { label: `${totalSelected} Selected`, count: totalSelected };
}

interface TriggerProps {
  isOpen: boolean;
  isAllSelected: boolean;
  selectedCount: number;
  triggerLabel: string;
  onToggle: () => void;
}

function AssetTypeTrigger({
  isOpen,
  isAllSelected,
  selectedCount,
  triggerLabel,
  onToggle,
}: TriggerProps) {
  const title = isAllSelected ? 'Filter by asset type' : `Filtering by: ${triggerLabel}`;

  return (
    <button
      id="asset-type-dropdown-trigger"
      type="button"
      className={clsx(styles.triggerBtn, !isAllSelected && styles.triggerActive)}
      onClick={onToggle}
      aria-haspopup="listbox"
      aria-expanded={isOpen}
      title={title}
    >
      <span className={styles.triggerIconWrap}>
        <svg width={14} height={14} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
          <polygon points="12 2 2 7 12 12 22 7 12 2" />
          <polyline points="2 17 12 22 22 17" />
          <polyline points="2 12 12 17 22 12" />
        </svg>
      </span>
      <span className={styles.triggerText}>{triggerLabel}</span>
      {!isAllSelected && <span className={styles.countBadge}>{selectedCount}</span>}
      <span className={clsx(styles.chevron, isOpen && styles.chevronRotated)}>
        <svg width={11} height={11} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2.5} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
          <polyline points="6 9 12 15 18 9" />
        </svg>
      </span>
    </button>
  );
}

function computeNextFormatSelection(
  ext: string,
  def: KindDefinition,
  selectedKinds: AssetKind[],
  selectedExts: string[]
): { kinds: AssetKind[]; exts: string[] } {
  // Explode the toggled kind into explicit extensions so single formats can be deselected
  const nextKinds = selectedKinds.filter((k) => k !== def.kind);
  const activeExts = new Set(selectedExts);
  if (selectedKinds.includes(def.kind)) {
    def.formats.forEach((f) => activeExts.add(f));
  }
  if (activeExts.has(ext)) {
    activeExts.delete(ext);
  } else {
    activeExts.add(ext);
  }
  return { kinds: nextKinds, exts: Array.from(activeExts) };
}

function useAssetTypeFilters() {
  const kinds = useAssetStore((s) => s.query.filter.kinds);
  const extensions = useAssetStore((s) => s.query.filter.extensions);
  const setFilter = useAssetStore((s) => s.setFilter);
  const search = useAssetStore((s) => s.search);

  const selectedKinds = useMemo(() => kinds ?? [], [kinds]);
  const selectedExts = useMemo(() => extensions ?? [], [extensions]);
  const isAllSelected = selectedKinds.length === 0 && selectedExts.length === 0;

  const applyFilter = useCallback(
    (nextKinds: AssetKind[], nextExts: string[]) => {
      setFilter({
        kinds: nextKinds.length > 0 ? nextKinds : undefined,
        extensions: nextExts.length > 0 ? nextExts : undefined,
      });
      search();
    },
    [setFilter, search]
  );

  const handleSelectKindAll = useCallback(
    (def: KindDefinition) => {
      const nextKinds = selectedKinds.includes(def.kind) ? selectedKinds : [...selectedKinds, def.kind];
      const nextExts = selectedExts.filter((e) => !def.formats.includes(e));
      applyFilter(nextKinds, nextExts);
    },
    [selectedKinds, selectedExts, applyFilter]
  );

  const handleSelectKindNone = useCallback(
    (def: KindDefinition) => {
      const nextKinds = selectedKinds.filter((k) => k !== def.kind);
      const nextExts = selectedExts.filter((e) => !def.formats.includes(e));
      applyFilter(nextKinds, nextExts);
    },
    [selectedKinds, selectedExts, applyFilter]
  );

  const handleToggleKind = useCallback(
    (def: KindDefinition) => {
      if (isKindSelected(def, selectedKinds, selectedExts) === 'all') {
        handleSelectKindNone(def);
      } else {
        handleSelectKindAll(def);
      }
    },
    [selectedKinds, selectedExts, handleSelectKindAll, handleSelectKindNone]
  );

  const handleToggleFormat = useCallback(
    (ext: string, def: KindDefinition) => {
      const next = computeNextFormatSelection(ext, def, selectedKinds, selectedExts);
      applyFilter(next.kinds, next.exts);
    },
    [selectedKinds, selectedExts, applyFilter]
  );

  const handleSelectAll = useCallback(() => applyFilter([], []), [applyFilter]);

  const handleSelectNone = useCallback(() => {
    setFilter({ kinds: undefined, extensions: ['__none__'] });
    search();
  }, [setFilter, search]);

  return {
    selectedKinds,
    selectedExts,
    isAllSelected,
    handleSelectKindAll,
    handleSelectKindNone,
    handleToggleKind,
    handleToggleFormat,
    handleSelectAll,
    handleSelectNone,
  };
}

export function AssetTypeDropdown() {
  const [isOpen, setIsOpen] = useState(false);
  const containerRef = useRef<HTMLDivElement>(null);
  const closeDropdown = useCallback(() => setIsOpen(false), []);
  const toggleDropdown = useCallback(() => setIsOpen((prev) => !prev), []);

  useOutsideDismiss(containerRef, isOpen, closeDropdown);

  const filters = useAssetTypeFilters();
  const validExts = useMemo(
    () => filters.selectedExts.filter((e) => ALL_FORMATS.has(e)),
    [filters.selectedExts]
  );

  const { label: triggerLabel, count: selectedCount } = useMemo(
    () => getTriggerInfo(filters.selectedKinds, validExts),
    [filters.selectedKinds, validExts]
  );

  return (
    <div className={styles.container} ref={containerRef}>
      <AssetTypeTrigger
        isOpen={isOpen}
        isAllSelected={filters.isAllSelected}
        selectedCount={selectedCount}
        triggerLabel={triggerLabel}
        onToggle={toggleDropdown}
      />
      {isOpen && (
        <AssetTypePopover
          selectedKinds={filters.selectedKinds}
          selectedExts={validExts}
          isAllSelected={filters.isAllSelected}
          onToggleKind={filters.handleToggleKind}
          onSelectKindAll={filters.handleSelectKindAll}
          onSelectKindNone={filters.handleSelectKindNone}
          onToggleFormat={filters.handleToggleFormat}
          onSelectAll={filters.handleSelectAll}
          onSelectNone={filters.handleSelectNone}
        />
      )}
    </div>
  );
}
