import { icons } from './tagForgeData';

const DEFAULT_TAG_PATH =
  'M21.41 11.58l-9-9A2 2 0 0 0 11 2H4a2 2 0 0 0-2 2v7a2 2 0 0 0 .59 1.42l9 9a2 2 0 0 0 2.82 0l7-7a2 2 0 0 0 0-2.84zM6.5 8A1.5 1.5 0 1 1 8 6.5 1.5 1.5 0 0 1 6.5 8z';

export function TagIcon({
  iconId,
  size = 16,
  color,
}: {
  iconId: string | null;
  size?: number;
  color?: string;
}) {
  const icon = iconId ? icons[iconId] : undefined;
  return (
    <svg
      viewBox={icon?.viewBox || '0 0 24 24'}
      width={size}
      height={size}
      fill={color ?? 'currentColor'}
      aria-hidden="true"
      focusable="false"
      style={{ display: 'block', flexShrink: 0 }}
    >
      <path d={icon ? icon.path : DEFAULT_TAG_PATH} />
    </svg>
  );
}
