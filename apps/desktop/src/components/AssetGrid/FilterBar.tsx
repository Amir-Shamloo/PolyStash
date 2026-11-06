import { useMemo, useCallback } from 'react';
import clsx from 'clsx';
import { useAssetStore } from '../../store';
import type { ColorLabel, FilterSpec, SearchQuery } from '../../types';
import { IconFilter, IconX, IconHeartFilled } from '../Icons';
import styles from './FilterBar.module.css';

const COLOR_LABELS: ColorLabel[] = [
  'red', 'orange', 'yellow', 'green', 'teal', 'blue', 'purple', 'pink',
];

const FIVE_MB = 5 * 1024 * 1024;
const FIFTY_MB = 50 * 1024 * 1024;
const ONE_DAY_MS = 86400000;

interface PresetOption {
  value: string;
  label: string;
}

const DATE_OPTIONS: PresetOption[] = [
  { value: '', label: 'Any date' },
  { value: 'today', label: 'Today' },
  { value: '7d', label: 'Last 7 days' },
  { value: '30d', label: 'Last 30 days' },
];

const SIZE_OPTIONS: PresetOption[] = [
  { value: '', label: 'Any size' },
  { value: 'lt5', label: '< 5 MB' },
  { value: '5to50', label: '5–50 MB' },
  { value: 'gt50', label: '> 50 MB' },
];

function presetFromDateRange(from: number | undefined): string {
  if (from === undefined) return '';
  const now = Date.now();
  const d = new Date();
  d.setHours(0, 0, 0, 0);
  if (from >= d.getTime()) return 'today';
  if (from >= now - 7 * ONE_DAY_MS - 10000) return '7d';
  if (from >= now - 30 * ONE_DAY_MS - 10000) return '30d';
  return '';
}

function dateRangeFromPreset(val: string): { from: number } | undefined {
  if (val === 'today') {
    const d = new Date();
    d.setHours(0, 0, 0, 0);
    return { from: d.getTime() };
  }
  const daysOffset: Record<string, number> = {
    '7d': 7,
    '30d': 30,
  };
  const days = daysOffset[val];
  return days !== undefined ? { from: Date.now() - days * ONE_DAY_MS } : undefined;
}

const SIZE_PRESET_MAP: Record<string, { min?: number; max?: number }> = {
  lt5: { max: FIVE_MB },
  '5to50': { min: FIVE_MB, max: FIFTY_MB },
  gt50: { min: FIFTY_MB },
};

function sizeRangeFromPreset(val: string): { min?: number; max?: number } | undefined {
  return SIZE_PRESET_MAP[val];
}

function presetFromSizeRange(r: { min?: number; max?: number } | undefined): string {
  if (!r) return '';
  const key = `${r.min ?? 'none'}-${r.max ?? 'none'}`;
  const keyMap: Record<string, string> = {
    [`none-${FIVE_MB}`]: 'lt5',
    [`${FIVE_MB}-${FIFTY_MB}`]: '5to50',
    [`${FIFTY_MB}-none`]: 'gt50',
  };
  return keyMap[key] ?? '';
}

function hasArrayFilters(f: FilterSpec): boolean {
  const lists = [f.kinds, f.colorLabels, f.tags, f.extensions];
  for (const list of lists) {
    if (list && list.length > 0) return true;
  }
  return false;
}

function hasRangeFilters(f: FilterSpec): boolean {
  if (f.rating !== undefined) return true;
  if (f.dateRange !== undefined) return true;
  return f.sizeRange !== undefined;
}

function checkHasActiveFilters(f: FilterSpec): boolean {
  if (f.isFavorite) return true;
  if (hasRangeFilters(f)) return true;
  return hasArrayFilters(f);
}

function toggleColorLabelItem(
  current: ColorLabel[] | undefined,
  label: ColorLabel
): ColorLabel[] | undefined {
  const list = current ?? [];
  const next = list.includes(label) ? list.filter((l) => l !== label) : [...list, label];
  return next.length > 0 ? next : undefined;
}

interface ColorLabelButtonProps {
  label: ColorLabel;
  isSelected: boolean;
  onToggle: (label: ColorLabel) => void;
}

function ColorLabelButton({ label, isSelected, onToggle }: ColorLabelButtonProps) {
  return (
    <button
      id={`filter-color-${label}`}
      type="button"
      className={clsx(styles.colorDot, isSelected && styles.colorActive)}
      style={{ background: `var(--swatch-${label})` }}
      title={`Filter by ${label}`}
      aria-label={`Color label ${label}`}
      onClick={() => onToggle(label)}
    />
  );
}

interface ColorLabelsRowProps {
  selectedLabels?: ColorLabel[];
  onToggle: (label: ColorLabel) => void;
}

function ColorLabelsRow({ selectedLabels = [], onToggle }: ColorLabelsRowProps) {
  return (
    <div className={styles.colorGroup} role="group" aria-label="Color label filters">
      {COLOR_LABELS.map((label) => (
        <ColorLabelButton
          key={label}
          label={label}
          isSelected={selectedLabels.includes(label)}
          onToggle={onToggle}
        />
      ))}
    </div>
  );
}

interface PresetSelectProps {
  id: string;
  value: string;
  options: PresetOption[];
  ariaLabel: string;
  onChange: (val: string) => void;
}

function PresetSelect({ id, value, options, ariaLabel, onChange }: PresetSelectProps) {
  return (
    <select
      id={id}
      className={styles.select}
      value={value}
      onChange={(e) => onChange(e.target.value)}
      aria-label={ariaLabel}
    >
      {options.map((opt) => (
        <option key={opt.value} value={opt.value}>
          {opt.label}
        </option>
      ))}
    </select>
  );
}

interface SelectFilterProps {
  onChange: (patch: Partial<FilterSpec>) => void;
}

function DateFilterSelect({
  from,
  onChange,
}: SelectFilterProps & { from: number | undefined }) {
  const handleChange = (val: string) => {
    onChange({ dateRange: dateRangeFromPreset(val) });
  };

  return (
    <PresetSelect
      id="filter-date-added"
      value={presetFromDateRange(from)}
      options={DATE_OPTIONS}
      ariaLabel="Filter by date added"
      onChange={handleChange}
    />
  );
}

function SizeFilterSelect({
  sizeRange,
  onChange,
}: SelectFilterProps & { sizeRange: FilterSpec['sizeRange'] }) {
  const handleChange = (val: string) => {
    onChange({ sizeRange: sizeRangeFromPreset(val) });
  };

  return (
    <PresetSelect
      id="filter-size"
      value={presetFromSizeRange(sizeRange)}
      options={SIZE_OPTIONS}
      ariaLabel="Filter by file size"
      onChange={handleChange}
    />
  );
}

function RatingFilterSelect({
  rating,
  onChange,
}: SelectFilterProps & { rating: FilterSpec['rating'] }) {
  const handleChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    const min = parseInt(e.target.value, 10);
    onChange({ rating: min > 0 ? { min, max: 5 } : undefined });
  };

  return (
    <select
      id="filter-rating"
      className={styles.select}
      value={rating?.min ?? 0}
      onChange={handleChange}
      aria-label="Filter by rating"
    >
      <option value={0}>Any rating</option>
      <option value={1}>★+</option>
      <option value={2}>★★+</option>
      <option value={3}>★★★+</option>
      <option value={4}>★★★★+</option>
      <option value={5}>★★★★★</option>
    </select>
  );
}

interface ButtonProps {
  isActive?: boolean;
  onClick: () => void;
}

function AllAssetsButton({ isActive, onClick }: ButtonProps) {
  return (
    <button
      type="button"
      id="filter-all"
      className={clsx(styles.pill, isActive && styles.activePill)}
      onClick={onClick}
      title="Show all assets (clear filters)"
    >
      All assets
    </button>
  );
}

function FavoriteFilterButton({
  isFavorite,
  onClick,
}: {
  isFavorite?: boolean;
  onClick: () => void;
}) {
  return (
    <button
      type="button"
      id="filter-favorites"
      className={clsx(styles.favoritePill, isFavorite && styles.favoriteActive)}
      onClick={onClick}
      title="Filter favorite assets"
    >
      <IconHeartFilled
        size={12}
        className={clsx(isFavorite ? styles.heartActive : styles.heartMuted)}
      />
      <span>Favorites</span>
    </button>
  );
}

function SortOrderButton({
  order,
  onToggle,
}: {
  order: SearchQuery['sort']['order'];
  onToggle: () => void;
}) {
  const isAsc = order === 'asc';
  const label = isAsc ? 'Ascending (click to toggle)' : 'Descending (click to toggle)';

  return (
    <button
      id="filter-sort-order"
      type="button"
      className={styles.sortOrderBtn}
      onClick={onToggle}
      title={label}
      aria-label={isAsc ? 'Sort ascending' : 'Sort descending'}
    >
      {isAsc ? '↑' : '↓'}
    </button>
  );
}

function ClearFiltersButton({ onClear }: { onClear: () => void }) {
  return (
    <button
      id="filter-clear"
      type="button"
      className={styles.clearBtn}
      onClick={onClear}
      title="Clear all filters"
    >
      <IconX size={11} />
      <span>Clear</span>
    </button>
  );
}

interface SortAndClearProps {
  sort: SearchQuery['sort'];
  hasActiveFilters: boolean;
  onSetSort: (field: SearchQuery['sort']['field'], order: SearchQuery['sort']['order']) => void;
  onClear: () => void;
  onSearch: () => void;
}

function SortAndClearControls({
  sort,
  hasActiveFilters,
  onSetSort,
  onClear,
  onSearch,
}: SortAndClearProps) {
  const handleSortFieldChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    const field = e.target.value as SearchQuery['sort']['field'];
    const order = field === 'name' ? 'asc' : 'desc';
    onSetSort(field, order);
    onSearch();
  };

  const handleSortOrderToggle = () => {
    const nextOrder = sort.order === 'asc' ? 'desc' : 'asc';
    onSetSort(sort.field, nextOrder);
    onSearch();
  };

  return (
    <div className={styles.rightGroup}>
      <div className={styles.sortGroup}>
        <label htmlFor="filter-sort-field" className={styles.sortLabel}>
          Sort:
        </label>
        <select
          id="filter-sort-field"
          className={styles.select}
          value={sort.field}
          onChange={handleSortFieldChange}
          aria-label="Sort assets by"
        >
          <option value="date">Date</option>
          <option value="name">Name</option>
          <option value="size">Size</option>
          <option value="rating">Rating</option>
          <option value="favorite">Favorites</option>
          <option value="kind">Kind</option>
        </select>

        <SortOrderButton order={sort.order} onToggle={handleSortOrderToggle} />
      </div>

      {hasActiveFilters && <ClearFiltersButton onClear={onClear} />}
    </div>
  );
}

export function FilterBar() {
  const { query, setFilter, resetFilter, setSort, search } = useAssetStore();
  const { filter, sort } = query;

  const hasActiveFilters = useMemo(() => checkHasActiveFilters(filter), [filter]);

  const handleAllClick = useCallback(() => {
    resetFilter();
    search();
  }, [resetFilter, search]);

  const toggleFavorite = useCallback(() => {
    setFilter({ isFavorite: filter.isFavorite ? undefined : true });
    search();
  }, [filter.isFavorite, setFilter, search]);

  const handleToggleColorLabel = useCallback(
    (label: ColorLabel) => {
      setFilter({ colorLabels: toggleColorLabelItem(filter.colorLabels, label) });
      search();
    },
    [filter.colorLabels, setFilter, search]
  );

  const handleFilterChange = useCallback(
    (patch: Partial<FilterSpec>) => {
      setFilter(patch);
      search();
    },
    [setFilter, search]
  );

  return (
    <div className={styles.filterBar} id="filter-bar">
      <div className={styles.leftGroup}>
        <span className={styles.filterIconWrap} aria-hidden>
          <IconFilter size={13} />
        </span>

        <AllAssetsButton isActive={!hasActiveFilters} onClick={handleAllClick} />
        <FavoriteFilterButton isFavorite={filter.isFavorite} onClick={toggleFavorite} />

        <div className={styles.divider} />

        <ColorLabelsRow selectedLabels={filter.colorLabels} onToggle={handleToggleColorLabel} />

        <div className={styles.divider} />

        <DateFilterSelect from={filter.dateRange?.from} onChange={handleFilterChange} />
        <SizeFilterSelect sizeRange={filter.sizeRange} onChange={handleFilterChange} />
        <RatingFilterSelect rating={filter.rating} onChange={handleFilterChange} />
      </div>

      <SortAndClearControls
        sort={sort}
        hasActiveFilters={hasActiveFilters}
        onSetSort={setSort}
        onClear={handleAllClick}
        onSearch={search}
      />
    </div>
  );
}
