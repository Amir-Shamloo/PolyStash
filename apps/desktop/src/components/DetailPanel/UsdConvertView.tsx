import { useEffect, useState, type ReactNode } from 'react';
import type { Asset } from '../../types';
import { convertUsdAsset, hasUsdConversionFailed, refreshAssetInStore } from '../../lib/usdConversion';

/** Converts a USD asset to a cached GLB; the router then swaps this view for the 3D viewer. */
export function UsdConvertView({ asset, fallback }: { asset: Asset; fallback: ReactNode }) {
  const [, setTick] = useState(0);

  useEffect(() => {
    if (hasUsdConversionFailed(asset.id)) return;
    convertUsdAsset(asset)
      .then(() => refreshAssetInStore(asset.id))
      .catch(() => undefined)
      .finally(() => setTick((n) => n + 1));
  }, [asset]);

  if (hasUsdConversionFailed(asset.id)) return <>{fallback}</>;
  return (
    <div style={{ width: '100%', height: '100%', display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
      <span style={{ fontSize: '11px', color: 'var(--color-text-muted)' }}>Converting USD…</span>
    </div>
  );
}
