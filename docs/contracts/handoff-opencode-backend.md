# OpenCode hand-off: Library Folder Explorer — BACKEND slice

You are the backend engineer for the PolyStash Tauri v2 desktop app (Rust + SQLx/SQLite).
Own the slice below end to end. Do NOT touch any file under `apps/desktop/src/` (frontend).

## Repo & branch

- Working dir: `/workspaces/PolyStash` (already on branch `feat/folder-explorer-sidebar`)
- Read the authoritative contract first: `docs/contracts/folder-explorer.md` — it is the
  spec. Sections "Immutable contracts" and "Backend — OpenCode" are binding.
- Ground your edits in Codebase Memory MCP project `workspaces-PolyStash`
  (`search_code` with `regex=true`, `get_code_snippet`, `get_file_outline`).
- Project knowledge bank: http://127.0.0.1:8888/mcp/PolyStash/ (hindsight MCP is bound
  to it). Recall before you start; retain decisions/lessons at the end.

## Your scope (exact files)

1. `apps/desktop/src-tauri/src/models.rs`
   - `FilterSpec` gains `directory_prefix: Option<String>` with `#[serde(rename = "directoryPrefix")]`.
   - Leave the existing `directory` field completely alone.

2. `apps/desktop/src-tauri/src/commands/asset.rs`
   - In the same function that already builds the `directory` condition, add the
     `directory_prefix` condition: matches `a.file_path = ?` OR `a.file_path LIKE ? ESCAPE '\'`
     with `<prefix>%` and `<prefix>/%`.
   - Do not change any other filter behaviour.

3. `apps/desktop/src-tauri/src/commands/directory.rs`  (NEW FILE)
   - `list_library_directories(library_id: String, state) -> Result<Vec<String>, String>`
   - `list_subdirectories(library_id: String, parent: String, state) -> Result<Vec<String>, String>`
     where `parent` is `"."` for the library root.
   - Path normalization helper: reject `..` segments and absolute paths, backslashes → `/`.
   - Both commands resolve the library through `Database::get_library_handle(Some(&id))`.

4. `apps/desktop/src-tauri/src/commands/mod.rs` — add the module.
5. `apps/desktop/src-tauri/src/lib.rs` — register both commands in `invoke_handler!`.

## Hard rules

- No `unwrap()`/`panic!` on user or file input. Signature style matches
  `commands/library.rs`: `Result<T> = std::result::Result<T, String>`, `.map_err(|e| e.to_string())`.
- No N+1 queries per directory — group at the SQL level.
- Directory rows come from `dir_mtimes`; an asset's directory is derived from
  `assets.file_path`. Return a directory only when a descendant asset exists.
- Skip hidden dirs (`.polystash`, any dot-dir), mirroring `watcher.rs`.
- Do NOT touch `Cargo.toml`, `pnpm-lock.yaml`, `.github/**`, or any frontend file.
- Do NOT run git or gh commands. Do not commit.

## Suggested tools

- Skills: `antislop-code` (code comment hygiene), `code-simplification`, `performance-optimization`.
- MCPs: `codebase-memory` (grounding), `hindsight` (project bank), `context7`
  (SQLx/Tauri API confirmation), `codescene`.

## Acceptance criteria

- `cargo clippy --all-targets -- -D warnings` passes (CI runs this on Windows; reason
  about it statically — do NOT run cargo on this machine, it has no GTK/WebKit deps).
- CodeScene `pre_commit_code_health_safeguard` reports no degradation for the files you touch.
- Paths returned are relative posix, sorted, deduplicated, roots included as `.`.
- A `../` or absolute input returns `Err`, never a full-library result.

## Report back

Changed files with a one-line description each, the exact SQL you added, and anything in
the contract you deviated from.
