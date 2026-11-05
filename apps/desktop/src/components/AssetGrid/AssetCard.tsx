import { memo, useCallback, useMemo } from 'react';
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
} from '../Icons';
import { ThumbnailCarousel, getAssetThumbnailUrls } from './ThumbnailCarousel';
import { useModelSnapshot } from '../../lib/previewSnapshot';
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
  other: 'hsl(220  8% 50%)',
};

function formatSize(bytes: number): string {
  if (bytes < 1024) return `${bytes}B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)}KB`;
  if (bytes < 1024 * 1024 * 1024) return `${(bytes / 1024 / 1024).toFixed(1)}MB`;
  return `${(bytes / 1024 / 1024 / 1024).toFixed(2)}GB`;
}

interface CardThumbMediaProps {
  asset: Asset;
  snapshotUrl: string | null;
}

function CardThumbMedia({ asset, snapshotUrl }: CardThumbMediaProps) {
  const thumbs = getAssetThumbnailUrls(asset, snapshotUrl);
  const KindIcon = KIND_ICON[asset.kind] ?? IconImage;
  const kindColor = KIND_COLOR[asset.kind] ?? 'var(--color-text-disabled)';

  if (thumbs.length > 0) {
    return (
      <ThumbnailCarousel
        asset={asset}
        snapshotUrl={snapshotUrl}
        imgClassName={styles.thumbImg}
        alt={asset.fileName}
      />
    );
  }

  if (asset.kind === 'video' && asset.filePath) {
    return (
      <div className={styles.videoThumbWrapper}>
        <video
          src={convertFileSrc(asset.filePath)}
          preload="metadata"
          muted
          playsInline
          className={styles.thumbVideo}
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

  return (
    <div className={styles.thumbPlaceholder}>
      <KindIcon size={36} />
      <span style={{ fontSize: '11px', fontWeight: 600, color: kindColor, marginTop: 4 }}>
        .{asset.extension.toUpperCase()}
      </span>
    </div>
  );
}

interface CardThumbAreaProps {
  asset: Asset;
  snapshotUrl: string | null;
  onToggleFavorite: (e: React.MouseEvent) => void;
}

function CardThumbArea({ asset, snapshotUrl, onToggleFavorite }: CardThumbAreaProps) {
  const KindIcon = KIND_ICON[asset.kind] ?? IconImage;
  const kindColor = KIND_COLOR[asset.kind];

  return (
    <div className={styles.thumb}>
      <CardThumbMedia asset={asset} snapshotUrl={snapshotUrl} />

      <button
        id={`card-fav-${asset.id}`}
        type="button"
        className={clsx(styles.favoriteBtn, asset.isFavorite && styles.favoriteActive)}
        onClick={onToggleFavorite}
        title={asset.isFavorite ? 'Remove from favorites' : 'Add to favorites'}
        aria-label={asset.isFavorite ? 'Remove from favorites' : 'Add to favorites'}
      >
        {asset.isFavorite ? (
          <IconHeartFilled size={13} className={styles.favoriteHeartFilled} />
        ) : (
          <IconHeart size={13} />
        )}
      </button>

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
  if (!colIds || colIds.length === 0) return [];
  const map = new Map(collections.map((c) => [c.id, c.name]));
  return colIds
    .map((id) => ({ id: `col-${id}`, name: map.get(id) ?? '', type: 'collection' as const }))
    .filter((item) => item.name.length > 0);
}

function buildTagChips(tagIds: string[] | undefined, tagMap: Map<string, { name: string; color?: string }>): ChipItem[] {
  if (!tagIds || tagIds.length === 0) return [];
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

      {chips.length > 0 && (
        <div
          className={styles.chipsScroll}
          onWheel={(e) => {
            if (e.deltaY !== 0) e.currentTarget.scrollLeft += e.deltaY;
          }}
        >
          {chips.map((chip) => (
            <span
              key={chip.id}
              className={clsx(
                styles.chip,
                chip.type === 'collection' ? styles.collectionChip : styles.tagChip
              )}
              title={chip.type === 'collection' ? `Collection: ${chip.name}` : `Tag: ${chip.name}`}
            >
              {chip.type === 'collection' ? (
                <IconCollection size={9} className={styles.chipIcon} />
              ) : (
                <span
                  className={styles.chipDot}
                  style={{ background: chip.color ? `var(--swatch-${chip.color})` : 'hsl(220 85% 60%)' }}
                />
              )}
              <span className={styles.chipText}>{chip.name}</span>
            </span>
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
  const snapshotUrl = useModelSnapshot(asset);
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
        onToggleFavorite={handleToggleFavorite}
      />
      <CardBody asset={asset} chips={chips} />
    </div>
  );
});
