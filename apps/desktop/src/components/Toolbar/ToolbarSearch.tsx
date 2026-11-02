import { useCallback } from 'react';
import { useAssetStore } from '../../store';
import { IconSearch } from '../Icons';
import styles from './Toolbar.module.css';
import clsx from 'clsx';

export function ToolbarSearch() {
  const { query, setQuery, search, loading } = useAssetStore();

  const handleSearch = useCallback(
    (e: React.ChangeEvent<HTMLInputElement>) => {
      setQuery({ text: e.target.value });
      search();
    },
    [setQuery, search]
  );

  const isSubActive = query.includeSubcollections !== false;

  return (
    <div className={styles.searchContainer}>
      <label
        className={clsx(
          styles.subcollectionsCheckboxLabel,
          isSubActive && styles.subcollectionsCheckboxActive
        )}
        title={
          isSubActive
            ? 'Filtering includes child collections (Click to filter selected collection only)'
            : 'Filtering only includes selected collection (Click to include children too)'
        }
      >
        <input
          id="toggle-include-subcollections"
          type="checkbox"
          className={styles.subcollectionsCheckboxInput}
          checked={isSubActive}
          onChange={(e) => {
            setQuery({ includeSubcollections: e.target.checked });
            search();
          }}
        />
        <span className={styles.subcollectionsCheckboxCustom}>
          {isSubActive && (
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth={3} strokeLinecap="round" strokeLinejoin="round">
              <polyline points="20 6 9 17 4 12" />
            </svg>
          )}
        </span>
        <span className={styles.subcollectionsCheckboxText}>Include subcollections</span>
      </label>

      <div className={styles.searchWrap}>
        <IconSearch className={styles.searchIcon} />
        <input
          id="toolbar-search"
          type="search"
          placeholder="Search assets, tags, metadata…"
          className={styles.searchInput}
          value={query.text}
          onChange={handleSearch}
          aria-label="Search assets"
        />
        {loading && <div className={clsx(styles.spinner, 'animate-spin')} />}
      </div>
    </div>
  );
}
