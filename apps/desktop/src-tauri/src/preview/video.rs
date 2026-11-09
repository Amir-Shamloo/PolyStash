use anyhow::{anyhow, Result};
use serde_json::{json, Map, Value};
use std::path::Path;
use std::process::Stdio;
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
        .stdin(Stdio::null())
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

/// Codec, resolution, duration, frame rate, and bitrate from `ffprobe`.
/// Returns `None` when ffprobe is not installed or cannot parse the file, which
/// leaves the asset without video metadata instead of failing the preview.
pub(crate) async fn probe_metadata(source: &Path) -> Option<Value> {
    let out = Command::new("ffprobe")
        .args([
            "-v",
            "error",
            "-print_format",
            "json",
            "-show_format",
            "-show_streams",
        ])
        .arg(source)
        .stdin(Stdio::null())
        .output()
        .await
        .ok()?;
    if !out.status.success() {
        return None;
    }

    let probe: Value = serde_json::from_slice(&out.stdout).ok()?;
    let stream = probe
        .get("streams")?
        .as_array()?
        .iter()
        .find(|stream| stream.get("codec_type").and_then(Value::as_str) == Some("video"))?;
    let format = probe.get("format");

    let mut fields = Map::new();
    insert_number(&mut fields, "width", stream.get("width"));
    insert_number(&mut fields, "height", stream.get("height"));
    insert_number(
        &mut fields,
        "duration",
        format
            .and_then(|format| format.get("duration"))
            .or_else(|| stream.get("duration")),
    );
    insert_number(
        &mut fields,
        "bitRate",
        format.and_then(|format| format.get("bit_rate")),
    );
    insert_float(
        &mut fields,
        "frameRate",
        stream.get("r_frame_rate").and_then(frame_rate),
    );
    insert_number(&mut fields, "frameCount", stream.get("nb_frames"));

    if let Some(codec) = stream.get("codec_name").and_then(Value::as_str) {
        fields.insert("codec".to_string(), json!(codec));
    }

    Some(Value::Object(fields))
}

fn insert_number(fields: &mut Map<String, Value>, key: &str, value: Option<&Value>) {
    insert_float(fields, key, numeric(value));
}

fn insert_float(fields: &mut Map<String, Value>, key: &str, value: Option<f64>) {
    if let Some(number) = value {
        fields.insert(key.to_string(), json!(number));
    }
}

fn numeric(value: Option<&Value>) -> Option<f64> {
    match value? {
        Value::Number(number) => number.as_f64(),
        Value::String(text) => text.parse().ok(),
        _ => None,
    }
}

/// ffprobe reports frame rate as a rational string such as `30000/1001`.
fn frame_rate(value: &Value) -> Option<f64> {
    let (numerator, denominator) = value.as_str()?.split_once('/')?;
    let numerator: f64 = numerator.parse().ok()?;
    let denominator: f64 = denominator.parse().ok()?;
    (denominator > 0.0).then(|| ((numerator / denominator) * 100.0).round() / 100.0)
}
