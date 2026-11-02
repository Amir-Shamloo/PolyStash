import { memo } from 'react';
import { IconChevronDown, IconX } from '../Icons';
import styles from './ToolbarFilter.module.css';
import clsx from 'clsx';

export interface DropdownItemConfig {
  id: string;
  label: string;
  isActive: boolean;
  onSelect: () => void;
}

interface FilterDropdownProps {
  id: string;
  label: string;
  title: string;
  isActive: boolean;
  isOpen: boolean;
  onToggle: () => void;
  onClear: () => void;
  items: DropdownItemConfig[];
}

export const FilterDropdown = memo(function FilterDropdown({
  id,
  label,
  title,
  isActive,
  isOpen,
  onToggle,
  onClear,
  items,
}: FilterDropdownProps) {
  return (
    <div className={styles.filterDropdownWrap}>
      <button
        type="button"
        id={id}
        className={clsx(styles.filterBtn, isActive && styles.filterBtnActive)}
        onClick={onToggle}
        title={title}
      >
        <span>{label}</span>
        {isActive ? (
          <span
            className={styles.clearIconBtn}
            onClick={(e) => {
              e.stopPropagation();
              onClear();
            }}
            title={`Clear ${title.toLowerCase()}`}
            role="button"
            tabIndex={0}
          >
            <IconX size={10} />
          </span>
        ) : (
          <IconChevronDown size={11} className={styles.dropdownChevron} />
        )}
      </button>

      {isOpen && (
        <div className={styles.dropdownMenu}>
          {items.map((item) => (
            <button
              key={item.id}
              type="button"
              className={clsx(styles.dropdownItem, item.isActive && styles.dropdownItemActive)}
              onClick={item.onSelect}
            >
              <span>{item.label}</span>
              {item.isActive && <span>✓</span>}
            </button>
          ))}
          {isActive && (
            <>
              <div className={styles.dropdownDivider} />
              <button
                type="button"
                className={clsx(styles.dropdownItem, styles.dropdownClearItem)}
                onClick={onClear}
              >
                Clear filter
              </button>
            </>
          )}
        </div>
      )}
    </div>
  );
});
