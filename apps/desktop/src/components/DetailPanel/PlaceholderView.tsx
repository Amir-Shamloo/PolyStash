import type { AssetKind } from '../../types';
import { IconModel3D, IconImage, IconVideo, IconFileText, IconFolder } from '../Icons';

const KIND_ICON: Record<AssetKind, React.FC<{ size?: number; style?: React.CSSProperties }>> = {
  '3d_model': IconModel3D,
  material: IconImage,
  texture: IconImage,
  hdri: IconImage,
  ies: IconImage,
  video: IconVideo,
  image: IconImage,
  document: IconFileText,
  folder: IconFolder,
  other: IconImage,
};

const KIND_COLOR: Record<AssetKind, string> = {
  '3d_model': 'hsl(220 85% 55%)',
  material: 'hsl(268 72% 60%)',
  texture: 'hsl(178 64% 46%)',
  hdri: 'hsl( 38 92% 56%)',
  ies: 'hsl( 45 95% 50%)',
  video: 'hsl(  4 80% 55%)',
  image: 'hsl(140 64% 46%)',
  document: 'hsl(213 84% 55%)',
  folder: 'hsl( 38 92% 56%)',
  other: 'hsl(220  8% 50%)',
};

export function PlaceholderView({ kind, ext, message }: { kind: AssetKind; ext: string; message?: string }) {
  const KindIcon = KIND_ICON[kind] ?? IconImage;
  const color = KIND_COLOR[kind] ?? 'var(--color-text-disabled)';
  return (
    <div
      style={{
        width: '100%',
        height: '100%',
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        gap: '8px',
      }}
    >
      <KindIcon size={36} style={{ color }} />
      <span style={{ fontSize: '24px', fontWeight: 700, fontFamily: 'var(--font-mono)', color }}>
        .{ext.toUpperCase()}
      </span>
      {message && (
        <span style={{ fontSize: '11px', color: 'var(--color-text-muted)', textAlign: 'center', maxWidth: '80%' }}>
          {message}
        </span>
      )}
    </div>
  );
}
