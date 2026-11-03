use anyhow::Result;
use base64::{engine::general_purpose::STANDARD, Engine as _};
use image::{DynamicImage, Rgb, RgbImage};
use roxmltree::Document;
use std::path::{Path, PathBuf};

use super::image::to_u8;
use super::save_thumb;

/// V-Ray material thumbnails: companion image, embedded base64 preview,
/// referenced texture, or a generated sphere swatch.
pub(crate) fn extract_vrmat_thumb(source: &Path, output: &Path) -> Result<()> {
    if let Some(img) = companion_image(source).and_then(|path| image::open(path).ok()) {
        return save_thumb(&img, output);
    }

    let bytes = std::fs::read(source)?;
    let xml = String::from_utf8_lossy(&bytes);
    let doc = Document::parse(&xml)?;

    if let Some(img) = preview_image(&doc) {
        return save_thumb(&img, output);
    }
    if let Some(img) = referenced_texture(source, &doc) {
        return save_thumb(&img, output);
    }

    let color = material_color(&doc).unwrap_or([0.72, 0.74, 0.78]);
    render_swatch(color, output)
}

fn companion_image(source: &Path) -> Option<PathBuf> {
    const EXTENSIONS: [&str; 4] = ["jpg", "jpeg", "png", "webp"];
    const SUFFIXES: [&str; 3] = ["", "_preview", "_thumb"];

    let stem = source.file_stem()?.to_str()?;
    let dir = source.parent()?;
    for suffix in SUFFIXES {
        for extension in EXTENSIONS {
            let candidate = dir.join(format!("{stem}{suffix}.{extension}"));
            if candidate.is_file() {
                return Some(candidate);
            }
        }
    }
    None
}

fn preview_image(doc: &Document) -> Option<DynamicImage> {
    let node = doc.descendants().find(|node| node.has_tag_name("preview"))?;
    let text = node.text()?.trim();
    let payload = text.rsplit_once("base64,").map_or(text, |(_, value)| value);
    let encoded: String = payload.chars().filter(|c| !c.is_whitespace()).collect();
    let bytes = STANDARD.decode(encoded).ok()?;
    image::load_from_memory(&bytes).ok()
}

fn referenced_texture(source: &Path, doc: &Document) -> Option<DynamicImage> {
    let mut candidates: Vec<(u8, PathBuf)> = doc
        .descendants()
        .filter(|node| node.has_tag_name("parameter"))
        .filter_map(texture_candidate)
        .collect();
    candidates.sort_by_key(|(priority, _)| *priority);

    candidates
        .into_iter()
        .filter_map(|(_, reference)| resolve_texture(source, &reference))
        .find_map(|path| image::open(path).ok())
}

fn texture_candidate(node: roxmltree::Node) -> Option<(u8, PathBuf)> {
    let name = node.attribute("name")?.to_ascii_lowercase();
    if !has_any(&name, &["file", "bitmap", "filename"]) {
        return None;
    }

    let value = node.attribute("value").or_else(|| node.text())?.trim();
    if value.is_empty() {
        return None;
    }

    let context = node
        .ancestors()
        .filter_map(|ancestor| ancestor.attribute("name"))
        .collect::<Vec<_>>()
        .join(" ");
    let priority = texture_priority(&format!("{name} {context}").to_ascii_lowercase());
    Some((priority, PathBuf::from(value.replace('\\', "/"))))
}

fn texture_priority(context: &str) -> u8 {
    if has_any(context, &["normal", "bump", "gloss", "rough", "mask"]) {
        2
    } else if has_any(context, &["diffuse", "color", "colour", "albedo", "base"]) {
        0
    } else {
        1
    }
}

fn has_any(haystack: &str, needles: &[&str]) -> bool {
    needles.iter().any(|needle| haystack.contains(needle))
}

fn resolve_texture(source: &Path, reference: &Path) -> Option<PathBuf> {
    if reference.is_absolute() && reference.is_file() {
        return Some(reference.to_path_buf());
    }

    let dir = source.parent()?;
    let joined = dir.join(reference);
    if joined.is_file() {
        return Some(joined);
    }

    let sibling = dir.join(reference.file_name()?);
    sibling.is_file().then_some(sibling)
}

fn material_color(doc: &Document) -> Option<[f32; 3]> {
    color_from_parameter(doc).or_else(|| color_from_element(doc))
}

fn color_from_parameter(doc: &Document) -> Option<[f32; 3]> {
    doc.descendants()
        .filter(|node| node.has_tag_name("parameter"))
        .filter(|node| {
            node.attribute("name").is_some_and(|name| {
                has_any(&name.to_ascii_lowercase(), &["color", "colour", "diffuse"])
            })
        })
        .filter_map(|node| node.attribute("value").or_else(|| node.text()))
        .find_map(parse_color)
}

fn color_from_element(doc: &Document) -> Option<[f32; 3]> {
    let node = doc.descendants().find(|node| node.has_tag_name("color"))?;
    let channel = |key: &str| node.attribute(key)?.parse::<f32>().ok();
    Some(normalize_color([
        channel("r")?,
        channel("g")?,
        channel("b")?,
    ]))
}

fn normalize_color(mut rgb: [f32; 3]) -> [f32; 3] {
    if rgb.iter().any(|channel| *channel > 1.0) {
        for channel in &mut rgb {
            *channel /= 255.0;
        }
    }
    rgb.map(|channel| channel.clamp(0.0, 1.0))
}

fn parse_color(value: &str) -> Option<[f32; 3]> {
    let value = value.trim();
    if let Some(hex) = value.strip_prefix('#') {
        return parse_hex_color(hex);
    }

    let numbers: Vec<f32> = value
        .split(|c: char| c.is_whitespace() || c == ',')
        .filter(|part| !part.is_empty())
        .map(|part| part.parse::<f32>().ok())
        .collect::<Option<Vec<_>>>()?;
    if numbers.len() < 3 {
        return None;
    }

    Some(normalize_color([numbers[0], numbers[1], numbers[2]]))
}

fn parse_hex_color(hex: &str) -> Option<[f32; 3]> {
    if hex.len() != 6 {
        return None;
    }
    let channel = |index: usize| {
        u8::from_str_radix(&hex[index..index + 2], 16)
            .ok()
            .map(|value| value as f32 / 255.0)
    };
    Some([channel(0)?, channel(2)?, channel(4)?])
}

/// Shaded sphere swatch used when a material exposes no usable image.
fn render_swatch(color: [f32; 3], output: &Path) -> Result<()> {
    let size = 256u32;
    let mut img = RgbImage::from_pixel(size, size, Rgb([16, 18, 22]));
    let center = size as f32 * 0.5;
    let radius = size as f32 * 0.36;

    for y in 0..size {
        for x in 0..size {
            let dx = (x as f32 + 0.5 - center) / radius;
            let dy = (y as f32 + 0.5 - center) / radius;
            let distance_squared = dx * dx + dy * dy;
            if distance_squared > 1.0 {
                continue;
            }

            let z = (1.0 - distance_squared).sqrt();
            let light = (dx * 0.35 - dy * 0.45 + z * 0.82).max(0.0);
            let shade = 0.18 + 0.82 * light;
            img.put_pixel(x, y, Rgb(color.map(|channel| to_u8(channel * shade))));
        }
    }

    save_thumb(&DynamicImage::ImageRgb8(img), output)
}
