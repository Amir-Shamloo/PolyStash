# Contract: Unified Preview Pipeline, Studio 3D Viewer & UI Polish (v0.0.3)

Status: active · Base: `main` @ `7700621` · Orchestrator: Hermes (sole git/gh owner)

Supersedes the extension tables in `preview-rust-migration.md` where they disagree.
That document's §Immutable contracts (job lifecycle, events, columns, thumb naming) still bind.

## Goal

Close the gaps left by the Rust migration: CAD/BIM/archive/video/document thumbnails,
frontend 3D fallback snapshots, an interactive studio viewer, a fully automatic pipeline
with no manual button, multi-thumbnail carousels, and the folder-chevron bug fix.

## Immutable contracts (both agents depend on these — do not change)

1. `preview_jobs` lifecycle: `queued|pending → processing → done|error`.
2. Tauri events, byte-compatible:
   - `preview:done` `{ assetId: string, thumbnailPath?: string /* absolute, resolve_path'd */, thumbnailPaths?: string[] /* absolute, ordered; absent for single-thumb */, previewModel?: string }`
   - `preview:error` `{ assetId: string, jobId: string }`
3. `assets.thumbnail_path` stays the **single** library-relative primary thumb
   (`.polystash/thumbs/<stem>_thumb.png|jpg`). **Unchanged column semantics.**
4. **New:** `assets.metadata.$["polystash.thumbs"]` = JSON array of library-relative paths,
   ordered, index 0 == `thumbnail_path`. Written by the worker when >1 variant exists.
   Frontend reads it for the carousel; a missing/`[]` value means "single thumbnail".
5. `THUMB_SIZE = 512`. Variant naming: `<stem>_thumb[_<n>].png|jpg` (n = 1..N-1), plus the
   unchanged `<stem>_thumb.png|jpg` for n == 0.
6. Existing-thumbnail fast path, startup crash-recovery reset, batch-of-10 polling,
   1s/1.5s idle sleeps — all remain.
7. `preview_status` is a closed set: `none|pending|generating|done|error`. **No asset may
   remain `pending`/`generating` forever**: every path terminates in `done` (with a
   placeholder when extraction is impossible) or `error`.

## File ownership (strict — violations are defects)

- **OpenCode:** `apps/desktop/src-tauri/**` only.
- **Antigravity:** `apps/desktop/src/**` only.
- **Hermes:** git/gh, `docs/**`, `apps/desktop/package.json` + `pnpm-lock.yaml` install. Agents NEVER run git.

## Tier A — Rust backend (`apps/desktop/src-tauri/src/preview/`)

Add modules (naming: descriptive, no `image.rs` — it shadows the `image` crate):

| Module | Extensions | Algorithm |
|---|---|---|
| `ole.rs` (extend) | `rvt` `rfa` `rte` | keep `RevitPreview4.0` PNG scan; add `\x05SummaryInformation` DIB fallback (reuse the `.max` path) then placeholder. |
| `archive.rs` (extend) | `skp` | keep; add header PNG scan of the first 1 MB when the zip path fails. |
| `archive.rs` (new fn) | `sbsar` | `zip` crate → first entry whose name ends `preview.png` (case-insensitive), else any `*.png` entry; decode → save. Fallback: placeholder (frontend shader ball takes over). |
| `cad.rs` (new) | `dwg` `dxf` | `dwg`: retarget the existing sentinel/PNG scan to the **first 8 MB** and also accept `BM`/DIB candidates. `dxf`: ASCII/UTF-8 header scan of the first 4 MB for `\x89PNG` or a base64 `<thumbnail>` payload → decode. Fallback: placeholder. |
| `hdr.rs` (new) or extend `raster.rs` | `hdr` `exr` `hdri` | keep the existing Reinhard tone-map for `exr`/`hdr`; route `hdri` to the same fn. |
| `cineware.rs` (new) | `c4d` `blend` | embedded-preview byte scan: `c4d` — locate `\x89PNG` (Cinema 4D stores a preview near the `FORM`/`MC4D` header) within the first 4 MB and decode; `blend` — locate `\x89PNG` within the first 4 MB (Blender's thumbnail block) and decode. No hit → **placeholder** (never error). Document in a comment that this is a byte-scan heuristic, not a parser. |
| `raster.rs` (extend) | `tif` `tiff` `avif` | confirm `tif`/`tiff` decode; add `avif` only if the `image` crate build already decodes it — otherwise leave `avif` to the generic placeholder and note it in the report. |
| `video.rs` (extend) | `mp4` `mov` `avi` `mkv` `webm` `m4v` | keep the `ffmpeg` CLI path; **ffmpeg missing OR non-zero exit → placeholder + `done`**, never `error`. |
| `document.rs` (new) | `pdf` | decide in this order: (a) if a permissively-licensed pure-Rust PDF rasterizer can be added to `Cargo.toml` without a native toolchain, use it; (b) otherwise probe for an external renderer on PATH (`pdftoppm`, `mutool`) via `tokio::process::Command` exactly like `video.rs`; (c) otherwise placeholder + `done`. Report which branch you took and why. |
| `mod.rs` (multi-thumb) | all | extraction fns may return `Vec<PathBuf>`; save variant `n` as `<stem>_thumb_<n>` and write `metadata.$["polystash.thumbs"]`. Return the primary as today. |

**Also required:**
- `MESH_EXTENSIONS` in `preview/mod.rs` gains `ply`, `usdz`, `x3d`, `wrl` (frontend renders these; no backend thumb).
- Every new handler: no `unwrap()`/`panic!` on file data — degrade to the next handler or a
  placeholder with `tracing::warn!`.
- `Cargo.toml` additions must be needed; each new dep gets `version = "X"` as the file already
  styles it. Do NOT touch `pnpm-lock.yaml`, `.github/**`, or any frontend file.

## Tier B — Frontend (`apps/desktop/src`)

1. **Remove the manual preview button flow.** In `components/DetailPanel/DetailPanel.tsx` delete
   `handleGeneratePreview`, the `IconRefresh` regenerate button, and the `isPending`/`isError`
   button branches. The `previewStatus` branch becomes: thumbnail → image; else model →
   `ThreeViewer`; else the kind-coloured format icon (existing `thumbPlaceholder` styling).
   Keep `api.requestPreview` in `api/index.ts` — it is still used by the backend-triggered paths.
2. **Offscreen 3D snapshot fallback (Category B).** New `lib/previewSnapshot.ts`:
   `captureModelSnapshot(url, extension): Promise<Blob|null>` → a **single-frame** offscreen
   `WebGLRenderer` render of the loaded model (`scene.updateMatrixWorld`, `renderer.render`,
   `toBlob`, dispose everything in a `finally`). Used for `.blend` when no backend thumbnail
   exists, and for `.sbsar` (a PBR shader ball: sphere + `MeshStandardMaterial` under an
   `Environment` preset). Wire it as: card/detail shows the kind-coloured icon until the
   snapshot resolves, then swaps in the blob URL. Persist nothing to disk.
3. **`ThreeViewer.tsx` → studio scene.** Keep the existing error boundary and loader dispatch.
   Replace `PresentationControls` with the studio assembly:
   - `<Stage>` for auto-framing + `<Environment preset={env}>` for reflections.
   - `<ContactShadows>` under the model.
   - `<OrbitControls enableDamping />`.
   - A toolbar with exactly four controls: HDRI environment on/off (and preset switch),
     wireframe toggle, auto-rotate toggle, reset camera. Wire wireframe by traversing the
     loaded scene and setting `material.wireframe`; reset by re-framing the controls target.
4. **Carousel (grid card + detail + QuickLook).** Read `metadata['polystash.thumbs']` (array of
   **library-relative** paths → resolve each with the same helper the app already uses for
   `thumbnailPath`). New `components/AssetGrid/ThumbnailCarousel.tsx`: shows index 0, with
   pagination dots and hover arrows; no dots when there is only one. Use it in `AssetCard.tsx`,
   `DetailPanel.tsx`, and `QuickLookModal.tsx`.
5. **Sidebar chevron bug fix.** `components/Sidebar/FolderTree.tsx`: a folder with no
   subdirectories currently renders the same chevron slot as one with children. Fix so the
   chevron/arrow is rendered **only** when the item can actually expand — use the headless-tree
   item's child information (e.g. `item.isExpandable()` / `getChildren().length`), not a
   post-expansion `isEmptyFolder` heuristic. Collapsed folders that have children keep their
   chevron.
6. **New non-3D viewer branches stay as they are** (raster → `<img>`, video → `<video>`,
   document → text/icon). Do not route them into the 3D canvas.

## Acceptance criteria

1. `cargo clippy --all-targets -- -D warnings` passes (CI runs it on Windows; reason about it
   statically — **do NOT run cargo locally**, this VM has no GTK/WebKit deps).
2. `tsc -b` and `eslint . --max-warnings 0` pass (CI runs them; do NOT run them locally).
3. No asset can stay `pending`/`generating`: unsupported/corrupt input → placeholder + `done`.
4. No manual "Generate preview" affordance remains anywhere in the UI.
5. Multi-thumb assets expose `metadata['polystash.thumbs']` and the carousel navigates them.
6. A folder with no subdirectories renders no chevron; one with children still does.
7. CodeScene: no code-health degradation for touched files. Project target: **10.0**.

## Not-Doing

- No headless 3D rendering in Rust; no backend mesh conversion.
- No Cineware/Melange and no Substance Engine DLL integration (proprietary / platform-bound) —
  `.c4d` uses the byte-scan → frontend viewer, `.sbsar` uses the frontend shader ball.
- No `.max` multi-camera multi-thumbnail harvest (single thumbnail stays).
- No new formats beyond those listed above.
