import { useState, useRef, useEffect, useCallback } from 'react';
import clsx from 'clsx';
import styles from './SectionSettingsPopover.module.css';

interface SectionSettingsPopoverProps {
  title: string;
  toggleLabel: string;
  description: string;
  checked: boolean;
  onChange: (val: boolean) => void;
  idPrefix: string;
}

const POPOVER_WIDTH = 224;
const VIEWPORT_PADDING = 8;

function computePopoverPosition(btnEl: HTMLButtonElement | null): React.CSSProperties {
  if (!btnEl) return { right: 0 };
  const rect = btnEl.getBoundingClientRect();
  const style: React.CSSProperties = { right: 0, left: 'auto' };

  const expectedLeft = rect.right - POPOVER_WIDTH;
  if (expectedLeft < VIEWPORT_PADDING) {
    const shiftRight = VIEWPORT_PADDING - expectedLeft;
    style.right = -shiftRight;
  }

  const spaceBelow = window.innerHeight - rect.bottom;
  if (spaceBelow < 140 && rect.top > 140) {
    style.top = 'auto';
    style.bottom = 'calc(100% + 4px)';
  }

  return style;
}

export function SectionSettingsPopover({
  title,
  toggleLabel,
  description,
  checked,
  onChange,
  idPrefix,
}: SectionSettingsPopoverProps) {
  const [isOpen, setIsOpen] = useState(false);
  const [popoverStyle, setPopoverStyle] = useState<React.CSSProperties>({ right: 0 });
  const containerRef = useRef<HTMLDivElement>(null);
  const buttonRef = useRef<HTMLButtonElement>(null);

  const updatePosition = useCallback(() => {
    setPopoverStyle(computePopoverPosition(buttonRef.current));
  }, []);

  const handleToggle = useCallback(() => {
    setIsOpen((prev) => {
      const next = !prev;
      if (next) {
        setPopoverStyle(computePopoverPosition(buttonRef.current));
      }
      return next;
    });
  }, []);

  useEffect(() => {
    if (!isOpen) return;
    updatePosition();

    const handleOutsideClick = (e: MouseEvent) => {
      if (containerRef.current && !containerRef.current.contains(e.target as Node)) {
        setIsOpen(false);
      }
    };

    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.key === 'Escape') {
        setIsOpen(false);
      }
    };

    document.addEventListener('mousedown', handleOutsideClick);
    document.addEventListener('keydown', handleKeyDown);
    window.addEventListener('resize', updatePosition);
    return () => {
      document.removeEventListener('mousedown', handleOutsideClick);
      document.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('resize', updatePosition);
    };
  }, [isOpen, updatePosition]);

  return (
    <div className={styles.container} ref={containerRef}>
      <button
        ref={buttonRef}
        id={`${idPrefix}-settings-btn`}
        type="button"
        className={clsx(styles.gearBtn, isOpen && styles.gearBtnActive)}
        onClick={handleToggle}
        title={title}
        aria-label={title}
        aria-haspopup="dialog"
        aria-expanded={isOpen}
      >
        <svg
          width={13}
          height={13}
          viewBox="0 0 24 24"
          fill="none"
          stroke="currentColor"
          strokeWidth={2}
          strokeLinecap="round"
          strokeLinejoin="round"
          aria-hidden
        >
          <path d="M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.39a2 2 0 0 0-.73-2.73l-.15-.08a2 2 0 0 1-1-1.74v-.5a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z" />
          <circle cx="12" cy="12" r="3" />
        </svg>
      </button>

      {isOpen && (
        <div
          id={`${idPrefix}-settings-popover`}
          className={styles.popover}
          style={popoverStyle}
          role="dialog"
          aria-label={title}
        >
          <div className={styles.popoverHeader}>
            <span className={styles.popoverTitle}>{title}</span>
            <button
              type="button"
              className={styles.closeBtn}
              onClick={() => setIsOpen(false)}
              aria-label="Close"
            >
              ✕
            </button>
          </div>

          <div className={styles.popoverBody}>
            <label className={styles.toggleRow} htmlFor={`${idPrefix}-toggle-input`}>
              <div className={styles.textColumn}>
                <span className={styles.toggleLabel}>{toggleLabel}</span>
                <span className={styles.toggleDesc}>{description}</span>
              </div>

              <div className={styles.switchWrap}>
                <input
                  id={`${idPrefix}-toggle-input`}
                  type="checkbox"
                  className={styles.switchInput}
                  checked={checked}
                  onChange={(e) => onChange(e.target.checked)}
                />
                <span className={clsx(styles.switchSlider, checked && styles.switchChecked)}>
                  <span className={styles.switchKnob} />
                </span>
              </div>
            </label>
          </div>
        </div>
      )}
    </div>
  );
}
