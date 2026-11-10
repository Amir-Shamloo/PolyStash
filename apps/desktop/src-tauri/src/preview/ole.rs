use anyhow::{anyhow, Result};
use std::io::Read;
use std::path::Path;

use super::{find_png, save_thumb};

/// The classic `BITMAPINFOHEADER`, tried before any other variant.
const CLASSIC_HEADER: u32 = 40;

/// DIB header sizes this decoder accepts: OS/2 core, the classic info header,
/// and the V4/V5 extensions Rhino and OLE containers may emit.
const DIB_HEADER_SIZES: [u32; 4] = [12, CLASSIC_HEADER, 108, 124];

/// Upper bound on either dimension of an embedded preview, in pixels.
const MAX_DIMENSION: u32 = 20_000;

/// Header-derived layout needed to turn a bare DIB into a BMP file.
struct DibLayout {
    header_size: u32,
    width: u32,
    height: i32,
    bit_count: u16,
    compression: u32,
}

impl DibLayout {
    /// Parse a DIB header, accepting the core, info, and V4/V5 layouts.
    fn parse(dib: &[u8]) -> Option<Self> {
        let header_size = read_u32(dib, 0)?;
        if !DIB_HEADER_SIZES.contains(&header_size) || dib.len() < header_size as usize {
            return None;
        }

        if header_size == 12 {
            let width = u32::from(read_u16(dib, 4)?);
            let height = i32::from(read_u16(dib, 6)?);
            let bit_count = read_u16(dib, 10)?;
            return Some(Self {
                header_size,
                width,
                height,
                bit_count,
                compression: 0,
            });
        }

        let width = i32::from_le_bytes(dib.get(4..8)?.try_into().ok()?);
        let height = i32::from_le_bytes(dib.get(8..12)?.try_into().ok()?);
        let bit_count = read_u16(dib, 14)?;
        let compression = read_u32(dib, 16)?;
        let width = u32::try_from(width).ok()?;

        Some(Self {
            header_size,
            width,
            height,
            bit_count,
            compression,
        })
    }

    /// Bytes between the BMP file header and the pixel data.
    fn pixel_offset(&self) -> u64 {
        let palette_entries = if self.bit_count <= 8 {
            1u64 << self.bit_count
        } else {
            0
        };
        let palette_entry_size = if self.header_size == 12 { 3 } else { 4 };
        // BITMAPINFOHEADER bitfields store their colour masks after the header;
        // V4/V5 keep them inside it, and core headers have none.
        let masks = match (self.header_size, self.compression) {
            (CLASSIC_HEADER, 3) => 12, // BI_BITFIELDS
            (CLASSIC_HEADER, 6) => 16, // BI_ALPHABITFIELDS
            _ => 0,
        };

        14 + u64::from(self.header_size) + masks + palette_entries * palette_entry_size
    }

    /// Bytes of row-padded pixel data.
    fn pixel_bytes(&self) -> u64 {
        let row = (u64::from(self.width) * u64::from(self.bit_count)).div_ceil(32) * 4;
        row * u64::from(self.height.unsigned_abs())
    }
}

fn read_u16(bytes: &[u8], offset: usize) -> Option<u16> {
    let raw: [u8; 2] = bytes.get(offset..offset + 2)?.try_into().ok()?;
    Some(u16::from_le_bytes(raw))
}

fn read_u32(bytes: &[u8], offset: usize) -> Option<u32> {
    let raw: [u8; 4] = bytes.get(offset..offset + 4)?.try_into().ok()?;
    Some(u32::from_le_bytes(raw))
}

/// Sane embedded-DIB dimensions (biHeight may be negative for top-down DIBs).
fn plausible_layout(layout: DibLayout) -> bool {
    (1..=MAX_DIMENSION).contains(&layout.width)
        && layout.height != 0
        && layout.height.unsigned_abs() <= MAX_DIMENSION
        && supported_bit_count(&layout)
}

/// Bit depths the `image` crate can decode for the given header variant.
fn supported_bit_count(layout: &DibLayout) -> bool {
    if layout.header_size == 12 {
        matches!(layout.bit_count, 1 | 4 | 8 | 24)
    } else {
        matches!(layout.bit_count, 1 | 2 | 4 | 8 | 16 | 24 | 32)
    }
}

/// True when the four bytes at `index` are a supported DIB header size.
fn is_dib_header(bytes: &[u8], index: usize) -> bool {
    read_u32(bytes, index).is_some_and(|size| DIB_HEADER_SIZES.contains(&size))
}

/// Wrap a raw DIB (no file header) into a full BMP so the `image` crate can
/// decode it. Palette and colour-mask sizes depend on the header variant.
pub(crate) fn dib_to_bmp(dib: &[u8]) -> Result<Vec<u8>> {
    let layout = DibLayout::parse(dib).ok_or_else(|| anyhow!("Unsupported DIB header"))?;
    let offset = layout.pixel_offset();
    // Copy only the header, palette, and pixel rows the layout describes, so a
    // DIB found in the middle of a large file does not drag the rest along.
    let dib_bytes = offset.saturating_sub(14) + layout.pixel_bytes();
    let needed = usize::try_from(dib_bytes).unwrap_or(dib.len()).min(dib.len());
    let file_size = 14 + needed;

    let bf_size = u32::try_from(file_size).map_err(|_| anyhow!("DIB too large"))?;
    let bf_off_bits = u32::try_from(offset).map_err(|_| anyhow!("DIB too large"))?;

    let mut bmp = Vec::with_capacity(file_size);
    bmp.extend_from_slice(b"BM");
    bmp.extend_from_slice(&bf_size.to_le_bytes());
    bmp.extend_from_slice(&[0u8; 4]); // reserved
    bmp.extend_from_slice(&bf_off_bits.to_le_bytes());
    bmp.extend_from_slice(&dib[..needed]);
    Ok(bmp)
}

/// Every plausible DIB header offset, classic `BITMAPINFOHEADER` matches first
/// so a stray core-header match cannot shadow the layout older files used.
pub(crate) fn dib_offsets(bytes: &[u8]) -> impl Iterator<Item = usize> + '_ {
    let is_classic = |index: usize| read_u32(bytes, index) == Some(CLASSIC_HEADER);
    let classic = dib_candidates(bytes).filter(move |&index| is_classic(index));
    let others = dib_candidates(bytes).filter(move |&index| !is_classic(index));
    classic.chain(others)
}

fn dib_candidates(bytes: &[u8]) -> impl Iterator<Item = usize> + '_ {
    (0..bytes.len()).filter(move |&index| {
        is_dib_header(bytes, index)
            && DibLayout::parse(&bytes[index..]).is_some_and(plausible_layout)
    })
}

/// Locate a supported DIB header with sane dimensions inside a byte buffer.
/// Used to find embedded DIB thumbnails in OLE summary streams and 3dm files.
pub(crate) fn find_dib(bytes: &[u8]) -> Option<usize> {
    dib_offsets(bytes).next()
}

/// Extract the embedded OLE thumbnail from a 3ds Max (.max) or Revit (.rvt/.rfa/.rte)
/// compound file and write a resized thumbnail. Max stores a DIB in the
/// SummaryInformation stream; Revit stores a PNG in the `RevitPreview4.0` stream.
pub(crate) fn extract_ole_thumb(source: &Path, output: &Path) -> Result<()> {
    // OLE files are compound-binary; spurious files fail at open
    let mut cf = cfb::open(source)?;
    let mut raw = Vec::new();

    for name in ["RevitPreview4.0", "\u{5}SummaryInformation"] {
        if cf.exists(name) && cf.is_stream(name) {
            cf.open_stream(name)?.read_to_end(&mut raw)?;

            if let Some(idx) = find_png(&raw) {
                let img = image::load_from_memory(&raw[idx..])?;
                return save_thumb(&img, output);
            }
            if let Some(idx) = find_dib(&raw) {
                let bmp = dib_to_bmp(&raw[idx..])?;
                let img = image::load_from_memory(&bmp)?;
                return save_thumb(&img, output);
            }
            raw.clear();
        }
    }

    Err(anyhow!("No embedded thumbnail found in OLE file"))
}
