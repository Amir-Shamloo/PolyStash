import type { Asset } from '../types';
import { api } from '../api';
import { useAssetStore } from '../store';
import { convertUsdToGlbBase64 } from './usdToGlb';

export const USD_EXTENSIONS = new Set(['usd', 'usda', 'usdc', 'usdz']);

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
  const stem = asset.fileName.replace(/\.[^.]+$/, '');
  const task = convertUsdToGlbBase64(asset.filePath, asset.extension)
    .then((base64) => api.saveConvertedUsdGlb(asset.id, `${stem}.glb`, base64))
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
