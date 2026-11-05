use anyhow::{anyhow, Result};
use std::path::Path;

use super::archive::{decode_embedded_image, read_prefix};
use super::material::decode_base64;
use super::save_thumb;

/// `.dxf` is ASCII/UTF-8 in practice; AutoCAD writes either a bare image or a
/// base64 `<thumbnail>` block into the header section. Scan the first 4 MB.
pub(crate) fn extract_dxf_thumb(source: &Path, output: &Path) -> Result<()> {
    let head = read_prefix(source, 4 * 1024 * 1024)?;

    if let Some(img) = decode_embedded_image(&head) {
        return save_thumb(&img, output);
    }

    if let Some(payload) = dxf_base64_payload(&head) {
        if let Some(bytes) = decode_base64(&payload) {
            if let Some(img) = decode_embedded_image(&bytes) {
                return save_thumb(&img, output);
            }
        }
    }

    Err(anyhow!("No embedded thumbnail found in DXF file"))
}

/// AutoCAD's DXF preview header ends with a `thumbnail` group name followed by a
/// long base64 line. Take the first long base64 run after that marker.
fn dxf_base64_payload(bytes: &[u8]) -> Option<String> {
    let text = String::from_utf8_lossy(bytes);
    let marker = text.to_ascii_lowercase().find("thumbnail")?;
    let tail = &text[marker..];

    let payload: String = tail
        .chars()
        .skip_while(|c| *c != '\n')
        .skip(1)
        .take_while(|c| c.is_ascii_alphanumeric() || matches!(c, '+' | '/' | '='))
        .collect();

    (payload.len() > 64).then_some(payload)
}
