# Antigravity hand-off: Library Folder Explorer — FRONTEND slice

You are the frontend engineer for the PolyStash desktop app (React 18 + TypeScript + Vite +
Zustand). Own the slice below end to end. Do NOT touch anything under
`apps/desktop/src-tauri/` (Rust backend).

## Repo & branch

- Working dir: `/workspaces/PolyStash` (already on branch `feat/folder-explorer-sidebar`)
- Read the authoritative contract first: `docs/contracts/folder-explorer.md` — sections
  "Immutable contracts" and "Frontend — Antigravity" are binding.
- Project knowledge bank: http://127.0.0.1:8888/mcp/PolyStash/ (hindsight MCP is bound
  to it). Recall before you start; retain decisions/lessons at the end.

## The library

`@headless-tree/react` v1.8.0 (`useTree`) + `@headless-tree/core` (features, data loaders).
Docs: https://headless-tree.lukasbach.com/getstarted and
https://headless-tree.lukasbach.com/features/async-dataloader — read them before coding.
Minimal shape, already verified by the orchestrator:

```tsx
const tree = useTree<string>({
  rootItemId: 'root',
  getItemName: (item) => item.getItemData(),
  isItemFolder: () => true,
  dataLoader: {
    getItem: (id) => id,
    getChildren: async (id) => fetchChildren(id),   // asyncDataLoaderFeature
  },
  features: [asyncDataLoaderFeature, selectionFeature, hotkeysCoreFeature],
});
// <div {...tree.getContainerProps()}>{tree.getItems().map(i =>
//   <button {...i.getProps()} key={i.getId()} style={{paddingLeft: `${i.getItemMeta().level * 14}px`}}>…</button>)}</div>
```

## Your scope (exact files)

1. `apps/desktop/package.json` — add `"@headless-tree/core": "^1.8.0"` and
   `"@headless-tree/react": "^1.8.0"` to `dependencies`. Do NOT run pnpm/npm install and
   do NOT edit `pnpm-lock.yaml` — the orchestrator installs.

2. `apps/desktop/src/types/index.ts` — add `directoryPrefix?: string` to `FilterSpec`.

3. `apps/desktop/src/api/index.ts` — add:
   - `listLibraryDirectories: (libraryId: string) => invoke<string[]>('list_library_directories', { libraryId })`
   - `listSubdirectories: (libraryId: string, parent: string) => invoke<string[]>('list_subdirectories', { libraryId, parent })`
   (Command names are snake_case, argument keys camelCase — that is how Tauri v2 maps them.
   Match the existing entries in that file exactly.)

4. `apps/desktop/src/store/index.ts` — folder slice in `useUIStore`:
   `activeFolder: ActiveFolder | null` (`{ libraryId: string | null; relativePath: string }`),
   `setActiveFolder`, and `folderTreeVersion: number` + `refreshFolderTree()` used as a React
   `key` on the tree so it re-mounts after index updates. Persist nothing new.
   Also wire `activeFolder` into the asset query: the resulting `directoryPrefix` is
   `undefined` for All Libraries, `''` for a library root, the relative path otherwise.
   Find where the sidebar already calls `setQuery`/`search()` and follow that pattern —
   put the translation in ONE exported helper next to the sentinel parser.

5. `apps/desktop/src/components/Sidebar/FolderTree.tsx` (NEW) and
   `apps/desktop/src/components/Sidebar/FolderTree.module.css` (NEW).

6. `apps/desktop/src/components/Sidebar/Sidebar.tsx` — add a new "Folders"
   `SidebarSection` (icon `IconFolder`) directly **above** the Collections section,
   rendering `<FolderTree />`. Keep `expanded` state in the existing `expanded` object
   (add a `folders: true` key). Remove nothing from Libraries/Collections/Tags.

## Node id sentinel grammar (share it, do not duplicate)

- `'root'` — All Libraries (no folder filter)
- `'lib:<libraryId>'` — a library root
- `'dir:<libraryId>:<relativePath>'` — a folder inside a library

Export `buildFolderId` / `parseFolderId` from ONE module (put them in
`FolderTree.tsx` and import from the store, or in the store and import from the tree —
one direction only, no cycle). `parseFolderId` must handle a relative path containing `:`.

`getItemName` for `lib:` nodes is the library's display name (read `useLibraryStore`).

## Hard rules

- No new global CSS. Style with `FolderTree.module.css` using the existing design tokens
  (`--color-bg-hover`, `--color-brand-500`, `--color-text-*`, `--color-border`, …) and the
  existing `.truncate` utility. Windows-Explorer feel: 22px rows, chevron on folders that
  have children, folder icon, name truncation, hover + selected states.
- Do NOT change `AssetGrid`, `Toolbar`, `DetailPanel`, or any existing behaviour.
- No dead code, no unused imports/props, no `any` without a documented reason:
  `tsc -b` and `eslint . --max-warnings 0` must pass. Do NOT run them — CI does.
- Do NOT run git or gh commands. Do not commit.

## Suggested tools

- Skills: `antislop`, `antislop-ui`, `antislop-code` (mandatory reading for UI/text/code),
  `code-simplification`.
- MCPs: `context7` (confirm the headless-tree / React API before using it),
  `hindsight` (project bank), `chrome-devtools` (only if you must inspect a live page).

## Acceptance criteria

- Folders section shows one row per library; expanding fetches one level lazily and shows
  only directories that exist inside the library index — never files.
- Clicking a folder filters the grid to that folder and everything below it; clicking the
  library row filters to the library root; "All Libraries" clears the folder filter.
- A library removed/added in the Libraries section appears/disappears in the tree after
  `library:updated`.
- Loading state visible while children resolve (`item.isLoading()`).
