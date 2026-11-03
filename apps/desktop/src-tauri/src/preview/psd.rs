use anyhow::{anyhow, Result};
use image::DynamicImage;
use std::path::Path;

use super::save_thumb;

/// Decode the flattened composite of a Photoshop document.
pub(crate) fn extract_psd_thumb(source: &Path, output: &Path) -> Result<()> {
    let bytes = std::fs::read(source)?;
    let options = ag_psd::psd::ReadOptions {
        use_raw_data: Some(true),
        skip_layer_image_data: Some(true),
        skip_thumbnail: Some(true),
        ..Default::default()
    };
    let psd = ag_psd::read_psd(&bytes, &options)?;
    let pixels = ag_psd::get_composite_image_data(&psd)?
        .or(psd.image_data)
        .or(psd.canvas)
        .ok_or_else(|| anyhow!("PSD contains no composite image"))?;
    save_thumb(&pixel_data_to_image(pixels)?, output)
}

fn pixel_data_to_image(pixels: ag_psd::PixelData) -> Result<DynamicImage> {
    let (width, height) = (pixels.width, pixels.height);
    let expected = width as usize * height as usize;
    let length = pixels.data.len();

    if length == expected * 4 {
        let img = image::RgbaImage::from_raw(width, height, pixels.data)
            .ok_or_else(|| anyhow!("Invalid PSD RGBA buffer"))?;
        Ok(DynamicImage::ImageRgba8(img))
    } else if length == expected * 3 {
        let img = image::RgbImage::from_raw(width, height, pixels.data)
            .ok_or_else(|| anyhow!("Invalid PSD RGB buffer"))?;
        Ok(DynamicImage::ImageRgb8(img))
    } else {
        Err(anyhow!("Unsupported PSD pixel layout ({} bytes)", length))
    }
}
