import { useEffect, useRef } from 'react';
import { api } from '../../api';
import type { Asset } from '../../types';

const THROTTLE_MS = 150;
const CARD_SELECTOR = '[id^="asset-"]';
const ID_PREFIX = 'asset-';

function needsPreview(asset: Asset | undefined): asset is Asset {
  return (
    !!asset &&
    !asset.thumbnailPath &&
    asset.previewStatus !== 'pending' &&
    asset.previewStatus !== 'generating' &&
    asset.previewStatus !== 'done'
  );
}

function collectPendingIds(inView: Set<string>, requested: Set<string>, assets: Asset[]): string[] {
  const byId = new Map(assets.map((a) => [a.id, a]));
  return [...inView].filter((id) => !requested.has(id) && needsPreview(byId.get(id)));
}

function trackVisibility(inView: Set<string>, entries: IntersectionObserverEntry[]) {
  for (const entry of entries) {
    const id = entry.target.id.slice(ID_PREFIX.length);
    if (entry.isIntersecting) inView.add(id);
    else inView.delete(id);
  }
}

function requestPreviews(ids: string[], requested: Set<string>) {
  ids.forEach((id) => requested.add(id));
  api.requestPreview(ids).catch(() => ids.forEach((id) => requested.delete(id)));
}

/** Requests thumbnail generation for cards inside `container` that are in view and lack one. */
export function useVisibleAssetPreviewRequest(container: HTMLElement | null, assets: Asset[]) {
  const assetsRef = useRef(assets);
  assetsRef.current = assets;
  const inView = useRef(new Set<string>());
  const requested = useRef(new Set<string>());

  useEffect(() => {
    if (!container || typeof IntersectionObserver === 'undefined') return;
    inView.current.clear();
    const observer = new IntersectionObserver((entries) => trackVisibility(inView.current, entries), {
      root: container,
    });
    container.querySelectorAll(CARD_SELECTOR).forEach((el) => observer.observe(el));
    return () => observer.disconnect();
  }, [container, assets]);

  useEffect(() => {
    if (!container || typeof IntersectionObserver === 'undefined') return;
    const timer = setInterval(() => {
      const ids = collectPendingIds(inView.current, requested.current, assetsRef.current);
      if (ids.length > 0) requestPreviews(ids, requested.current);
    }, THROTTLE_MS);
    return () => clearInterval(timer);
  }, [container, assets]);
}
