# Contract: Library Folder Explorer Sidebar (@headless-tree/react)

Status: active · Base: main @ 3f92735 · Orchestrator: Hermes (sole git/gh owner)
Domains: backend = OpenCode (`src-tauri`), frontend = Antigravity (`src`)

## Goal

Add a Windows-Explorer-style folder tree to the left sidebar that shows **only the
directories inside libraries the user added** — never arbitrary user-PC folders.
The tree is built on `@headless-tree/react` (`useTree`) with lazy child loading
driven by the existing per-library asset index (no direct filesystem walking).
Clicking a folder filters the asset grid to that folder and everything below it.

## Scope boundary (hard)

- The tree's roots are the user's libraries; its nodes are directories that already
  exist in the library's SQLite index. Paths are always **library-relative**
  (forward slashes) and are resolved to absolute paths only inside Rust.
- No new folder-picker, no OS-wide browsing, no creating/renaming/deleting folders.
- No drag-and-drop in this feature.

## Immutable contracts (frontend depends on these — do not change)

1. New `FilterSpec` field `directoryPrefix?: string` (Rust
   `directory_prefix: Option<String>`) — **stricter than the existing
   `directory`**: it matches `file_path = ? OR file_path LIKE ?` with
   `<prefix>%` / `<prefix>/%` escaped with `ESCAPE '\'`. `directory` stays
   untouched (its loose `%dir%` LIKE is existing behaviour — do not modify).
2. New Tauri command `list_library_directories` (payload key `libraryId`) →
   `string[]`, relative posix paths, `"."` = the library root itself.
3. New Tauri command `list_subdirectories` (`libraryId`, `parent` where parent is
   the library **root id sentinel** or a relative posix dir path) → `string[]`
   of child directory paths (relative), sorted.
4. Every directory path that arrives from the UI is normalized in Rust:
   reject `..` segments, reject absolute paths, backslashes → `/`.
   A rejected path is an `Err(String)`, never a silent whole-library result.
5. No new Tauri events. Existing `library:updated` covers index refreshes.

## Backend — OpenCode (`apps/desktop/src-tauri`)

Files to change (exact):

| File | Change |
|---|---|
| `src/models.rs` | add `pub directory_prefix: Option<String>` to `FilterSpec` + `#[serde(rename = "directoryPrefix")]` |
| `src/commands/asset.rs` | in the existing filter builder, after the `directory` block, add the `directory_prefix` condition |
| `src/commands/directory.rs` | **new file**: `list_library_directories`, `list_subdirectories`, path-normalization helper |
| `src/commands/mod.rs` | `pub mod directory;` (alphabetical) |
| `src/lib.rs` | register both commands in `invoke_handler` |

Implementation notes:

- `Database::get_library_handle(Some(library_id))` gives the `LibraryHandle`
  (`pool` + `root_path`). A missing handle is an `Err("library not found")`.
- Distinguish **directories from files** by querying the `dir_mtimes` table
  (primary key `dir_path`) for directory rows, and `assets.file_path` for the
  reverse check. `dir_mtimes` paths are absolute on disk; `assets.file_path` is
  library-relative. Normalize both to relative posix before grouping.
- Directory rows are returned **only if they contain a descendant asset**; that is
  what keeps the tree scoped to indexed library content.
- SQL must be one query per level where practical (no N+1 per directory).
- Reference: `watcher.rs` walker skips hidden dirs (`.polystash`, dot-dirs) —
  mirror that when emitting directory rows.
- Rust style: `sqlx::query_as`, `.map_err(|e| e.to_string())`, no `unwrap()`,
  `tracing::warn!` on skipped rows. Match the existing file structure.
- Cargo deps: none. Do not touch `Cargo.toml`, `pnpm-lock.yaml`, `.github/**`.

## Frontend — Antigravity (`apps/desktop/src`)

Files to change (exact):

| File | Change |
|---|---|
| `package.json` | add `@headless-tree/core` and `@headless-tree/react` (`^1.8.0`) to dependencies |
| `src/types/index.ts` | `directoryPrefix?: string` on `FilterSpec` |
| `src/api/index.ts` | `listLibraryDirectories`, `listSubdirectories` |
| `src/store/index.ts` | folder slice: `folderTreeVersion`, `activeFolder`, `setActiveFolder`, `refreshFolderTree` |
| `src/components/Sidebar/FolderTree.tsx` | **new**: the `useTree` tree |
| `src/components/Sidebar/FolderTree.module.css` | **new**: styles only |
| `src/components/Sidebar/Sidebar.tsx` | new "Folders" `SidebarSection` above Collections |

Implementation notes:

- Use `useTree<string>` with `asyncDataLoaderFeature`, `selectionFeature`,
  `hotkeysCoreFeature`, `rootItemId: 'root'`.
- Sentinels: `'root'` = whole library (shows all libraries, no root filter);
  `lib:<libraryId>` = a library root; `dir:<libraryId>:<relativePath>` = a folder.
  Keep the sentinel parsing in one exported helper so the store and the tree share it.
- `dataLoader.getChildren` for a `lib:` node calls `listLibraryDirectories`, for a
  `dir:` node calls `listSubdirectories`; `getItem` returns the node id itself;
  `getItemName` returns the last path segment (`'root'` → "All Libraries",
  `lib:` → library name).
- `isItemFolder: () => true` — a node with no children is a leaf directory.
- Selection: `item.isSelected()`; on `onClick`
  `setActiveFolder(parseFolderId(item.getId()))` then `search()`.
- `directoryPrefix` is `undefined` for root, `''` for a library root, the relative
  path for a folder.
- Refresh: bump the tree key (`refreshFolderTree()`) when `library:updated` fires
  or a library is added/removed; collapse state persists across refresh.
- Conform to the existing sidebar `.module.css` token vocabulary
  (`--color-bg-hover`, `--color-brand-500`, …) and the existing `.truncate` util.
  Follow the antislop skill (no generic AI-slop styling, no dead CSS).
- Remove nothing from Collections/Tags. Libraries section stays as-is.
- TypeScript strict + `eslint . --max-warnings 0` must pass: no unused imports,
  no `any` without cause, no dead code.

## Acceptance criteria

1. Sidebar shows a "Folders" section; expanding a library reveals only its
   directories, lazily, one level per fetch.
2. Selecting a folder filters the grid to that folder + subfolders; selecting the
   library root shows the whole library; "All Libraries" clears the folder filter.
3. Directories with no indexed assets never appear. Files never appear.
4. Removing a library removes its subtree; adding one adds it.
5. `cargo clippy --all-targets -- -D warnings` clean (Windows CI), `tsc -b`,
   `vite build`, `eslint --max-warnings 0` clean.
6. CodeScene: no Code Health degradation in touched files (target 10.0).
7. No `..` or absolute path from the UI reaches SQL.

## Out of scope

Folder create/rename/delete, drag-and-drop into folders, context menus, file
rows inside the tree, virtualization (libraries are curated asset folders, the
tree stays small; revisit only if a real library exceeds a few thousand folders).
