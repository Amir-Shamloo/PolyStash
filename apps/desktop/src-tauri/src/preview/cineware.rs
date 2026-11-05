use anyhow::{anyhow, Result};
use std::path::Path;

use super::archive::read_prefix;
use super::save_thumb;

/// Cinema 4D previews sit in the IFF `FORM`/`MC4D` header block, usually as a
/// `THMB` chunk holding JPEG or PNG bytes. The header is the only part worth
/// reading, so the scan is bounded to the first 8 MB.
const C4D_HEADER_WINDOW: u64 = 8 * 1024 * 1024;

/// Blender stores a rendered preview near the start of the file as a plain PNG
/// chunk. This is a byte-scan heuristic, not a format parser: no usable preview
/// means the caller falls back to the neutral placeholder (and the frontend
/// renders the model live).
pub(crate) fn extract_preview_png_thumb(source: &Path, output: &Path) -> Result<()> {
    let head = read_prefix(source, 4 * 1024 * 1024)?;
    let index = super::find_png(&head)
        .ok_or_else(|| anyhow!("No embedded preview image found"))?;
    let img = image::load_from_memory(&head[index..])?;
    save_thumb(&img, output)
}

/// Cinema 4D (`.c4d`) thumbnail extraction. The file is an IFF container whose
/// `MC4D`/`FORM` header carries a `THMB` preview chunk encoded as JPEG or PNG.
/// This is a best-effort byte scan of the header window, not a chunk parser: a
/// miss returns `Err` so the caller can fall back to the neutral placeholder.
pub(crate) fn extract_c4d_thumb(source: &Path, output: &Path) -> Result<()> {
    let head = read_prefix(source, C4D_HEADER_WINDOW)?;

    if !has_c4d_container(&head) {
        tracing::warn!(
            "{} has no MC4D/FORM marker near the start; scanning for a preview anyway",
            source.display()
        );
    }

    let index = preview_signature(&head)
        .ok_or_else(|| anyhow!("No embedded C4D preview image found"))?;
    let img = image::load_from_memory(&head[index..])?;
    save_thumb(&img, output)
}

/// True when the container fourCC appears in the opening bytes of the file.
fn has_c4d_container(head: &[u8]) -> bool {
    let probe = head.get(..64).unwrap_or(head);
    probe.windows(4).any(|w| w == b"MC4D" || w == b"FORM")
}

/// Offset of the earliest preview signature: JPEG (`FF D8 FF`) start-of-image
/// or the PNG magic. JPEG wins ties only if it appears first.
fn preview_signature(bytes: &[u8]) -> Option<usize> {
    let jpeg = bytes.windows(3).position(|w| w == [0xFF, 0xD8, 0xFF]);
    let png = super::find_png(bytes);

    match (jpeg, png) {
        (Some(jpeg), Some(png)) => Some(jpeg.min(png)),
        (jpeg, png) => jpeg.or(png),
    }
}
