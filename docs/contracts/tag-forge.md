# Tag Forge — feature contract (v0.1.7)

Tag Forge is PolyStash's tag-library manager: a 3-column editor that lets the user
browse a curated predefined taxonomy, stage subtrees, edit the staged hierarchy
(rename / nest / reorder / delete / undo / redo), paint tags with icons and colors,
and commit the result as the workspace tag library.

The feature name is **Tag Forge**. No other product, brand, or feature name may
appear in code, data, comments, or UI copy.

## Source data (already vendored, do not modify)

`apps/desktop/src/data/tagForge/`

| Path | Shape |
|---|---|
| `templates.json` | `Array<{ name: string; granularityEntries: { Summary: Ref[]; Balanced: Ref[]; Comprehensive: Ref[] } }>` (10 templates) |
| `categories_index.json` | `Array<{ guid; category; granularity; name; filename; totalTags }>` (125) |
| `categories/*.json` | `{ category; granularity; rootGuid; rootName; totalTags; tree: PredefinedNode; flatTags: PredefinedTag[] }` |
| `icons.json` | `Record<string, { id; name; category; categoryName; description; viewBox; path; tags: string[] }>` (1656) |
| `icon_categories.json` | `Array<{ id; name; iconIds: string[] }>` (40) |
| `color_palettes.json` | `{ defaultColor: "#9D9D9D"; palettes: Array<{ id; name; colors: string[] }> }` |

`PredefinedTag` = `{ guid; name; path; parentGuid: string|null; isRoot; color: string;
colorApplyOnChildren: boolean; iconId: string|null; iconApplyOnChildren: boolean;
orderingPriority: number; aliases: string[] }`.

Data facts you can rely on:
- 3,712 predefined tags, **only 2,106 distinct names** — duplicate names across
  categories are normal and must be preserved.
- Every predefined tag has a `color`; 1,494 have an `iconId`.
- `tree` and `flatTags` carry the same nodes; use `tree` for rendering, `flatTags` for search.

### Data quirks you must handle (validated, do not "fix" the data)

1. **Granularity `Unknown`.** 7 categories (`Lighting and Atmospheric Conditions`,
   `Marketplaces`, `Material Properties`, `Project Phase (BIM)`, `Providers (BIM)`,
   `Spaces (Location)`, `Workflows`) only have a file whose `granularity` is
   `"Unknown"`. Build each granularity's forest from the entries matching that
   granularity **plus** every `Unknown` entry, so those categories are always
   reachable. Never render an "Unknown"Granularity pill.
2. **Template refs can dangle.** `templates.json` references root guids; 34 of them
   do not resolve to any node in the target granularity (and one ref has only a
   `guid` key). Resolve by `guid`; silently skip anything unresolved. Never crash or
   render an empty pill row for them.
3. **Colors are ARGB, not CSS.** 2,735 colors are `#AARRGGBB` (e.g. `#FFE8C5DC`) and
   862 are `#9D9D9D`. Convert 9-char ARGB to CSS `#RRGGBBAA` before using it as a CSS
   color; pass 7-char hex through unchanged. Compare colors case-insensitively.
4. **One unknown icon id.** `DefaultTagIconGeometry` is referenced by the data but is
   absent from `icons.json`; `TagIcon` must fall back to the default glyph for it.

## Backend — OpenCode owns these files

### 1. `apps/desktop/src-tauri/migrations/004_tag_forge.sql` (NEW)

Rebuild `tags` without the `name` UNIQUE constraint (duplicate names are legitimate
in a hierarchical taxonomy) and add `icon_id`. Exact body:

```sql
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
```

Do not wrap it in a transaction. `asset_tags` keeps its `ON DELETE CASCADE` FK to
`tags(id)`; the rebuild preserves ids, so assignments survive.

### 2. `apps/desktop/src-tauri/src/db.rs`

- Add `async fn apply_tag_forge(pool: &Pool<Sqlite>)` that executes
  `include_str!("../migrations/004_tag_forge.sql")` and ignores the result
  (idempotent startup, same pattern as `apply_smart_column_drop`).
- Call it immediately after `apply_smart_column_drop(&pool)` in
  `connect_library_sqlite`, and after `apply_smart_column_drop(&master_pool)` in
  `Database::new`.
- In `sync_library_metadata`, add `icon_id` to both tag sync directions:
  `SELECT id, name, color, parent_id, icon_id FROM tags` (tuple becomes
  `(String, String, Option<String>, Option<String>, Option<String>)`) and
  `INSERT OR IGNORE INTO tags (id, name, color, parent_id, icon_id) VALUES (?, ?, ?, ?, ?)`.

### 3. `apps/desktop/src-tauri/src/models.rs`

```rust
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Tag {
    pub id: String,
    pub name: String,
    pub color: Option<String>,
    #[serde(rename = "parentId")]
    pub parent_id: Option<String>,
    #[serde(rename = "iconId")]
    pub icon_id: Option<String>,
}

#[derive(Debug, Clone, Deserialize)]
pub struct TagDraft {
    pub id: String,
    pub name: String,
    #[serde(rename = "parentId")]
    pub parent_id: Option<String>,
    pub color: Option<String>,
    #[serde(rename = "iconId")]
    pub icon_id: Option<String>,
}
```

### 4. `apps/desktop/src-tauri/src/commands/tag.rs`

```rust
pub struct TagPatch {
    #[serde(rename = "parentId", default)]
    pub parent_id: Option<String>,
    #[serde(default)]
    pub color: Option<String>,
    #[serde(rename = "iconId", default)]
    pub icon_id: Option<String>,
}
```

`TagPatch` keeps both command signatures at or below 4 arguments (a Code Health gate).

Signatures (Tauri command style matches the existing file; `Result<T> = std::result::Result<T, String>`):

```rust
list_tags(state) -> Result<Vec<Tag>>                                  // SELECT id, name, color, parent_id, icon_id ORDER BY name ASC
create_tag(name: String, patch: TagPatch, state) -> Result<Tag>       // 3 args
update_tag(id: String, name: String, patch: TagPatch, state) -> Result<Tag>   // 4 args, NEW
delete_tag(id: String, state) -> Result<()>                           // unchanged behaviour
save_tag_tree(tags: Vec<TagDraft>, state) -> Result<Vec<Tag>>         // NEW
```
Wire format: `create_tag` → `{ name, patch }`, `update_tag` → `{ id, name, patch }`,
where `patch` is `{ parentId?, color?, iconId? }`.

`save_tag_tree` semantics (binding):
1. Validate: every `name` non-blank; no duplicate `id`; every `parentId` either null
   or present in the payload; the parent graph is acyclic (return
   `Err("tag hierarchy contains a cycle")`).
   **Duplicate names are allowed** — the predefined taxonomy legitimately reuses
   names across categories (3,712 tags, 2,106 distinct names), which is why the
   UNIQUE constraint is being dropped. Do not reject them.
2. On the active pool, in one transaction: `DELETE FROM tags WHERE id NOT IN
   (<payload ids>)`, then upsert every row (`INSERT ... ON CONFLICT(id) DO UPDATE
   SET name=excluded.name, color=excluded.color, parent_id=excluded.parent_id,
   icon_id=excluded.icon_id`).
3. Replay the same two steps on every pool from `state.db.get_all_libraries()`
   and on `state.db.master_pool`, each in its own transaction.
4. Return the freshly-read `list_tags()` result.

`create_tag` / `update_tag` keep the existing "replicate to all pools + master"
pattern, extended with `color` and `icon_id`.

### 5. `apps/desktop/src-tauri/src/commands/asset.rs`

`sync_asset_tags` (around line 685): extend the selected/inserted tuple with
`icon_id` so a tag copied from the master pool keeps its icon. No behaviour change.

### 6. `apps/desktop/src-tauri/src/lib.rs`

Register `commands::tag::update_tag` and `commands::tag::save_tag_tree` in
`invoke_handler!` next to the existing three tag commands.

### Backend hard rules

- No `unwrap()`/`expect()` on user input; `.map_err(|e| e.to_string())`.
- Do not touch any file under `apps/desktop/src/` (frontend owns those).
- Do not touch `Cargo.toml`, `pnpm-lock.yaml`, `.github/**`, or the data folder.
- Do not run git or `gh`. Do not run cargo (this VM has no GTK/WebKit toolchain);
  reason statically about clippy.

## Frontend — Antigravity owns these files

### Types & glue

`types/index.ts`:

```ts
export interface Tag {
  id: string;
  name: string;
  color?: string | null;   // hex string from the tag Forge palette, e.g. "#FFE8C5DC"
  parentId?: string | null;
  iconId?: string | null;
}
```

`ColorLabel` is unchanged and still used for **asset** color labels.

`api/index.ts`:

```ts
createTag: (name: string, patch?: TagPatch) =>
  invoke<Tag>('create_tag', { name, patch: patch ?? {} }),
updateTag: (id: string, name: string, patch?: TagPatch) =>
  invoke<Tag>('update_tag', { id, name, patch: patch ?? {} }),
saveTagTree: (tags: Tag[]) => invoke<Tag[]>('save_tag_tree', { tags }),
```

`types/index.ts` also gains `TagPatch { parentId?; color?; iconId? }`.

`store/index.ts` (tag store): add `updateTag(...)` and `saveTagTree(tags: Tag[])`
which call the API and replace `tags`/`tagMap` with the returned list.

`useUIStore`: add `tagForgeOpen: boolean` and `setTagForgeOpen(open: boolean)`.

`App.tsx`: render `<TagForgeModal />` (it self-gates on `tagForgeOpen`).
`components/Sidebar/Sidebar.tsx`: the Tags section header gets a small button
(title `Open Tag Forge`) that calls `setTagForgeOpen(true)`.

Color rendering: tags now carry hex. Replace the two `var(--swatch-${tag.color})`
usages (Sidebar tag dot, OrganizeTab `TagItem` dot) with a helper that returns the
hex when `color` starts with `#`, otherwise falls back to the existing
`var(--swatch-...)` mapping.

### New files under `apps/desktop/src/components/TagForge/`

| File | Responsibility |
|---|---|
| `tagForgeData.ts` | Loads the vendored JSON (static re-export; the modal dynamically imports this module so the ~9 MB payload stays out of the initial bundle). Exposes `templates`, `categoriesIndex`, `loadCategory(granularity, filename)`, `icons`, `iconCategories`, `palettes`, and the `PredefinedTag` / `PredefinedNode` types. |
| `tagForgeModel.ts` | Pure, framework-free tree logic (no React). Detailed API below. |
| `tagForgeModel.test.ts` | `node --test` self-check of the model (no new dependency). |
| `TagIcon.tsx` | Renders an `iconId` as an `<svg>` from `icons.json`; default tag glyph when null/unknown. |
| `PredefinedPanel.tsx` | Column 1. |
| `StagedPanel.tsx` | Column 2. |
| `InspectorPanel.tsx` | Column 3. |
| `TagForgeModal.tsx` | Shell: header, 3-column grid, footer, keyboard shortcuts, save/cancel. |
| `TagForge.module.css` | All Tag Forge styles, using `global.css` custom properties. |

### `tagForgeModel.ts` — binding API

```ts
export interface ForgeTag {
  id: string;
  name: string;
  color: string | null;
  iconId: string | null;
  parentId: string | null;
  children: ForgeTag[];
  sourceGuid?: string;   // set when staged from the predefined library
  isCustom?: boolean;
}

export function nestTags(flat: Tag[]): ForgeTag[];
export function flattenTree(roots: ForgeTag[]): ForgeTag[];          // depth-first, parentId set
export function findTag(roots: ForgeTag[], id: string): ForgeTag | null;
export function findParentId(roots: ForgeTag[], id: string): string | null;
export function stagePredefined(node: PredefinedNode, parentId?: string | null): ForgeTag;
export function createCustomTag(parentId?: string | null): ForgeTag;  // crypto.randomUUID(), name "New Tag"
export function renameTag(roots: ForgeTag[], id: string, name: string): ForgeTag[];
export function removeTag(roots: ForgeTag[], id: string): ForgeTag[]; // drops the whole subtree
export function insertTag(roots: ForgeTag[], parentId: string | null, node: ForgeTag, index?: number): ForgeTag[];
export function moveTag(roots: ForgeTag[], id: string, newParentId: string | null, index: number): ForgeTag[];
export function duplicateTag(roots: ForgeTag[], id: string): ForgeTag[];  // deep copy, fresh ids, inserted after source
export function applyIcon(roots: ForgeTag[], id: string, iconId: string | null, recursive: boolean): ForgeTag[];
export function applyColor(roots: ForgeTag[], id: string, color: string | null, recursive: boolean): ForgeTag[];
export function countTags(roots: ForgeTag[]): number;
export function collectIds(roots: ForgeTag[]): Set<string>;
```

All operations are immutable (return a new forest, never mutate input).
`moveTag` must refuse to drop a node into its own descendant (return input unchanged).
Staged predefined nodes keep `guid` as `id`; custom nodes get a fresh UUID.

### Panel props (binding — the modal is the only owner of state)

`PredefinedPanel`:
```ts
{ onStage: (node: PredefinedNode) => void; stagedIds: Set<string> }
```
Owns its own search text, template pill, granularity and expand state. The modal
decides the parent a staged node lands under.

`StagedPanel`:
```ts
{
  forest: ForgeTag[]; selectedId: string | null; autoRenameId?: string | null;
  onSelect: (id: string | null) => void;
  onRename: (id: string, name: string) => void;
  onRemove: (id: string) => void;
  onMove: (id: string, newParentId: string | null, index: number) => void;
  onAddRoot: () => void;
  onDuplicate: (id: string) => void;
  onUndo: () => void; onRedo: () => void; canUndo: boolean; canRedo: boolean;
}
```
Owns its own search filter and expand/collapse state. Inline rename is local to the
row; `autoRenameId` makes that row start editing (used after `+ Add Tag`).

`InspectorPanel`:
```ts
{
  selected: ForgeTag | null;
  applyToSubtags: boolean;
  onToggleApplyToSubtags: (v: boolean) => void;
  onApplyIcon: (iconId: string | null) => void;
  onApplyColor: (color: string | null) => void;
}
```
Owns its own icon search, category pill, palette tab and hex input state.

`TagForgeModal` owns everything else: it reads `tagForgeOpen` / `setTagForgeOpen`
from `useUIStore`, and takes an optional `onSaved?: () => void` callback. It holds the
forest, the history stacks, `selectedId`, `applyToSubtags`, and the save/cancel flow.

### Modal state and save flow (binding)

- **Open**: fetch `api.listTags()` once per opening, `nestTags()` the result into the
  initial forest, clear history, `selectedId = null`, `applyToSubtags = false`,
  record `baselineIds = collectIds(forest)`.
- **History**: `past: ForgeTag[][]`, `present: ForgeTag[]`, `future: ForgeTag[][]`.
  Every mutation goes through `commit(next)` which pushes `present` onto `past` and
  clears `future`. Cap `past` at 100. `undo`/`redo` move one entry; `canUndo` /
  `canRedo` are `past.length > 0` / `future.length > 0`.
- **Staging**: `onStage(node)` → no-op when `stagedIds.has(node.guid)`; otherwise
  `commit(insertTag(present, selectedId, stagePredefined(node, selectedId)))` and
  select the new node.
- **Add root**: `commit(insertTag(present, null, createCustomTag(null)))`, select it,
  and pass its id as `autoRenameId`.
- **Save**: `flat = flattenTree(present)`. If any `baselineIds` are missing, confirm
  first (`window.confirm` naming the count). Then `api.saveTagTree(flat)` →
  `useTagStore.saveTagTree(saved)` → `onSaved?.()` → close. Errors render inline in
  the footer and do not close the modal. `Save Tags` is disabled when
  `past.length === 0`.
- **Cancel / Escape / overlay click**: if `past.length > 0`, `window.confirm`
  about discarding; then close without saving.
- **Keyboard**: `Ctrl+Z` undo, `Ctrl+Y` and `Ctrl+Shift+Z` redo, globally while open
  and focus is not in an input/textarea/contenteditable.

### UI behaviour (binding)

**Column 1 — Predefined.** Search input (matches `name` and `aliases`,
case-insensitive), 10 template pills (single-select toggle), granularity
Summary/Balanced/Comprehensive, Expand All / Collapse All, tree with chevron +
`TagIcon` + color dot + name + a `→` stage button. Staging inserts the node and its
whole subtree at the end of the staged roots (or under the currently selected
staged tag when one is selected). Staged predefined ids are stable, so restaging an
already-staged tag is a no-op.

**Column 2 — Staged.** Header: Undo, Redo, Expand All, Collapse All, `+ Add Tag`,
search filter. Rows: chevron, icon, color dot, name; double-click or `F2` renames
inline; `Delete` removes the subtree; drag a row onto another to nest it, or
between rows to reorder. Footer: `Save Tags` (primary) and `Cancel`.

**Column 3 — Inspector.** `Apply to Subtags` checkbox, current-tag card (icon on
its color), icon search + 40 category pills + icon grid, hex input + `Tag
(Default)` reset, 8 palette tabs + swatch grid. Everything applies to the selected
staged tag immediately and is undoable.

**History.** A single snapshot stack: every mutation pushes the previous forest.
`Ctrl+Z` / `Ctrl+Y` (and `Ctrl+Shift+Z`) work while the modal is open and focus is
not in a text input. `canUndo` / `canRedo` drive the button states.

**Save.** `Save Tags` calls `api.saveTagTree(flattenTree(roots))`. When the commit
would delete tags, show a confirmation first (reuse `ConfirmDeleteDialog` or an
inline confirm) stating how many tags will be removed. On success: replace the tag
store, close the modal, toast `Tags saved`. `Cancel` closes without saving.

### Frontend hard rules

- No new npm dependency. No `pnpm install`, no lockfile change.
- Do not touch `apps/desktop/src-tauri/**` (backend owns it), `Cargo.toml`,
  `pnpm-lock.yaml`, `.github/**`, or `src/data/tagForge/**`.
- Do not run git or `gh`. Do not commit.
- Match existing conventions: CSS modules + `clsx`, `global.css` custom properties,
  Zustand stores, `api/index.ts` invoke wrappers.
- No product/brand names other than PolyStash and Tag Forge.
- Do not run `vite build` on this VM (CI builds). `tsc --noEmit` and `eslint` are fine.

## Acceptance criteria

- [ ] `pnpm --filter @polystash/desktop typecheck` passes.
- [ ] `pnpm --filter @polystash/desktop lint` passes with `--max-warnings 0`.
- [ ] `node --test apps/desktop/src/components/TagForge/` passes.
- [ ] `cargo clippy --all-targets -- -D warnings` passes in CI; `cargo test --all-targets` passes.
- [ ] Migration 004 is idempotent and preserves existing tag rows and `asset_tags` links.
- [ ] Duplicate tag names can coexist; a tag can be nested under a parent.
- [ ] Full flow: stage a predefined subtree → rename → recolor → re-icon →
  drag-reorder → undo → redo → save → the sidebar tag list reflects the result.
- [ ] CodeScene `pre_commit_code_health_safeguard` shows no degradation on touched files.

## IP note (informational, already raised with the user)

The vendored taxonomy/icon data was extracted from a third-party commercial product.
The decompiled specs, demo video, and reference screenshot were used as reference
only and are **not** part of the repo. Confirm the licensing of the icon set and
taxonomy before public distribution.
