import { useState } from 'react';
import clsx from 'clsx';
import type { Asset, ColorLabel, Collection, Tag } from '../../types';
import { useTagStore, useCollectionStore } from '../../store';
import {
  IconStar,
  IconX,
  IconCheck,
  IconHeart,
  IconHeartFilled,
  IconTag,
  IconCollection,
} from '../Icons';
import styles from './DetailPanel.module.css';

const COLOR_LABELS: ColorLabel[] = [
  'red', 'orange', 'yellow', 'green', 'teal', 'blue', 'purple', 'pink',
];

const STARS = [1, 2, 3, 4, 5];

interface RatingProps {
  rating: number;
  onChange: (r: number) => void;
}

interface StarButtonProps {
  n: number;
  isLit: boolean;
  onHover: (n: number) => void;
  onLeave: () => void;
  onClick: (n: number) => void;
}

function StarButton({ n, isLit, onHover, onLeave, onClick }: StarButtonProps) {
  const ariaLabel = `Rate ${n} star${n > 1 ? 's' : ''}`;
  return (
    <button
      id={`detail-star-${n}`}
      type="button"
      className={clsx(styles.starBtn, isLit && styles.starOn)}
      onMouseEnter={() => onHover(n)}
      onMouseLeave={onLeave}
      onClick={() => onClick(n)}
      aria-label={ariaLabel}
    >
      <IconStar size={16} />
    </button>
  );
}

function StarRatingRow({ rating, onChange }: RatingProps) {
  const [hoverRating, setHoverRating] = useState(0);
  const effectiveRating = hoverRating || rating;
  const handleLeave = () => setHoverRating(0);
  const handleClick = (n: number) => onChange(rating === n ? 0 : n);

  return (
    <div className={styles.starRow} role="group" aria-label="Asset rating">
      {STARS.map((n) => (
        <StarButton
          key={n}
          n={n}
          isLit={effectiveRating >= n}
          onHover={setHoverRating}
          onLeave={handleLeave}
          onClick={handleClick}
        />
      ))}
    </div>
  );
}

interface ColorDotProps {
  label: ColorLabel;
  isActive: boolean;
  onClick: (l: ColorLabel) => void;
}

function ColorDot({ label, isActive, onClick }: ColorDotProps) {
  return (
    <button
      id={`detail-color-${label}`}
      type="button"
      className={clsx(styles.colorDot, isActive && styles.colorDotActive)}
      style={{ background: `var(--swatch-${label})` }}
      title={label}
      aria-label={`Color label ${label}`}
      onClick={() => onClick(label)}
    />
  );
}

function ColorNoneButton({ isActive, onClick }: { isActive: boolean; onClick: () => void }) {
  return (
    <button
      type="button"
      className={clsx(styles.colorNoneBtn, isActive && styles.colorNoneActive)}
      onClick={onClick}
      title="No label"
      aria-label="Clear color label"
    >
      <IconX size={10} />
    </button>
  );
}

interface ColorLabelProps {
  active: ColorLabel;
  onChange: (l: ColorLabel) => void;
}

function ColorLabelRow({ active, onChange }: ColorLabelProps) {
  return (
    <div className={styles.colorRow} role="group" aria-label="Color label selection">
      <ColorNoneButton isActive={active === 'none'} onClick={() => onChange('none')} />
      {COLOR_LABELS.map((label) => (
        <ColorDot
          key={label}
          label={label}
          isActive={active === label}
          onClick={onChange}
        />
      ))}
    </div>
  );
}

interface FavoriteToggleProps {
  isFavorite: boolean;
  onClick: () => void;
}

function FavoriteToggleButton({ isFavorite, onClick }: FavoriteToggleProps) {
  return (
    <button
      id="detail-organize-favorite"
      type="button"
      className={clsx(styles.infoFavoriteBtn, isFavorite && styles.infoFavoriteActive)}
      onClick={onClick}
    >
      {isFavorite ? (
        <>
          <IconHeartFilled size={14} className={styles.favoriteHeartFilled} />
          <span>In Favorites</span>
        </>
      ) : (
        <>
          <IconHeart size={14} />
          <span>Add to Favorites</span>
        </>
      )}
    </button>
  );
}

interface StatusRatingSectionProps {
  isFavorite: boolean;
  rating: number;
  colorLabel: ColorLabel;
  onFavoriteChange: () => void;
  onRatingChange: (r: number) => void;
  onColorLabelChange: (l: ColorLabel) => void;
}

function StatusRatingSection({
  isFavorite,
  rating,
  colorLabel,
  onFavoriteChange,
  onRatingChange,
  onColorLabelChange,
}: StatusRatingSectionProps) {
  return (
    <div className={styles.organizeGroup}>
      <div className={styles.organizeGroupHeader}>
        <span className={styles.organizeGroupTitle}>Status & Rating</span>
      </div>

      <div className={styles.statusGrid}>
        <div className={styles.statusRow}>
          <span className={styles.statusLabel}>Favorite</span>
          <FavoriteToggleButton isFavorite={isFavorite} onClick={onFavoriteChange} />
        </div>

        <div className={styles.statusRow}>
          <span className={styles.statusLabel}>Rating</span>
          <StarRatingRow rating={rating} onChange={onRatingChange} />
        </div>

        <div className={styles.statusRow}>
          <span className={styles.statusLabel}>Color Label</span>
          <ColorLabelRow active={colorLabel} onChange={onColorLabelChange} />
        </div>
      </div>
    </div>
  );
}

interface TagItemProps {
  tag: Tag;
  isAssigned: boolean;
  onToggle: (id: string) => void;
}

function TagItem({ tag, isAssigned, onToggle }: TagItemProps) {
  const dotColor = tag.color ? `var(--swatch-${tag.color})` : 'var(--color-text-muted)';
  const title = isAssigned ? 'Click to remove tag' : 'Click to assign tag';

  return (
    <button
      id={`detail-tag-${tag.id}`}
      type="button"
      className={clsx(styles.tagPill, isAssigned && styles.tagActive)}
      onClick={() => onToggle(tag.id)}
      title={title}
    >
      <span className={styles.tagDot} style={{ background: dotColor }} />
      <span className={styles.tagPillName}>{tag.name}</span>
      {isAssigned && <IconCheck size={11} className={styles.tagCheckIcon} />}
    </button>
  );
}

interface TagGroupBlockProps {
  title: string;
  tags: Tag[];
  isAssigned: boolean;
  onToggle: (id: string) => void;
}

function TagGroupBlock({ title, tags, isAssigned, onToggle }: TagGroupBlockProps) {
  if (tags.length === 0) return null;
  return (
    <div className={styles.tagsSubGroup}>
      <span className={styles.tagsSubLabel}>{title}</span>
      <div className={styles.tagsGrid}>
        {tags.map((tag) => (
          <TagItem key={tag.id} tag={tag} isAssigned={isAssigned} onToggle={onToggle} />
        ))}
      </div>
    </div>
  );
}

function partitionTags(tags: Tag[], assetTags: string[]) {
  const assigned = tags.filter((t) => assetTags.includes(t.id));
  const available = tags.filter((t) => !assetTags.includes(t.id));
  return { assigned, available };
}

interface TagsSectionProps {
  tags: Tag[];
  assetTags: string[];
  onToggleTag: (tagId: string) => void;
}

function TagsSection({ tags, assetTags, onToggleTag }: TagsSectionProps) {
  if (tags.length === 0) {
    return (
      <div className={styles.organizeGroup}>
        <div className={styles.organizeGroupHeader}>
          <div className={styles.groupTitleWithIcon}>
            <IconTag size={13} />
            <span className={styles.organizeGroupTitle}>Tags</span>
          </div>
        </div>
        <p className={styles.noItemsText}>No tags yet. Create tags in the sidebar.</p>
      </div>
    );
  }

  const { assigned, available } = partitionTags(tags, assetTags);
  const availableTitle = assigned.length > 0 ? 'Available Tags' : 'All Tags';

  return (
    <div className={styles.organizeGroup}>
      <div className={styles.organizeGroupHeader}>
        <div className={styles.groupTitleWithIcon}>
          <IconTag size={13} />
          <span className={styles.organizeGroupTitle}>Tags</span>
        </div>
        {assigned.length > 0 && (
          <span className={styles.organizeBadge}>{assigned.length} assigned</span>
        )}
      </div>

      <div className={styles.tagsContainer}>
        <TagGroupBlock
          title="Assigned"
          tags={assigned}
          isAssigned={true}
          onToggle={onToggleTag}
        />
        <TagGroupBlock
          title={availableTitle}
          tags={available}
          isAssigned={false}
          onToggle={onToggleTag}
        />
      </div>
    </div>
  );
}

function resolveCollectionPath(col: Collection, collections: Collection[]): string {
  const parts = [col.name];
  let curr = col;
  while (curr.parentId) {
    const parent = collections.find((c) => c.id === curr.parentId);
    if (!parent) break;
    parts.unshift(parent.name);
    curr = parent;
  }
  return parts.join(' / ');
}

function isCollectionMember(col: Collection, assetId: string, assetCollections: string[]): boolean {
  if (col.assetIds?.includes(assetId)) return true;
  return assetCollections.includes(col.id);
}

interface CollectionItemProps {
  col: Collection;
  collections: Collection[];
  assetId: string;
  assetCollections: string[];
  onToggle: (colId: string, isMember: boolean) => void;
}

function CollectionItem({
  col,
  collections,
  assetId,
  assetCollections,
  onToggle,
}: CollectionItemProps) {
  const isMember = isCollectionMember(col, assetId, assetCollections);
  const pathName = resolveCollectionPath(col, collections);
  const dotBg = isMember ? 'var(--color-brand-400)' : 'var(--color-text-muted)';

  return (
    <button
      id={`detail-col-${col.id}`}
      type="button"
      className={clsx(styles.collectionPill, isMember && styles.collectionPillActive)}
      onClick={() => onToggle(col.id, isMember)}
      title={pathName}
    >
      <span className={styles.collectionDot} style={{ background: dotBg }} />
      <span className={styles.collectionPathName}>{pathName}</span>
      <span className={styles.collectionCount}>({col.assetIds.length})</span>
      {isMember && <IconCheck size={11} className={styles.tagCheckIcon} />}
    </button>
  );
}

interface CollectionsSectionProps {
  collections: Collection[];
  assetId: string;
  assetCollections?: string[];
  onToggle: (colId: string, isMember: boolean) => void;
}

function CollectionsSection({
  collections,
  assetId,
  assetCollections = [],
  onToggle,
}: CollectionsSectionProps) {
  const hasNoCollections = collections.length === 0;

  return (
    <div className={styles.organizeGroup}>
      <div className={styles.organizeGroupHeader}>
        <div className={styles.groupTitleWithIcon}>
          <IconCollection size={13} />
          <span className={styles.organizeGroupTitle}>Collections</span>
        </div>
      </div>

      {hasNoCollections ? (
        <p className={styles.noItemsText}>No collections yet. Create collections in the sidebar.</p>
      ) : (
        <div className={styles.collectionsGrid}>
          {collections.map((col) => (
            <CollectionItem
              key={col.id}
              col={col}
              collections={collections}
              assetId={assetId}
              assetCollections={assetCollections}
              onToggle={onToggle}
            />
          ))}
        </div>
      )}
    </div>
  );
}

interface OrganizeTabProps {
  asset: Asset;
  onRatingChange: (r: number) => void;
  onColorLabelChange: (l: ColorLabel) => void;
  onFavoriteChange: () => void;
  onToggleTag: (tagId: string) => void;
}

export function OrganizeTab({
  asset,
  onRatingChange,
  onColorLabelChange,
  onFavoriteChange,
  onToggleTag,
}: OrganizeTabProps) {
  const tags = useTagStore((s) => s.tags);
  const collections = useCollectionStore((s) => s.collections);
  const addToCollection = useCollectionStore((s) => s.addToCollection);
  const removeFromCollection = useCollectionStore((s) => s.removeFromCollection);

  const handleCollectionToggle = async (colId: string, isMember: boolean) => {
    const action = isMember ? removeFromCollection : addToCollection;
    await action(colId, [asset.id]);
    useCollectionStore.getState().fetchCollections();
  };

  return (
    <div className={styles.organizeSection}>
      <StatusRatingSection
        isFavorite={asset.isFavorite}
        rating={asset.rating}
        colorLabel={asset.colorLabel}
        onFavoriteChange={onFavoriteChange}
        onRatingChange={onRatingChange}
        onColorLabelChange={onColorLabelChange}
      />

      <TagsSection
        tags={tags}
        assetTags={asset.tags}
        onToggleTag={onToggleTag}
      />

      <CollectionsSection
        collections={collections}
        assetId={asset.id}
        assetCollections={asset.collections}
        onToggle={handleCollectionToggle}
      />
    </div>
  );
}
