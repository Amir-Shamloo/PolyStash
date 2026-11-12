import { useCallback, useEffect, useRef, useState } from 'react';
import { IconPlus } from '../Icons';
import styles from './FullscreenPreviewModal.module.css';

const MIN_ZOOM = 1;
const MAX_ZOOM = 16;
const BUTTON_STEP = 1.5;

interface View {
  zoom: number;
  x: number;
  y: number;
}

const FIT: View = { zoom: 1, x: 0, y: 0 };
const clampZoom = (z: number) => Math.min(MAX_ZOOM, Math.max(MIN_ZOOM, z));

function IconMinus({ size = 14 }: { size?: number }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" aria-hidden>
      <path d="M5 12h14" />
    </svg>
  );
}

/** Zooms around a point given relative to the container centre, keeping that point fixed. */
function zoomAt(view: View, factor: number, px: number, py: number): View {
  const zoom = clampZoom(view.zoom * factor);
  if (zoom === MIN_ZOOM) return FIT;
  const ratio = zoom / view.zoom;
  return { zoom, x: px - (px - view.x) * ratio, y: py - (py - view.y) * ratio };
}

function pinchDistance(points: Map<number, { x: number; y: number }>): number {
  const [a, b] = [...points.values()];
  return Math.hypot(a.x - b.x, a.y - b.y);
}

export function ZoomableImage({ src, alt }: { src: string; alt: string }) {
  const containerRef = useRef<HTMLDivElement>(null);
  const pointers = useRef(new Map<number, { x: number; y: number }>());
  const lastPinch = useRef(0);
  const [view, setView] = useState<View>(FIT);
  const [dragging, setDragging] = useState(false);

  const centerOffset = useCallback((clientX: number, clientY: number) => {
    const rect = containerRef.current?.getBoundingClientRect();
    if (!rect) return { x: 0, y: 0 };
    return { x: clientX - rect.left - rect.width / 2, y: clientY - rect.top - rect.height / 2 };
  }, []);

  useEffect(() => {
    const el = containerRef.current;
    if (!el) return;
    const onWheel = (e: WheelEvent) => {
      e.preventDefault();
      const p = centerOffset(e.clientX, e.clientY);
      setView((v) => zoomAt(v, Math.exp(-e.deltaY * 0.0015), p.x, p.y));
    };
    el.addEventListener('wheel', onWheel, { passive: false });
    return () => el.removeEventListener('wheel', onWheel);
  }, [centerOffset]);

  const stepZoom = (factor: number) => setView((v) => zoomAt(v, factor, 0, 0));

  const onPointerDown = (e: React.PointerEvent) => {
    e.currentTarget.setPointerCapture(e.pointerId);
    pointers.current.set(e.pointerId, { x: e.clientX, y: e.clientY });
    if (pointers.current.size === 2) lastPinch.current = pinchDistance(pointers.current);
    setDragging(true);
  };

  const onPointerMove = (e: React.PointerEvent) => {
    const prev = pointers.current.get(e.pointerId);
    if (!prev) return;
    pointers.current.set(e.pointerId, { x: e.clientX, y: e.clientY });
    if (pointers.current.size === 2) {
      const dist = pinchDistance(pointers.current);
      const [a, b] = [...pointers.current.values()];
      const mid = centerOffset((a.x + b.x) / 2, (a.y + b.y) / 2);
      const factor = lastPinch.current > 0 ? dist / lastPinch.current : 1;
      lastPinch.current = dist;
      setView((v) => zoomAt(v, factor, mid.x, mid.y));
      return;
    }
    const dx = e.clientX - prev.x;
    const dy = e.clientY - prev.y;
    setView((v) => (v.zoom > MIN_ZOOM ? { ...v, x: v.x + dx, y: v.y + dy } : v));
  };

  const onPointerEnd = (e: React.PointerEvent) => {
    pointers.current.delete(e.pointerId);
    if (pointers.current.size === 0) setDragging(false);
  };

  const zoomed = view.zoom > MIN_ZOOM;
  return (
    <div
      ref={containerRef}
      className={styles.zoomContainer}
      data-zoomed={zoomed}
      data-dragging={dragging}
      onPointerDown={onPointerDown}
      onPointerMove={onPointerMove}
      onPointerUp={onPointerEnd}
      onPointerCancel={onPointerEnd}
      onDoubleClick={() => setView(zoomed ? FIT : (v) => zoomAt(v, 2, 0, 0))}
    >
      <img
        src={src}
        alt={alt}
        draggable={false}
        className={styles.textureImage}
        style={{ transform: `translate(${view.x}px, ${view.y}px) scale(${view.zoom})` }}
      />
      <div className={styles.zoomControls} onPointerDown={(e) => e.stopPropagation()} onDoubleClick={(e) => e.stopPropagation()}>
        <button type="button" className={styles.zoomBtn} onClick={() => stepZoom(1 / BUTTON_STEP)} disabled={!zoomed} aria-label="Zoom out" title="Zoom out">
          <IconMinus />
        </button>
        <button type="button" className={styles.zoomReset} onClick={() => setView(FIT)} aria-label="Reset zoom" title="Reset zoom">
          {Math.round(view.zoom * 100)}%
        </button>
        <button type="button" className={styles.zoomBtn} onClick={() => stepZoom(BUTTON_STEP)} disabled={view.zoom >= MAX_ZOOM} aria-label="Zoom in" title="Zoom in">
          <IconPlus size={14} />
        </button>
      </div>
    </div>
  );
}
