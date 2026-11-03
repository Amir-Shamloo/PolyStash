use anyhow::{anyhow, Result};
use std::path::Path;
use tokio::process::Command;

/// Extract a video frame with ffmpeg and write it to `output`.
pub(crate) async fn extract_video_thumb(source: &Path, output: &Path) -> Result<()> {
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

pub fn is_video(ext: &str) -> bool {
    mime_guess::from_ext(ext)
        .first_raw()
        .is_some_and(|m| m.starts_with("video/"))
}
