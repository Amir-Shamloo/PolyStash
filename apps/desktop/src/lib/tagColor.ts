export function tagColorCss(color?: string | null): string {
  if (!color) return 'var(--color-text-muted)';
  if (color.startsWith('#')) return color;
  return `var(--swatch-${color})`;
}
