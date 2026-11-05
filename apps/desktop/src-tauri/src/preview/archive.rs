use anyhow::{anyhow, Result};
use image::DynamicImage;
use std::fs::File;
use std::io::Read;
use std::path::Path;

use super::ole::{dib_to_bmp, find_dib};
use super::{find_png, save_thumb};

/// Read at most `limit` bytes from the start of a file.
pub(crate) fn read_prefix(path: &Path, limit: u64) -> Result<Vec<u8>> {
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
pub(crate) fn decode_embedded_image(bytes: &[u8]) -> Option<DynamicImage> {
    decode_at(bytes, find_png)
        .or_else(|| decode_at(bytes, find_bmp))
        .or_else(|| decode_dib_at(bytes))
}

/// SketchUp stores newer models as ZIP containers with a thumbnail entry and
/// older ones as plain binaries with an embedded PNG.
pub(crate) fn extract_skp_thumb(source: &Path, output: &Path) -> Result<()> {
    let thumbs = extract_skp_thumbs(source)?;
    save_thumb(thumbs.first().ok_or_else(|| anyhow!("No SketchUp preview"))?, output)
}

/// Every embedded SketchUp preview, ordered as found in the archive.
pub(crate) fn extract_skp_thumbs(source: &Path) -> Result<Vec<DynamicImage>> {
    let images = skp_zip_thumbs(source).unwrap_or_default();
    if !images.is_empty() {
        return Ok(images);
    }

    let head = read_prefix(source, 1024 * 1024)?;
    let index = find_png(&head)
        .ok_or_else(|| anyhow!("No embedded thumbnail found in SketchUp file"))?;
    Ok(vec![image::load_from_memory(&head[index..])?])
}

fn skp_zip_thumbs(source: &Path) -> Result<Vec<DynamicImage>> {
    let file = File::open(source)?;
    let mut archive = zip::ZipArchive::new(file)?;
    let names: Vec<String> = archive
        .file_names()
        .filter_map(|n| n.ok().map(|s| s.into_owned()))
        .collect();

    let ordered = skp_thumbnail_entries(&names);
    let mut images = Vec::new();
    for name in ordered {
        let Ok(mut entry) = archive.by_name(&name) else {
            continue;
        };
        let mut bytes = Vec::new();
        if entry.read_to_end(&mut bytes).is_err() {
            continue;
        }
        if let Ok(img) = image::load_from_memory(&bytes) {
            images.push(img);
        }
    }
    Ok(images)
}

/// Preferred thumbnail entries first, then any remaining `thumb*` image, so a
/// container with several previews yields an ordered gallery.
fn skp_thumbnail_entries(names: &[String]) -> Vec<String> {
    const PREFERRED: [&str; 3] = [
        "meta/model_thumbnail.png",
        "meta/preview_thumbnail.png",
        "thumbnails/thumbnail.png",
    ];

    let mut ordered: Vec<String> = PREFERRED
        .iter()
        .filter_map(|preferred| names.iter().find(|name| name.eq_ignore_ascii_case(preferred)))
        .cloned()
        .collect();
    let extras: Vec<String> = names
        .iter()
        .filter(|name| is_thumbnail_name(name) && !ordered.iter().any(|kept| kept == *name))
        .cloned()
        .collect();
    ordered.extend(extras);
    ordered
}

fn is_thumbnail_name(name: &str) -> bool {
    let lower = name.to_ascii_lowercase();
    lower.contains("thumb") && (lower.ends_with(".png") || lower.ends_with(".jpg") || lower.ends_with(".jpeg"))
}

/// Substance archives are ZIP containers holding a rendered `preview.png`.
/// Falling back is fine: the frontend renders a PBR shader ball when no
/// backend thumbnail exists.
pub(crate) fn extract_sbsar_thumb(source: &Path, output: &Path) -> Result<()> {
    let img = sbsar_preview(source)
        .ok_or_else(|| anyhow!("No preview.png inside Substance archive"))?;
    save_thumb(&img, output)
}

fn sbsar_preview(source: &Path) -> Option<DynamicImage> {
    let file = File::open(source).ok()?;
    let mut archive = zip::ZipArchive::new(file).ok()?;
    let names: Vec<String> = archive
        .file_names()
        .filter_map(|n| n.ok().map(|s| s.into_owned()))
        .collect();
    let name = sbsar_preview_entry(&names)?;

    let mut entry = archive.by_name(&name).ok()?;
    let mut bytes = Vec::new();
    entry.read_to_end(&mut bytes).ok()?;
    image::load_from_memory(&bytes).ok()
}

fn sbsar_preview_entry(names: &[String]) -> Option<String> {
    names
        .iter()
        .find(|name| name.eq_ignore_ascii_case("preview.png"))
        .or_else(|| {
            names
                .iter()
                .find(|name| name.to_ascii_lowercase().ends_with(".png"))
        })
        .cloned()
}