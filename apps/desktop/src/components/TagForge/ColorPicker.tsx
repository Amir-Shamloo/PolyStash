import { useEffect, useRef, useState } from 'react';
import clsx from 'clsx';
import { normalizeHexColor, palettes, toCssColor } from './tagForgeData';
import styles from './InspectorPanel.module.css';

interface ColorPickerProps {
  selectedId: string | null;
  selectedColor: string | null;
  disabled: boolean;
  onApplyColor: (color: string | null) => void;
}

function sameColor(a: string | null, b: string): boolean {
  return a !== null && a.toLowerCase() === b.toLowerCase();
}

function PaletteTabs({
  activePaletteId,
  onSelect,
}: {
  activePaletteId: string | undefined;
  onSelect: (id: string) => void;
}) {
  return (
    <div className={styles.pills} role="tablist" aria-label="Palettes">
      {palettes.palettes.map((p) => {
        const selected = p.id === activePaletteId;
        return (
          <button
            key={p.id}
            type="button"
            role="tab"
            aria-selected={selected}
            className={clsx(styles.pill, selected && styles.pillActive)}
            onClick={() => onSelect(p.id)}
          >
            {p.name}
          </button>
        );
      })}
    </div>
  );
}

function ColorSwatchGrid({
  colors,
  selectedColor,
  disabled,
  onApplyColor,
}: {
  colors: string[];
  selectedColor: string | null;
  disabled: boolean;
  onApplyColor: (color: string | null) => void;
}) {
  return (
    <div className={styles.colorGrid}>
      {colors.map((c, i) => (
        <button
          key={`${c}-${i}`}
          type="button"
          disabled={disabled}
          title={c}
          aria-label={c}
          className={clsx(styles.colorCell, sameColor(selectedColor, c) && styles.active)}
          style={{ background: toCssColor(c) }}
          onClick={() => onApplyColor(c)}
        />
      ))}
    </div>
  );
}

function HexColorRow({
  hex,
  hexInvalid,
  disabled,
  onHexChange,
  onSubmitHex,
  onResetDefault,
}: {
  hex: string;
  hexInvalid: boolean;
  disabled: boolean;
  onHexChange: (value: string) => void;
  onSubmitHex: () => void;
  onResetDefault: () => void;
}) {
  return (
    <div className={styles.hexRow}>
      <input
        className={clsx(styles.input, hexInvalid && styles.inputInvalid)}
        type="text"
        placeholder="#RRGGBB"
        value={hex}
        disabled={disabled}
        aria-invalid={hexInvalid}
        onChange={(e) => onHexChange(e.target.value)}
        onKeyDown={(e) => {
          if (e.key === 'Enter') onSubmitHex();
        }}
        onBlur={onSubmitHex}
      />
      <button
        type="button"
        className={styles.defaultBtn}
        disabled={disabled}
        onClick={onResetDefault}
      >
        Tag (Default)
      </button>
    </div>
  );
}

export function ColorPicker({
  selectedId,
  selectedColor,
  disabled,
  onApplyColor,
}: ColorPickerProps) {
  const [paletteId, setPaletteId] = useState(palettes.palettes[0]?.id ?? '');
  const [hex, setHex] = useState('');
  const lastAppliedRef = useRef<string | null>(null);

  useEffect(() => {
    setHex(selectedColor ?? '');
    lastAppliedRef.current = null;
  }, [selectedId]);

  const palette = palettes.palettes.find((p) => p.id === paletteId) ?? palettes.palettes[0];
  const normalizedHex = normalizeHexColor(hex);
  const hexInvalid = hex.trim() !== '' && normalizedHex === null;

  const submitHex = () => {
    if (disabled || normalizedHex === null) return;
    if (normalizedHex === lastAppliedRef.current) return;
    lastAppliedRef.current = normalizedHex;
    onApplyColor(normalizedHex);
  };

  const handleHexChange = (value: string) => {
    lastAppliedRef.current = null;
    setHex(value);
  };

  return (
    <div className={clsx(styles.section, styles.colorsSection)}>
      <HexColorRow
        hex={hex}
        hexInvalid={hexInvalid}
        disabled={disabled}
        onHexChange={handleHexChange}
        onSubmitHex={submitHex}
        onResetDefault={() => onApplyColor(palettes.defaultColor)}
      />
      {hexInvalid && <span className={styles.hint}>Enter a hex colour like #3A7BD5</span>}
      <PaletteTabs activePaletteId={palette?.id} onSelect={setPaletteId} />
      <ColorSwatchGrid
        colors={palette?.colors ?? []}
        selectedColor={selectedColor}
        disabled={disabled}
        onApplyColor={onApplyColor}
      />
    </div>
  );
}
