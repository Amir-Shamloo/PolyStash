use anyhow::{anyhow, Result};
use serde_json::{json, Map, Value};
use std::path::Path;
use std::process::Stdio;

use super::{aspect_ratio, round_two, structural};

/// Extract a video frame with ffmpeg and write it to `output`.
///
/// ffmpeg is an external tool that may simply not be installed on a user
/// machine; either way the caller completes the job without a thumbnail and
/// the frontend shows the video's kind icon.
pub(crate) async fn extract_video_thumb(source: &Path, output: &Path) -> Result<()> {
    if let Some(parent) = output.parent() {
        let _ = tokio::fs::create_dir_all(parent).await;
    }

    let out = super::spawn_command("ffmpeg")
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
        .map_err(|e| anyhow!("Could not run ffmpeg: {e}"))?;

    if !out.status.success() {
        return Err(anyhow!(
            "ffmpeg failed for {}: {}",
            source.display(),
            String::from_utf8_lossy(&out.stderr)
        ));
    }

    Ok(())
}

pub fn is_video(ext: &str) -> bool {
    mime_guess::from_ext(ext)
        .first_raw()
        .is_some_and(|m| m.starts_with("video/"))
}

/// Everything the details panel shows for a video: the `ffprobe` stream fields
/// plus the container headers read from the file itself.
///
/// ffprobe is optional, so the structural parse still returns metadata when it
/// is missing. `None` means neither source produced a field.
pub(crate) async fn metadata(source: &Path) -> Option<Value> {
    let mut fields = probe_metadata(source)
        .await
        .and_then(|probe| probe.as_object().cloned())
        .unwrap_or_default();
    fields.extend(structural::extract(source));
    (!fields.is_empty()).then_some(Value::Object(fields))
}

/// Codec, resolution, duration, frame rate, and bitrate from `ffprobe`.
/// Returns `None` when ffprobe is not installed or cannot parse the file, which
/// leaves the asset without video metadata instead of failing the preview.
pub(crate) async fn probe_metadata(source: &Path) -> Option<Value> {
    let out = super::spawn_command("ffprobe")
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
    let width = numeric(stream.get("width"));
    let height = numeric(stream.get("height"));
    insert_float(&mut fields, "width", width);
    insert_float(&mut fields, "height", height);
    insert_float(
        &mut fields,
        "aspectRatio",
        width.zip(height).and_then(|(w, h)| aspect_ratio(w, h)),
    );
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
    insert_string(&mut fields, "profile", stream.get("profile"));
    insert_string(&mut fields, "pixelFormat", stream.get("pix_fmt"));
    insert_string(&mut fields, "colorSpace", stream.get("color_space"));
    insert_number(&mut fields, "level", stream.get("level"));

    if let Some(audio) = probe
        .get("streams")
        .and_then(Value::as_array)
        .and_then(|streams| streams.iter().find(|s| s.get("codec_type").and_then(Value::as_str) == Some("audio")))
    {
        insert_string(&mut fields, "audioCodec", audio.get("codec_name"));
        insert_string(&mut fields, "audioSampleRate", audio.get("sample_rate"));
        insert_number(&mut fields, "audioChannels", audio.get("channels"));
        insert_string(&mut fields, "audioChannelLayout", audio.get("channel_layout"));
    }

    // Every container tag (title, artist, creation time, encoder, ...) is part of
    // the file's metadata, so expose the whole set rather than a hand-picked few.
    if let Some(tags) = format
        .and_then(|format| format.get("tags"))
        .and_then(Value::as_object)
    {
        for (key, value) in tags {
            if let Some(text) = value.as_str() {
                if !text.trim().is_empty() {
                    fields.insert(key.to_ascii_lowercase(), json!(text));
                }
            }
        }
    }

    Some(Value::Object(fields))
}

fn insert_string(fields: &mut Map<String, Value>, key: &str, value: Option<&Value>) {
    if let Some(text) = value.and_then(Value::as_str) {
        if !text.is_empty() {
            fields.insert(key.to_string(), json!(text));
        }
    }
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
    (denominator > 0.0).then(|| round_two(numerator / denominator))
}
