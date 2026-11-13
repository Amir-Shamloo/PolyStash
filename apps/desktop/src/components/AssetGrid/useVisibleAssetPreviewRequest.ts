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

/** Requests thumbnail generation for cards inside `container` that are in view and lack one. */
export function useVisibleAssetPreviewRequest(container: HTMLElement | null, assets: Asset[]) {
  const assetsRef = useRef(assets);
  assetsRef.current = assets;
  const inView = useRef(new Set<string>());
  const requested = useRef(new Set<string>());

  useEffect(() => {
    if (!container || typeof IntersectionObserver === 'undefined') return;
    inView.current.clear();
    let timer: ReturnType<typeof setTimeout> | null = null;

    const flush = () => {
      timer = null;
      const byId = new Map(assetsRef.current.map((a) => [a.id, a]));
      const ids = [...inView.current].filter(
        (id) => !requested.current.has(id) && needsPreview(byId.get(id))
      );
      if (ids.length === 0) return;
      ids.forEach((id) => requested.current.add(id));
      api.requestPreview(ids).catch(() => ids.forEach((id) => requested.current.delete(id)));
    };
    const schedule = () => {
      if (timer === null) timer = setTimeout(flush, THROTTLE_MS);
    };

    const observer = new IntersectionObserver(
      (entries) => {
        for (const entry of entries) {
          const id = entry.target.id.slice(ID_PREFIX.length);
          if (entry.isIntersecting) inView.current.add(id);
          else inView.current.delete(id);
        }
        schedule();
      },
      { root: container }
    );
    container.querySelectorAll(CARD_SELECTOR).forEach((el) => observer.observe(el));
    schedule();

    return () => {
      observer.disconnect();
      if (timer !== null) clearTimeout(timer);
    };
  }, [container, assets]);
}
