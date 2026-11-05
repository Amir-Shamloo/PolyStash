use anyhow::{anyhow, Result};
use std::io::Read;
use std::path::Path;

use super::{find_png, save_thumb};

/// Wrap a raw BITMAPINFOHEADER DIB (no file header) into a full BMP so the
/// `image` crate can decode it.
pub(crate) fn dib_to_bmp(dib: &[u8]) -> Result<Vec<u8>> {
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

/// Sane embedded-DIB dimensions (biHeight may be negative for top-down DIBs).
fn plausible_dimensions(w: u32, h: i32) -> bool {
    (1..=20000).contains(&w) && h != 0 && h.unsigned_abs() <= 20000
}

/// Locate a BITMAPINFOHEADER (biSize == 40) with sane dimensions inside a byte
/// buffer. Used to find embedded DIB thumbnails in OLE summary streams.
pub(crate) fn find_dib(bytes: &[u8]) -> Option<usize> {
    let mut i = 0;
    while i + 44 <= bytes.len() {
        if bytes[i..i + 4] == [0x28u8, 0, 0, 0] {
            let w = u32::from_le_bytes(bytes[i + 4..i + 8].try_into().ok()?);
            let h = i32::from_le_bytes(bytes[i + 8..i + 12].try_into().ok()?);
            if plausible_dimensions(w, h) {
                return Some(i);
            }
        }
        i += 1;
    }
    None
}

/// Extract the embedded OLE thumbnail from a 3ds Max (.max) or Revit (.rvt/.rfa/.rte)
/// compound file and write a resized thumbnail. Max stores a DIB in the
/// SummaryInformation stream; Revit stores a PNG in the `RevitPreview4.0` stream.
pub(crate) fn extract_ole_thumb(source: &Path, output: &Path) -> Result<()> {
    // OLE files are compound-binary; spurious files fail at open
    let mut cf = cfb::open(source)?;
    let mut raw = Vec::new();

    for name in ["RevitPreview4.0", "\u{5}SummaryInformation"] {
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
