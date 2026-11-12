import type { Asset } from '../types';
import { api } from '../api';
import { useAssetStore } from '../store';
import { convertUsdToGlbBase64 } from './usdToGlb';

export const USD_EXTENSIONS = new Set(['usd', 'usda', 'usdc', 'usdz']);

/** Mirrors the backend's `thumb_stem` (FNV-1a + murmur3 finalizer, UTF-8 bytes). */
function previewStem(filePath: string, fileName: string): string {
  const normalized = filePath.replace(/[\\/][^\\/]*$/, '').replace(/\\/g, '/');
  let hash = 0x811c9dc5;
  for (const byte of new TextEncoder().encode(normalized)) {
    hash ^= byte;
    hash = Math.imul(hash, 0x01000193);
  }
  hash ^= hash >>> 16;
  hash = Math.imul(hash, 0x85ebca6b);
  hash ^= hash >>> 13;
  hash = Math.imul(hash, 0xc2b2ae35);
  hash ^= hash >>> 16;
  const stem = fileName.replace(/\.[^.]+$/, '');
  return `${stem}_${(hash & 0xffff).toString(16).padStart(4, '0')}`;
}

const inflight = new Map<string, Promise<string>>();
const failed = new Set<string>();

export function hasUsdConversionFailed(assetId: string): boolean {
  return failed.has(assetId);
}

export function markUsdConversionFailed(assetId: string): void {
  failed.add(assetId);
}

/** Converts a USD asset to a cached GLB and returns its path. Concurrent callers share one run. */
export function convertUsdAsset(asset: Asset): Promise<string> {
  const running = inflight.get(asset.id);
  if (running) return running;
  const name = previewStem(asset.filePath, asset.fileName);
  const task = convertUsdToGlbBase64(asset.filePath, asset.extension)
    .then((base64) => api.saveConvertedUsdGlb(asset.id, `${name}.glb`, base64))
    .catch((err) => {
      console.warn('USD to GLB conversion failed:', err);
      failed.add(asset.id);
      throw err;
    })
    .finally(() => inflight.delete(asset.id));
  inflight.set(asset.id, task);
  return task;
}

/** Pulls the asset (now carrying metadata.preview_model) back into the store. */
export async function refreshAssetInStore(assetId: string): Promise<void> {
  const updated = await api.getAsset(assetId);
  useAssetStore.setState((s) => ({
    assets: s.assets.map((a) => (a.id === assetId ? updated : a)),
    assetMap: new Map(s.assetMap).set(assetId, updated),
  }));
}
