use anyhow::Result;
use image::imageops::FilterType;
use image::{DynamicImage, Rgb, RgbImage};
use std::path::Path;

use super::{save_thumb, THUMB_SIZE};

/// Checks if an extension is a standard raster image that Rust can decode directly.
pub fn is_native_image(ext: &str) -> bool {
    matches!(
        ext.to_lowercase().as_str(),
        "jpg" | "jpeg" | "png" | "bmp" | "gif" | "tif" | "tiff" | "tga" | "webp"
    )
}

/// In-process image downsampling using the native Rust `image` crate.
/// Uses Triangle (bilinear) filtering for 4x–8x faster performance than Lanczos3.
pub fn generate_image_thumb(source: &Path, output: &Path) -> Result<()> {
    let img = image::open(source)?;
    save_thumb(&img, output)
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
