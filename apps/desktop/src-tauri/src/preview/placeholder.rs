use anyhow::Result;
use image::{DynamicImage, Rgb, RgbImage};
use std::path::Path;

use super::{save_thumb, THUMB_SIZE};

/// Neutral tile for formats without an extractor (ai, 3dm, 3ds, ply, unknown).
pub(crate) fn generate_placeholder(output: &Path) -> Result<()> {
    let img = RgbImage::from_pixel(THUMB_SIZE, THUMB_SIZE, Rgb([24, 26, 32]));
    save_thumb(&DynamicImage::ImageRgb8(img), output)
}
