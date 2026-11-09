import { memo, useCallback, useMemo, useState, useEffect } from 'react';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import type { Asset, AssetKind } from '../../types';
import { useTagStore, useCollectionStore, useAssetStore } from '../../store';
import {
  IconModel3D,
  IconImage,
  IconVideo,
  IconStar,
  IconCollection,
  IconHeart,
  IconHeartFilled,
  IconFileText,
  IconFolder,
} from '../Icons';
import { ThumbnailCarousel, getAssetThumbnailUrls } from './ThumbnailCarousel';
import { useLazyModelSnapshot } from '../../lib/previewSnapshot';
import styles from './AssetCard.module.css';

export interface AssetCardProps {
  asset: Asset;
  selected: boolean;
  onClick: (asset: Asset, e: React.MouseEvent) => void;
}

const KIND_ICON: Record<AssetKind, React.FC<{ size?: number }>> = {
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

function formatSize(bytes: number): string {
  if (bytes < 1024) return `${bytes}B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)}KB`;
  if (bytes < 1024 * 1024 * 1024) return `${(bytes / 1024 / 1024).toFixed(1)}MB`;
  return `${(bytes / 1024 / 1024 / 1024).toFixed(2)}GB`;
}

function getKindIcon(kind: AssetKind): React.FC<{ size?: number }> {
  return KIND_ICON[kind] ?? IconImage;
}

function getKindColor(kind: AssetKind): string {
  return KIND_COLOR[kind] ?? 'var(--color-text-disabled)';
}

function CardThumbPlaceholder({ asset }: { asset: Asset }) {
  const KindIcon = getKindIcon(asset.kind);
  const kindColor = getKindColor(asset.kind);
  const extLabel = asset.extension ? `.${asset.extension.toUpperCase()}` : asset.kind.toUpperCase();

  return (
    <div className={styles.thumbPlaceholder} style={{ color: kindColor }}>
      <KindIcon size={36} />
      <span style={{ fontSize: '11px', fontWeight: 600, color: kindColor, marginTop: 4 }}>
        {extLabel}
      </span>
    </div>
  );
}

function canRenderAnimatedGif(asset: Asset, mediaError: boolean): boolean {
  if (mediaError || !asset.filePath) return false;
  return asset.extension.toLowerCase() === 'gif';
}

function canRenderVideoThumb(asset: Asset, mediaError: boolean): boolean {
  if (mediaError || !asset.filePath) return false;
  return asset.kind === 'video';
}

function VideoCardPreview({ filePath, onError }: { filePath: string; onError: () => void }) {
  return (
    <div className={styles.videoThumbWrapper}>
      <video
        src={convertFileSrc(filePath)}
        preload="metadata"
        muted
        playsInline
        className={styles.thumbVideo}
        onError={onError}
        onLoadedMetadata={(e) => {
          try {
            e.currentTarget.currentTime = 0.1;
          } catch {}
        }}
      />
      <div className={styles.videoPlayOverlay}>
        <IconVideo size={13} />
      </div>
    </div>
  );
}

interface CardThumbMediaProps {
  asset: Asset;
  snapshotUrl: string | null;
}

function CardThumbMedia({ asset, snapshotUrl }: CardThumbMediaProps) {
  const [mediaError, setMediaError] = useState(false);

  useEffect(() => {
    setMediaError(false);
  }, [asset.id, snapshotUrl]);

  const placeholder = useMemo(
    () => <CardThumbPlaceholder asset={asset} />,
    [asset]
  );

  if (canRenderAnimatedGif(asset, mediaError)) {
    return (
      <img
        src={convertFileSrc(asset.filePath)}
        alt={asset.fileName}
        className={styles.thumbImg}
        loading="lazy"
        onError={() => setMediaError(true)}
      />
    );
  }

  const thumbs = getAssetThumbnailUrls(asset, snapshotUrl);
  if (thumbs.length > 0) {
    return (
      <ThumbnailCarousel
        asset={asset}
        snapshotUrl={snapshotUrl}
        imgClassName={styles.thumbImg}
        alt={asset.fileName}
        fallback={placeholder}
      />
    );
  }

  if (canRenderVideoThumb(asset, mediaError)) {
    return <VideoCardPreview filePath={asset.filePath} onError={() => setMediaError(true)} />;
  }

  return placeholder;
}

function FavoriteOverlayButton({
  assetId,
  isFavorite,
  onClick,
}: {
  assetId: string;
  isFavorite: boolean;
  onClick: (e: React.MouseEvent) => void;
}) {
  const label = isFavorite ? 'Remove from favorites' : 'Add to favorites';
  return (
    <button
      id={`card-fav-${assetId}`}
      type="button"
      className={clsx(styles.favoriteBtn, isFavorite && styles.favoriteActive)}
      onClick={onClick}
      title={label}
      aria-label={label}
    >
      {isFavorite ? (
        <IconHeartFilled size={13} className={styles.favoriteHeartFilled} />
      ) : (
        <IconHeart size={13} />
      )}
    </button>
  );
}

interface CardThumbAreaProps {
  asset: Asset;
  snapshotUrl: string | null;
  isGenerating: boolean;
  onToggleFavorite: (e: React.MouseEvent) => void;
}

function CardThumbArea({
  asset,
  snapshotUrl,
  isGenerating,
  onToggleFavorite,
}: CardThumbAreaProps) {
  const KindIcon = getKindIcon(asset.kind);
  const kindColor = getKindColor(asset.kind);

  return (
    <div className={styles.thumb}>
      <CardThumbMedia asset={asset} snapshotUrl={snapshotUrl} />

      {isGenerating && (
        <div className={styles.spinnerOverlay} aria-label="Generating 3D preview">
          <div className={styles.spinnerCircle} />
        </div>
      )}

      <FavoriteOverlayButton
        assetId={asset.id}
        isFavorite={asset.isFavorite}
        onClick={onToggleFavorite}
      />

      {asset.colorLabel !== 'none' && (
        <div
          className={styles.colorStripe}
          style={{ background: `var(--swatch-${asset.colorLabel})` }}
        />
      )}

      <div className={styles.kindChip} style={{ color: kindColor }}>
        <KindIcon size={10} />
        <span>{asset.extension.toUpperCase()}</span>
      </div>
    </div>
  );
}

interface ChipItem {
  id: string;
  name: string;
  type: 'collection' | 'tag';
  color?: string;
}

function buildCollectionChips(colIds: string[] | undefined, collections: { id: string; name: string }[]): ChipItem[] {
  if (!colIds) return [];
  const map = new Map(collections.map((c) => [c.id, c.name]));
  return colIds
    .map((id) => ({ id: `col-${id}`, name: map.get(id) ?? '', type: 'collection' as const }))
    .filter((item) => item.name.length > 0);
}

function buildTagChips(tagIds: string[] | undefined, tagMap: Map<string, { name: string; color?: string }>): ChipItem[] {
  if (!tagIds) return [];
  return tagIds.map((id) => {
    const tag = tagMap.get(id);
    return {
      id: `tag-${id}`,
      name: tag?.name ?? id,
      type: 'tag' as const,
      color: tag?.color,
    };
  });
}

function useAssetChips(asset: Asset): ChipItem[] {
  const tagMap = useTagStore((s) => s.tagMap);
  const collections = useCollectionStore((s) => s.collections);

  return useMemo(() => [
    ...buildCollectionChips(asset.collections, collections),
    ...buildTagChips(asset.tags, tagMap),
  ], [asset.collections, asset.tags, collections, tagMap]);
}

function CardChipBadge({ chip }: { chip: ChipItem }) {
  const isCollection = chip.type === 'collection';
  const dotBg = chip.color ? `var(--swatch-${chip.color})` : 'hsl(220 85% 60%)';

  return (
    <span
      className={clsx(styles.chip, isCollection ? styles.collectionChip : styles.tagChip)}
      title={isCollection ? `Collection: ${chip.name}` : `Tag: ${chip.name}`}
    >
      {isCollection ? (
        <IconCollection size={9} className={styles.chipIcon} />
      ) : (
        <span className={styles.chipDot} style={{ background: dotBg }} />
      )}
      <span className={styles.chipText}>{chip.name}</span>
    </span>
  );
}

interface CardBodyProps {
  asset: Asset;
  chips: ChipItem[];
}

function CardBody({ asset, chips }: CardBodyProps) {
  return (
    <div className={styles.body}>
      <p className={clsx(styles.name, 'truncate')} title={asset.fileName}>
        {asset.fileName}
      </p>

      <div className={styles.meta}>
        {asset.rating > 0 && (
          <div className={styles.stars}>
            {Array.from({ length: 5 }).map((_, i) => (
              <IconStar
                key={i}
                size={10}
                className={clsx(styles.star, i < asset.rating && styles.starFilled)}
              />
            ))}
          </div>
        )}
        <span className={styles.size}>{formatSize(asset.sizeBytes)}</span>
      </div>

      {Object.keys(asset.metadata).length > 0 && (
        <div className={styles.technicalMeta}>
          {Object.entries(asset.metadata).slice(0, 4).map(([key, value]) => (
            <div key={key} className={styles.metaRow}>
              <span className={styles.metaKey}>{key}</span>
              <span className={styles.metaValue}>{String(value)}</span>
            </div>
          ))}
        </div>
      )}


      {chips.length > 0 && (
        <div
          className={styles.chipsScroll}
          onWheel={(e) => {
            if (e.deltaY !== 0) e.currentTarget.scrollLeft += e.deltaY;
          }}
        >
          {chips.map((chip) => (
            <CardChipBadge key={chip.id} chip={chip} />
          ))}
        </div>
      )}
    </div>
  );
}

export const AssetCard = memo(function AssetCard({
  asset,
  selected,
  onClick,
}: AssetCardProps) {
  const patchAsset = useAssetStore((s) => s.patchAsset);
  const { ref: cardRef, snapshotUrl, isGenerating } = useLazyModelSnapshot(asset);
  const chips = useAssetChips(asset);

  const handleClick = useCallback(
    (e: React.MouseEvent) => {
      e.stopPropagation();
      onClick(asset, e);
    },
    [asset, onClick]
  );

  const handleToggleFavorite = useCallback(
    (e: React.MouseEvent) => {
      e.stopPropagation();
      patchAsset(asset.id, { isFavorite: !asset.isFavorite });
    },
    [asset.id, asset.isFavorite, patchAsset]
  );

  const handleDragStart = useCallback(
    (e: React.DragEvent) => {
      e.dataTransfer.setData('application/polystash-asset-id', asset.id);
      e.dataTransfer.setData('text/plain', asset.id);
      e.dataTransfer.effectAllowed = 'copy';
    },
    [asset.id]
  );

  return (
    <div
      ref={cardRef}
      id={`asset-${asset.id}`}
      className={clsx(styles.card, selected && styles.selected)}
      onClick={handleClick}
      draggable
      onDragStart={handleDragStart}
      title={asset.fileName}
      role="button"
      tabIndex={0}
      onKeyDown={(e) => e.key === 'Enter' && handleClick(e as unknown as React.MouseEvent)}
    >
      <CardThumbArea
        asset={asset}
        snapshotUrl={snapshotUrl}
        isGenerating={isGenerating}
        onToggleFavorite={handleToggleFavorite}
      />
      <CardBody asset={asset} chips={chips} />
    </div>
  );
});
