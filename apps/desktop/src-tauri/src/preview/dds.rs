//! Minimal DDS reader for the Substance renderer's output.
//!
//! The `image` crate only decodes DXT variants, while the SDK's `writeDDSFile`
//! emits a wider range of surfaces: raw 24/32-bit pixels, 16-bit integer
//! channels, half/full floats, and BC4/BC5 blocks. Those are decoded here;
//! DXT1/3/5 is still delegated to `image`.

use image::RgbaImage;

const DDPF_ALPHAPIXELS: u32 = 0x1;
const DDPF_FOURCC: u32 = 0x4;
const DDPF_RGB: u32 = 0x40;
const DDPF_LUMINANCE: u32 = 0x2_0000;

const HEADER_SIZE: usize = 128;
/// Offset of the FourCC inside the file (header offset 80 plus the magic).
const FOURCC_OFFSET: usize = HEADER_SIZE - 44;

// FourCC tags emitted by the Substance renderer.
const FOURCC_RGBA16: u32 = 36; // D3DFMT_A16B16G16R16
const FOURCC_R16F: u32 = 111; // D3DFMT_R16F
const FOURCC_RGBA16F: u32 = 113; // D3DFMT_A16B16G16R16F
const FOURCC_R32F: u32 = 114; // D3DFMT_R32F
const FOURCC_RGBA32F: u32 = 116; // D3DFMT_A32B32G32R32F
const FOURCC_ATI1: u32 = u32::from_le_bytes(*b"ATI1"); // BC4
const FOURCC_ATI2: u32 = u32::from_le_bytes(*b"ATI2"); // BC5

struct DdsHeader {
    width: usize,
    height: usize,
    pf_flags: u32,
    bit_count: u32,
    fourcc: Option<u32>,
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
    let pf_flags = le_u32(header, 76)?;
    Some(DdsHeader {
        width: le_u32(header, 12)? as usize,
        height: le_u32(header, 8)? as usize,
        pf_flags,
        bit_count: le_u32(header, 84)?,
        fourcc: if pf_flags & DDPF_FOURCC != 0 {
            Some(le_u32(header, 80)?)
        } else {
            None
        },
        masks: [
            le_u32(header, 88)?,
            le_u32(header, 92)?,
            le_u32(header, 96)?,
            le_u32(header, 100)?,
        ],
    })
}

/// One-line header summary for the SBSAR render log.
pub(crate) fn describe(bytes: &[u8]) -> String {
    let Some(header) = parse_header(bytes) else {
        return "not a DDS".to_string();
    };
    let fourcc = header
        .fourcc
        .map_or_else(|| "none".to_string(), |code| format!("0x{code:08X}"));
    format!(
        "fourcc={fourcc} pf=0x{:08X} size={}x{}",
        header.pf_flags, header.width, header.height
    )
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
        1 => data.get(start).map(|byte| u32::from(*byte)),
        2 => le_u16(data, start).map(u32::from),
        3 => {
            let bytes = data.get(start..start + 3)?;
            Some(u32::from(bytes[0]) | u32::from(bytes[1]) << 8 | u32::from(bytes[2]) << 16)
        }
        4 => le_u32(data, start),
        _ => None,
    }
}

/// DXT2/DXT4 share the DXT3/DXT5 block layout, but `image` matches the FourCC
/// exactly, so the tag is rewritten before handing the file over.
fn rewrite_fourcc(bytes: &[u8], fourcc: u32) -> Option<Vec<u8>> {
    let replacement: u32 = match fourcc {
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

/// A16B16G16R16: four 16-bit channels, scaled through the header masks.
fn decode_rgba16(header: &DdsHeader, data: &[u8]) -> Option<RgbaImage> {
    if header.width == 0 || header.height == 0 {
        return None;
    }
    let [r_mask, g_mask, b_mask, a_mask] = header.masks;
    let mut pixels = Vec::with_capacity(header.width * header.height * 4);
    for index in 0..header.width * header.height {
        let base = index * 8;
        let r = channel(u32::from(le_u16(data, base)?), r_mask);
        let g = channel(u32::from(le_u16(data, base + 2)?), g_mask);
        let b = channel(u32::from(le_u16(data, base + 4)?), b_mask);
        let a = if a_mask == 0 {
            255
        } else {
            channel(u32::from(le_u16(data, base + 6)?), a_mask)
        };
        pixels.extend_from_slice(&[r, g, b, a]);
    }
    RgbaImage::from_raw(header.width as u32, header.height as u32, pixels)
}

/// How one float surface is laid out, plus the reader for its channel width.
struct FloatFormat {
    channels: usize,
    bytes_per_channel: usize,
    read: fn(&[u8], usize) -> Option<f32>,
}

const HALF_R: FloatFormat = FloatFormat {
    channels: 1,
    bytes_per_channel: 2,
    read: read_half,
};
const HALF_RGBA: FloatFormat = FloatFormat {
    channels: 4,
    bytes_per_channel: 2,
    read: read_half,
};
const FLOAT_R: FloatFormat = FloatFormat {
    channels: 1,
    bytes_per_channel: 4,
    read: read_float,
};
const FLOAT_RGBA: FloatFormat = FloatFormat {
    channels: 4,
    bytes_per_channel: 4,
    read: read_float,
};

fn read_half(data: &[u8], offset: usize) -> Option<f32> {
    le_u16(data, offset).map(half_to_f32)
}

fn read_float(data: &[u8], offset: usize) -> Option<f32> {
    le_u32(data, offset).map(f32::from_bits)
}

/// IEEE-754 binary16 to f32.
fn half_to_f32(bits: u16) -> f32 {
    let exponent = i32::from((bits >> 10) & 0x1F);
    let mantissa = f32::from(bits & 0x03FF);
    let magnitude = match exponent {
        0 => mantissa * 2f32.powi(-24),
        0x1F if mantissa == 0.0 => f32::INFINITY,
        0x1F => f32::NAN,
        _ => (1.0 + mantissa / 1024.0) * 2f32.powi(exponent - 15),
    };
    if bits & 0x8000 != 0 {
        -magnitude
    } else {
        magnitude
    }
}

/// Linear to sRGB (IEC 61966-2-1) for colour data.
fn linear_to_srgb(value: f32) -> f32 {
    if value <= 0.003_130_8 {
        12.92 * value
    } else {
        1.055 * value.powf(1.0 / 2.4) - 0.055
    }
}

fn quantise(value: f32, srgb: bool) -> u8 {
    let value = if srgb { linear_to_srgb(value) } else { value };
    (value.clamp(0.0, 1.0) * 255.0).round() as u8
}

/// Quantise one float pixel. Four-channel surfaces are colour and get the
/// sRGB transfer on RGB; alpha stays linear. One-channel surfaces are data
/// (roughness/height) and are replicated as grayscale.
fn float_pixel(data: &[u8], base: usize, format: &FloatFormat) -> Option<[u8; 4]> {
    let stride = format.bytes_per_channel;
    let sample = |channel: usize| (format.read)(data, base + channel * stride);
    let srgb = format.channels == 4;
    let mut rgba = [255u8; 4];
    for channel in 0..format.channels {
        rgba[channel] = quantise(sample(channel)?, srgb && channel < 3);
    }
    if format.channels == 1 {
        rgba[1] = rgba[0];
        rgba[2] = rgba[0];
    }
    Some(rgba)
}

fn decode_float(header: &DdsHeader, data: &[u8], format: &FloatFormat) -> Option<RgbaImage> {
    if header.width == 0 || header.height == 0 {
        return None;
    }
    let stride = format.channels * format.bytes_per_channel;
    if data.len() < header.width * header.height * stride {
        return None;
    }
    let mut pixels = Vec::with_capacity(header.width * header.height * 4);
    for index in 0..header.width * header.height {
        pixels.extend_from_slice(&float_pixel(data, index * stride, format)?);
    }
    RgbaImage::from_raw(header.width as u32, header.height as u32, pixels)
}

/// BC3-style alpha palette: two endpoints plus six interpolated values, or
/// four interpolated values plus 0 and 255.
fn bc4_palette(low: u8, high: u8) -> [u8; 8] {
    let mut palette = [0u8; 8];
    palette[0] = low;
    palette[1] = high;
    if low > high {
        for step in 1..=6 {
            palette[step + 1] = blend(low, high, step, 7);
        }
    } else {
        for step in 1..=4 {
            palette[step + 1] = blend(low, high, step, 5);
        }
        palette[7] = 255;
    }
    palette
}

/// `step`/`divisor` interpolation between two endpoints, rounding down.
fn blend(low: u8, high: u8, step: usize, divisor: usize) -> u8 {
    let low_weight = divisor - step;
    ((usize::from(low) * low_weight + usize::from(high) * step) / divisor) as u8
}

/// Decode one 8-byte BC4 block into 16 channel values in row-major order.
fn bc4_block(block: &[u8; 8]) -> [u8; 16] {
    let palette = bc4_palette(block[0], block[1]);
    let indices = u64::from_le_bytes([
        block[2], block[3], block[4], block[5], block[6], block[7], 0, 0,
    ]);
    let mut values = [0u8; 16];
    for (index, value) in values.iter_mut().enumerate() {
        *value = palette[((indices >> (index * 3)) & 0x7) as usize];
    }
    values
}

/// Decode a BC4-encoded channel plane into `width * height` values.
fn bc4_plane(header: &DdsHeader, data: &[u8]) -> Option<Vec<u8>> {
    let blocks_x = header.width.div_ceil(4);
    let blocks_y = header.height.div_ceil(4);
    let mut plane = vec![0u8; header.width * header.height];
    for block_y in 0..blocks_y {
        for block_x in 0..blocks_x {
            let offset = (block_y * blocks_x + block_x) * 8;
            let block: [u8; 8] = data.get(offset..offset + 8)?.try_into().ok()?;
            let values = bc4_block(&block);
            blit_block(&mut plane, header, (block_x, block_y), &values);
        }
    }
    Some(plane)
}

fn blit_block(plane: &mut [u8], header: &DdsHeader, origin: (usize, usize), values: &[u8; 16]) {
    let (block_x, block_y) = origin;
    for row in 0..4 {
        let y = block_y * 4 + row;
        if y >= header.height {
            return;
        }
        let x = block_x * 4;
        let count = (header.width - x).min(4);
        let destination = y * header.width + x;
        plane[destination..destination + count].copy_from_slice(&values[row * 4..row * 4 + count]);
    }
}

fn bc4_plane_size(header: &DdsHeader) -> usize {
    header.width.div_ceil(4) * header.height.div_ceil(4) * 8
}

/// ATI1 (BC4) is one channel, ATI2 (BC5) is two; both use BC3 alpha-style
/// blocks. The result keeps the red channel and, for BC5, green as well.
fn decode_bc(header: &DdsHeader, data: &[u8], channels: usize) -> Option<RgbaImage> {
    if header.width == 0 || header.height == 0 {
        return None;
    }
    let red = bc4_plane(header, data)?;
    let green = if channels == 2 {
        bc4_plane(header, data.get(bc4_plane_size(header)..)?)?
    } else {
        vec![0; red.len()]
    };
    let pixels = interleave_channels(&red, &green);
    RgbaImage::from_raw(header.width as u32, header.height as u32, pixels)
}

fn interleave_channels(red: &[u8], green: &[u8]) -> Vec<u8> {
    let mut pixels = Vec::with_capacity(red.len() * 4);
    for (r, g) in red.iter().zip(green) {
        pixels.extend_from_slice(&[*r, *g, 0, 255]);
    }
    pixels
}

fn decode_compressed(header: &DdsHeader, bytes: &[u8]) -> Option<RgbaImage> {
    let data = bytes.get(HEADER_SIZE..)?;
    match header.fourcc? {
        FOURCC_ATI1 => decode_bc(header, data, 1),
        FOURCC_ATI2 => decode_bc(header, data, 2),
        FOURCC_RGBA16 => decode_rgba16(header, data),
        FOURCC_R16F => decode_float(header, data, &HALF_R),
        FOURCC_RGBA16F => decode_float(header, data, &HALF_RGBA),
        FOURCC_R32F => decode_float(header, data, &FLOAT_R),
        FOURCC_RGBA32F => decode_float(header, data, &FLOAT_RGBA),
        other => decode_dxt(bytes, other),
    }
}

/// Decode a DDS surface to RGBA, or `None` when the format is unsupported.
pub(crate) fn decode(bytes: &[u8]) -> Option<RgbaImage> {
    let header = parse_header(bytes)?;
    if header.fourcc.is_some() {
        return decode_compressed(&header, bytes);
    }
    if header.pf_flags & (DDPF_RGB | DDPF_LUMINANCE) == 0 {
        return None;
    }

    decode_uncompressed(&header, &bytes[HEADER_SIZE..])
}
