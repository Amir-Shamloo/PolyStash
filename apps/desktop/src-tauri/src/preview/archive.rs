use anyhow::{anyhow, Result};
use image::DynamicImage;
use sevenz_rust2::{Password, SevenZReader};
use std::fs::File;
use std::io::Read;
use std::path::{Path, PathBuf};
use std::sync::OnceLock;

use super::ole::{dib_offsets, dib_to_bmp};
use super::{find_png, save_thumb};

/// Upper bound on embedded-DIB candidates decoded before giving up.
const MAX_DIB_CANDIDATES: usize = 8;

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
pub(crate) fn find_bmp(bytes: &[u8]) -> Option<usize> {
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
    dib_offsets(bytes)
        .take(MAX_DIB_CANDIDATES)
        .find_map(|index| decode_dib(&bytes[index..]))
}

fn decode_dib(dib: &[u8]) -> Option<DynamicImage> {
    let bmp = dib_to_bmp(dib).ok()?;
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

/// Entry-name rank for the Substance preview: exact `preview.png` first.
const SBSAR_PREFERRED: u8 = 0;

/// Substance archives are 7z containers holding a rendered preview image.
/// Falling back is fine: the frontend renders a PBR shader ball when no
/// backend thumbnail exists.
pub(crate) fn extract_sbsar_thumb(source: &Path, output: &Path) -> Result<()> {
    let img = sbsar_preview(source)
        .ok_or_else(|| anyhow!("No preview image inside Substance archive"))?;
    save_thumb(&img, output)
}

fn sbsar_preview(source: &Path) -> Option<DynamicImage> {
    let mut archive = SevenZReader::open(source, Password::empty()).ok()?;
    let mut best: Option<(u8, Vec<u8>)> = None;
    let mut found_preferred = false;

    // Sevenz entries must be read in order, so every entry is drained and only
    // the best-ranked image is retained.
    let _ = archive.for_each_entries(|entry, input| {
        let priority = sbsar_entry_priority(entry.name());
        found_preferred |= priority == Some(SBSAR_PREFERRED);

        let bytes = read_entry(input, priority.is_some())?;
        if let Some(priority) = priority {
            if best.as_ref().is_none_or(|(seen, _)| priority < *seen) {
                best = Some((priority, bytes));
            }
        }
        Ok(!found_preferred)
    });

    let (_, bytes) = best?;
    image::load_from_memory(&bytes).ok()
}

/// Read a candidate entry into memory; drain every other entry so a solid
/// archive stays sequential without keeping unrelated data around.
fn read_entry(
    input: &mut dyn Read,
    keep: bool,
) -> std::result::Result<Vec<u8>, sevenz_rust2::Error> {
    let mut bytes = Vec::new();
    if keep {
        input.read_to_end(&mut bytes)?;
    } else {
        std::io::copy(input, &mut std::io::sink())?;
    }
    Ok(bytes)
}

fn sbsar_entry_priority(name: &str) -> Option<u8> {
    let lower = name.to_ascii_lowercase();
    if !is_image_name(&lower) {
        return None;
    }

    let file_name = lower.rsplit(['/', '\\']).next().unwrap_or_default();
    if file_name == "preview.png" {
        Some(SBSAR_PREFERRED)
    } else if lower.contains("preview") || lower.contains("thumb") {
        Some(1)
    } else {
        Some(2)
    }
}

fn is_image_name(name: &str) -> bool {
    [".png", ".jpg", ".jpeg"]
        .into_iter()
        .any(|extension| name.ends_with(extension))
}

/// The Adobe Substance SDK's prebuilt renderer (tutorial_00_basic) renders every
/// graph output to DDS in its working directory. It is embedded so the release
/// always ships a working SBSAR renderer without an SDK install.
const SBSAR_RENDERER_EXE: &[u8] = include_bytes!("../../bin/sbsar/sbsar_preview.exe");
const SBSAR_LINKER_DLL: &[u8] = include_bytes!("../../bin/sbsar/substance_linker.dll");
const SBSAR_SSE2_DLL: &[u8] = include_bytes!("../../bin/sbsar/substance_sse2_blend.dll");

static SBSAR_RENDERER: OnceLock<Option<PathBuf>> = OnceLock::new();

/// Extract the embedded renderer once and return the executable path.
fn sbsar_renderer_exe() -> Option<PathBuf> {
    SBSAR_RENDERER
        .get_or_init(|| {
            let dir = std::env::temp_dir().join("polystash_sbsar");
            let exe = dir.join("sbsar_preview.exe");
            let ready = exe.is_file()
                && dir.join("substance_linker.dll").is_file()
                && dir.join("substance_sse2_blend.dll").is_file();
            if ready {
                return Some(exe);
            }
            std::fs::create_dir_all(&dir).ok()?;
            std::fs::write(&exe, SBSAR_RENDERER_EXE).ok()?;
            std::fs::write(dir.join("substance_linker.dll"), SBSAR_LINKER_DLL).ok()?;
            std::fs::write(dir.join("substance_sse2_blend.dll"), SBSAR_SSE2_DLL).ok()?;
            Some(exe)
        })
        .clone()
}

/// Map a Substance output identifier onto the frontend's PBR slot names.
fn sbsar_map_key(identifier: &str) -> Option<&'static str> {
    const KEYS: [(&str, &str); 8] = [
        ("basecolor", "baseColor"),
        ("diffuse", "baseColor"),
        ("normal", "normal"),
        ("roughness", "roughness"),
        ("metallic", "metallic"),
        ("height", "height"),
        ("displacement", "height"),
        ("opacity", "opacity"),
    ];
    KEYS.iter()
        .find(|(needle, _)| identifier.contains(needle))
        .map(|(_, key)| *key)
}

/// Render PBR maps with the Substance SDK and convert them to PNG. Returns the
/// PBR slot and library-relative path for every map the renderer produced.
pub(crate) async fn render_sbsar_maps(
    source: &Path,
    file_stem: &str,
    thumb_dir: &Path,
) -> Option<Vec<(String, String)>> {
    let renderer = sbsar_renderer_exe()?.to_str()?.to_string();
    let out_dir = thumb_dir.join(format!("{file_stem}_sbsar"));
    tokio::fs::create_dir_all(&out_dir).await.ok()?;

    // The renderer writes `output_<graph>_<output>[<uid>].dds` into its working directory.
    let status = super::spawn_command(&renderer)
        .arg(source)
        .current_dir(&out_dir)
        .status()
        .await
        .ok()?;
    if !status.success() {
        return None;
    }

    let mut maps = Vec::new();
    let mut entries = tokio::fs::read_dir(&out_dir).await.ok()?;
    while let Some(entry) = entries.next_entry().await.ok()? {
        let path = entry.path();
        if path.extension().and_then(|e| e.to_str()) != Some("dds") {
            continue;
        }
        let name = path.file_name()?.to_str()?.to_ascii_lowercase();
        let key = sbsar_map_key(&name)?.to_string();
        let png_name = format!("{file_stem}_sbsar_{key}.png");
        let bytes = tokio::fs::read(&path).await.ok()?;
        let image = image::load_from_memory_with_format(&bytes, image::ImageFormat::Dds).ok()?;
        image.save(thumb_dir.join(&png_name)).ok()?;
        maps.push((key, format!(".polystash/thumbs/{png_name}")));
    }

    maps.sort_by(|a, b| a.0.cmp(&b.0));
    maps.dedup_by(|a, b| a.0 == b.0);
    (!maps.is_empty()).then_some(maps)
}
