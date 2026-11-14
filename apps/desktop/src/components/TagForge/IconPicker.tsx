import { useState } from 'react';
import clsx from 'clsx';
import { TagIcon } from './TagIcon';
import { iconCategories, type IconDefinition } from './tagForgeData';
import { ALL_ICON_CATEGORY, useIconFiltering } from './useIconFiltering';
import styles from './InspectorPanel.module.css';

interface IconPickerProps {
  selectedIconId: string | null;
  disabled: boolean;
  onApplyIcon: (iconId: string | null) => void;
}

const categoryOptions = [{ id: ALL_ICON_CATEGORY, name: 'All' }, ...iconCategories];

function IconCategoryPills({
  category,
  disabled,
  onSelect,
}: {
  category: string;
  disabled: boolean;
  onSelect: (id: string) => void;
}) {
  return (
    <div className={styles.pills} role="group" aria-label="Icon categories">
      {categoryOptions.map((c) => {
        const active = category === c.id;
        return (
          <button
            key={c.id}
            type="button"
            disabled={disabled}
            className={clsx(styles.pill, active && styles.pillActive)}
            aria-pressed={active}
            onClick={() => onSelect(c.id)}
          >
            {c.name}
          </button>
        );
      })}
    </div>
  );
}

function IconGrid({
  visibleIcons,
  selectedIconId,
  disabled,
  onApplyIcon,
}: {
  visibleIcons: IconDefinition[];
  selectedIconId: string | null;
  disabled: boolean;
  onApplyIcon: (iconId: string | null) => void;
}) {
  if (visibleIcons.length === 0) {
    return (
      <div className={styles.iconGrid}>
        <span className={styles.empty}>No icons match</span>
      </div>
    );
  }

  return (
    <div className={styles.iconGrid}>
      {visibleIcons.map((icon) => {
        const active = selectedIconId === icon.id;
        return (
          <button
            key={icon.id}
            type="button"
            disabled={disabled}
            title={icon.name}
            aria-label={icon.name}
            aria-pressed={active}
            className={clsx(styles.iconCell, active && styles.active)}
            onClick={() => onApplyIcon(icon.id)}
          >
            <TagIcon iconId={icon.id} size={24} />
          </button>
        );
      })}
    </div>
  );
}

export function IconPicker({ selectedIconId, disabled, onApplyIcon }: IconPickerProps) {
  const [search, setSearch] = useState('');
  const [category, setCategory] = useState(ALL_ICON_CATEGORY);
  const { visibleIcons } = useIconFiltering(search, category);

  return (
    <div className={clsx(styles.section, styles.iconsSection)}>
      <input
        className={styles.input}
        type="search"
        placeholder="Search icons"
        value={search}
        disabled={disabled}
        onChange={(e) => setSearch(e.target.value)}
      />
      <IconCategoryPills category={category} disabled={disabled} onSelect={setCategory} />
      <IconGrid
        visibleIcons={visibleIcons}
        selectedIconId={selectedIconId}
        disabled={disabled}
        onApplyIcon={onApplyIcon}
      />
    </div>
  );
}
