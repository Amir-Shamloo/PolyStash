import { Canvas, useFrame, useThree } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei';
import { Suspense, useRef, useState, useCallback, useEffect } from 'react';
import * as THREE from 'three';
import clsx from 'clsx';
import { convertFileSrc } from '@tauri-apps/api/core';
import {
  parsePhotometry,
  serializeIES,
  summarizeIES,
  buildIntensityCurves,
  type PhotometricDocument,
  type IntensityCurve,
} from '@ies-tools/core';
import { IESLoader } from 'three/examples/jsm/loaders/IESLoader.js';
import { IconSun, IconRefresh, IconCameraReset, IconInfo } from '../Icons';
import { ThreeErrorBoundary } from './ThreeViewer';
import styles from './ThreeViewer.module.css';

export interface IesProfileInfo {
  name: string;
  sourceFormat: string;
  peakCandela: number | null;
  lumens: number | null;
  watts: number | null;
  colorTemperature: string | null;
  curves: IntensityCurve[];
  maxCurveIntensity: number;
}

/**
 * three r167 IESLoader produces a 1-channel RedFormat DataTexture.
 * Sampling a RedFormat texture in WebGL returns (r, 0, 0, 1), which turns white
 * spotlights pure red when multiplied by directLight.color.
 * Convert the single-channel data into 4-channel grayscale RGBA (r, r, r, 1).
 */
export function convertIesTextureToGrayscaleRgba(tex: THREE.DataTexture): THREE.DataTexture {
  const image = tex.image as { data: Uint16Array | Float32Array | Uint8Array; width: number; height: number } | undefined;
  if (!image || !image.data) return tex;

  const src = image.data;
  const width = image.width || 180;
  const height = image.height || 1;
  const count = src.length;

  let dst: Uint16Array<ArrayBuffer> | Float32Array<ArrayBuffer> | Uint8Array<ArrayBuffer>;
  let oneAlpha: number;

  if (src instanceof Uint16Array) {
    dst = new Uint16Array(count * 4);
    oneAlpha = THREE.DataUtils.toHalfFloat(1.0);
  } else if (src instanceof Float32Array) {
    dst = new Float32Array(count * 4);
    oneAlpha = 1.0;
  } else {
    dst = new Uint8Array(count * 4);
    oneAlpha = 255;
  }

  for (let i = 0; i < count; i++) {
    const r = src[i];
    const base = i * 4;
    dst[base] = r;
    dst[base + 1] = r;
    dst[base + 2] = r;
    dst[base + 3] = oneAlpha;
  }

  const rgbaTex = new THREE.DataTexture(dst, width, height, THREE.RGBAFormat, tex.type);
  rgbaTex.minFilter = THREE.LinearFilter;
  rgbaTex.magFilter = THREE.LinearFilter;
  rgbaTex.wrapS = tex.wrapS;
  rgbaTex.wrapT = tex.wrapT;
  rgbaTex.needsUpdate = true;
  tex.dispose();
  return rgbaTex;
}

export const IES_SPOT_ANGLE = Math.PI / 2.4;
const GOBO_SIZE = 1024;

function readIesSample(data: Uint16Array | Float32Array | Uint8Array, index: number): number {
  const v = data[index * 4];
  if (data instanceof Uint16Array) return THREE.DataUtils.fromHalfFloat(v);
  if (data instanceof Uint8Array) return v / 255;
  return v;
}

/**
 * three's WebGL SpotLight has no IES support: `iesMap` is ignored and `map` is a
 * perspective-projected gobo, so a 180x1 vertical-angle strip used as `map` is sampled
 * at the wrong uv and blacks the scene out. Bake the strip into a radial 2D gobo that
 * matches the spot cone's projection (r = tan(theta) / tan(angle)).
 */
export function buildIesGoboTexture(tex: THREE.DataTexture, spotAngle = IES_SPOT_ANGLE): THREE.DataTexture {
  const image = tex.image as { data: Uint16Array | Float32Array | Uint8Array; width: number };
  const { data, width } = image;
  let peak = 0;
  for (let i = 0; i < width; i++) peak = Math.max(peak, readIesSample(data, i));
  const norm = peak > 0 ? 1 / peak : 1;
  const tanAngle = Math.tan(spotAngle);
  const out = new Uint8Array(GOBO_SIZE * GOBO_SIZE * 4);
  for (let y = 0; y < GOBO_SIZE; y++) {
    for (let x = 0; x < GOBO_SIZE; x++) {
      const dx = ((x + 0.5) / GOBO_SIZE) * 2 - 1;
      const dy = ((y + 0.5) / GOBO_SIZE) * 2 - 1;
      const theta = Math.atan(Math.hypot(dx, dy) * tanAngle);
      const pos = Math.min(width - 1, (theta * 180) / Math.PI);
      const lo = Math.floor(pos);
      const hi = Math.min(width - 1, lo + 1);
      const sample = readIesSample(data, lo) + (readIesSample(data, hi) - readIesSample(data, lo)) * (pos - lo);
      const value = Math.round(Math.min(1, sample * norm) * 255);
      const o = (y * GOBO_SIZE + x) * 4;
      out[o] = out[o + 1] = out[o + 2] = value;
      out[o + 3] = 255;
    }
  }
  const gobo = new THREE.DataTexture(out, GOBO_SIZE, GOBO_SIZE, THREE.RGBAFormat);
  gobo.colorSpace = THREE.NoColorSpace;
  gobo.minFilter = THREE.LinearFilter;
  gobo.magFilter = THREE.LinearFilter;
  gobo.needsUpdate = true;
  return gobo;
}

interface OrbitHandle {
  target: THREE.Vector3;
  reset: () => void;
  update: () => void;
}

function IconMaximize({ size = 13 }: { size?: number }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
      <path d="M8 3H5a2 2 0 0 0-2 2v3m18 0V5a2 2 0 0 0-2-2h-3m0 18h3a2 2 0 0 0 2-2v-3M3 16v3a2 2 0 0 0 2 2h3" />
    </svg>
  );
}

function IconMinimize({ size = 13 }: { size?: number }) {
  return (
    <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={2} strokeLinecap="round" strokeLinejoin="round" aria-hidden>
      <path d="M4 14h6m0 0v6m0-6L3 21m17-7h-6m0 0v6m0-6l7 7M14 10V4m0 6h6m-6 0l7-7M10 10V4m0 6H4m6 0L3 3" />
    </svg>
  );
}

interface IESToolbarProps {
  autoRotate: boolean;
  isFullscreen?: boolean;
  showInfo: boolean;
  onToggleInfo: () => void;
  onToggleRotate: () => void;
  onResetCamera: () => void;
  onToggleFullscreen?: () => void;
}

function IESToolbar({
  autoRotate,
  isFullscreen = false,
  showInfo,
  onToggleInfo,
  onToggleRotate,
  onResetCamera,
  onToggleFullscreen,
}: IESToolbarProps) {
  return (
    <div className={styles.toolbar} role="toolbar" aria-label="IES Light Viewer Controls">
      <button
        type="button"
        className={clsx(styles.toolbarBtn, autoRotate && styles.toolbarBtnActive)}
        onClick={onToggleRotate}
        aria-label="Toggle auto-rotation"
        aria-pressed={autoRotate}
        title="Toggle auto-rotation"
      >
        <IconRefresh size={13} />
      </button>

      <button
        type="button"
        className={clsx(styles.toolbarBtn, showInfo && styles.toolbarBtnActive)}
        onClick={onToggleInfo}
        aria-label="Toggle photometric information"
        aria-pressed={showInfo}
        title="Photometric information"
      >
        <IconInfo size={13} />
      </button>

      <button
        type="button"
        className={styles.toolbarBtn}
        onClick={onResetCamera}
        aria-label="Reset camera and target"
        title="Reset camera and target"
      >
        <IconCameraReset size={13} />
      </button>

      {onToggleFullscreen && (
        <button
          type="button"
          className={styles.toolbarBtn}
          onClick={onToggleFullscreen}
          aria-label={isFullscreen ? 'Exit fullscreen' : 'Enter fullscreen'}
          title={isFullscreen ? 'Exit fullscreen' : 'Fullscreen'}
        >
          {isFullscreen ? <IconMinimize size={13} /> : <IconMaximize size={13} />}
        </button>
      )}
    </div>
  );
}

const LIGHT_POS = new THREE.Vector3(0, 4.2, 0);

function IESPhotometricScene({ texture }: { texture: THREE.Texture }) {
  const { scene } = useThree();
  const spotLightRef = useRef<THREE.SpotLight>(null);
  const rigRef = useRef<THREE.Group>(null);

  useEffect(() => {
    const light = spotLightRef.current;
    if (!light) return;
    const gobo = buildIesGoboTexture(texture as THREE.DataTexture);
    light.map = gobo;
    light.target.position.set(0, 0, 0);
    scene.add(light.target);
    light.target.updateMatrixWorld();
    return () => {
      scene.remove(light.target);
      light.map = null;
      gobo.dispose();
    };
  }, [texture, scene]);

  useFrame(({ clock }) => {
    const light = spotLightRef.current;
    if (!light) return;
    const t = clock.getElapsedTime();
    // Subtle circular / pendulum sweep around nadir so photometric lobes shimmer smoothly
    const swayX = Math.sin(t * 0.7) * 0.85;
    const swayZ = Math.cos(t * 0.5) * 0.55;
    light.target.position.set(swayX, 0, swayZ);
    light.target.updateMatrixWorld();
    if (rigRef.current) {
      rigRef.current.lookAt(swayX, 0, swayZ);
    }
  });

  return (
    <>
      <color attach="background" args={['#0d0f13']} />
      <fog attach="fog" args={['#0d0f13', 14, 34]} />

      <ambientLight intensity={0.22} color="#ffffff" />

      <spotLight
        ref={spotLightRef}
        color="#FFFFFF"
        intensity={220}
        position={[LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z]}
        angle={IES_SPOT_ANGLE}
        penumbra={0.3}
        distance={28}
        decay={2}
        castShadow
        shadow-mapSize={[4096, 4096]}
        shadow-bias={-0.00005}
        shadow-normalBias={0.02}
        shadow-radius={3}
      />

      {/* Luminaire / Emitter housing at the light position */}
      <group position={[LIGHT_POS.x, LIGHT_POS.y, LIGHT_POS.z]}>
        {/* Suspension stem */}
        <mesh position={[0, 0.55, 0]}>
          <cylinderGeometry args={[0.025, 0.025, 1.1, 16]} />
          <meshStandardMaterial color="#2a2e37" roughness={0.5} metalness={0.7} />
        </mesh>
        {/* Aimable fixture head */}
        <group ref={rigRef}>
          <mesh rotation={[Math.PI / 2, 0, 0]}>
            <cylinderGeometry args={[0.22, 0.28, 0.36, 32]} />
            <meshStandardMaterial color="#1f232b" roughness={0.4} metalness={0.65} />
          </mesh>
          {/* Glowing emitter aperture */}
          <mesh position={[0, 0, 0.181]}>
            <circleGeometry args={[0.2, 32]} />
            <meshBasicMaterial color="#ffffff" toneMapped={false} />
          </mesh>
        </group>
      </group>

      {/* Receiving ground plane */}
      <mesh rotation={[-Math.PI * 0.5, 0, 0]} position={[0, 0, 0]} receiveShadow>
        <planeGeometry args={[60, 60]} />
        <meshStandardMaterial color="#565b66" roughness={0.72} metalness={0.05} dithering />
      </mesh>

      {/* Curved backdrop / rear receiver wall */}
      <mesh position={[0, 4.5, -3.6]} receiveShadow>
        <planeGeometry args={[28, 9]} />
        <meshStandardMaterial color="#484d57" roughness={0.78} metalness={0.04} dithering />
      </mesh>

      {/* Subtle reference pedestal on the floor */}
      <mesh position={[0, 0.45, -0.8]} castShadow receiveShadow>
        <cylinderGeometry args={[0.42, 0.48, 0.9, 32]} />
        <meshStandardMaterial color="#6e7380" roughness={0.48} metalness={0.08} />
      </mesh>
    </>
  );
}

const CCT_KEYWORDS = ['COLOR', 'CCT', 'KELVIN', 'TEMP'];

function formatKelvinText(raw: string): string {
  const trimmed = raw.trim();
  return /^\d+$/.test(trimmed) ? `${trimmed}K` : trimmed;
}

function isColorTempKey(key: string): boolean {
  const upper = key.toUpperCase();
  return CCT_KEYWORDS.some((kw) => upper.includes(kw));
}

function findLampSetColorTemp(doc: PhotometricDocument): string | null {
  const match = doc.lampSets.find((ls) => Boolean(ls.colorTemperature?.trim()));
  return match ? formatKelvinText(match.colorTemperature) : null;
}

function findMetadataColorTemp(metadata?: Record<string, string>): string | null {
  if (!metadata) return null;
  const entry = Object.entries(metadata).find(([k, v]) => isColorTempKey(k) && Boolean(v.trim()));
  return entry ? formatKelvinText(entry[1]) : null;
}

function extractColorTemp(doc: PhotometricDocument): string | null {
  return findLampSetColorTemp(doc) ?? findMetadataColorTemp(doc.profile.metadata);
}

type IesSummary = ReturnType<typeof summarizeIES>;

function resolveProfileDisplayName(summary: IesSummary, fallbackName: string): string {
  const candidates = [summary.luminaire, summary.lampCatalog, summary.manufacturer];
  const found = candidates.find((c) => Boolean(c?.trim()));
  return found ? found.trim() : fallbackName;
}

function resolvePeakCandela(summary: IesSummary): number | null {
  if (summary.peakIntensityCandela != null) return summary.peakIntensityCandela;
  return summary.peakCandela > 0 ? summary.peakCandela : null;
}

function resolveLumens(summary: IesSummary): number | null {
  const direct = summary.declaredLampFluxLumens ?? summary.integratedFluxLumens;
  if (direct != null) return direct;
  if (summary.lumensPerLamp <= 0) return null;
  return summary.lumensPerLamp * Math.max(1, summary.lampCount);
}

function buildSafeCurves(
  doc: PhotometricDocument,
  fallbackMax: number
): { curves: IntensityCurve[]; maxCurveIntensity: number } {
  try {
    const built = buildIntensityCurves(doc.profile, { principalPlanes: true, samples: 91 });
    return {
      curves: built.curves,
      maxCurveIntensity: built.maximum > 0 ? built.maximum : fallbackMax,
    };
  } catch {
    return { curves: [], maxCurveIntensity: fallbackMax };
  }
}

interface IesSourceSpec {
  filePath: string;
  ext: string;
}

function extractProfileInfo(doc: PhotometricDocument, spec: IesSourceSpec): IesProfileInfo {
  const summary = summarizeIES(doc.profile);
  const baseName = spec.filePath.split(/[/\\]/).pop()?.replace(/\.[^.]+$/, '') || 'Photometric Profile';
  const { curves, maxCurveIntensity } = buildSafeCurves(doc, summary.peakCandela || 1);

  return {
    name: resolveProfileDisplayName(summary, baseName),
    sourceFormat: (doc.sourceFormat || spec.ext || 'ies').toUpperCase(),
    peakCandela: resolvePeakCandela(summary),
    lumens: resolveLumens(summary),
    watts: summary.inputWatts > 0 ? summary.inputWatts : null,
    colorTemperature: extractColorTemp(doc),
    curves,
    maxCurveIntensity,
  };
}

export async function resolveIesSourceUrl(filePath: string, ext: string): Promise<string> {
  if (ext !== 'ldt') {
    return convertFileSrc(filePath);
  }
  const text = await fetch(convertFileSrc(filePath)).then((r) => r.text());
  const doc = parsePhotometry(text, 'ldt');
  const { value } = serializeIES(doc);
  return URL.createObjectURL(new Blob([value], { type: 'text/plain' }));
}

export async function loadIesProfileAndTexture(
  spec: IesSourceSpec
): Promise<{ texture: THREE.DataTexture; info: IesProfileInfo }> {
  const rawText = await fetch(convertFileSrc(spec.filePath)).then((r) => {
    if (!r.ok) throw new Error(`HTTP ${r.status}`);
    return r.text();
  });
  const format = spec.ext === 'ldt' ? 'ldt' : 'ies';
  const doc = parsePhotometry(rawText, format);
  const iesText = format === 'ldt' ? serializeIES(doc).value : rawText;
  const rawTexture = new IESLoader().parse(iesText);
  const texture = convertIesTextureToGrayscaleRgba(rawTexture);
  const info = extractProfileInfo(doc, spec);
  return { texture, info };
}

function useIesTexture(spec: IesSourceSpec) {
  const { filePath, ext: cleanExt } = spec;
  const [texture, setTexture] = useState<THREE.Texture | null>(null);
  const [profileInfo, setProfileInfo] = useState<IesProfileInfo | null>(null);
  const [errorMsg, setErrorMsg] = useState<string | null>(null);

  useEffect(() => {
    let isCancelled = false;
    setTexture(null);
    setProfileInfo(null);
    setErrorMsg(null);

    const failMsg = cleanExt === 'ldt'
      ? 'Unable to parse LDT photometric data'
      : 'Unable to parse IES photometric data';

    loadIesProfileAndTexture({ filePath, ext: cleanExt })
      .then(({ texture: tex, info }) => {
        if (!isCancelled) {
          setTexture(tex);
          setProfileInfo(info);
        }
      })
      .catch(() => {
        if (!isCancelled) setErrorMsg(failMsg);
      });

    return () => {
      isCancelled = true;
    };
  }, [filePath, cleanExt]);

  return { texture, profileInfo, errorMsg };
}

function buildPolarCurveSvgPath(
  curve: IntensityCurve,
  center: { cx: number; cy: number; maxR: number; denom: number }
): string {
  if (!curve.points.length) return '';
  const { cx, cy, maxR, denom } = center;
  const segments = curve.points.map((pt, idx) => {
    const rad = (pt.angle * Math.PI) / 180;
    const r = Math.min(1, Math.max(0, pt.intensity / denom)) * maxR;
    const x = cx + r * Math.sin(rad);
    const y = cy + r * Math.cos(rad);
    return `${idx === 0 ? 'M' : 'L'}${x.toFixed(1)},${y.toFixed(1)}`;
  });
  return `${segments.join(' ')} Z`;
}

function CandelaPolarPlot({
  curves,
  maxIntensity,
}: {
  curves: IntensityCurve[];
  maxIntensity: number;
}) {
  const size = 96;
  const cx = size / 2;
  const cy = size / 2;
  const maxR = size * 0.41;
  const denom = maxIntensity > 0 ? maxIntensity : 1;
  const plotGeom = { cx, cy, maxR, denom };

  const c0 = curves[0];
  const c90 = curves[1];

  return (
    <svg
      width={size}
      height={size}
      viewBox={`0 0 ${size} ${size}`}
      aria-label="Candela polar distribution curve"
      style={{ display: 'block', margin: '2px auto 0', overflow: 'visible' }}
    >
      <circle cx={cx} cy={cy} r={maxR * 0.33} fill="none" stroke="rgba(255,255,255,0.1)" strokeWidth={0.75} />
      <circle cx={cx} cy={cy} r={maxR * 0.66} fill="none" stroke="rgba(255,255,255,0.1)" strokeWidth={0.75} />
      <circle cx={cx} cy={cy} r={maxR} fill="none" stroke="rgba(255,255,255,0.16)" strokeWidth={0.85} />
      <line x1={cx - maxR} y1={cy} x2={cx + maxR} y2={cy} stroke="rgba(255,255,255,0.12)" strokeWidth={0.75} />
      <line x1={cx} y1={cy - maxR} x2={cx} y2={cy + maxR} stroke="rgba(255,255,255,0.12)" strokeWidth={0.75} />

      {c90 && (
        <path
          d={buildPolarCurveSvgPath(c90, plotGeom)}
          fill="rgba(56, 189, 248, 0.14)"
          stroke="#38bdf8"
          strokeWidth={1.25}
          strokeDasharray="2.5 1.5"
        />
      )}
      {c0 && (
        <path
          d={buildPolarCurveSvgPath(c0, plotGeom)}
          fill="rgba(250, 204, 21, 0.18)"
          stroke="#facc15"
          strokeWidth={1.5}
        />
      )}
      <circle cx={cx} cy={cy} r={1.6} fill="#ffffff" />
    </svg>
  );
}

function InfoMetricRow({ label, value }: { label: string; value: string | null | undefined }) {
  if (!value) return null;
  return (
    <div className={styles.infoRow}>
      <span className={styles.infoLabel}>{label}</span>
      <span className={styles.infoValue}>{value}</span>
    </div>
  );
}

function formatMetricWithUnit(val: number | null | undefined, unit: string): string | null {
  if (val == null) return null;
  return `${Math.round(val).toLocaleString()} ${unit}`;
}

function buildEmptyProfileInfo(fallbackExt: string): IesProfileInfo {
  const upper = fallbackExt.toUpperCase();
  return {
    name: `${upper} Photometric`,
    sourceFormat: upper,
    peakCandela: null,
    lumens: null,
    watts: null,
    colorTemperature: null,
    curves: [],
    maxCurveIntensity: 1,
  };
}

function PhotometricInfoPanel({
  profileInfo,
  fallbackExt,
}: {
  profileInfo: IesProfileInfo | null;
  fallbackExt: string;
}) {
  const info = profileInfo ?? buildEmptyProfileInfo(fallbackExt);

  return (
    <div
      className={styles.infoPanel}
      role="region"
      aria-label="Photometric Profile Info"
      style={{ maxWidth: 'min(240px, calc(100% - 24px))', maxHeight: 'calc(100% - 24px)', overflowY: 'auto' }}
    >
      <div className={styles.infoRow}>
        <span className={styles.infoLabel}>Profile</span>
        <span
          className={styles.infoValue}
          style={{ maxWidth: 150, overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}
          title={info.name}
        >
          {info.name}
        </span>
      </div>
      <InfoMetricRow label="Source" value={info.sourceFormat} />
      <InfoMetricRow label="Intensity" value={formatMetricWithUnit(info.peakCandela, 'cd')} />
      <InfoMetricRow label="Flux" value={formatMetricWithUnit(info.lumens, 'lm')} />
      <InfoMetricRow label="Color Temp" value={info.colorTemperature} />
      {info.curves.length > 0 && (
        <div style={{ marginTop: 4, paddingTop: 4, borderTop: '1px solid rgba(255,255,255,0.08)' }}>
          <div className={styles.infoRow} style={{ marginBottom: 2 }}>
            <span className={styles.infoLabel}>Candela Curve</span>
            <span className={styles.infoLabel} style={{ fontSize: 10 }}>C0 / C90</span>
          </div>
          <CandelaPolarPlot
            curves={info.curves}
            maxIntensity={info.maxCurveIntensity}
          />
        </div>
      )}
    </div>
  );
}

export interface IESLightViewerProps {
  filePath: string;
  extension?: string;
  isFullscreen?: boolean;
  onToggleFullscreen?: () => void;
}

export function IESLightViewer({
  filePath,
  extension,
  isFullscreen,
  onToggleFullscreen,
}: IESLightViewerProps) {
  const [autoRotate, setAutoRotate] = useState(false);
  const [showInfo, setShowInfo] = useState(Boolean(isFullscreen));
  useEffect(() => setShowInfo(Boolean(isFullscreen)), [isFullscreen]);
  const controlsRef = useRef<OrbitHandle | null>(null);
  const cleanExt = (extension || filePath.split('.').pop() || '').toLowerCase();
  const { texture, profileInfo, errorMsg } = useIesTexture({ filePath, ext: cleanExt });

  const handleResetCamera = useCallback(() => {
    if (controlsRef.current) {
      controlsRef.current.target.set(0, 1.5, 0);
      controlsRef.current.reset();
      controlsRef.current.update();
    }
  }, []);

  if (errorMsg) {
    return (
      <div className={styles.fallback} style={{ flexDirection: 'column', gap: 8 }}>
        <IconSun size={36} style={{ color: 'hsl(45 95% 50%)' }} />
        <span style={{ fontSize: '12px' }}>{errorMsg}</span>
        <span style={{ fontSize: '11px', color: 'var(--color-text-muted)' }}>
          .{cleanExt.toUpperCase()}
        </span>
      </div>
    );
  }

  return (
    <div className={styles.viewerContainer}>
      <IESToolbar
        autoRotate={autoRotate}
        isFullscreen={isFullscreen}
        showInfo={showInfo}
        onToggleInfo={() => setShowInfo((v) => !v)}
        onToggleRotate={() => setAutoRotate((v) => !v)}
        onResetCamera={handleResetCamera}
        onToggleFullscreen={onToggleFullscreen}
      />

      {showInfo && <PhotometricInfoPanel profileInfo={profileInfo} fallbackExt={cleanExt} />}

      <ThreeErrorBoundary
        key={filePath}
        extension={cleanExt}
      >
        <Canvas
          shadows="soft"
          dpr={Math.min(Math.max(window.devicePixelRatio || 1, 2), 3)}
          camera={{ position: [5.5, 3.2, 6.8], fov: 40, near: 0.1, far: 100 }}
          gl={{
            toneMapping: THREE.ACESFilmicToneMapping,
            toneMappingExposure: 1.15,
            outputColorSpace: THREE.SRGBColorSpace,
            antialias: true,
            powerPreference: 'high-performance',
          }}
        >
          <Suspense fallback={null}>
            {texture && <IESPhotometricScene texture={texture} />}
            <OrbitControls
              ref={controlsRef as never}
              enableDamping
              enablePan={false}
              minDistance={2}
              maxDistance={30}
              autoRotate={autoRotate}
              autoRotateSpeed={0.5}
              target={[0, 1.5, 0]}
              makeDefault
            />
          </Suspense>
        </Canvas>
      </ThreeErrorBoundary>
    </div>
  );
}
