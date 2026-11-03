use anyhow::{anyhow, Result};
use image::DynamicImage;
use std::fs::File;
use std::io::Read;
use std::path::Path;

use super::ole::{dib_to_bmp, find_dib};
use super::{find_png, save_thumb};

/// Marker that precedes the preview block in classic AutoCAD DWG files.
const DWG_SENTINEL: [u8; 16] = [
    0x1F, 0x25, 0x6D, 0x07, 0xD4, 0x36, 0x28, 0x28, 0x9D, 0x57, 0xCA, 0x3F, 0x9D, 0x44, 0x10, 0x2B,
];

/// Read at most `limit` bytes from the start of a file.
fn read_prefix(path: &Path, limit: u64) -> Result<Vec<u8>> {
    let file = File::open(path)?;
    let mut buffer = Vec::new();
    file.take(limit).read_to_end(&mut buffer)?;
    Ok(buffer)
}

fn read_le_u32(bytes: &[u8], start: usize) -> u32 {
    u32::from_le_bytes([
        bytes[start],
        bytes[start + 1],
        bytes[start + 2],
        bytes[start + 3],
    ])
}

fn is_plausible_bmp_size(size: u32) -> bool {
    size == 0 || size >= 54
}

fn is_plausible_bmp_header(bytes: &[u8], index: usize) -> bool {
    let offset = read_le_u32(bytes, index + 10);
    let available = (bytes.len() - index) as u32;
    is_plausible_bmp_size(read_le_u32(bytes, index + 2)) && offset >= 54 && offset <= available
}

/// Locate a plausible `BM` bitmap file header inside a byte buffer.
fn find_bmp(bytes: &[u8]) -> Option<usize> {
    let mut index = 0;
    while index + 14 <= bytes.len() {
        if &bytes[index..index + 2] == b"BM" && is_plausible_bmp_header(bytes, index) {
            return Some(index);
        }
        index += 1;
    }
    None
}

fn decode_at(bytes: &[u8], finder: fn(&[u8]) -> Option<usize>) -> Option<DynamicImage> {
    let index = finder(bytes)?;
    image::load_from_memory(&bytes[index..]).ok()
}

fn decode_dib_at(bytes: &[u8]) -> Option<DynamicImage> {
    let index = find_dib(bytes)?;
    let bmp = dib_to_bmp(&bytes[index..]).ok()?;
    image::load_from_memory(&bmp).ok()
}

/// Decode the first embedded PNG, BMP, or bare DIB found in an image block.
fn decode_embedded_image(bytes: &[u8]) -> Option<DynamicImage> {
    decode_at(bytes, find_png)
        .or_else(|| decode_at(bytes, find_bmp))
        .or_else(|| decode_dib_at(bytes))
}

/// SketchUp stores newer models as ZIP containers with a thumbnail entry and
/// older ones as plain binaries with an embedded PNG.
pub(crate) fn extract_skp_thumb(source: &Path, output: &Path) -> Result<()> {
    if let Ok(img) = skp_zip_thumb(source) {
        return save_thumb(&img, output);
    }

    let head = read_prefix(source, 1024 * 1024)?;
    let index = find_png(&head)
        .ok_or_else(|| anyhow!("No embedded thumbnail found in SketchUp file"))?;
    let img = image::load_from_memory(&head[index..])?;
    save_thumb(&img, output)
}

fn skp_zip_thumb(source: &Path) -> Result<DynamicImage> {
    let file = File::open(source)?;
    let mut archive = zip::ZipArchive::new(file)?;
    let mut names: Vec<String> = Vec::new();
    for name in archive.file_names() {
        names.push(name?.into_owned());
    }
    let name = skp_thumbnail_entry(&names)
        .ok_or_else(|| anyhow!("No thumbnail entry in SketchUp archive"))?;

    let mut entry = archive.by_name(&name)?;
    let mut bytes = Vec::new();
    entry.read_to_end(&mut bytes)?;
    Ok(image::load_from_memory(&bytes)?)
}

fn skp_thumbnail_entry(names: &[String]) -> Option<String> {
    const PREFERRED: [&str; 3] = [
        "meta/model_thumbnail.png",
        "meta/preview_thumbnail.png",
        "thumbnails/thumbnail.png",
    ];

    for preferred in PREFERRED {
        if let Some(name) = names.iter().find(|name| name.eq_ignore_ascii_case(preferred)) {
            return Some(name.clone());
        }
    }

    names.iter().find(|name| is_thumbnail_name(name)).cloned()
}

fn is_thumbnail_name(name: &str) -> bool {
    let lower = name.to_ascii_lowercase();
    lower.contains("thumb") && (lower.ends_with(".png") || lower.ends_with(".jpg") || lower.ends_with(".jpeg"))
}

/// AutoCAD previews live behind a sentinel and may be PNG, BMP, or a raw DIB.
pub(crate) fn extract_dwg_thumb(source: &Path, output: &Path) -> Result<()> {
    let head = read_prefix(source, 8 * 1024 * 1024)?;

    if let Some(position) = find_subslice(&head, &DWG_SENTINEL) {
        let block = &head[position + DWG_SENTINEL.len()..];
        if let Some(img) = decode_embedded_image(block) {
            return save_thumb(&img, output);
        }
    }

    let fallback = &head[..head.len().min(4 * 1024 * 1024)];
    if let Some(index) = find_png(fallback) {
        let img = image::load_from_memory(&fallback[index..])?;
        return save_thumb(&img, output);
    }
    Err(anyhow!("No embedded thumbnail found in DWG file"))
}

fn find_subslice(haystack: &[u8], needle: &[u8]) -> Option<usize> {
    if needle.is_empty() || haystack.len() < needle.len() {
        return None;
    }
    haystack.windows(needle.len()).position(|window| window == needle)
}
