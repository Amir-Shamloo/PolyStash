use anyhow::{anyhow, Result};
use serde_json::{json, Map, Value};
use std::io::ErrorKind;
use std::path::{Path, PathBuf};
use std::process::{Output, Stdio};
use tokio::process::Command;

/// External page-1 rasterizers, in preference order. Branch (b) of the contract:
/// no PDF crate is pulled in, an existing system renderer is reused instead.
const PDF_RENDERERS: [&str; 2] = ["pdftoppm", "mutool"];

/// Render page 1 of a PDF. Returns `Err` when no renderer is installed so the
/// caller can fall back to the neutral placeholder and still complete the job.
pub(crate) async fn extract_pdf_thumb(source: &Path, output: &Path) -> Result<()> {
    if let Some(parent) = output.parent() {
        let _ = tokio::fs::create_dir_all(parent).await;
    }

    let mut last_error = None;
    for renderer in PDF_RENDERERS {
        match render_page(renderer, source, output)
            .await
            .and_then(|()| normalize_page(output))
        {
            Ok(()) => return Ok(()),
            Err(e) => last_error = Some(e.to_string()),
        }
    }

    Err(anyhow!(
        "PDF thumbnail generation failed: {}",
        last_error.unwrap_or_else(|| "no renderer installed".to_string())
    ))
}

async fn render_page(renderer: &str, source: &Path, output: &Path) -> Result<()> {
    if renderer == "pdftoppm" {
        run_pdftoppm(source, output).await
    } else {
        run_mutool(source, output).await
    }
}

async fn run_pdftoppm(source: &Path, output: &Path) -> Result<()> {
    let prefix = output.with_extension("");
    let out = Command::new("pdftoppm")
        .arg("-png")
        .args(["-f", "1", "-l", "1", "-scale-to"])
        .arg(super::THUMB_SIZE.to_string())
        .arg(source)
        .arg(&prefix)
        .stdin(Stdio::null())
        .output()
        .await
        .map_err(|e| command_error("pdftoppm", e))?;
    ensure_success("pdftoppm", &out)?;

    // pdftoppm names the file <prefix>-1.png; move it to the expected path.
    let produced = page_one_path(&prefix);
    if !produced.exists() {
        return Err(anyhow!("pdftoppm produced no page image"));
    }
    let _ = tokio::fs::remove_file(output).await;
    tokio::fs::rename(&produced, output).await?;
    Ok(())
}

async fn run_mutool(source: &Path, output: &Path) -> Result<()> {
    let width = super::THUMB_SIZE.to_string();
    let out = Command::new("mutool")
        .args(["draw", "-o"])
        .arg(output)
        .args(["-w", &width])
        .arg(source)
        .arg("1")
        .stdin(Stdio::null())
        .output()
        .await
        .map_err(|e| command_error("mutool", e))?;
    ensure_success("mutool", &out)?;

    if !output.exists() {
        return Err(anyhow!("mutool produced no page image"));
    }
    Ok(())
}

fn page_one_path(prefix: &Path) -> PathBuf {
    let name = prefix
        .file_name()
        .and_then(|n| n.to_str())
        .unwrap_or("page");
    prefix.with_file_name(format!("{name}-1.png"))
}

fn command_error(renderer: &str, error: std::io::Error) -> anyhow::Error {
    if error.kind() == ErrorKind::NotFound {
        anyhow!("{renderer} is not installed")
    } else {
        anyhow!("could not run {renderer}: {error}")
    }
}

fn ensure_success(renderer: &str, out: &Output) -> Result<()> {
    if out.status.success() {
        return Ok(());
    }
    Err(anyhow!(
        "{renderer} failed: {}",
        String::from_utf8_lossy(&out.stderr).trim()
    ))
}

/// Re-encode the rendered page so the stored thumbnail fits `THUMB_SIZE` on its
/// longest edge regardless of page orientation or which renderer produced it.
fn normalize_page(output: &Path) -> Result<()> {
    let img = image::open(output)?;
    super::save_thumb(&img, output)
}

/// Page count, page size, and authorship from `pdfinfo`, when it is installed.
/// A missing tool is not an error; the preview itself does not depend on it.
pub(crate) async fn pdf_metadata(source: &Path) -> Option<Value> {
    let out = Command::new("pdfinfo")
        .arg(source)
        .stdin(Stdio::null())
        .output()
        .await
        .ok()?;
    if !out.status.success() {
        return None;
    }

    let text = String::from_utf8_lossy(&out.stdout);
    let mut fields = Map::new();
    for line in text.lines() {
        let Some((key, value)) = line.split_once(':') else {
            continue;
        };
        insert_pdf_field(&mut fields, key.trim(), value.trim());
    }

    (!fields.is_empty()).then(|| Value::Object(fields))
}

fn insert_pdf_field(fields: &mut Map<String, Value>, key: &str, value: &str) {
    if value.is_empty() {
        return;
    }

    match key {
        "Pages" => {
            if let Ok(pages) = value.parse::<u64>() {
                fields.insert("pages".to_string(), json!(pages));
            }
        }
        "Page size" => {
            fields.insert("pageSize".to_string(), json!(value));
        }
        "Author" => {
            fields.insert("author".to_string(), json!(value));
        }
        "Creator" => {
            fields.insert("creator".to_string(), json!(value));
        }
        "Producer" => {
            fields.insert("producer".to_string(), json!(value));
        }
        _ => {}
    }
}
