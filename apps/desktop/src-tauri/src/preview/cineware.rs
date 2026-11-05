use anyhow::{anyhow, Result};
use std::path::Path;

use super::archive::read_prefix;
use super::save_thumb;

/// Cinema 4D (`.c4d`) and Blender (`.blend`) both store a rendered preview near
/// the start of the file as a plain PNG chunk. This is a byte-scan heuristic,
/// not a format parser: no usable preview means the caller falls back to the
/// neutral placeholder (and the frontend renders the model live).
pub(crate) fn extract_preview_png_thumb(source: &Path, output: &Path) -> Result<()> {
    let head = read_prefix(source, 4 * 1024 * 1024)?;
    let index = super::find_png(&head)
        .ok_or_else(|| anyhow!("No embedded preview image found"))?;
    let img = image::load_from_memory(&head[index..])?;
    save_thumb(&img, output)
}
