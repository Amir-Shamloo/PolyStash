use anyhow::{anyhow, Result};
use std::path::Path;
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

    let stem = output.with_extension("");
    let mut last_error = None;

    for renderer in PDF_RENDERERS {
        let result = match renderer {
            // pdftoppm writes <prefix>-1.png next to the prefix.
            "pdftoppm" => run_pdftoppm(source, &stem).await,
            _ => run_mutool(source, output).await,
        };

        match result {
            Ok(()) if output.exists() => return Ok(()),
            Ok(()) => last_error = Some(format!("{renderer} produced no output")),
            Err(e) => last_error = Some(e.to_string()),
        }
    }

    Err(anyhow!(
        "No PDF renderer available: {}",
        last_error.unwrap_or_else(|| "none installed".to_string())
    ))
}

async fn run_pdftoppm(source: &Path, prefix: &Path) -> Result<()> {
    let out = Command::new("pdftoppm")
        .arg("-png")
        .arg("-f")
        .arg("1")
        .arg("-l")
        .arg("1")
        .arg("-scale-to")
        .arg(super::THUMB_SIZE.to_string())
        .arg(source)
        .arg(prefix)
        .stdin(std::process::Stdio::null())
        .output()
        .await?;

    if !out.status.success() {
        return Err(anyhow!(
            "pdftoppm failed: {}",
            String::from_utf8_lossy(&out.stderr)
        ));
    }

    // pdftoppm names the file <prefix>-1.png; rename it to the expected path.
    let produced = prefix.with_file_name(format!(
        "{}-1.png",
        prefix
            .file_name()
            .and_then(|name| name.to_str())
            .unwrap_or("page")
    ));
    if produced.exists() {
        tokio::fs::rename(&produced, prefix.with_extension("png")).await?;
    }
    Ok(())
}

async fn run_mutool(source: &Path, output: &Path) -> Result<()> {
    let out = Command::new("mutool")
        .arg("draw")
        .args(["-o"])
        .arg(output)
        .args(["-w"])
        .arg(super::THUMB_SIZE.to_string())
        .arg(source)
        .arg("1")
        .stdin(std::process::Stdio::null())
        .output()
        .await?;

    if out.status.success() {
        Ok(())
    } else {
        Err(anyhow!(
            "mutool failed: {}",
            String::from_utf8_lossy(&out.stderr)
        ))
    }
}
