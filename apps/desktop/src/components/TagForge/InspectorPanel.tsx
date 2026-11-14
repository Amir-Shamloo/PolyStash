import { ColorPicker } from './ColorPicker';
import { IconPicker } from './IconPicker';
import { SelectedTagCard } from './SelectedTagCard';
import type { ForgeTag } from './tagForgeModel';
import styles from './InspectorPanel.module.css';

interface InspectorPanelProps {
  selected: ForgeTag | null;
  applyToSubtags: boolean;
  onToggleApplyToSubtags: (v: boolean) => void;
  onApplyIcon: (iconId: string | null) => void;
  onApplyColor: (color: string | null) => void;
}

export function InspectorPanel({
  selected,
  applyToSubtags,
  onToggleApplyToSubtags,
  onApplyIcon,
  onApplyColor,
}: InspectorPanelProps) {
  const disabled = selected === null;

  return (
    <section className={styles.panel} aria-label="Inspector">
      <header className={styles.header}>
        <h3 className={styles.title}>Custom Icons and Colors</h3>
        <label className={styles.checkbox}>
          <input
            type="checkbox"
            checked={applyToSubtags}
            onChange={(e) => onToggleApplyToSubtags(e.target.checked)}
          />
          Apply to Subtags
        </label>
      </header>

      <SelectedTagCard selected={selected} />

      <IconPicker
        selectedIconId={selected?.iconId ?? null}
        disabled={disabled}
        onApplyIcon={onApplyIcon}
      />

      <ColorPicker
        selectedId={selected?.id ?? null}
        selectedColor={selected?.color ?? null}
        disabled={disabled}
        onApplyColor={onApplyColor}
      />
    </section>
  );
}
