# Antigravity hand-off: Unified Preview Pipeline — FRONTEND slice

You are the frontend engineer for the PolyStash desktop app (React 18 + TypeScript + Vite +
Zustand + Three.js). Own the slice below end to end. Do NOT touch anything under
`apps/desktop/src-tauri/` (Rust backend).

## Repo & branch

- Working dir: `/workspaces/PolyStash` (already on branch `feat/preview-pipeline-v2`)
- Read the authoritative contract first: `docs/contracts/preview-pipeline-v2.md` —
  sections "Immutable contracts" (items 2 and 4) and "Tier B" are binding.
- Project knowledge bank: MCP `http://127.0.0.1:8888/mcp/PolyStash/`. Recall before you
  start; retain decisions/lessons at the end.

## Your scope (exact files)

1. `apps/desktop/src/components/DetailPanel/DetailPanel.tsx`
   - **Delete the manual preview flow**: `handleGeneratePreview`, the `IconRefresh`
     regenerate button, and the `isPending`/`isError` button branches.
   - The preview area becomes a pure branch: thumbnail → `<img>`; else model → `ThreeViewer`;
     else the kind-coloured format icon using the existing `thumbPlaceholder` styling.
   - Keep every other behaviour (rename, delete, tags, collections, rating) untouched.

2. `apps/desktop/src/lib/previewSnapshot.ts` (NEW)
   - `captureModelSnapshot(url: string, extension: string): Promise<Blob | null>` — a
     **single-frame** offscreen render: `WebGLRenderer` with `preserveDrawingBuffer`, load the
     model with the same loader dispatch as `ThreeViewer`, `scene.updateMatrixWorld(true)`,
     `renderer.render`, `toBlob`, then dispose renderer/geometry/materials in a `finally`.
     Return `null` on any failure — never throw.

3. `apps/desktop/src/components/DetailPanel/ThreeViewer.tsx`
   - Keep `ThreeErrorBoundary`, `getExtension`, `MESH_EXTENSIONS`, `getModelPath`, and the
     loader hooks/`Model` dispatch as-is.
   - **Add `ply`, `usdz`, `x3d`, `wrl`** to `MESH_EXTENSIONS`.
   - Replace `PresentationControls` with the studio assembly: `<Stage>`, `<Environment preset>`,
     `<ContactShadows>`, `<OrbitControls enableDamping />`, all from `@react-three/drei`
     (verify exports via context7 before using them).
   - Add a toolbar with exactly four controls: HDRI environment on/off + preset switch,
     wireframe toggle (traverse the loaded scene setting `material.wireframe`), auto-rotate
     toggle, reset camera (re-frame the controls target). Keep it unobtrusive and keyboard
     reachable; every control gets an accessible label.
   - For `.blend` / `.sbsar` when no backend thumbnail exists, use `captureModelSnapshot`
     (`.sbsar` renders a PBR shader ball: sphere + `MeshStandardMaterial` under an
     `Environment` preset). Do NOT persist the blob to disk.

4. `apps/desktop/src/components/AssetGrid/ThumbnailCarousel.tsx` (NEW) + module CSS
   - Reads `asset.metadata['polystash.thumbs']` (array of **library-relative** paths) and
     resolves each with the same helper the app already uses for `thumbnailPath`.
   - Shows index 0 with pagination dots and hover arrows; **renders nothing extra when there is
     only one thumbnail**. Keyboard navigable; dots and arrows carry accessible labels.

5. `apps/desktop/src/components/AssetGrid/AssetCard.tsx` — use `ThumbnailCarousel`.
6. `apps/desktop/src/components/QuickLook/QuickLookModal.tsx` — use `ThumbnailCarousel`.
7. `apps/desktop/src/components/Sidebar/FolderTree.tsx` — **chevron bug fix**: a folder with no
   subdirectories currently renders the same chevron slot as one with children. Render the
   chevron **only** when the item can actually expand — use the headless-tree item's child
   information (`item.isExpandable()` / `getChildren().length`), not the post-expansion
   `isEmptyFolder` heuristic. Collapsed folders that have children keep their chevron.
8. `apps/desktop/src/components/AssetGrid/AssetListRow.tsx` — same chevron/thumbnail consistency
   as the card if it renders a preview; keep it minimal.

## Hard rules

- No new global CSS. Style with module CSS using the existing design tokens
  (`--color-bg-hover`, `--color-brand-500`, `--color-text-*`, `--color-border`, ...) and the
  existing `.truncate` utility.
- **No asset may appear stuck in `generating`/`pending`**: the UI must always resolve to a
  thumbnail, a placeholder icon, or a viewer.
- No dead code, no unused imports/props, no `any` without a documented reason: `tsc -b` and
  `eslint . --max-warnings 0` must pass. Do NOT run them locally — CI does.
- Do NOT change `App.tsx` event wiring, store actions, `api/index.ts`, or the layout shell.
- Do NOT run git or gh commands. Do not commit.

## Suggested tools

- Skills: `antislop`, `antislop-ui`, `antislop-code`, `antislop-human` (mandatory for UI/text/
  code), `code-simplification`, `performance-optimization`.
- MCPs: `context7` (confirm every `@react-three/drei` / Three.js API before using it),
  `hindsight` (project bank), `chrome-devtools` (only if you must inspect a live page),
  `codescene` (`pre_commit_code_health_safeguard` before reporting done),
  `sequential-thinking` for the viewer state machine.

## Acceptance criteria

- No "Generate preview" / retry affordance remains anywhere in the UI.
- The studio viewer auto-frames models of any scale, has grounded contact shadows, smooth
  damped orbit, and the four toolbar toggles all work.
- A multi-thumbnail asset shows working dots/arrows in the card, detail panel and QuickLook;
  a single-thumbnail asset shows none.
- A folder with no subdirectories renders no chevron; one with children still does, collapsed
  or expanded.
- Images, videos and documents still use their own viewers — never the 3D canvas.
- CodeScene: no code-health degradation for touched files. Project target: **10.0**.

## Report back

Changed files with a one-line description each; any new npm dependency and why; anything in
the contract you deviated from and why.
