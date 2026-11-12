mod archive;
mod cad;
mod cineware;
mod dds;
mod document;
mod embedded;
mod material;
mod ole;
mod psd;
mod raster;
mod structural;
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
        video::metadata(source).await
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

    /// Complete a job whose preview is a converted GLB: the asset's model
    /// preview points at the GLB and the frontend renders the thumbnail from it,
    /// exactly like the direct GLB path. `rel_thumb` overrides the card
    /// thumbnail when the format has a better image of its own.
    async fn complete_model(&self, job: &JobRef<'_>, rel_glb: &str, rel_thumb: Option<&str>) {
        let _ = sqlx::query("UPDATE preview_jobs SET status = 'done' WHERE id = ?")
            .bind(job.id)
            .execute(self.pool)
            .await;

        let thumb_rel = rel_thumb.unwrap_or(rel_glb);
        let _ = sqlx::query("UPDATE assets SET preview_status = 'done', thumbnail_path = ? WHERE id = ?")
            .bind(thumb_rel)
            .bind(job.asset_id)
            .execute(self.pool)
            .await;

        let mut patch = serde_json::Map::new();
        patch.insert("preview_model".to_string(), Value::String(rel_glb.to_string()));
        if let Some(fields) = self.technical_metadata(job).await {
            patch.extend(fields);
        }
        self.merge_metadata(job.asset_id, patch).await;

        let absolute = resolve_path(self.root_path, rel_glb);
        let thumbnail = match rel_thumb {
            Some(rel) => resolve_path(self.root_path, rel),
            None => absolute.clone(),
        };
        let _ = self.state.app_handle.emit("preview:done", json!({
            "assetId": job.asset_id,
            "thumbnailPath": thumbnail,
            "thumbnailPaths": [thumbnail],
            "previewModel": absolute,
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

/// Complete a Collada job. Collada has no reliable embedded preview, so the job
/// completes without a thumbnail and the frontend renders the mesh with
/// three.js — the same loader and camera the detail panel uses.
async fn complete_dae_job(jobs: &LibraryJobs<'_>, job: &JobRef<'_>, _source: &Path) {
    jobs.complete(job, &[]).await;
}

/// True when `path` is a complete GLB: correct magic, version 2, and a declared
/// length that matches the file on disk.
fn valid_glb(path: &Path) -> bool {
    let Ok(mut file) = std::fs::File::open(path) else {
        return false;
    };
    let Ok(len) = file.metadata().map(|meta| meta.len()) else {
        return false;
    };
    let mut header = [0_u8; 12];
    let Ok(()) = std::io::Read::read_exact(&mut file, &mut header) else {
        return false;
    };
    let magic = u32::from_le_bytes(header[0..4].try_into().unwrap_or_default());
    let version = u32::from_le_bytes(header[4..8].try_into().unwrap_or_default());
    let declared = u32::from_le_bytes(header[8..12].try_into().unwrap_or_default());
    magic == 0x4654_6C67 && version == 2 && u64::from(declared) == len
}

/// Remove a metadata key so a stale preview is never shown after a fallback.
async fn remove_metadata_key(jobs: &LibraryJobs<'_>, job: &JobRef<'_>, key: &str) {
    let existing: Option<(String,)> = sqlx::query_as("SELECT metadata FROM assets WHERE id = ?")
        .bind(job.asset_id)
        .fetch_optional(jobs.pool)
        .await
        .unwrap_or(None);
    let Some((raw,)) = existing else {
        return;
    };
    let mut metadata: serde_json::Map<String, Value> = serde_json::from_str(&raw).unwrap_or_default();
    if metadata.remove(key).is_none() {
        return;
    }
    let _ = sqlx::query("UPDATE assets SET metadata = ? WHERE id = ?")
        .bind(Value::Object(metadata).to_string())
        .bind(job.asset_id)
        .execute(jobs.pool)
        .await;
}

/// Complete a Cinema 4D job. The Cineware GLB always feeds the 3D viewer; an
/// embedded preview image is only used as a better card thumbnail. If the
/// helper is unavailable or the scene has no exportable geometry, fall back to
/// the embedded `THMB` preview image.
async fn complete_c4d_job(jobs: &LibraryJobs<'_>, job: &JobRef<'_>, source: &Path) {
    let file_stem = thumb_stem(source);
    let glb_name = format!("{file_stem}_preview.glb");
    let glb_rel = format!(".polystash/thumbs/{glb_name}");
    let glb_dest = jobs.thumb_dir.join(&glb_name);

    let glb_ready = if valid_glb(&glb_dest) {
        true
    } else {
        cineware::export_c4d_glb(source, &glb_dest).await.is_ok()
    };

    if !glb_ready {
        tracing::warn!(
            "Cineware GLB export failed for {}; falling back to the embedded preview.",
            source.display()
        );
        remove_metadata_key(jobs, job, "preview_model").await;
        complete_with_extraction(jobs, job, source, "c4d").await;
        return;
    }

    // The GLB is the viewer; a stored preview image beats a rendered thumbnail.
    let thumb_name = format!("{file_stem}_thumb.png");
    let thumb_dest = jobs.thumb_dir.join(&thumb_name);
    let thumb = match cineware::extract_c4d_thumb(source, &thumb_dest) {
        Ok(()) => Some(format!(".polystash/thumbs/{thumb_name}")),
        Err(_) => None,
    };
    jobs.complete_model(job, &glb_rel, thumb.as_deref()).await;
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

    // Cinema 4D gets a real mesh preview through the Cineware helper; the
    // embedded THMB image is only the fallback. Run before the stale-thumbnail
    // shortcut so an older PNG does not hide the converted GLB.
    if ext == "c4d" {
        complete_c4d_job(jobs, job, &full_source).await;
        return;
    }

    if complete_existing_thumbnail(jobs, job, &full_source, &ext).await {
        return;
    }

    // PDFs render page 1 in the frontend with PDF.js, so no backend rasterizer
    // (and no external `pdftoppm`/`mutool`) is needed.
    if ext == "pdf" {
        jobs.complete(job, &[]).await;
        return;
    }

    // Collada is a mesh format; the frontend renders it with three.js.
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

    // Formats the frontend renders itself need no backend thumbnail.
    if frontend_only(&ext) {
        jobs.complete(job, &[]).await;
        return;
    }

    if let Some(rel_thumb) = native_image_thumb(&full_source, &ext, file_stem, jobs.thumb_dir) {
        jobs.complete(job, &[rel_thumb]).await;
        return;
    }

    complete_with_extraction(jobs, job, &full_source, &ext).await;
}

/// True for formats the frontend decodes or renders on its own, so the backend
/// only records the job as done.
fn frontend_only(ext: &str) -> bool {
    MESH_EXTENSIONS.contains(&ext)
        || GPU_TEXTURE_EXTENSIONS.contains(&ext)
        || CATEGORY_D_EXTENSIONS.contains(&ext)
}

/// Complete a job whose thumbnail already exists. SBSAR maps are viewer state
/// rather than thumbnail state, so they are republished either way. Returns
/// false when no thumbnail was on disk.
async fn complete_existing_thumbnail(
    jobs: &LibraryJobs<'_>,
    job: &JobRef<'_>,
    source: &Path,
    ext: &str,
) -> bool {
    let file_stem = thumb_stem(source);
    let Some(rel_thumb) = existing_thumb(jobs.thumb_dir, file_stem) else {
        return false;
    };

    tracing::info!("Thumbnail already exists for {}, skipping generation", file_stem);
    let mut gallery = vec![rel_thumb];
    gallery.extend(existing_thumb_set(jobs.thumb_dir, file_stem).into_iter().skip(1));
    if ext == "sbsar" {
        if let Some(maps) = archive::render_sbsar_maps(source, file_stem, jobs.thumb_dir).await {
            publish_sbsar_textures(jobs, job, &maps).await;
        }
    }
    jobs.complete(job, &gallery).await;
    true
}

/// The card thumbnail is the base-colour map, downscaled to thumbnail size.
fn write_sbsar_card_thumbnail(jobs: &LibraryJobs<'_>, file_stem: &str, maps: &[(String, String)]) {
    let thumb_dest = jobs.thumb_dir.join(format!("{file_stem}_thumb.png"));
    if thumb_dest.exists() {
        return;
    }
    let Some((_, rel)) = maps.iter().find(|(key, _)| key == "baseColor") else {
        return;
    };
    let Ok(image) = image::open(resolve_path(jobs.root_path, rel)) else {
        return;
    };
    let _ = save_thumb(&image, &thumb_dest);
}

/// Substance job: the PBR maps feed the 3D viewer and are always produced; the
/// embedded archive preview is only a shortcut for the card thumbnail.
async fn complete_sbsar_job(jobs: &LibraryJobs<'_>, job: &JobRef<'_>, source: &Path) {
    let file_stem = thumb_stem(source);
    let thumb_name = format!("{file_stem}_thumb.png");
    let thumb_dest = jobs.thumb_dir.join(&thumb_name);

    match archive::render_sbsar_maps(source, file_stem, jobs.thumb_dir).await {
        Some(maps) => {
            write_sbsar_card_thumbnail(jobs, file_stem, &maps);
            publish_sbsar_textures(jobs, job, &maps).await;
        }
        None => remove_metadata_key(jobs, job, "sbsar_textures").await,
    }

    if !thumb_dest.exists() {
        let _ = archive::extract_sbsar_thumb(source, &thumb_dest);
    }

    let rel = thumb_dest
        .exists()
        .then(|| format!(".polystash/thumbs/{thumb_name}"))
        .into_iter()
        .collect::<Vec<_>>();
    jobs.complete(job, &rel).await;
}

/// Point the asset's `sbsar_textures` at the rendered PBR maps so the frontend
/// can build the shader ball.
async fn publish_sbsar_textures(
    jobs: &LibraryJobs<'_>,
    job: &JobRef<'_>,
    maps: &[(String, String)],
) {
    let textures: serde_json::Map<String, Value> = maps
        .iter()
        .map(|(key, rel)| (key.clone(), Value::String(resolve_path(jobs.root_path, rel))))
        .collect();
    let mut patch = serde_json::Map::new();
    patch.insert("sbsar_textures".to_string(), Value::Object(textures));
    jobs.merge_metadata(job.asset_id, patch).await;
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
