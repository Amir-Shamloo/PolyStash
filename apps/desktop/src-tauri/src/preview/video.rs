use anyhow::{anyhow, Result};
use std::path::Path;
use tokio::process::Command;

use super::placeholder::generate_placeholder;

/// Extract a video frame with ffmpeg and write it to `output`.
///
/// ffmpeg is an external tool that may simply not be installed on a user
/// machine, so a missing binary degrades to the neutral placeholder and lets
/// the job complete — only a renderer that ran and failed is worth retrying.
pub(crate) async fn extract_video_thumb(source: &Path, output: &Path) -> Result<()> {
    if let Some(parent) = output.parent() {
        let _ = tokio::fs::create_dir_all(parent).await;
    }

    let out = match Command::new("ffmpeg")
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
        .await
    {
        Ok(out) => out,
        Err(e) if e.kind() == std::io::ErrorKind::NotFound => {
            tracing::warn!("ffmpeg is not installed; using a placeholder video thumbnail");
            return generate_placeholder(output);
        }
        Err(e) => return Err(anyhow!("Could not run ffmpeg: {e}")),
    };

    if !out.status.success() {
        tracing::warn!(
            "ffmpeg failed for {}: {}; using a placeholder thumbnail",
            source.display(),
            String::from_utf8_lossy(&out.stderr)
        );
        return generate_placeholder(output);
    }

    Ok(())
}

pub fn is_video(ext: &str) -> bool {
    mime_guess::from_ext(ext)
        .first_raw()
        .is_some_and(|m| m.starts_with("video/"))
}
