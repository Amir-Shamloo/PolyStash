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

function fileNameOf(filePath: string): string {
  const normalized = filePath.replace(/\\/g, '/');
  return normalized.slice(normalized.lastIndexOf('/') + 1) || filePath;
}

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
        <span className={styles.infoLabel}>Name</span>
        <span className={clsx(styles.infoValue, styles.mono)} title={fileNameOf(filePath)}>
          {fileNameOf(filePath)}
        </span>

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

const SBSAR_TEXTURES_KEY = 'sbsar_textures';

const SBSAR_SLOT_LABELS: Record<string, string> = {
  baseColor: 'Base Color',
  normal: 'Normal',
  roughness: 'Roughness',
  metallic: 'Metallic',
  height: 'Height',
  opacity: 'Opacity',
};

function SbsarTextureList({ textures }: { textures: Record<string, unknown> }) {
  const rows = Object.entries(textures).filter(
    (entry): entry is [string, string] => typeof entry[1] === 'string' && entry[1].length > 0
  );
  if (rows.length === 0) return null;
  return (
    <div className={styles.metaSubGroup}>
      <span className={styles.metaSubGroupTitle}>Texture Maps</span>
      <div className={styles.infoGrid}>
        {rows.map(([slot, path]) => (
          <Fragment key={slot}>
            <span className={styles.metaKeyLabel}>{SBSAR_SLOT_LABELS[slot] ?? slot}</span>
            <div className={styles.infoValue}>
              <span className={styles.mono} title={path}>{fileNameOf(path)}</span>
              <button
                type="button"
                className={styles.miniPathBtn}
                onClick={() => api.revealInExplorer(path).catch(console.error)}
                title="Open in file explorer"
                aria-label={`Open ${SBSAR_SLOT_LABELS[slot] ?? slot} texture in file explorer`}
              >
                <IconExternal size={12} />
              </button>
            </div>
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

  // One stable ordering everywhere: alphabetical inside each group.
  for (const group of [result.geometry, result.dimensions, result.render, result.lighting, result.general]) {
    group.sort(([a], [b]) => a.localeCompare(b));
  }

  return result;
}

interface TechnicalMetadataProps {
  metadata: Record<string, unknown>;
  extension: string;
}

function countKeys(value: unknown): number {
  return value && typeof value === 'object' ? Object.keys(value as Record<string, unknown>).length : 0;
}

function shaderBallFieldCount(categorized: ReturnType<typeof partitionMetadata>, textureCount: number): number {
  const groups = categorized.render.length + categorized.lighting.length + categorized.general.length;
  return textureCount ? groups + 1 : groups;
}

/** Shader balls have no mesh data; their texture maps replace the geometry groups. */
function MetadataGroups({
  isShaderBall,
  categorized,
  sbsarTextures,
  textureCount,
}: {
  isShaderBall: boolean;
  categorized: ReturnType<typeof partitionMetadata>;
  sbsarTextures: unknown;
  textureCount: number;
}) {
  if (isShaderBall) {
    return (
      <>
        {textureCount > 0 && (
          <SbsarTextureList textures={sbsarTextures as Record<string, unknown>} />
        )}
        <MetaSubGroup title="Lighting & Photometry" entries={categorized.lighting} />
        <MetaSubGroup title="Renderer & Engine" entries={categorized.render} />
        <MetaSubGroup title="Additional Properties" entries={categorized.general} />
      </>
    );
  }

  return (
    <>
      <MetaSubGroup title="Geometry & Meshes" entries={categorized.geometry} />
      <MetaSubGroup title="Dimensions & Resolution" entries={categorized.dimensions} />
      <MetaSubGroup title="Lighting & Photometry" entries={categorized.lighting} />
      <MetaSubGroup title="Renderer & Engine" entries={categorized.render} />
      <MetaSubGroup title="Additional Properties" entries={categorized.general} />
    </>
  );
}

function TechnicalMetadataSection({ metadata, extension }: TechnicalMetadataProps) {
  const isShaderBall = extension.toLowerCase().replace(/^\./, '') === 'sbsar';
  const entries = useMemo(
    () =>
      Object.entries(metadata || {}).filter(
        ([k]) => !(isShaderBall && k === SBSAR_TEXTURES_KEY)
      ),
    [metadata, isShaderBall]
  );
  const categorized = useMemo(() => partitionMetadata(entries), [entries]);
  const sbsarTextures = isShaderBall ? metadata?.[SBSAR_TEXTURES_KEY] : undefined;
  const textureCount = countKeys(sbsarTextures);
  const fieldCount = isShaderBall
    ? shaderBallFieldCount(categorized, textureCount)
    : entries.length;

  // Assets without metadata show no section at all, not an empty placeholder.
  if (entries.length === 0 && textureCount === 0) return null;

  return (
    <div className={styles.infoSection}>
      <div className={styles.infoSectionHeader}>
        <span className={styles.infoSectionTitle}>Technical Metadata</span>
        <span className={styles.metaCountBadge}>{fieldCount} fields</span>
      </div>

      <div className={styles.metaGroupsContainer}>
        <MetadataGroups
          isShaderBall={isShaderBall}
          categorized={categorized}
          sbsarTextures={sbsarTextures}
          textureCount={textureCount}
        />
      </div>
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

      <TechnicalMetadataSection metadata={asset.metadata} extension={asset.extension} />
    </div>
  );
}
