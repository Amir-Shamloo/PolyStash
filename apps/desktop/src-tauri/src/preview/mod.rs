mod archive;
mod cad;
mod cineware;
mod document;
mod embedded;
mod material;
mod ole;
mod psd;
mod raster;
mod video;

use anyhow::{anyhow, Result};
use image::DynamicImage;
use serde_json::{json, Value};
use std::path::{Path, PathBuf};
use std::sync::Arc;
use tauri::Emitter;
use crate::db::LibraryHandle;
use crate::AppState;
use crate::db::resolve_path;

pub const THUMB_SIZE: u32 = 512;

/// Formats rendered live in the frontend with three.js; the backend emits
/// no thumbnail for these and completes the job immediately.
///
/// `usd` is not here: raw USDA text has no headless preview, so it is a
/// Category D icon. `usdz` keeps its three.js `USDZLoader` preview.
const MESH_EXTENSIONS: [&str; 10] = [
    "gltf", "glb", "fbx", "obj", "stl", "dae", "ply", "usdz", "3ds", "wrl",
];

/// Containers that can genuinely hold several previews; a gallery is only
/// harvested for these, so single-image formats never pay for the extra scan.
const GALLERY_EXTENSIONS: [&str; 1] = ["skp"];

/// GPU-compressed texture formats the frontend renders with KTX2Loader and the
/// backend therefore skips. `dds` is *not* here: `image` decodes it natively.
const GPU_TEXTURE_EXTENSIONS: [&str; 2] = ["ktx", "ktx2"];

/// Formats the frontend shows as a plain kind icon: no backend thumbnail and
/// no live 3D preview, so they complete immediately.
const CATEGORY_D_EXTENSIONS: [&str; 6] = ["sbs", "mdl", "mat", "abc", "x3d", "usd"];

pub use raster::{generate_image_thumb, is_native_image};

/// Build a subprocess command that never opens a console window on Windows.
pub(crate) fn spawn_command(program: &str) -> tokio::process::Command {
    #[cfg(windows)]
    {
        // tokio's Command exposes `creation_flags` natively on Windows.
        let mut cmd = tokio::process::Command::new(program);
        cmd.creation_flags(0x0800_0000); // CREATE_NO_WINDOW
        cmd
    }
    #[cfg(not(windows))]
    {
        tokio::process::Command::new(program)
    }
}

/// Round a metadata float to the two decimals the frontend displays.
pub(crate) fn round_two(value: f64) -> f64 {
    (value * 100.0).round() / 100.0
}

/// Width/height ratio, absent when the height is zero.
pub(crate) fn aspect_ratio(width: f64, height: f64) -> Option<f64> {
    (height > 0.0).then(|| round_two(width / height))
}

/// Resize and write a decoded image to `output` as JPEG (when the output path
/// ends in .jpg/.jpeg) or PNG otherwise.
pub(crate) fn save_thumb(img: &DynamicImage, output: &Path) -> Result<()> {
    let thumb = img.resize(THUMB_SIZE, THUMB_SIZE, image::imageops::FilterType::Triangle);

    if let Some(parent) = output.parent() {
        let _ = std::fs::create_dir_all(parent);
    }

    let is_jpg = output
        .extension()
        .and_then(|s| s.to_str())
        .is_some_and(|s| s.eq_ignore_ascii_case("jpg") || s.eq_ignore_ascii_case("jpeg"));

    if is_jpg {
        thumb.to_rgb8().save_with_format(output, image::ImageFormat::Jpeg)?;
    } else {
        thumb.save(output)?;
    }
    Ok(())
}

/// Locate the first embedded PNG signature in a byte buffer.
pub(crate) fn find_png(bytes: &[u8]) -> Option<usize> {
    const SIG: [u8; 8] = [0x89, b'P', b'N', b'G', 0x0d, 0x0a, 0x1a, 0x0a];
    bytes.windows(8).position(|w| w == SIG)
}

/// Identifiers for one queued preview job.
struct JobRef<'a> {
    id: &'a str,
    asset_id: &'a str,
    file_path: &'a str,
}

/// Per-library context shared by every job processed against that library.
struct LibraryJobs<'a> {
    state: &'a AppState,
    pool: &'a sqlx::Pool<sqlx::Sqlite>,
    root_path: &'a Path,
    thumb_dir: &'a Path,
}

/// Absolute source path for a stored (possibly library-relative) asset path.
fn resolve_source(root_path: &Path, file_path: &str) -> PathBuf {
    if Path::new(file_path).is_absolute() {
        PathBuf::from(file_path)
    } else {
        root_path.join(file_path)
    }
}

/// Relative thumb path when a thumbnail already exists on disk.
fn existing_thumb(thumb_dir: &Path, file_stem: &str) -> Option<String> {
    if thumb_dir.join(format!("{}_thumb.png", file_stem)).exists() {
        Some(format!(".polystash/thumbs/{}_thumb.png", file_stem))
    } else if thumb_dir.join(format!("{}_thumb.jpg", file_stem)).exists() {
        Some(format!(".polystash/thumbs/{}_thumb.jpg", file_stem))
    } else {
        None
    }
}

/// Gallery variant of [`existing_thumb`]: every `_thumb`/`_thumb_N` on disk,
/// primary first, so a re-run does not re-extract what already exists.
fn existing_thumb_set(thumb_dir: &Path, file_stem: &str) -> Vec<String> {
    let primary = existing_thumb(thumb_dir, file_stem);
    let mut paths: Vec<String> = primary.into_iter().collect();

    for index in 1..64 {
        let name = format!("{}_thumb_{}.png", file_stem, index);
        if thumb_dir.join(&name).exists() {
            paths.push(format!(".polystash/thumbs/{}", name));
        }
    }
    paths
}

/// Thumbnail a standard raster image, returning the relative thumb path.
fn native_image_thumb(source: &Path, ext: &str, file_stem: &str, thumb_dir: &Path) -> Option<String> {
    if !is_native_image(ext) {
        return None;
    }

    let thumb_file_name = if ext == "png" || ext == "webp" {
        format!("{}_thumb.png", file_stem)
    } else {
        format!("{}_thumb.jpg", file_stem)
    };

    match generate_image_thumb(source, &thumb_dir.join(&thumb_file_name)) {
        Ok(()) => Some(format!(".polystash/thumbs/{}", thumb_file_name)),
        Err(e) => {
            tracing::warn!("Native image thumbnail failed for {}: {}. Trying embedded extraction.", source.display(), e);
            None
        }
    }
}

/// Produce a thumbnail for a non-raster asset, returning the relative thumb path.
async fn generate_thumbnail(ext: &str, full_source: &Path, file_stem: &str, thumb_dir: &Path) -> Result<String> {
    if video::is_video(ext) {
        let thumb_file_name = format!("{}_thumb.jpg", file_stem);
        video::extract_video_thumb(full_source, &thumb_dir.join(&thumb_file_name)).await?;
        Ok(format!(".polystash/thumbs/{}", thumb_file_name))
    } else if matches!(ext, "max" | "rvt" | "rfa" | "rte") {
        let thumb_file_name = format!("{}_thumb.png", file_stem);
        ole::extract_ole_thumb(full_source, &thumb_dir.join(&thumb_file_name))?;
        Ok(format!(".polystash/thumbs/{}", thumb_file_name))
    } else {
        generate_embedded_thumbnail(ext, full_source, file_stem, thumb_dir)
    }
}

/// Embedded-thumbnail extraction for CAD/BIM, design, and material formats.
fn generate_embedded_thumbnail(ext: &str, source: &Path, file_stem: &str, thumb_dir: &Path) -> Result<String> {
    let thumb_file_name = format!("{}_thumb.png", file_stem);
    let thumb_dest = thumb_dir.join(&thumb_file_name);
    let rel_thumb = format!(".polystash/thumbs/{}", thumb_file_name);

    match ext {
        "skp" => archive::extract_skp_thumb(source, &thumb_dest)?,
        "sbsar" => archive::extract_sbsar_thumb(source, &thumb_dest)?,
        "dwg" => cad::extract_dwg_thumb(source, &thumb_dest)?,
        "dxf" => cad::extract_dxf_thumb(source, &thumb_dest)?,
        "3dm" => embedded::extract_3dm_thumb(source, &thumb_dest)?,
        "c4d" => cineware::extract_c4d_thumb(source, &thumb_dest)?,
        "blend" => cineware::extract_preview_png_thumb(source, &thumb_dest)?,
        "psd" | "psb" => psd::extract_psd_thumb(source, &thumb_dest)?,
        "exr" | "hdr" | "hdri" => raster::extract_hdr_thumb(source, &thumb_dest)?,
        "vrmat" => material::extract_vrmat_thumb(source, &thumb_dest)?,
        // No extractor for this format: let the caller complete without a
        // thumbnail so the frontend shows its kind icon or live preview.
        _ => return Err(anyhow!("No embedded thumbnail extractor for .{}", ext)),
    }

    Ok(rel_thumb)
}

/// Additional previews saved as `<stem>_thumb_<n>`, returned library-relative
/// in order. Empty for every container that only ever holds one preview.
fn generate_gallery(ext: &str, source: &Path, file_stem: &str, thumb_dir: &Path) -> Vec<String> {
    if !GALLERY_EXTENSIONS.contains(&ext) {
        return Vec::new();
    }

    let images = archive::extract_skp_thumbs(source).unwrap_or_default();
    images
        .iter()
        .enumerate()
        .skip(1)
        .filter_map(|(index, img)| {
            let name = format!("{}_thumb_{}.png", file_stem, index);
            save_thumb(img, &thumb_dir.join(&name))
                .ok()
                .map(|()| format!(".polystash/thumbs/{}", name))
        })
        .collect()
}

/// Technical metadata collected alongside preview generation. Mesh statistics
/// and bounding boxes are intentionally absent: the frontend computes those
/// from the live three.js load, where the geometry actually exists.
async fn collect_metadata(ext: &str, source: &Path) -> Option<serde_json::Map<String, Value>> {
    let fields = if video::is_video(ext) {
        video::probe_metadata(source).await
    } else if ext == "pdf" {
        document::pdf_metadata(source).await
    } else if supports_image_metadata(ext) {
        raster::image_metadata(source, ext)
    } else {
        None
    };

    fields?.as_object().cloned()
}

/// Formats whose header can be read by the `image` crate, including the linear
/// float formats handled through the tone-mapped thumbnail path.
fn supports_image_metadata(ext: &str) -> bool {
    is_native_image(ext) || matches!(ext, "exr" | "hdr" | "hdri")
}

impl LibraryJobs<'_> {
    /// Mark a job and its asset done and notify the frontend.
    async fn complete(&self, job: &JobRef<'_>, rel_thumbs: &[String]) {
        let _ = sqlx::query("UPDATE preview_jobs SET status = 'done' WHERE id = ?")
            .bind(job.id)
            .execute(self.pool)
            .await;

        match rel_thumbs.first() {
            Some(rel) => {
                let _ = sqlx::query("UPDATE assets SET preview_status = 'done', thumbnail_path = ? WHERE id = ?")
                    .bind(rel)
                    .bind(job.asset_id)
                    .execute(self.pool)
                    .await;
            }
            None => {
                let _ = sqlx::query("UPDATE assets SET preview_status = 'done' WHERE id = ?")
                    .bind(job.asset_id)
                    .execute(self.pool)
                    .await;
            }
        }

        let mut patch = serde_json::Map::new();
        if rel_thumbs.len() > 1 {
            patch.insert("polystash.thumbs".to_string(), json!(rel_thumbs));
        }
        if let Some(fields) = self.technical_metadata(job).await {
            patch.extend(fields);
        }
        if !patch.is_empty() {
            self.merge_metadata(job.asset_id, patch).await;
        }

        let absolute: Vec<String> = rel_thumbs
            .iter()
            .map(|rel| resolve_path(self.root_path, rel))
            .collect();
        let _ = self.state.app_handle.emit("preview:done", json!({
            "assetId": job.asset_id,
            "thumbnailPath": absolute.first(),
            "thumbnailPaths": absolute,
        }));
    }

    /// Format-specific metadata for the asset record. Extraction is best effort:
    /// a missing external tool or an unreadable header yields no fields.
    async fn technical_metadata(&self, job: &JobRef<'_>) -> Option<serde_json::Map<String, Value>> {
        let source = resolve_source(self.root_path, job.file_path);
        let ext = source
            .extension()
            .and_then(|s| s.to_str())
            .unwrap_or("")
            .to_lowercase();
        collect_metadata(&ext, &source).await
    }

    /// Merge `patch` into the asset's metadata, preserving unrelated keys such as
    /// `preview_model`.
    async fn merge_metadata(&self, asset_id: &str, patch: serde_json::Map<String, Value>) {
        let existing: Option<(String,)> = sqlx::query_as("SELECT metadata FROM assets WHERE id = ?")
            .bind(asset_id)
            .fetch_optional(self.pool)
            .await
            .unwrap_or(None);

        let mut metadata: serde_json::Map<String, Value> = existing
            .and_then(|(raw,)| serde_json::from_str(&raw).ok())
            .unwrap_or_default();
        metadata.extend(patch);

        let _ = sqlx::query("UPDATE assets SET metadata = ? WHERE id = ?")
            .bind(Value::Object(metadata).to_string())
            .bind(asset_id)
            .execute(self.pool)
            .await;
    }

}

/// Reset orphaned jobs left in 'processing' by a previous crash.
async fn recover_orphaned_jobs(state: &AppState) {
    for lib in state.db.get_all_libraries().await {
        let _ = sqlx::query("UPDATE preview_jobs SET status = 'queued' WHERE status = 'processing'")
            .execute(&lib.pool)
            .await;
    }
}

/// Poll one batch of queued jobs for a library.
async fn fetch_queued_jobs(pool: &sqlx::Pool<sqlx::Sqlite>) -> Vec<(String, String, String)> {
    sqlx::query_as(
        "SELECT p.id, p.asset_id, a.file_path FROM preview_jobs p JOIN assets a ON p.asset_id = a.id WHERE p.status = 'queued' OR p.status = 'pending' LIMIT 10"
    )
    .fetch_all(pool)
    .await
    .unwrap_or_default()
}

/// Complete a Collada job. A format with an embedded picture gets it as the
/// thumbnail; without one the job completes with no thumb so the frontend
/// renders the mesh with three.js.
async fn complete_dae_job(jobs: &LibraryJobs<'_>, job: &JobRef<'_>, source: &Path) {
    let file_stem = thumb_stem(source);
    let thumb_file_name = format!("{}_thumb.png", file_stem);
    let thumb_dest = jobs.thumb_dir.join(&thumb_file_name);

    match embedded::extract_dae_thumb(source, &thumb_dest) {
        Ok(()) => {
            let rel = format!(".polystash/thumbs/{}", thumb_file_name);
            jobs.complete(job, &[rel]).await;
        }
        Err(_) => jobs.complete(job, &[]).await,
    }
}

/// File stem used to name the thumbnail, or a fixed fallback for unusual paths.
fn thumb_stem(path: &Path) -> &str {
    path.file_stem()
        .and_then(|stem| stem.to_str())
        .unwrap_or("preview")
}

/// Generate the thumbnail for one job and record the outcome.
async fn process_job(jobs: &LibraryJobs<'_>, job: &JobRef<'_>) {
    let full_source = resolve_source(jobs.root_path, job.file_path);
    let file_stem = thumb_stem(&full_source);
    let ext = full_source
        .extension()
        .and_then(|s| s.to_str())
        .unwrap_or("")
        .to_lowercase();

    if let Some(rel_thumb) = existing_thumb(jobs.thumb_dir, file_stem) {
        tracing::info!("Thumbnail already exists for {}, skipping generation", file_stem);
        let mut gallery = vec![rel_thumb];
        gallery.extend(existing_thumb_set(jobs.thumb_dir, file_stem).into_iter().skip(1));
        jobs.complete(job, &gallery).await;
        return;
    }

    // PDFs render page 1 in the frontend with PDF.js, so no backend rasterizer
    // (and no external `pdftoppm`/`mutool`) is needed.
    if ext == "pdf" {
        jobs.complete(job, &[]).await;
        return;
    }

    // Collada is a mesh format, but some exports carry a preview image; prefer
    // it over the live frontend render when it exists.
    if ext == "dae" {
        complete_dae_job(jobs, job, &full_source).await;
        return;
    }

    // Substance archives render PBR maps through the optional SDK tool; the
    // embedded archive preview stays the fast path when one is present.
    if ext == "sbsar" {
        complete_sbsar_job(jobs, job, &full_source).await;
        return;
    }

    // Mesh formats render live in the frontend; no backend thumbnail.
    if MESH_EXTENSIONS.contains(&ext.as_str()) {
        jobs.complete(job, &[]).await;
        return;
    }

    // ktx/ktx2 are GPU-compressed textures that only the frontend KTX2Loader
    // can render, so complete them with no thumb.
    if GPU_TEXTURE_EXTENSIONS.contains(&ext.as_str()) {
        jobs.complete(job, &[]).await;
        return;
    }

    // No backend thumbnail and no live 3D preview: done, kind icon on frontend.
    if CATEGORY_D_EXTENSIONS.contains(&ext.as_str()) {
        jobs.complete(job, &[]).await;
        return;
    }

    if let Some(rel_thumb) = native_image_thumb(&full_source, &ext, file_stem, jobs.thumb_dir) {
        jobs.complete(job, &[rel_thumb]).await;
        return;
    }

    complete_with_extraction(jobs, job, &full_source, &ext).await;
}

/// Substance job: the embedded archive preview first, otherwise PBR maps
/// rendered with the optional Substance SDK tool.
async fn complete_sbsar_job(jobs: &LibraryJobs<'_>, job: &JobRef<'_>, source: &Path) {
    let file_stem = thumb_stem(source);
    let thumb_name = format!("{file_stem}_thumb.png");
    let thumb_dest = jobs.thumb_dir.join(&thumb_name);

    if archive::extract_sbsar_thumb(source, &thumb_dest).is_err() {
        if let Some(maps) = archive::render_sbsar_maps(source, &file_stem, jobs.thumb_dir).await {
            if maps.iter().any(|(key, _)| key == "baseColor") {
                let _ = std::fs::copy(
                    jobs.thumb_dir.join(format!("{file_stem}_sbsar_baseColor.png")),
                    &thumb_dest,
                );
            }

            let textures: serde_json::Map<String, Value> = maps
                .iter()
                .map(|(key, rel)| (key.clone(), Value::String(resolve_path(jobs.root_path, rel))))
                .collect();
            let mut patch = serde_json::Map::new();
            patch.insert("sbsar_textures".to_string(), Value::Object(textures));
            jobs.merge_metadata(job.asset_id, patch).await;
        }
    }

    let rel = thumb_dest
        .exists()
        .then(|| format!(".polystash/thumbs/{thumb_name}"))
        .into_iter()
        .collect::<Vec<_>>();
    jobs.complete(job, &rel).await;
}

/// Run the format-specific extractor (plus any gallery) and record the result.
async fn complete_with_extraction(jobs: &LibraryJobs<'_>, job: &JobRef<'_>, source: &Path, ext: &str) {
    let file_stem = thumb_stem(source);

    match generate_thumbnail(ext, source, file_stem, jobs.thumb_dir).await {
        Ok(primary) => {
            let mut gallery = vec![primary];
            gallery.extend(generate_gallery(ext, source, file_stem, jobs.thumb_dir));
            jobs.complete(job, &gallery).await;
        }
        Err(e) => {
            // An extractor that cannot handle a file is a normal outcome, not a
            // failure: complete without a thumbnail so the asset never stalls
            // and the frontend can show its kind icon or live preview.
            tracing::warn!("No thumbnail extracted for {}: {}", source.display(), e);
            jobs.complete(job, &[]).await;
        }
    }
}

/// Drain one batch of queued jobs for a library; true when any job was seen.
async fn process_library(state: &AppState, lib: &LibraryHandle) -> bool {
    let thumb_dir = lib.root_path.join(".polystash").join("thumbs");
    let _ = tokio::fs::create_dir_all(&thumb_dir).await;

    let jobs = fetch_queued_jobs(&lib.pool).await;
    if jobs.is_empty() {
        return false;
    }

    let jobs_ctx = LibraryJobs {
        state,
        pool: &lib.pool,
        root_path: &lib.root_path,
        thumb_dir: &thumb_dir,
    };

    for (id, asset_id, file_path) in jobs {
        let job = JobRef {
            id: &id,
            asset_id: &asset_id,
            file_path: &file_path,
        };
        let claimed = sqlx::query("UPDATE preview_jobs SET status = 'processing' WHERE id = ?")
            .bind(job.id)
            .execute(jobs_ctx.pool)
            .await;
        match claimed {
            Ok(_) => process_job(&jobs_ctx, &job).await,
            Err(e) => tracing::error!("Failed to update job status to processing: {}", e),
        }

        // Yield execution between items to allow other async runtime tasks to progress
        tokio::task::yield_now().await;
    }
    true
}

pub fn start_preview_worker(state: Arc<AppState>) {
    tauri::async_runtime::spawn(async move {
        // Crash-recovery: Reset any orphaned jobs left in 'processing' ONCE at startup
        recover_orphaned_jobs(&state).await;

        loop {
            let libs = state.db.get_all_libraries().await;
            if libs.is_empty() {
                tokio::time::sleep(tokio::time::Duration::from_millis(1500)).await;
                continue;
            }

            let mut processed_any = false;
            for lib in &libs {
                processed_any |= process_library(&state, lib).await;
            }

            if !processed_any {
                tokio::time::sleep(tokio::time::Duration::from_millis(1000)).await;
            }
        }
    });
}
