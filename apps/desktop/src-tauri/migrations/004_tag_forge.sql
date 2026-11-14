-- Tag Forge migration: duplicate tag names are legitimate in a hierarchical
-- taxonomy, so the name UNIQUE constraint is dropped, and tags gain an icon.
-- The table is rebuilt because SQLite cannot drop a column constraint in place.
-- asset_tags keeps its FK to tags(id); ids are preserved, so links survive.
PRAGMA foreign_keys = OFF;
PRAGMA legacy_alter_table = ON;
ALTER TABLE tags RENAME TO tags_old;
CREATE TABLE tags (
  id         TEXT PRIMARY KEY,
  name       TEXT NOT NULL,
  color      TEXT,
  parent_id  TEXT REFERENCES tags(id) ON DELETE SET NULL,
  icon_id    TEXT
);
INSERT INTO tags (id, name, color, parent_id) SELECT id, name, color, parent_id FROM tags_old;
DROP TABLE tags_old;
PRAGMA legacy_alter_table = OFF;
PRAGMA foreign_key_check;
PRAGMA foreign_keys = ON;
