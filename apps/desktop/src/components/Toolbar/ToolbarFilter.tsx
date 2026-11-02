import { useRef, useEffect, useState, memo } from 'react';
import { useAssetStore } from '../../store';
import { IconHeartFilled, IconInbox, IconX } from '../Icons';
import { FilterDropdown, DropdownItemConfig } from './FilterDropdown';
import {
  getDateOptions,
  SIZE_OPTIONS,
  PREVIEW_OPTIONS,
  DateFilterOption,
  SizeFilterOption,
  PreviewFilterOption,
} from './filterOptions';
import styles from './ToolbarFilter.module.css';
import clsx from 'clsx';

export const ToolbarFilter = memo(function ToolbarFilter() {
  const { query, search, resetFilter, setFilter, clearFilter } = useAssetStore();
  const [openDropdown, setOpenDropdown] = useState<'date' | 'size' | 'preview' | null>(null);
  const clusterRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const handleOutsideClick = (e: MouseEvent) => {
      if (clusterRef.current && !clusterRef.current.contains(e.target as Node)) {
        setOpenDropdown(null);
      }
    };
    document.addEventListener('mousedown', handleOutsideClick);
    return () => document.removeEventListener('mousedown', handleOutsideClick);
  }, []);

  const currentFilter = query.filter;

  const isAllActive =
    !currentFilter.isFavorite &&
    !currentFilter.unorganized &&
    (!currentFilter.kinds || currentFilter.kinds.length === 0) &&
    !currentFilter.rating &&
    (!currentFilter.colorLabels || currentFilter.colorLabels.length === 0) &&
    !currentFilter.dateRange &&
    !currentFilter.sizeRange &&
    !currentFilter.previewStatus &&
    (!currentFilter.tags || currentFilter.tags.length === 0);

  const isFavActive = Boolean(currentFilter.isFavorite);
  const isUnorgActive = Boolean(currentFilter.unorganized);
  const isDateActive = Boolean(currentFilter.dateRange);
  const isSizeActive = Boolean(currentFilter.sizeRange);
  const isPreviewActive = Boolean(currentFilter.previewStatus);

  const dateOptions = getDateOptions();
  const activeDateOption = dateOptions.find((opt) => currentFilter.dateRange?.from === opt.from);
  const activeSizeOption = SIZE_OPTIONS.find(
    (opt) => currentFilter.sizeRange?.min === opt.min && currentFilter.sizeRange?.max === opt.max
  );
  const activePreviewOption = PREVIEW_OPTIONS.find(
    (opt) => currentFilter.previewStatus === opt.status
  );

  const toggleDropdown = (key: 'date' | 'size' | 'preview') => {
    setOpenDropdown((cur) => (cur === key ? null : key));
  };

  const handleClear = (key: 'isFavorite' | 'unorganized' | 'dateRange' | 'sizeRange' | 'previewStatus') => {
    clearFilter(key);
    setOpenDropdown(null);
    search();
  };

  const toDateItem = (opt: DateFilterOption): DropdownItemConfig => {
    const isSelected = currentFilter.dateRange?.from === opt.from;
    return {
      id: opt.label,
      label: opt.label,
      isActive: isSelected,
      onSelect: () => {
        if (isSelected) {
          clearFilter('dateRange');
        } else {
          setFilter({ dateRange: { from: opt.from } });
        }
        setOpenDropdown(null);
        search();
      },
    };
  };

  const toSizeItem = (opt: SizeFilterOption): DropdownItemConfig => {
    const isSelected =
      currentFilter.sizeRange?.min === opt.min && currentFilter.sizeRange?.max === opt.max;
    return {
      id: opt.label,
      label: opt.label,
      isActive: isSelected,
      onSelect: () => {
        if (isSelected) {
          clearFilter('sizeRange');
        } else {
          setFilter({ sizeRange: { min: opt.min, max: opt.max } });
        }
        setOpenDropdown(null);
        search();
      },
    };
  };

  const toPreviewItem = (opt: PreviewFilterOption): DropdownItemConfig => {
    const isSelected = currentFilter.previewStatus === opt.status;
    return {
      id: opt.status,
      label: opt.label,
      isActive: isSelected,
      onSelect: () => {
        if (isSelected) {
          clearFilter('previewStatus');
        } else {
          setFilter({ previewStatus: opt.status });
        }
        setOpenDropdown(null);
        search();
      },
    };
  };

  return (
    <div className={styles.filterCluster} ref={clusterRef}>
      {/* 1. All assets */}
      <button
        type="button"
        id="toolbar-filter-all"
        className={clsx(styles.filterBtn, isAllActive && styles.filterBtnActive)}
        onClick={() => {
          resetFilter();
          search();
        }}
        title="Show all assets (clear filters)"
      >
        <span>All</span>
        {isAllActive && <span className={styles.filterActiveDot}>●</span>}
      </button>

      {/* 2. Favorites */}
      <button
        type="button"
        id="toolbar-filter-favorites"
        className={clsx(styles.filterBtn, isFavActive && styles.favActive)}
        onClick={() => {
          if (isFavActive) {
            clearFilter('isFavorite');
          } else {
            setFilter({ isFavorite: true });
          }
          search();
        }}
        title="Toggle favorites"
      >
        <IconHeartFilled size={13} />
        <span>Favorites</span>
        {isFavActive && (
          <span
            className={styles.clearIconBtn}
            onClick={(e) => {
              e.stopPropagation();
              handleClear('isFavorite');
            }}
            title="Clear favorites filter"
            role="button"
            tabIndex={0}
          >
            <IconX size={10} />
          </span>
        )}
      </button>

      {/* 3. Unorganized */}
      <button
        type="button"
        id="toolbar-filter-unorganized"
        className={clsx(styles.filterBtn, isUnorgActive && styles.filterBtnActive)}
        onClick={() => {
          if (isUnorgActive) {
            clearFilter('unorganized');
          } else {
            setFilter({ unorganized: true });
          }
          search();
        }}
        title="Toggle unorganized assets"
      >
        <IconInbox size={13} />
        <span>Unorganized</span>
        {isUnorgActive && (
          <span
            className={styles.clearIconBtn}
            onClick={(e) => {
              e.stopPropagation();
              handleClear('unorganized');
            }}
            title="Clear unorganized filter"
            role="button"
            tabIndex={0}
          >
            <IconX size={10} />
          </span>
        )}
      </button>

      {/* 4. Date Added dropdown */}
      <FilterDropdown
        id="toolbar-filter-date"
        label={activeDateOption ? activeDateOption.label : 'Date'}
        title="Filter by date added"
        isActive={isDateActive}
        isOpen={openDropdown === 'date'}
        onToggle={() => toggleDropdown('date')}
        onClear={() => handleClear('dateRange')}
        items={dateOptions.map(toDateItem)}
      />

      {/* 5. File Size dropdown */}
      <FilterDropdown
        id="toolbar-filter-size"
        label={activeSizeOption ? activeSizeOption.label : 'Size'}
        title="Filter by file size"
        isActive={isSizeActive}
        isOpen={openDropdown === 'size'}
        onToggle={() => toggleDropdown('size')}
        onClear={() => handleClear('sizeRange')}
        items={SIZE_OPTIONS.map(toSizeItem)}
      />

      {/* 6. Preview Status dropdown */}
      <FilterDropdown
        id="toolbar-filter-preview"
        label={activePreviewOption ? activePreviewOption.label : 'Preview'}
        title="Filter by preview status"
        isActive={isPreviewActive}
        isOpen={openDropdown === 'preview'}
        onToggle={() => toggleDropdown('preview')}
        onClear={() => handleClear('previewStatus')}
        items={PREVIEW_OPTIONS.map(toPreviewItem)}
      />
    </div>
  );
});
