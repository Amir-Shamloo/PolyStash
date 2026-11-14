import { TagIcon } from './TagIcon';
import { toCssColor } from './tagForgeData';
import type { ForgeTag } from './tagForgeModel';
import styles from './InspectorPanel.module.css';

interface SelectedTagCardProps {
  selected: ForgeTag | null;
}

export function SelectedTagCard({ selected }: SelectedTagCardProps) {
  if (!selected) {
    return (
      <div className={styles.card}>
        <span className={styles.placeholder}>Select a tag</span>
      </div>
    );
  }

  const background = selected.color ? toCssColor(selected.color) : undefined;

  return (
    <div className={styles.card}>
      <div className={styles.swatch} style={{ background }}>
        <TagIcon iconId={selected.iconId} size={32} />
      </div>
      <span className={styles.cardName}>{selected.name}</span>
    </div>
  );
}
