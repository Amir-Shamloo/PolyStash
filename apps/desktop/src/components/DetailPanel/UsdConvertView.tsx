import { useEffect, useState, type ReactNode } from 'react';
import type { Asset } from '../../types';
import { api } from '../../api';
import { useAssetStore } from '../../store';
import { convertUsdToGlbBase64 } from '../../lib/usdToGlb';

const inflight = new Map<string, Promise<void>>();
const failed = new Set<string>();

function convertAndCache(asset: Asset): Promise<string> {
  const stem = asset.fileName.replace(/\.[^.]+$/, '');
  return convertUsdToGlbBase64(asset.filePath, asset.extension).then((base64) =>
    api.saveConvertedUsdGlb(asset.id, `${stem}.glb`, base64)
  );
}

/** Converts a USD asset to a cached GLB; the router then swaps this view for the 3D viewer. */
export function UsdConvertView({ asset, fallback }: { asset: Asset; fallback: ReactNode }) {
  const [, setFailedTick] = useState(0);

  useEffect(() => {
    if (failed.has(asset.id) || inflight.has(asset.id)) return;
    const task = convertAndCache(asset)
      .then(async () => {
        const updated = await api.getAsset(asset.id);
        useAssetStore.setState((s) => ({
          assets: s.assets.map((a) => (a.id === asset.id ? updated : a)),
          assetMap: new Map(s.assetMap).set(asset.id, updated),
        }));
      })
      .catch((err) => {
        console.warn('USD to GLB conversion failed:', err);
        failed.add(asset.id);
      })
      .finally(() => {
        inflight.delete(asset.id);
        setFailedTick((n) => n + 1);
      });
    inflight.set(asset.id, task);
  }, [asset]);

  if (failed.has(asset.id)) return <>{fallback}</>;
  return (
    <div style={{ width: '100%', height: '100%', display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
      <span style={{ fontSize: '11px', color: 'var(--color-text-muted)' }}>Converting USD…</span>
    </div>
  );
}
