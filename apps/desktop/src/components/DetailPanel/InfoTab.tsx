import { useMemo, Fragment } from 'react';
import clsx from 'clsx';
import type { Asset } from '../../types';
import { api } from '../../api';
import { IconExternal, IconCopy } from '../Icons';
import styles from './DetailPanel.module.css';

interface InfoTabProps {
  asset: Asset;
}

function formatBytes(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
  if (bytes < 1024 * 1024 * 1024) return `${(bytes / 1024 / 1024).toFixed(2)} MB`;
  return `${(bytes / 1024 / 1024 / 1024).toFixed(2)} GB`;
}

function formatDate(timestamp: number): string {
  if (!timestamp) return 'Unknown';
  const d = new Date(timestamp);
  return d.toLocaleString(undefined, {
    year: 'numeric',
    month: 'short',
    day: 'numeric',
    hour: '2-digit',
    minute: '2-digit',
  });
}

function formatMetaValue(val: unknown): string {
  if (val === null || val === undefined) return '—';
  if (typeof val === 'boolean') return val ? 'Yes' : 'No';
  if (typeof val === 'number') return val.toLocaleString();
  if (typeof val === 'object') {
    try {
      return JSON.stringify(val, null, 2);
    } catch {
      return String(val);
    }
  }
  return String(val);
}

const GEOMETRY_KEYS = new Set([
  'vertices', 'vertexcount', 'vertex_count', 'faces', 'facecount', 'face_count',
  'polygons', 'polycount', 'poly_count', 'triangles', 'trianglecount', 'meshes',
  'meshcount', 'materials', 'materialcount', 'bones', 'animations', 'scenes', 'nodes',
]);

const DIMENSION_KEYS = new Set([
  'width', 'height', 'resolution', 'channels', 'bitdepth', 'bit_depth',
  'colorspace', 'color_space', 'hdr', 'dpi', 'aspectratio', 'aspect_ratio',
  'duration', 'fps', 'codec', 'framerate', 'frame_rate',
]);

const RENDER_KEYS = new Set([
  'renderer', 'engine', 'vrayversion', 'vray_version', 'coronaversion',
  'maxversion', 'blenderversion', 'software', 'application', 'generator',
]);

const LIGHTING_KEYS = new Set([
  'lighttype', 'light_type', 'lumens', 'candela', 'colortemp', 'color_temp',
  'distribution', 'watt', 'beamangle', 'beam_angle', 'fieldangle', 'multiplier',
]);

interface FilePropsSectionProps {
  extension: string;
  sizeBytes: number;
  filePath: string;
  modifiedAt: number;
  indexedAt: number;
}

function FilePropertiesSection({
  extension,
  sizeBytes,
  filePath,
  modifiedAt,
  indexedAt,
}: FilePropsSectionProps) {
  const handleCopyPath = () => {
    navigator.clipboard.writeText(filePath);
  };

  const handleReveal = () => {
    api.revealInExplorer(filePath).catch(console.error);
  };

  return (
    <div className={styles.infoSection}>
      <div className={styles.infoSectionHeader}>
        <span className={styles.infoSectionTitle}>File Properties</span>
      </div>

      <div className={styles.infoGrid}>
        <span className={styles.infoLabel}>Format</span>
        <span className={styles.infoValue}>
          <span className={styles.formatChip}>.{extension.toUpperCase()}</span>
        </span>

        <span className={styles.infoLabel}>Size</span>
        <span className={clsx(styles.infoValue, styles.mono)}>
          {formatBytes(sizeBytes)}{' '}
          <span className={styles.bytesHint}>({sizeBytes.toLocaleString()} bytes)</span>
        </span>

        <span className={styles.infoLabel}>Path</span>
        <div className={styles.pathCell}>
          <span
            className={clsx(styles.infoValue, styles.mono, styles.pathText)}
            title={`${filePath} (Click to reveal)`}
            onClick={handleReveal}
          >
            {filePath}
          </span>
          <div className={styles.pathActions}>
            <button
              type="button"
              className={styles.miniPathBtn}
              onClick={handleCopyPath}
              title="Copy full path"
              aria-label="Copy full path"
            >
              <IconCopy size={12} />
            </button>
            <button
              type="button"
              className={styles.miniPathBtn}
              onClick={handleReveal}
              title="Reveal in file explorer"
              aria-label="Reveal in file explorer"
            >
              <IconExternal size={12} />
            </button>
          </div>
        </div>

        <span className={styles.infoLabel}>Modified</span>
        <span className={styles.infoValue}>{formatDate(modifiedAt)}</span>

        <span className={styles.infoLabel}>Added</span>
        <span className={styles.infoValue}>{formatDate(indexedAt)}</span>
      </div>
    </div>
  );
}

interface MetaSubGroupProps {
  title: string;
  entries: [string, unknown][];
}

function MetaSubGroup({ title, entries }: MetaSubGroupProps) {
  if (entries.length === 0) return null;
  return (
    <div className={styles.metaSubGroup}>
      <span className={styles.metaSubGroupTitle}>{title}</span>
      <div className={styles.infoGrid}>
        {entries.map(([k, v]) => (
          <Fragment key={k}>
            <span className={styles.metaKeyLabel}>{k}</span>
            <span className={clsx(styles.infoValue, styles.mono)}>{formatMetaValue(v)}</span>
          </Fragment>
        ))}
      </div>
    </div>
  );
}

interface CategorizedMetadata {
  geometry: [string, unknown][];
  dimensions: [string, unknown][];
  render: [string, unknown][];
  lighting: [string, unknown][];
  general: [string, unknown][];
}

function partitionMetadata(entries: [string, unknown][]): CategorizedMetadata {
  const result: CategorizedMetadata = {
    geometry: [],
    dimensions: [],
    render: [],
    lighting: [],
    general: [],
  };

  for (const [k, v] of entries) {
    const lower = k.toLowerCase().replace(/[-_]/g, '');
    if (GEOMETRY_KEYS.has(lower)) {
      result.geometry.push([k, v]);
    } else if (DIMENSION_KEYS.has(lower)) {
      result.dimensions.push([k, v]);
    } else if (RENDER_KEYS.has(lower)) {
      result.render.push([k, v]);
    } else if (LIGHTING_KEYS.has(lower)) {
      result.lighting.push([k, v]);
    } else {
      result.general.push([k, v]);
    }
  }

  return result;
}

interface TechnicalMetadataProps {
  metadata: Record<string, unknown>;
}

function TechnicalMetadataSection({ metadata }: TechnicalMetadataProps) {
  const entries = useMemo(() => Object.entries(metadata || {}), [metadata]);
  const categorized = useMemo(() => partitionMetadata(entries), [entries]);

  return (
    <div className={styles.infoSection}>
      <div className={styles.infoSectionHeader}>
        <span className={styles.infoSectionTitle}>Technical Metadata</span>
        {entries.length > 0 && (
          <span className={styles.metaCountBadge}>{entries.length} fields</span>
        )}
      </div>

      {entries.length === 0 ? (
        <p className={styles.noMetadataText}>No format-specific metadata extracted.</p>
      ) : (
        <div className={styles.metaGroupsContainer}>
          <MetaSubGroup title="Geometry & Meshes" entries={categorized.geometry} />
          <MetaSubGroup title="Dimensions & Resolution" entries={categorized.dimensions} />
          <MetaSubGroup title="Lighting & Photometry" entries={categorized.lighting} />
          <MetaSubGroup title="Renderer & Engine" entries={categorized.render} />
          <MetaSubGroup title="Additional Properties" entries={categorized.general} />
        </div>
      )}
    </div>
  );
}

export function InfoTab({ asset }: InfoTabProps) {
  return (
    <div className={styles.infoTabContainer}>
      <FilePropertiesSection
        extension={asset.extension}
        sizeBytes={asset.sizeBytes}
        filePath={asset.filePath}
        modifiedAt={asset.modifiedAt}
        indexedAt={asset.indexedAt}
      />

      <TechnicalMetadataSection metadata={asset.metadata} />
    </div>
  );
}
