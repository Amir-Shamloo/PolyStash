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
/// Offset of the FourCC inside the file (header offset 80 plus the magic).
const FOURCC_OFFSET: usize = HEADER_SIZE - 44;

struct DdsHeader {
    width: usize,
    height: usize,
    pf_flags: u32,
    bit_count: u32,
    masks: [u32; 4],
}

fn le_u16(bytes: &[u8], offset: usize) -> Option<u16> {
    let slice: [u8; 2] = bytes.get(offset..offset + 2)?.try_into().ok()?;
    Some(u16::from_le_bytes(slice))
}

fn le_u32(bytes: &[u8], offset: usize) -> Option<u32> {
    let slice: [u8; 4] = bytes.get(offset..offset + 4)?.try_into().ok()?;
    Some(u32::from_le_bytes(slice))
}

fn parse_header(bytes: &[u8]) -> Option<DdsHeader> {
    if bytes.len() < HEADER_SIZE || &bytes[..4] != b"DDS " {
        return None;
    }
    let header = &bytes[4..HEADER_SIZE];
    Some(DdsHeader {
        width: le_u32(header, 12)? as usize,
        height: le_u32(header, 8)? as usize,
        pf_flags: le_u32(header, 76)?,
        bit_count: le_u32(header, 84)?,
        masks: [
            le_u32(header, 88)?,
            le_u32(header, 92)?,
            le_u32(header, 96)?,
            le_u32(header, 100)?,
        ],
    })
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

fn sample(data: &[u8], index: usize, bytes_per_pixel: usize) -> Option<u32> {
    let start = index * bytes_per_pixel;
    match bytes_per_pixel {
        1 => data.get(start).map(u32::from),
        2 => le_u16(data, start).map(u32::from),
        4 => le_u32(data, start),
        _ => None,
    }
}

/// DXT2/DXT4 share the DXT3/DXT5 block layout, but `image` matches the FourCC
/// exactly, so the tag is rewritten before handing the file over.
fn rewrite_fourcc(bytes: &[u8], fourcc: u32) -> Option<Vec<u8>> {
    let replacement = match fourcc {
        0x32_54_58_44 => 0x33_54_58_44, // DXT2 -> DXT3
        0x34_54_58_44 => 0x35_54_58_44, // DXT4 -> DXT5
        _ => return None,
    };

    let mut patched = bytes.to_vec();
    patched[FOURCC_OFFSET..FOURCC_OFFSET + 4].copy_from_slice(&replacement.to_le_bytes());
    Some(patched)
}

fn decode_dxt(bytes: &[u8], fourcc: u32) -> Option<RgbaImage> {
    let decoded = match rewrite_fourcc(bytes, fourcc) {
        Some(patched) => image::load_from_memory_with_format(&patched, image::ImageFormat::Dds),
        None => image::load_from_memory_with_format(bytes, image::ImageFormat::Dds),
    }
    .ok()?;
    Some(decoded.to_rgba8())
}

fn push_pixel(pixels: &mut Vec<u8>, raw: u32, header: &DdsHeader) {
    let [r_mask, g_mask, b_mask, a_mask] = header.masks;
    let luminance = header.pf_flags & DDPF_LUMINANCE != 0;
    let has_alpha = header.pf_flags & DDPF_ALPHAPIXELS != 0;

    let (r, g, b) = if luminance {
        let value = channel(raw, r_mask);
        (value, value, value)
    } else {
        (channel(raw, r_mask), channel(raw, g_mask), channel(raw, b_mask))
    };
    let alpha = if has_alpha { channel(raw, a_mask) } else { 255 };
    pixels.extend_from_slice(&[r, g, b, alpha]);
}

fn decode_uncompressed(header: &DdsHeader, data: &[u8]) -> Option<RgbaImage> {
    if header.width == 0 || header.height == 0 {
        return None;
    }
    let bytes_per_pixel = (header.bit_count / 8) as usize;
    if bytes_per_pixel == 0 || bytes_per_pixel > 4 {
        return None;
    }
    if data.len() < header.width * header.height * bytes_per_pixel {
        return None;
    }

    let mut pixels = Vec::with_capacity(header.width * header.height * 4);
    for index in 0..header.width * header.height {
        push_pixel(&mut pixels, sample(data, index, bytes_per_pixel)?, header);
    }

    RgbaImage::from_raw(header.width as u32, header.height as u32, pixels)
}

/// Decode a DDS surface to RGBA, or `None` when the format is unsupported.
pub(crate) fn decode(bytes: &[u8]) -> Option<RgbaImage> {
    let header = parse_header(bytes)?;
    if header.pf_flags & DDPF_FOURCC != 0 {
        let fourcc = le_u32(&bytes[4..HEADER_SIZE], 80)?;
        return decode_dxt(bytes, fourcc);
    }
    if header.pf_flags & (DDPF_RGB | DDPF_LUMINANCE) == 0 {
        return None;
    }

    decode_uncompressed(&header, &bytes[HEADER_SIZE..])
}
