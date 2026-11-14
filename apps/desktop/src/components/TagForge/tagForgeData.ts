import type { PredefinedTag, PredefinedNode } from './tagForgeModel';
import templatesJson from '../../data/tagForge/templates.json';
import categoriesIndexJson from '../../data/tagForge/categories_index.json';
import iconsJson from '../../data/tagForge/icons.json';
import iconCategoriesJson from '../../data/tagForge/icon_categories.json';
import palettesJson from '../../data/tagForge/color_palettes.json';

export type GranularityLevel = 'Summary' | 'Balanced' | 'Comprehensive';

export interface TemplateRef {
  guid: string;
  category?: string;
  name?: string;
  totalTags?: number;
}

export interface IndustryTemplate {
  name: string;
  granularityEntries: Record<GranularityLevel, TemplateRef[]>;
}

export interface CategoryEntry {
  guid: string;
  category: string;
  granularity: string;
  name: string;
  filename: string;
  totalTags: number;
}

export interface IconDefinition {
  id: string;
  name: string;
  category: string;
  categoryName: string;
  description: string;
  viewBox: string;
  path: string;
  tags: string[];
}

export interface IconCategory {
  id: string;
  name: string;
  iconIds: string[];
}

export interface ColorPalettesConfig {
  defaultColor: string;
  palettes: Array<{ id: string; name: string; colors: string[] }>;
}

interface CategoryFile {
  category: string;
  granularity: string;
  rootGuid: string;
  rootName: string;
  totalTags: number;
  tree: PredefinedNode;
  flatTags: PredefinedTag[];
}

export type { PredefinedTag, PredefinedNode };

export const templates = templatesJson as unknown as IndustryTemplate[];
export const categoriesIndex = categoriesIndexJson as unknown as CategoryEntry[];
export const icons = iconsJson as unknown as Record<string, IconDefinition>;
export const iconCategories = iconCategoriesJson as unknown as IconCategory[];
export const palettes = palettesJson as unknown as ColorPalettesConfig;

const categoryModules = import.meta.glob('../../data/tagForge/categories/*.json', {
  eager: true,
}) as Record<string, { default?: CategoryFile } & Partial<CategoryFile>>;

const categoryFiles: CategoryFile[] = Object.values(categoryModules).map(
  (mod) => (mod.default ?? mod) as CategoryFile,
);

const forestCache = new Map<GranularityLevel, PredefinedNode[]>();

export function loadPredefinedForest(granularity: GranularityLevel): PredefinedNode[] {
  const cached = forestCache.get(granularity);
  if (cached) return cached;
  const forest = categoryFiles
    .filter((f) => f.granularity === granularity || f.granularity === 'Unknown')
    .sort((a, b) => a.category.localeCompare(b.category) || a.rootName.localeCompare(b.rootName))
    .map((f) => f.tree);
  forestCache.set(granularity, forest);
  return forest;
}

function collectUniqueNodes(
  refs: TemplateRef[],
  byGuid: Map<string, PredefinedNode>,
): PredefinedNode[] {
  const seen = new Set<string>();
  const roots: PredefinedNode[] = [];
  for (const ref of refs) {
    if (!ref?.guid) continue;
    const node = byGuid.get(ref.guid);
    if (!node || seen.has(node.guid)) continue;
    seen.add(node.guid);
    roots.push(node);
  }
  return roots;
}

export function resolveTemplateRoots(
  templateName: string,
  granularity: GranularityLevel,
): PredefinedNode[] {
  const template = templates.find((t) => t.name === templateName);
  const refs = template?.granularityEntries?.[granularity];
  if (!refs) return [];
  const byGuid = new Map(loadPredefinedForest(granularity).map((n) => [n.guid, n]));
  return collectUniqueNodes(refs, byGuid);
}

/** `#AARRGGBB` -> `#RRGGBBAA`; anything else is returned unchanged. */
export function toCssColor(color: string): string {
  if (color.length === 9 && color.startsWith('#')) {
    return `#${color.slice(3)}${color.slice(1, 3)}`;
  }
  return color;
}

/** Canonical `#RRGGBB` (or `#RRGGBBAA` when alpha < FF); null when invalid. */
export function normalizeHexColor(input: string): string | null {
  const raw = input.trim().replace(/^#/, '');
  if (!/^[0-9a-fA-F]+$/.test(raw)) return null;
  let rgb: string;
  let alpha = 'FF';
  if (raw.length === 3) {
    rgb = raw.split('').map((c) => c + c).join('');
  } else if (raw.length === 6) {
    rgb = raw;
  } else if (raw.length === 8) {
    alpha = raw.slice(0, 2);
    rgb = raw.slice(2);
  } else {
    return null;
  }
  const out = rgb.toUpperCase();
  const a = alpha.toUpperCase();
  return a === 'FF' ? `#${out}` : `#${out}${a}`;
}
