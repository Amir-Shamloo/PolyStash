# OpenCode hand-off: Unified Preview Pipeline — BACKEND slice

You are the backend engineer for the PolyStash Tauri v2 desktop app (Rust + SQLx/SQLite).
Own the slice below end to end. Do NOT touch any file under `apps/desktop/src/` (frontend)
or `apps/desktop/package.json` / `pnpm-lock.yaml`.

## Repo & branch

- Working dir: `/workspaces/PolyStash` (already on branch `feat/preview-pipeline-v2`)
- Read the authoritative contract first: `docs/contracts/preview-pipeline-v2.md` —
  sections "Immutable contracts" (Tier A rows) and "Tier A" are binding.
- Ground your edits in Codebase Memory MCP project `workspaces-PolyStash`
  (`search_code` with `regex=true`, `get_code_snippet`, `get_file_outline`). Update the
  index after your edits.
- Project knowledge bank: MCP `http://127.0.0.1:8888/mcp/PolyStash/`. Recall before you
  start; retain decisions/lessons at the end.

## Your scope (exact files)

1. `apps/desktop/src-tauri/src/preview/mod.rs`
   - `MESH_EXTENSIONS` gains `ply`, `usdz`, `x3d`, `wrl`.
   - Multi-thumbnail: extraction fns may return `Vec<PathBuf>`. Save variant `n` as
     `<stem>_thumb_<n>.png|jpg`, return the primary (index 0) as today, and write
     `metadata.$["polystash.thumbs"]` as an ordered JSON array of **library-relative** paths.
     `assets.thumbnail_path` semantics are unchanged.
   - Dispatch the new extensions to the new modules.
2. `apps/desktop/src-tauri/src/preview/ole.rs` — `rvt`/`rfa`/`rte`: keep `RevitPreview4.0`;
   add the `\x05SummaryInformation` DIB fallback (reuse the `.max` path), then placeholder.
3. `apps/desktop/src-tauri/src/preview/archive.rs` — `skp`: keep the zip path; add the
   first-1 MB header PNG scan. New fn for `sbsar`: zip → first `*preview.png` entry,
   else any `*.png` entry, decode → save; fallback placeholder.
4. `apps/desktop/src-tauri/src/preview/cad.rs` (NEW) — `dwg`: first-8 MB sentinel/PNG scan
   plus `BM`/DIB acceptance; `dxf`: first-4 MB scan for `\x89PNG` or a base64 `<thumbnail>`
   payload. Fallback placeholder.
5. `apps/desktop/src-tauri/src/preview/cineware.rs` (NEW) — `c4d` and `blend`: first-4 MB
   `\x89PNG` byte scan → decode. **No hit → placeholder, never `error`.** Comment that this
   is a heuristic scan, not a format parser.
6. `apps/desktop/src-tauri/src/preview/document.rs` (NEW) — `pdf`. Decide in this order:
   (a) a permissively-licensed pure-Rust PDF rasterizer if one can be added without a native
   toolchain; (b) else probe PATH for `pdftoppm`/`mutool` via `tokio::process::Command`;
   (c) else placeholder + `done`. Report the branch you took.
7. `apps/desktop/src-tauri/src/preview/video.rs` — ffmpeg missing OR non-zero exit →
   **placeholder + `done`**, never `error`.
8. `apps/desktop/src-tauri/src/preview/raster.rs` — confirm `tif`/`tiff` decode; add `avif`
   only if the current `image` crate build already decodes it; otherwise leave `avif` on the
   generic placeholder and say so in your report. Route `hdri` to the existing HDR tone-map.
9. `apps/desktop/src-tauri/src/watcher.rs` — extend the `needs_preview` extension list to
   cover the new formats (`pdf`, `sbsar`, `c4d`, `blend`, `dwg`, `dxf`, `rte`, `ies`, `ldt`,
   `hdri`, `avif`) so they get queued automatically. Do not reorder or rename anything else.

## Hard rules

- No `unwrap()`/`panic!` on user or file input. Signature style matches `commands/library.rs`:
  `Result<T> = std::result::Result<T, String>`, `.map_err(|e| e.to_string())`.
- **Every path terminates in `done` (placeholder when extraction is impossible) or `error`.
  Nothing may stay `pending`/`generating`.**
- Reuse `preview::placeholder::generate_placeholder` and `preview::save_thumb`; do not write
  parallel helpers for the same job.
- New modules must not be named `image.rs` or shadow a crate name.
- Do NOT touch `pnpm-lock.yaml`, `.github/**`, `apps/desktop/src/**`, or any file outside
  `apps/desktop/src-tauri/`.
- Do NOT run git or gh commands. Do not commit. Do NOT run `cargo` locally (this VM has no
  GTK/WebKit deps) — reason about clippy statically.

## Suggested tools

- Skills: `code-simplification`, `performance-optimization` (both mandatory), plus
  `safeguarding-ai-generated-code`, `guiding-refactoring-with-code-health`,
  `antislop-code` (comment hygiene).
- MCPs: `codebase-memory` (grounding + re-index), `hindsight` (project bank), `context7`
  (confirm `zip`/`cfb`/`image`/`tokio::process` APIs), `codescene`
  (`pre_commit_code_health_safeguard` before you report done), `sequential-thinking` for the
  PDF branch decision.

## Acceptance criteria

- `cargo clippy --all-targets -- -D warnings` passes (reason statically).
- CodeScene `pre_commit_code_health_safeguard` reports no degradation for the files you touch.
- Corrupt/missing/unsupported input never panics and never leaves a job unfinished.
- Thumbnail naming, `THUMB_SIZE`, statuses, and event payloads match the contract exactly.
- Multi-thumb extraction writes `metadata.$["polystash.thumbs"]` ordered, index 0 == `thumbnail_path`.

## Report back

Changed files with a one-line description each; the exact new dependency (if any) and why;
which PDF branch you took; anything in the contract you deviated from and why.
