mod archive;
mod image;
mod material;
mod ole;
mod placeholder;
mod psd;
mod video;

use anyhow::Result;
use image::DynamicImage;
use serde_json::json;
use std::path::{Path, PathBuf};
use std::sync::Arc;
use tauri::Emitter;
use crate::db::LibraryHandle;
use crate::AppState;
use crate::db::resolve_path;

pub const THUMB_SIZE: u32 = 512;

/// Formats rendered live in the frontend with three.js; the backend emits
/// no thumbnail for these and completes the job immediately.
const MESH_EXTENSIONS: [&str; 6] = ["gltf", "glb", "fbx", "obj", "stl", "dae"];

pub use image::{generate_image_thumb, is_native_image};

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
    } else if matches!(ext, "max" | "rvt" | "rfa") {
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
        "dwg" => archive::extract_dwg_thumb(source, &thumb_dest)?,
        "psd" | "psb" => psd::extract_psd_thumb(source, &thumb_dest)?,
        "exr" | "hdr" => image::extract_hdr_thumb(source, &thumb_dest)?,
        "vrmat" => material::extract_vrmat_thumb(source, &thumb_dest)?,
        // Deferred formats (ai, 3dm, 3ds, ply) and anything unknown: neutral tile.
        _ => placeholder::generate_placeholder(&thumb_dest)?,
    }

    Ok(rel_thumb)
}

impl LibraryJobs<'_> {
    /// Mark a job and its asset done and notify the frontend.
    async fn complete(&self, job: &JobRef<'_>, rel_thumb: Option<&str>) {
        let _ = sqlx::query("UPDATE preview_jobs SET status = 'done' WHERE id = ?")
            .bind(job.id)
            .execute(self.pool)
            .await;
        match rel_thumb {
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

        let resolved_abs = rel_thumb.map(|rel| resolve_path(self.root_path, rel));
        let _ = self.state.app_handle.emit("preview:done", json!({
            "assetId": job.asset_id,
            "thumbnailPath": resolved_abs,
        }));
    }

    /// Mark a job and its asset errored and notify the frontend.
    async fn fail(&self, job: &JobRef<'_>) {
        let _ = sqlx::query("UPDATE preview_jobs SET status = 'error' WHERE id = ?")
            .bind(job.id)
            .execute(self.pool)
            .await;
        let _ = sqlx::query("UPDATE assets SET preview_status = 'error' WHERE id = ?")
            .bind(job.asset_id)
            .execute(self.pool)
            .await;
        let _ = self.state.app_handle.emit("preview:error", json!({
            "assetId": job.asset_id,
            "jobId": job.id,
        }));
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

/// Generate the thumbnail for one job and record the outcome.
async fn process_job(jobs: &LibraryJobs<'_>, job: &JobRef<'_>) {
    let full_source = resolve_source(jobs.root_path, job.file_path);
    let file_stem = full_source
        .file_stem()
        .and_then(|s| s.to_str())
        .unwrap_or("preview");
    let ext = full_source
        .extension()
        .and_then(|s| s.to_str())
        .unwrap_or("")
        .to_lowercase();

    if let Some(rel_thumb) = existing_thumb(jobs.thumb_dir, file_stem) {
        tracing::info!("Thumbnail already exists for {}, skipping generation", file_stem);
        jobs.complete(job, Some(&rel_thumb)).await;
        return;
    }

    // Mesh formats render live in the frontend; no backend thumbnail.
    if MESH_EXTENSIONS.contains(&ext.as_str()) {
        jobs.complete(job, None).await;
        return;
    }

    if let Some(rel_thumb) = native_image_thumb(&full_source, &ext, file_stem, jobs.thumb_dir) {
        jobs.complete(job, Some(&rel_thumb)).await;
        return;
    }

    match generate_thumbnail(&ext, &full_source, file_stem, jobs.thumb_dir).await {
        Ok(rel_thumb) => jobs.complete(job, Some(&rel_thumb)).await,
        Err(e) => {
            tracing::error!("Job {} failed: {}", job.id, e);
            jobs.fail(job).await;
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
