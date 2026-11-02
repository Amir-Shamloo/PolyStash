use anyhow::{anyhow, Result};
use std::io::Read;
use std::path::{Path, PathBuf};
use image::imageops::FilterType;
use std::sync::Arc;
use tokio::process::Command;
use serde_json::json;
use tauri::Emitter;
use crate::AppState;
use crate::db::resolve_path;

pub const THUMB_SIZE: u32 = 512;

/// Checks if an extension is a standard raster image that Rust can decode directly.
pub fn is_native_image(ext: &str) -> bool {
    matches!(
        ext.to_lowercase().as_str(),
        "jpg" | "jpeg" | "png" | "bmp" | "gif" | "tif" | "tiff" | "tga" | "webp"
    )
}

/// Resize and write a decoded image to `output` as JPEG (when the output path
/// ends in .jpg/.jpeg) or PNG otherwise.
fn save_thumb(img: &image::DynamicImage, output: &Path) -> Result<()> {
    let thumb = img.resize(THUMB_SIZE, THUMB_SIZE, FilterType::Triangle);

    if let Some(parent) = output.parent() {
        let _ = std::fs::create_dir_all(parent);
    }

    let is_jpg = output
        .extension()
        .and_then(|s| s.to_str())
        .map(|s| s.eq_ignore_ascii_case("jpg") || s.eq_ignore_ascii_case("jpeg"))
        .unwrap_or(false);

    if is_jpg {
        thumb.to_rgb8().save_with_format(output, image::ImageFormat::Jpeg)?;
    } else {
        thumb.save(output)?;
    }
    Ok(())
}

/// Blazingly fast in-process image downsampling using the native Rust `image` crate.
/// Uses Triangle (bilinear) filtering for 4x–8x faster performance than Lanczos3.
pub fn generate_image_thumb(source: &Path, output: &Path) -> Result<()> {
    let img = image::open(source)?;
    save_thumb(&img, output)
}

/// Wrap a raw BITMAPINFOHEADER DIB (no file header) into a full BMP so the
/// `image` crate can decode it.
fn dib_to_bmp(dib: &[u8]) -> Result<Vec<u8>> {
    if dib.len() < 40 {
        return Err(anyhow!("DIB too short"));
    }
    let bi_size = u32::from_le_bytes(dib[0..4].try_into()?);
    if bi_size != 40 {
        return Err(anyhow!("Unsupported DIB header size {}", bi_size));
    }
    let bi_bit_count = u16::from_le_bytes(dib[14..16].try_into()?);
    let colors = if bi_bit_count <= 8 { 1u32 << bi_bit_count } else { 0 };
    let bf_off_bits = 14u32 + 40 + colors * 4;
    let bf_size = 14u32 + dib.len() as u32;

    let mut bmp = Vec::with_capacity(bf_size as usize);
    bmp.extend_from_slice(b"BM");
    bmp.extend_from_slice(&bf_size.to_le_bytes());
    bmp.extend_from_slice(&[0u8; 4]); // reserved
    bmp.extend_from_slice(&bf_off_bits.to_le_bytes());
    bmp.extend_from_slice(dib);
    Ok(bmp)
}

/// Locate a BITMAPINFOHEADER (biSize == 40) with sane dimensions inside a byte
/// buffer. Used to find embedded DIB thumbnails in OLE summary streams.
fn find_dib(bytes: &[u8]) -> Option<usize> {
    let mut i = 0;
    while i + 44 <= bytes.len() {
        if bytes[i..i + 4] == [0x28u8, 0, 0, 0] {
            let w = u32::from_le_bytes(bytes[i + 4..i + 8].try_into().ok()?);
            // biHeight may be negative (top-down DIB)
            let h = i32::from_le_bytes(bytes[i + 8..i + 12].try_into().ok()?);
            if (1..=20000).contains(&w) && h != 0 && h.unsigned_abs() <= 20000 {
                return Some(i);
            }
        }
        i += 1;
    }
    None
}

fn find_png(bytes: &[u8]) -> Option<usize> {
    const SIG: [u8; 8] = [0x89, b'P', b'N', b'G', 0x0d, 0x0a, 0x1a, 0x0a];
    bytes.windows(8).position(|w| w == SIG)
}

/// Extract the embedded OLE thumbnail from a 3ds Max (.max) or Revit (.rvt/.rfa)
/// compound file and write a resized thumbnail. Max stores a DIB in the
/// SummaryInformation stream; Revit stores a PNG in the `RevitPreview4.0` stream.
fn extract_ole_thumb(source: &Path, output: &Path) -> Result<()> {
    // shortcut: OLE files are compound-binary; spurious files fail at open
    let mut cf = cfb::open(source)?;
    let mut raw = Vec::new();

    for name in ["RevitPreview4.0", "\x05SummaryInformation"] {
        if cf.exists(name) && cf.is_stream(name) {
            cf.open_stream(name)?.read_to_end(&mut raw)?;

            if let Some(idx) = find_png(&raw) {
                let img = image::load_from_memory(&raw[idx..])?;
                return save_thumb(&img, output);
            }
            if let Some(idx) = find_dib(&raw) {
                let bmp = dib_to_bmp(&raw[idx..])?;
                let img = image::load_from_memory(&bmp)?;
                return save_thumb(&img, output);
            }
            raw.clear();
        }
    }

    Err(anyhow!("No embedded thumbnail found in OLE file"))
}

/// Extract a video frame with ffmpeg and write it to `output`.
async fn extract_video_thumb(source: &Path, output: &Path) -> Result<()> {
    if let Some(parent) = output.parent() {
        let _ = tokio::fs::create_dir_all(parent).await;
    }

    let out = Command::new("ffmpeg")
        .args(["-ss", "00:00:01", "-i"])
        .arg(source)
        .args([
            "-frames:v",
            "1",
            "-vf",
            "scale=720:720:force_original_aspect_ratio=decrease",
            "-y",
        ])
        .arg(output)
        .stdin(std::process::Stdio::null())
        .output()
        .await?;

    if out.status.success() {
        Ok(())
    } else {
        Err(anyhow!(
            "ffmpeg failed: {}",
            String::from_utf8_lossy(&out.stderr)
        ))
    }
}

fn is_video(ext: &str) -> bool {
    mime_guess::from_ext(ext)
        .first_raw()
        .map_or(false, |m| m.starts_with("video/"))
}

/// Produce a thumbnail for a non-raster asset, returning the relative thumb path.
async fn generate_thumbnail(ext: &str, full_source: &Path, file_stem: &str, thumb_dir: &Path) -> Result<String> {
    if is_video(ext) {
        let thumb_file_name = format!("{}_thumb.jpg", file_stem);
        let thumb_dest = thumb_dir.join(&thumb_file_name);
        let rel_thumb = format!(".polystash/thumbs/{}", thumb_file_name);
        extract_video_thumb(full_source, &thumb_dest).await?;
        Ok(rel_thumb)
    } else if matches!(ext, "max" | "rvt" | "rfa") {
        let thumb_file_name = format!("{}_thumb.png", file_stem);
        let thumb_dest = thumb_dir.join(&thumb_file_name);
        let rel_thumb = format!(".polystash/thumbs/{}", thumb_file_name);
        extract_ole_thumb(full_source, &thumb_dest)?;
        Ok(rel_thumb)
    } else {
        Err(anyhow!("No preview generator for .{}", ext))
    }
}

pub fn start_preview_worker(state: Arc<AppState>) {
    tauri::async_runtime::spawn(async move {
        // Crash-recovery: Reset any orphaned jobs left in 'processing' ONCE at startup
        for lib in state.db.get_all_libraries().await {
            let _ = sqlx::query("UPDATE preview_jobs SET status = 'queued' WHERE status = 'processing'")
                .execute(&lib.pool)
                .await;
        }

        loop {
            let libs = state.db.get_all_libraries().await;
            if libs.is_empty() {
                tokio::time::sleep(tokio::time::Duration::from_millis(1500)).await;
                continue;
            }

            let mut processed_any = false;

            for lib in libs {
                let pool = &lib.pool;
                let root_path = &lib.root_path;
                let thumb_dir = root_path.join(".polystash").join("thumbs");
                let _ = tokio::fs::create_dir_all(&thumb_dir).await;

                // Poll batch of up to 10 jobs to minimize SQLite round-trip overhead
                let jobs: Vec<(String, String, String)> = sqlx::query_as(
                    "SELECT p.id, p.asset_id, a.file_path FROM preview_jobs p JOIN assets a ON p.asset_id = a.id WHERE p.status = 'queued' OR p.status = 'pending' LIMIT 10"
                ).fetch_all(pool).await.unwrap_or_default();

                for (job_id, asset_id, file_path) in jobs {
                    processed_any = true;

                    if let Err(e) = sqlx::query("UPDATE preview_jobs SET status = 'processing' WHERE id = ?")
                        .bind(&job_id)
                        .execute(pool)
                        .await
                    {
                        tracing::error!("Failed to update job status to processing: {}", e);
                        continue;
                    }

                    let full_source = if Path::new(&file_path).is_absolute() {
                        PathBuf::from(&file_path)
                    } else {
                        root_path.join(&file_path)
                    };

                    let file_stem = full_source
                        .file_stem()
                        .and_then(|s| s.to_str())
                        .unwrap_or("preview");

                    let ext = full_source
                        .extension()
                        .and_then(|s| s.to_str())
                        .unwrap_or("")
                        .to_lowercase();

                    // Check if thumbnail already exists on disk
                    let thumb_png = thumb_dir.join(format!("{}_thumb.png", file_stem));
                    let thumb_jpg = thumb_dir.join(format!("{}_thumb.jpg", file_stem));

                    let existing_thumb = if thumb_png.exists() {
                        Some(format!(".polystash/thumbs/{}_thumb.png", file_stem))
                    } else if thumb_jpg.exists() {
                        Some(format!(".polystash/thumbs/{}_thumb.jpg", file_stem))
                    } else {
                        None
                    };

                    if let Some(rel_thumb) = existing_thumb {
                        tracing::info!("Thumbnail already exists for {}, skipping generation", file_stem);
                        let _ = sqlx::query("UPDATE preview_jobs SET status = 'done' WHERE id = ?")
                            .bind(&job_id)
                            .execute(pool)
                            .await;
                        let _ = sqlx::query("UPDATE assets SET preview_status = 'done', thumbnail_path = ? WHERE id = ?")
                            .bind(&rel_thumb)
                            .bind(&asset_id)
                            .execute(pool)
                            .await;

                        let resolved_abs = resolve_path(root_path, &rel_thumb);
                        let _ = state.app_handle.emit("preview:done", json!({
                            "assetId": &asset_id,
                            "thumbnailPath": resolved_abs,
                        }));
                        continue;
                    }

                    // ── 1. Fast Native Rust Path for Standard Raster Images ──
                    if is_native_image(&ext) {
                        let thumb_file_name = if ext == "png" || ext == "webp" {
                            format!("{}_thumb.png", file_stem)
                        } else {
                            format!("{}_thumb.jpg", file_stem)
                        };
                        let thumb_dest = thumb_dir.join(&thumb_file_name);
                        let rel_thumb = format!(".polystash/thumbs/{}", thumb_file_name);

                        match generate_image_thumb(&full_source, &thumb_dest) {
                            Ok(()) => {
                                let _ = sqlx::query("UPDATE preview_jobs SET status = 'done' WHERE id = ?")
                                    .bind(&job_id)
                                    .execute(pool)
                                    .await;
                                let _ = sqlx::query("UPDATE assets SET preview_status = 'done', thumbnail_path = ? WHERE id = ?")
                                    .bind(&rel_thumb)
                                    .bind(&asset_id)
                                    .execute(pool)
                                    .await;

                                let resolved_abs = resolve_path(root_path, &rel_thumb);
                                let _ = state.app_handle.emit("preview:done", json!({
                                    "assetId": &asset_id,
                                    "thumbnailPath": resolved_abs,
                                }));
                                continue;
                            }
                            Err(e) => {
                                tracing::warn!("Native Rust image thumbnail failed for {}: {}", full_source.display(), e);
                            }
                        }
                    }

                    // ── 2. Native Rust paths for video and OLE-embedded thumbnails ──
                    let thumb_result = generate_thumbnail(&ext, &full_source, file_stem, &thumb_dir).await;

                    match thumb_result {
                        Ok(rel_thumb) => {
                            let _ = sqlx::query("UPDATE preview_jobs SET status = 'done' WHERE id = ?")
                                .bind(&job_id)
                                .execute(pool)
                                .await;
                            let _ = sqlx::query("UPDATE assets SET preview_status = 'done', thumbnail_path = ? WHERE id = ?")
                                .bind(&rel_thumb)
                                .bind(&asset_id)
                                .execute(pool)
                                .await;

                            let resolved_abs = resolve_path(root_path, &rel_thumb);
                            let _ = state.app_handle.emit("preview:done", json!({
                                "assetId": &asset_id,
                                "thumbnailPath": resolved_abs,
                            }));
                        }
                        Err(e) => {
                            tracing::error!("Job {} failed: {}", job_id, e);
                            let _ = sqlx::query("UPDATE preview_jobs SET status = 'error' WHERE id = ?")
                                .bind(&job_id)
                                .execute(pool)
                                .await;
                            let _ = sqlx::query("UPDATE assets SET preview_status = 'error' WHERE id = ?")
                                .bind(&asset_id)
                                .execute(pool)
                                .await;
                            let _ = state.app_handle.emit("preview:error", json!({
                                "assetId": &asset_id,
                                "jobId": &job_id,
                            }));
                        }
                    }

                    // Yield execution between items to allow other async runtime tasks to progress
                    tokio::task::yield_now().await;
                }
            }

            if !processed_any {
                tokio::time::sleep(tokio::time::Duration::from_millis(1000)).await;
            }
        }
    });
}
