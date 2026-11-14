use anyhow::{anyhow, Result};
use image::DynamicImage;
use sevenz_rust2::{Password, SevenZReader};
use std::fs::File;
use std::io::Read;
use std::path::{Path, PathBuf};
use std::sync::OnceLock;
use std::time::Duration;

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

/// Entry-name ranks for the fallback scan, best first.
const SBSAR_ICON_NAME: u8 = 0;
const SBSAR_PREVIEW_NAME: u8 = 1;
const SBSAR_NAMED_IMAGE: u8 = 2;
const SBSAR_ANY_IMAGE: u8 = 3;

/// Directory holding the graph XML and the icon image it names.
const SBSAR_GRAPH_DIR: &str = "assemblies/content/0000/";

/// Substance archives are 7z containers holding a rendered preview image and a
/// graph description naming the icon to use. Falling back is fine: the frontend
/// renders a PBR shader ball when no backend thumbnail exists.
pub(crate) fn extract_sbsar_thumb(source: &Path, output: &Path) -> Result<()> {
    let img = sbsar_preview(source)
        .ok_or_else(|| anyhow!("No preview image inside Substance archive"))?;
    save_thumb(&img, output)
}

/// Graph-declared icon first, then the filename tiers ranked by
/// `sbsar_entry_priority`.
fn sbsar_preview(source: &Path) -> Option<DynamicImage> {
    sbsar_icon_preview(source).or_else(|| sbsar_heuristic_preview(source))
}

/// Preview named by the graph's `icon` attribute, when that entry decodes.
fn sbsar_icon_preview(source: &Path) -> Option<DynamicImage> {
    let icon = sbsar_icon_entry(source)?;
    let bytes = read_archive_entry(source, &icon)?;
    image::load_from_memory(&bytes).ok()
}

/// Archive path of the icon the first `<graph>` element declares.
fn sbsar_icon_entry(source: &Path) -> Option<String> {
    let xml = read_graph_xml(source)?;
    let icon = normalized_archive_name(graph_icon(&xml)?);
    let icon = icon.trim_start_matches("./");
    if icon.starts_with(SBSAR_GRAPH_DIR) {
        return Some(icon.to_string());
    }
    Some(format!("{SBSAR_GRAPH_DIR}{icon}"))
}

/// Graph description text: `assemblies/content/0000/*.xml`.
fn read_graph_xml(source: &Path) -> Option<String> {
    let mut archive = SevenZReader::open(source, Password::empty()).ok()?;
    let mut xml = None;

    let _ = archive.for_each_entries(|entry, input| {
        if !is_graph_xml(entry.name()) {
            return Ok(true);
        }
        let mut text = String::new();
        input.read_to_string(&mut text)?;
        xml = Some(text);
        Ok(false)
    });

    xml
}

fn is_graph_xml(name: &str) -> bool {
    let lower = normalized_archive_name(name);
    lower.starts_with(SBSAR_GRAPH_DIR) && lower.ends_with(".xml")
}

/// Value of `icon="..."` on the first `<graph>` element. The description is
/// small and one attribute does not justify an XML parser.
fn graph_icon(xml: &str) -> Option<&str> {
    let start = graph_element_start(xml)?;
    let tag_end = start + xml[start..].find('>')?;
    let tag = &xml[start..tag_end];
    let attribute = "icon=\"";
    let value = &tag[tag.find(attribute)? + attribute.len()..];
    let value = &value[..value.find('"')?];
    (!value.is_empty()).then_some(value)
}

/// `<graphs>` wraps the graph list, so skip it and match the element itself.
fn graph_element_start(xml: &str) -> Option<usize> {
    xml.match_indices("<graph").find_map(|(index, _)| {
        let after = xml[index + "<graph".len()..].chars().next()?;
        (after.is_ascii_whitespace() || after == '>').then_some(index)
    })
}

/// Case- and separator-insensitive archive entry name.
fn normalized_archive_name(name: &str) -> String {
    name.replace('\\', "/").to_ascii_lowercase()
}

/// Read one named entry, skipping earlier entries in the sequential archive.
fn read_archive_entry(source: &Path, wanted: &str) -> Option<Vec<u8>> {
    let wanted = normalized_archive_name(wanted);
    let mut archive = SevenZReader::open(source, Password::empty()).ok()?;
    let mut bytes: Option<Vec<u8>> = None;

    let _ = archive.for_each_entries(|entry, input| {
        if normalized_archive_name(entry.name()) != wanted {
            return Ok(true);
        }
        let mut data = Vec::new();
        input.read_to_end(&mut data)?;
        bytes = Some(data);
        Ok(false)
    });

    bytes
}

/// Priority heuristic used when the graph declares no usable icon. Substance
/// names its standard icon `icon.png` or `icon<uid>.png`.
fn sbsar_heuristic_preview(source: &Path) -> Option<DynamicImage> {
    let mut archive = SevenZReader::open(source, Password::empty()).ok()?;
    let mut best: Option<(u8, Vec<u8>)> = None;
    let mut found_best = false;
    let mut has_image = false;

    // Sevenz entries must be read in order, so every entry is drained and only
    // the best-ranked image is retained.
    let _ = archive.for_each_entries(|entry, input| {
        let priority = sbsar_entry_priority(entry.name());
        found_best |= priority == Some(SBSAR_ICON_NAME);
        has_image |= priority.is_some();

        let bytes = read_entry(input, priority.is_some())?;
        if let Some(priority) = priority {
            if best.as_ref().is_none_or(|(seen, _)| priority < *seen) {
                best = Some((priority, bytes));
            }
        }
        Ok(!found_best)
    });

    if !has_image {
        tracing::debug!("No image entry in Substance archive {}", source.display());
    }

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
    if is_icon_name(file_name) {
        Some(SBSAR_ICON_NAME)
    } else if file_name == "preview.png" {
        Some(SBSAR_PREVIEW_NAME)
    } else if lower.contains("preview") || lower.contains("thumb") {
        Some(SBSAR_NAMED_IMAGE)
    } else {
        Some(SBSAR_ANY_IMAGE)
    }
}

/// Substance's standard icon name: `icon.png` or `icon<uid>.png`.
fn is_icon_name(file_name: &str) -> bool {
    let Some(stem) = file_name.strip_suffix(".png") else {
        return false;
    };
    let Some(uid) = stem.strip_prefix("icon") else {
        return false;
    };
    uid.is_empty() || uid.bytes().all(|byte| byte.is_ascii_digit())
}

fn is_image_name(name: &str) -> bool {
    [".png", ".jpg", ".jpeg"]
        .into_iter()
        .any(|extension| name.ends_with(extension))
}

/// Helper generated by `build.rs`: the native Substance SDK renderer and the
/// two runtime DLLs embedded in the application binary (empty stub on
/// non-Windows builds or when the SDK libraries are missing).
mod embedded_helper {
    include!(concat!(env!("OUT_DIR"), "/sbsar_render_exe.rs"));
}

const SBSAR_RENDERER_EXE: &str = "sbsar_render.exe";
const SBSAR_LINKER_DLL: &str = "substance_linker.dll";
const SBSAR_SSE2_DLL: &str = "substance_sse2_blend.dll";
const SBSAR_HELPER_DIR: &str = "polystash_sbsar";

/// 2048x2048 renders are much slower than the old graph-default renders.
const SBSAR_RENDER_TIMEOUT: Duration = Duration::from_secs(420);

/// Native renders saturate the CPU for minutes, so only two helpers run at once.
const SBSAR_RENDER_CONCURRENCY: usize = 2;

static SBSAR_RENDERER: OnceLock<Option<PathBuf>> = OnceLock::new();
static SBSAR_RENDER_SLOTS: OnceLock<tokio::sync::Semaphore> = OnceLock::new();

/// Serializes the check-and-extract of the shared helper directory. Two
/// concurrent jobs can both find the directory stale and write the same files;
/// one would then run a half-written executable.
static SBSAR_HELPER_LOCK: OnceLock<std::sync::Mutex<()>> = OnceLock::new();

fn sbsar_render_slots() -> &'static tokio::sync::Semaphore {
    SBSAR_RENDER_SLOTS.get_or_init(|| tokio::sync::Semaphore::new(SBSAR_RENDER_CONCURRENCY))
}

/// True when `path` already holds exactly `bytes` (length check only).
pub(crate) fn file_matches(path: &Path, bytes: &[u8]) -> bool {
    std::fs::metadata(path).is_ok_and(|metadata| metadata.len() == bytes.len() as u64)
}

fn extract_sbsar_helper() -> Option<PathBuf> {
    let _guard = SBSAR_HELPER_LOCK
        .get_or_init(|| std::sync::Mutex::new(()))
        .lock()
        .unwrap_or_else(|poisoned| poisoned.into_inner());

    let exe_bytes = embedded_helper::SBSAR_RENDER_EXE?;
    let linker_bytes = embedded_helper::SBSAR_LINKER_DLL?;
    let sse2_bytes = embedded_helper::SBSAR_SSE2_DLL?;

    let dir = std::env::temp_dir().join(SBSAR_HELPER_DIR);
    let exe = dir.join(SBSAR_RENDERER_EXE);
    let linker = dir.join(SBSAR_LINKER_DLL);
    let sse2 = dir.join(SBSAR_SSE2_DLL);
    let ready = file_matches(&exe, exe_bytes)
        && file_matches(&linker, linker_bytes)
        && file_matches(&sse2, sse2_bytes);
    if ready {
        return Some(exe);
    }

    std::fs::create_dir_all(&dir).ok()?;
    std::fs::write(&exe, exe_bytes).ok()?;
    std::fs::write(&linker, linker_bytes).ok()?;
    std::fs::write(&sse2, sse2_bytes).ok()?;
    Some(exe)
}

/// Extract the embedded renderer once and return the executable path.
fn sbsar_renderer_exe() -> Option<PathBuf> {
    SBSAR_RENDERER.get_or_init(extract_sbsar_helper).clone()
}

/// Map a Substance output identifier onto the frontend's PBR slot names.
fn sbsar_map_key(identifier: &str) -> Option<&'static str> {
    const KEYS: [(&str, &str); 12] = [
        ("basecolor", "baseColor"),
        ("albedo", "baseColor"),
        ("diffuse", "baseColor"),
        ("normal", "normal"),
        ("roughness", "roughness"),
        ("metallic", "metallic"),
        ("metalness", "metallic"),
        ("height", "height"),
        ("displacement", "height"),
        ("opacity", "opacity"),
        ("alpha", "opacity"),
        ("color", "baseColor"),
    ];
    // Graphs name outputs `base_color`, `Base Color`, `baseColor`, ...
    let normalized: String = identifier
        .chars()
        .filter(|c| c.is_ascii_alphanumeric())
        .collect();
    KEYS.iter()
        .find(|(needle, _)| normalized.ends_with(needle))
        .map(|(_, key)| *key)
        .or_else(|| {
            let bare = strip_map_suffix(&normalized)?;
            KEYS.iter()
                .find(|(needle, _)| bare.ends_with(needle))
                .map(|(_, key)| *key)
        })
        .or_else(|| {
            KEYS.iter()
                .find(|(needle, _)| normalized.contains(needle))
                .map(|(_, key)| *key)
        })
}

/// Drop one trailing `texture`/`texturemap`/`map` token some renderers append.
/// Longest suffix first so `...texturemap` loses the whole token, not just `map`.
fn strip_map_suffix(normalized: &str) -> Option<&str> {
    ["texturemap", "texture", "map"]
        .into_iter()
        .find_map(|suffix| normalized.strip_suffix(suffix))
}

/// Maps already rendered for `file_stem`, read back from the output folder.
pub(crate) fn existing_sbsar_maps(thumb_dir: &Path, file_stem: &str) -> Vec<(String, String)> {
    collect_sbsar_maps(&thumb_dir.join(format!("{file_stem}_sbsar")), file_stem)
}

/// Sidecar recording which source file the rendered maps came from.
fn sbsar_stamp_path(thumb_dir: &Path, file_stem: &str) -> PathBuf {
    thumb_dir.join(format!("{file_stem}_sbsar.stamp"))
}

/// Fingerprint of the source archive: modification time plus size, both read
/// from a single `metadata` call so the freshness check stays cheap.
fn source_fingerprint(meta: &std::fs::Metadata) -> Option<String> {
    let modified = meta
        .modified()
        .ok()?
        .duration_since(std::time::UNIX_EPOCH)
        .ok()?;
    Some(format!("{}:{}", modified.as_nanos(), meta.len()))
}

/// True when the maps on disk were rendered from the current source file.
fn maps_are_current(thumb_dir: &Path, file_stem: &str, fingerprint: &str) -> bool {
    std::fs::read_to_string(sbsar_stamp_path(thumb_dir, file_stem))
        .is_ok_and(|stored| stored.trim() == fingerprint)
}

/// Delete the flat `<stem>_sbsar_*.png` copies written by the old layout.
fn clear_legacy_sbsar_maps(thumb_dir: &Path, file_stem: &str) {
    let prefix = format!("{file_stem}_sbsar_");
    let Ok(entries) = std::fs::read_dir(thumb_dir) else {
        return;
    };
    for entry in entries.flatten() {
        let name = entry.file_name();
        let Some(name) = name.to_str() else {
            continue;
        };
        if name.starts_with(&prefix) && name.ends_with(".png") {
            let _ = std::fs::remove_file(entry.path());
        }
    }
}

/// Maps already on disk when the sidecar says they match the current source.
fn cached_sbsar_maps(
    thumb_dir: &Path,
    file_stem: &str,
    fingerprint: Option<&str>,
) -> Option<Vec<(String, String)>> {
    let fingerprint = fingerprint?;
    if !maps_are_current(thumb_dir, file_stem, fingerprint) {
        return None;
    }
    let existing = existing_sbsar_maps(thumb_dir, file_stem);
    (!existing.is_empty()).then_some(existing)
}

/// Start each render from an empty output directory so a previous partial
/// render cannot contribute stale surfaces.
async fn prepare_sbsar_output(out_dir: &Path) -> Option<()> {
    let _ = tokio::fs::remove_dir_all(out_dir).await;
    tokio::fs::create_dir_all(out_dir).await.ok()?;
    Some(())
}

/// Run the native renderer. The helper writes PNGs incrementally into
/// `out_dir`, so a timeout still leaves whatever finished.
async fn run_sbsar_renderer(renderer: &Path, source: &Path, out_dir: &Path) -> bool {
    let Some(renderer) = renderer.to_str() else {
        return false;
    };
    let Ok(mut child) = super::spawn_command(renderer)
        .arg(source)
        .arg(out_dir)
        .kill_on_drop(true)
        .spawn()
    else {
        return false;
    };

    matches!(
        tokio::time::timeout(SBSAR_RENDER_TIMEOUT, child.wait()).await,
        Ok(Ok(status)) if status.success()
    )
}

/// Every rendered PNG in the SBSAR output folder that maps to a PBR slot,
/// published as `(slot, library-relative path)`. The folder itself is the
/// library copy, so no second file is written.
fn collect_sbsar_maps(out_dir: &Path, file_stem: &str) -> Vec<(String, String)> {
    let folder = format!("{file_stem}_sbsar");
    let Ok(entries) = std::fs::read_dir(out_dir) else {
        return Vec::new();
    };

    let mut maps: Vec<(String, String)> = entries
        .flatten()
        .filter_map(|entry| {
            let path = entry.path();
            let is_png = path
                .extension()
                .and_then(|ext| ext.to_str())
                .is_some_and(|ext| ext.eq_ignore_ascii_case("png"));
            if !is_png {
                return None;
            }

            // The helper names every PNG after the output's real identifier.
            let name = path.file_name()?.to_str()?;
            let identifier = path.file_stem()?.to_str()?;
            let key = sbsar_map_key(&identifier.to_ascii_lowercase())?;
            Some((key.to_string(), format!(".polystash/thumbs/{folder}/{name}")))
        })
        .collect();

    maps.sort();
    maps.dedup_by(|a, b| a.0 == b.0);
    maps
}

/// Render the PBR maps for an SBSAR, or return the ones already on disk when
/// they were rendered from the current source file.
pub(crate) async fn render_sbsar_maps(
    source: &Path,
    file_stem: &str,
    thumb_dir: &Path,
) -> Option<Vec<(String, String)>> {
    // Migrate libraries written by the old flat-copy layout even when the
    // cached maps are already current.
    clear_legacy_sbsar_maps(thumb_dir, file_stem);

    // Cheap freshness check: one `metadata` call plus a small sidecar read.
    let fingerprint = std::fs::metadata(source)
        .ok()
        .and_then(|meta| source_fingerprint(&meta));

    if let Some(maps) = cached_sbsar_maps(thumb_dir, file_stem, fingerprint.as_deref()) {
        return Some(maps);
    }

    let renderer = sbsar_renderer_exe()?;
    let out_dir = thumb_dir.join(format!("{file_stem}_sbsar"));

    let Ok(_render_slot) = sbsar_render_slots().acquire().await else {
        return None;
    };
    prepare_sbsar_output(&out_dir).await?;

    let success = run_sbsar_renderer(&renderer, source, &out_dir).await;
    let maps = collect_sbsar_maps(&out_dir, file_stem);
    if maps.is_empty() {
        return None;
    }

    // Written last: a crash mid-render leaves no valid stamp and re-renders.
    if success {
        if let Some(fingerprint) = fingerprint {
            let _ = tokio::fs::write(sbsar_stamp_path(thumb_dir, file_stem), fingerprint).await;
        }
    }
    Some(maps)
}
