import { useMemo } from 'react';
import { iconCategories, icons, type IconDefinition } from './tagForgeData';

export const ALL_ICON_CATEGORY = 'all';

const allIcons = Object.values(icons);

function getCategoryIcons(category: string): IconDefinition[] {
  if (category === ALL_ICON_CATEGORY) return allIcons;
  const match = iconCategories.find((c) => c.id === category);
  if (!match) return allIcons;
  return match.iconIds
    .map((id) => icons[id])
    .filter((icon): icon is IconDefinition => icon !== undefined);
}

function matchesIconQuery(icon: IconDefinition, query: string): boolean {
  if (icon.name.toLowerCase().includes(query)) return true;
  return icon.tags.some((tag) => tag.toLowerCase().includes(query));
}

export function useIconFiltering(search: string, category: string) {
  const visibleIcons = useMemo(() => {
    const base = getCategoryIcons(category);
    const query = search.trim().toLowerCase();
    if (!query) return base;
    return base.filter((icon) => matchesIconQuery(icon, query));
  }, [search, category]);

  return { visibleIcons };
}
