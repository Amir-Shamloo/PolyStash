# Contract: Native Preview Engine (Python → Rust) + Frontend 3D

Status: active · Base: main @ 0fcc9a7 · Orchestrator: Hermes (sole git/gh owner)

## Goal

Remove the Python runtime dependency and the Blender plugin entirely. Tier A: all
thumbnail/preview extraction runs natively in the Tauri Rust backend. Tier B: 3D mesh
formats render live in the React frontend via Three.js. No headless 3D renderer in Rust.

## Immutable contracts (frontend Zustand depends on these — do not change)

1. `preview_jobs` lifecycle: `queued|pending → processing → done|error`.
   Watcher inserts `pending`; `request_preview` inserts `queued`; worker resets orphaned
   `processing` → `queued` once at startup.
2. Tauri events (payloads byte-compatible):
   - `preview:done` `{ assetId: string, thumbnailPath?: string /* absolute, resolve_path'd */, previewModel?: string }`
   - `preview:error` `{ assetId: string, jobId: string }`
3. `assets` columns: `preview_status ∈ none|pending|generating|done|error`;
   `thumbnail_path` library-relative (e.g. `.polystash/thumbs/<stem>_thumb.png`);
   `metadata` JSON `$.preview_model` stays readable (legacy GLB conversions).
4. Thumbnail convention: `<libroot>/.polystash/thumbs/<file_stem>_thumb.png|jpg`, `THUMB_SIZE = 512`.
5. Existing-thumbnail fast path and startup crash-recovery reset remain.

## Tier A — Rust backend (`apps/desktop/src-tauri`)

Module layout under `src/preview/` (replace single-file `preview.rs`):

| Module | Extensions | Algorithm (port of `packages/preview-engine/worker.py` — read it BEFORE deleting) |
|---|---|---|
| `mod.rs` | all | `start_preview_worker` loop: poll jobs, dispatch by extension, status transitions, events. Refactor the 325-line loop body into small helpers (per CodeScene). |
| `image.rs` | jpg jpeg png bmp gif tif tiff tga webp | `image` crate, Triangle filter resize to `THUMB_SIZE`. webp/png → `.png` out, rest → `.jpg`. |
| `image.rs` (hdr) | exr hdr | `image` crate with features `exr`, `hdr` added. Decode → clamp ≥0 → Reinhard `x/(x+1)` → gamma `1/2.2` → u8. If max dim > 2048, strided downsample first (`step = max_dim/2048`). Output `_thumb.jpg`. |
| `ole.rs` | max | `cfb` crate. Stream `\x05SummaryInformation`, property 17 (thumbnail): find DIB start `28 00 00 00`, build `BM` header (bfOffBits = 14 + 40 + palette), decode via `image`. If max dim < 600, upscale ×(1024/max). Output `_thumb.png`. |
| `ole.rs` | rvt rfa | `cfb` crate. Stream `RevitPreview4.0`: find `\x89PNG` magic, decode from there. Graceful: no stream / no PNG → placeholder. |
| `archive.rs` | skp | `zip` crate. Try entries `meta/model_thumbnail.png`, `meta/preview_thumbnail.png`, `thumbnails/thumbnail.png`, else any entry containing `thumb` ending `.png`/`.jpg`. Classic (non-zip) SKP: scan first 1 MB for `\x89PNG\r\n\x1a\n` and decode from there. Output `_thumb.png`. |
| `archive.rs` | dwg | Read first 8 MB. Sentinel `1F 25 6D 07 D4 36 28 28 9D 57 CA 3F 9D 44 10 2B`; after it parse image directory: `total_len:u32, num_images:u8`, then per image `(type_code:u8, offset:u32, len:u32)`. Decode candidate bytes: PNG magic / `BM` / BITMAPINFOHEADER (`biSize==40` → wrap DIB in `BM` header). Fallback: first PNG magic within first 4 MB. Output `_thumb.png`. |
| `material.rs` | vrmat | In priority order: (1) companion image in same dir (`<stem>`, `<stem>_preview`, `<stem>_thumb` × jpg jpeg png webp); (2) image in `Previews|previews|Preview|preview|Renders|renders` dirs (parent or parent-parent), stem-name match preferred; (3) `<preview>…</preview>` element → base64-decode → image; (4) texture refs from XML `<parameter>` (name/label contains texture/color/file/bitmap) whose `<value>` ends in an image ext — search parent, `maps/`, `textures/` (and parent-parent). Score: +60 diff/alb/base/color/col/tex in filename, +50 diffuse/albedo/color/base param, −100 norm/nrm, −80 bump, −70 gloss/rough, −60 mask/alpha/disp/height, −50 ao/ambient, −40 refl/spec; pick best. (5) Fallback swatch: first `<parameter name="color|diffuse|diffuse_color|base_color">` `<r><g><b>` floats → draw radial-shaded sphere on 512×512 bg `(24,26,32)`, radius `0.38*size`, light offset `-0.35r`, color blend `c*0.45 + (255-c*0.45)*frac^2.2` from rim to center. Output `_thumb.jpg`. Crates: `roxmltree`, `base64`. |
| `psd.rs` | psd psb | Flattened composite extraction via `psd` crate if its API yields the composite bitmap; otherwise graceful placeholder. Verify crate capability via context7/docs.rs before committing to it. |
| `psd.rs` | ai | Deferred: graceful placeholder (PDF rendering out of scope). |
| `video.rs` | mp4 mov mkv avi webm m4v mpg mpeg wmv flv | `tokio::process::Command` → system `ffmpeg -ss 00:00:01 -i src -frames:v 1 -vf scale=720:720:force_original_aspect_ratio=decrease -y out.jpg`. ffmpeg absent → placeholder + done. ffmpeg ran but failed → error. |
| `mod.rs` (mesh) | gltf glb fbx obj stl dae | No backend thumbnail. Job → `done`, `preview_status → done`, emit `preview:done` **without** `thumbnailPath`; frontend renders the source live (Tier B). |
| `mod.rs` (deferred) | ai 3dm 3ds ply + unknown | Neutral placeholder tile: 512×512 RGB `(24,26,32)`; job → `done`, `thumbnail_path` set. |
| `placeholder.rs` | — | Shared placeholder generator used by deferred formats + missing-ffmpeg. |

Hard rules: never `unwrap()`/`panic!` on file data — corrupt/unsupported input degrades to
the next handler or placeholder with a `tracing::warn!`. Keep `THUMB_SIZE`, thumb naming,
existing-thumb fast path, batch-of-10 polling, and 1s/1.5s idle sleeps.

Deletions (same agent): `packages/preview-engine/**`, `plugins/blender/**`, the `python:`
job block in `.github/workflows/ci.yml` (lines ~70–80), `.gitignore` line
`packages/preview-engine/venv/`.

Cargo additions: `cfb`, `zip`, `roxmltree`, `base64`; `image` features += `exr`, `hdr`.
Verify exact APIs via context7 / docs.rs. Do NOT touch `pnpm-lock.yaml`.

## Tier B — Frontend (`apps/desktop/src`)

- `ThreeViewer.tsx`: extension-dispatched loaders rendering the **source** file:
  `.gltf/.glb` → `useGLTF`, `.fbx` → `useFBX`, `.obj` → `useOBJ`, `.stl` → `useSTL`,
  `.dae` → `useCollada` (all from `@react-three/drei` — verify exports via context7).
  Legacy fallback: `metadata.preview_model` or `.glb` `thumbnailPath` (old conversions).
- `QuickLookModal.tsx` + `DetailPanel.tsx`: `modelPath := legacy preview_model ||
  (extension ∈ {gltf,glb,fbx,obj,stl,dae} ? asset.filePath : .glb thumbnailPath)`.
  Image/video branches unchanged.
- Update UI copy that references the "preview engine worker" (DetailPanel ~line 693).
- Do NOT touch: `App.tsx` event wiring, store actions, `package.json` (drei already
  installed), grid-card rendering.

## File ownership (strict — violations are defects)

- OpenCode: `apps/desktop/src-tauri/**`, `packages/preview-engine` (delete),
  `plugins/blender` (delete), `.github/workflows/ci.yml`, `.gitignore`,
  `apps/desktop/src-tauri/Cargo.toml`.
- Antigravity: `apps/desktop/src/**` only.
- Hermes: git/gh (branches, commits, PR, merge) + verification. Agents NEVER run git.

## Acceptance criteria

1. No live references to `preview-engine`, `PythonWorker`, or python packaging remain;
   `packages/preview-engine/` and `plugins/blender/` absent.
2. CI green: frontend (`tsc -b && vite build`, eslint) + rust (`cargo clippy
   --all-targets -- -D warnings`) + DeepScan on the PR head.
3. Corrupt/missing/unsupported files never panic; every path terminates in done or error.
4. Events, schema, statuses, and thumbnail naming match §Immutable contracts exactly.
5. CodeScene: no code-health degradation vs base (safeguard before commits,
   `analyze_change_set` for the PR). AGENTS.md target: Code Health 10.0.

## Not-Doing

- No headless/software 3D rendering in Rust; no backend mesh conversion.
- No offscreen WebGL thumbnail capture in grid cards.
- No deep parsers for `.3dm`/`.3ds`/`.ply` (placeholder).
- No new features beyond the tables above; no dependency additions beyond those listed.
