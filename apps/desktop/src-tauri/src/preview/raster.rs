use anyhow::Result;
use image::{DynamicImage, ExtendedColorType, ImageDecoder, ImageReader, Rgb, RgbImage};
use serde_json::{json, Map, Value};
use std::path::Path;

use super::{aspect_ratio, round_two, save_thumb, structural};

/// Checks if an extension is a standard raster image that Rust can decode directly.
///
/// AVIF is deliberately absent: the `image` crate's `avif` feature is an encoder
/// only, so AVIF is decoded by the frontend instead of failing here.
pub fn is_native_image(ext: &str) -> bool {
    matches!(
        ext.to_lowercase().as_str(),
        "jpg" | "jpeg" | "png" | "bmp" | "gif" | "tif" | "tiff" | "tga" | "webp" | "dds"
    )
}

/// In-process image downsampling using the native Rust `image` crate.
/// Uses Triangle (bilinear) filtering for 4x–8x faster performance than Lanczos3.
///
/// GIF decodes to its first frame; the frontend animates the full file on the
/// card and preview panel.
pub fn generate_image_thumb(source: &Path, output: &Path) -> Result<()> {
    let img = image::open(source)?;
    save_thumb(&img, output)
}

/// Technical metadata for a raster image: no pixel decode.
///
/// Merges the decoded description (`width`, `height`, `aspectRatio`,
/// `megapixels`, `bitDepth`, `colorSpace`), the file's own structural headers
/// (JPEG markers, PNG chunks), and its EXIF tags. A decoder that cannot read
/// the file still leaves the structural and EXIF fields in place.
pub(crate) fn image_metadata(source: &Path, ext: &str) -> Option<Value> {
    let mut fields = decoded_metadata(source, ext).unwrap_or_default();
    fields.extend(structural::extract(source));
    exif_metadata(source, &mut fields);
    (!fields.is_empty()).then_some(Value::Object(fields))
}

/// Fields the `image` decoder derives from the file header.
fn decoded_metadata(source: &Path, ext: &str) -> Option<Map<String, Value>> {
    let reader = ImageReader::open(source).ok()?.with_guessed_format().ok()?;
    let decoder = reader.into_decoder().ok()?;
    let (width, height) = decoder.dimensions();
    let original = decoder.original_color_type();

    let mut fields = Map::new();
    fields.insert("width".to_string(), json!(width));
    fields.insert("height".to_string(), json!(height));
    if let Some(ratio) = aspect_ratio(f64::from(width), f64::from(height)) {
        fields.insert("aspectRatio".to_string(), json!(ratio));
    }
    fields.insert("megapixels".to_string(), json!(megapixels(width, height)));
    fields.insert("bitDepth".to_string(), json!(bits_per_channel(original)));
    fields.insert("colorSpace".to_string(), json!(color_space(original, ext)));
    Some(fields)
}

/// EXIF and other in-file tags, exposed with normalized `lower_snake` keys so
/// the details panel can list the file's full metadata.
fn exif_metadata(source: &Path, fields: &mut Map<String, Value>) {
    let Ok(file) = std::fs::File::open(source) else {
        return;
    };
    let mut reader = std::io::BufReader::new(file);
    let Ok(exif) = exif::Reader::new().read_from_container(&mut reader) else {
        return;
    };

    for field in exif.fields() {
        let value = field.display_value().to_string();
        if value.is_empty() {
            continue;
        }
        fields.insert(normalize_tag(field.tag), json!(value));
    }
}

fn normalize_tag(tag: exif::Tag) -> String {
    tag.to_string()
        .to_ascii_lowercase()
        .split_whitespace()
        .collect::<Vec<_>>()
        .join("_")
}

/// Sensor-style megapixel count, rounded to two decimals.
fn megapixels(width: u32, height: u32) -> f64 {
    round_two(f64::from(width) * f64::from(height) / 1_000_000.0)
}

fn bits_per_channel(color: ExtendedColorType) -> u16 {
    let channels = u16::from(color.channel_count().max(1));
    color.bits_per_pixel() / channels
}

/// Human-readable colour space; floats from EXR/HDR are linear, not sRGB.
fn color_space(color: ExtendedColorType, ext: &str) -> &'static str {
    if matches!(ext, "exr" | "hdr" | "hdri") {
        return "linear";
    }

    match color {
        ExtendedColorType::L1
        | ExtendedColorType::L2
        | ExtendedColorType::L4
        | ExtendedColorType::L8
        | ExtendedColorType::L16 => "grayscale",
        ExtendedColorType::La1
        | ExtendedColorType::La2
        | ExtendedColorType::La4
        | ExtendedColorType::La8
        | ExtendedColorType::La16 => "grayscale + alpha",
        ExtendedColorType::Cmyk8 | ExtendedColorType::Cmyk16 => "CMYK",
        ExtendedColorType::Rgba1
        | ExtendedColorType::Rgba2
        | ExtendedColorType::Rgba4
        | ExtendedColorType::Rgba8
        | ExtendedColorType::Rgba16
        | ExtendedColorType::Rgba32F
        | ExtendedColorType::Bgra8 => "sRGB + alpha",
        _ => "sRGB",
    }
}

/// EXR and HDR carry linear float data; Reinhard tonemap then sRGB gamma.
pub fn extract_hdr_thumb(source: &Path, output: &Path) -> Result<()> {
    let img = image::open(source)?;
    let toned = DynamicImage::ImageRgb8(tonemap_hdr(&img));
    save_thumb(&toned, output)
}

fn tonemap_hdr(img: &DynamicImage) -> RgbImage {
    let rgb = img.to_rgb32f();
    let mut out = RgbImage::new(rgb.width(), rgb.height());

    for (x, y, pixel) in rgb.enumerate_pixels() {
        let mapped = pixel.0.map(|channel| {
            let channel = channel.max(0.0);
            to_u8((channel / (1.0 + channel)).powf(1.0 / 2.2))
        });
        out.put_pixel(x, y, Rgb(mapped));
    }
    out
}

pub(crate) fn to_u8(value: f32) -> u8 {
    (value.clamp(0.0, 1.0) * 255.0).round() as u8
}
