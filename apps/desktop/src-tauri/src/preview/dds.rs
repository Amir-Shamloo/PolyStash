//! Minimal DDS reader for the Substance renderer's output.
//!
//! The `image` crate only decodes DXT variants, while the SDK's `writeDDSFile`
//! emits uncompressed RGBA/luminance surfaces, so those are decoded here and
//! DXT is delegated to `image`.

use image::RgbaImage;

const DDPF_ALPHAPIXELS: u32 = 0x1;
const DDPF_FOURCC: u32 = 0x4;
const DDPF_RGB: u32 = 0x40;
const DDPF_LUMINANCE: u32 = 0x2_0000;

const HEADER_SIZE: usize = 128;

fn le_u32(bytes: &[u8], offset: usize) -> Option<u32> {
    let slice: [u8; 4] = bytes.get(offset..offset + 4)?.try_into().ok()?;
    Some(u32::from_le_bytes(slice))
}

/// Scale one masked channel to 0..=255.
fn channel(value: u32, mask: u32) -> u8 {
    if mask == 0 {
        return 0;
    }
    let shift = mask.trailing_zeros();
    let max = mask >> shift;
    (((value & mask) >> shift) * 255 / max) as u8
}

fn decode_dxt(bytes: &[u8]) -> Option<RgbaImage> {
    let decoded = image::load_from_memory_with_format(bytes, image::ImageFormat::Dds).ok()?;
    Some(decoded.to_rgba8())
}

fn decode_uncompressed(header: &[u8], data: &[u8], width: usize, height: usize) -> Option<RgbaImage> {
    let pf_flags = le_u32(header, 76)?;
    let bit_count = le_u32(header, 84)?;
    if bit_count != 32 || data.len() < width * height * 4 {
        return None;
    }

    let r_mask = le_u32(header, 88)?;
    let g_mask = le_u32(header, 92)?;
    let b_mask = le_u32(header, 96)?;
    let a_mask = le_u32(header, 100)?;
    let luminance = pf_flags & DDPF_LUMINANCE != 0;
    let has_alpha = pf_flags & DDPF_ALPHAPIXELS != 0;

    let mut pixels = Vec::with_capacity(width * height * 4);
    for index in 0..width * height {
        let raw = le_u32(data, index * 4)?;
        let (r, g, b) = if luminance {
            let value = channel(raw, r_mask);
            (value, value, value)
        } else {
            (channel(raw, r_mask), channel(raw, g_mask), channel(raw, b_mask))
        };
        let alpha = if has_alpha { channel(raw, a_mask) } else { 255 };
        pixels.extend_from_slice(&[r, g, b, alpha]);
    }

    RgbaImage::from_raw(width as u32, height as u32, pixels).ok()
}

/// Decode a DDS surface to RGBA, or `None` when the format is unsupported.
pub(crate) fn decode(bytes: &[u8]) -> Option<RgbaImage> {
    if bytes.len() < HEADER_SIZE || &bytes[..4] != b"DDS " {
        return None;
    }

    let header = &bytes[4..HEADER_SIZE];
    let width = le_u32(header, 12)? as usize;
    let height = le_u32(header, 8)? as usize;
    if width == 0 || height == 0 {
        return None;
    }

    let pf_flags = le_u32(header, 76)?;
    if pf_flags & DDPF_FOURCC != 0 {
        return decode_dxt(bytes);
    }
    if pf_flags & (DDPF_RGB | DDPF_LUMINANCE) == 0 {
        return None;
    }

    decode_uncompressed(header, &bytes[HEADER_SIZE..], width, height)
}
