use anyhow::{anyhow, Result};
use flate2::read::ZlibDecoder;
use image::DynamicImage;
use std::io::Read;
use std::path::Path;

use super::archive::{find_bmp, read_prefix};
use super::ole::{dib_offsets, dib_parts_to_bmp, dib_to_bmp, find_dib, read_u16, read_u32};
use super::{find_png, save_thumb};

/// Scan windows tried in order for a Rhino preview. The bitmap usually sits in
/// the header, but files with large render data can push it further in, so the
/// window widens until the whole preview region has been covered.
const RHINO_SCAN_WINDOWS: [u64; 3] = [1024 * 1024, 16 * 1024 * 1024, 32 * 1024 * 1024];

/// Markers written next to a stored Rhino preview chunk. Image bytes follow the
/// marker when the bitmap is not the first image in the file.
const RHINO_MARKERS: [&[u8]; 3] = [b"Rhino", b"BITMAPINFO", b"PREVIEW"];

/// How far past a marker to look for the image signature.
const MARKER_PROBE: usize = 1024 * 1024;

/// Upper bound on marker occurrences inspected per file, keeping the scan
/// bounded when a string repeats in user text.
const MAX_MARKER_HITS: usize = 8;

/// Upper bound on decode attempts, so a file full of accidental signatures
/// cannot turn into an expensive decode loop.
const MAX_CANDIDATES: usize = 16;

/// Collada is XML text, so only a byte-embedded picture can serve as a preview.
const COLLADA_WINDOW: u64 = 4 * 1024 * 1024;

/// Upper bound on zlib streams attempted per scan window.
const MAX_ZLIB_CANDIDATES: usize = 8;

/// Upper bound on one decompressed preview chunk, guarding against
/// decompression bombs in a malformed file.
const MAX_DECOMPRESSED_BYTES: u64 = 16 * 1024 * 1024;

/// Compressed previews always start with a classic `BITMAPINFOHEADER`.
const PREVIEW_HEADER_BYTES: usize = 40;

/// Upper bound on compressed previews inspected per scan window.
const MAX_COMPRESSED_PREVIEWS: usize = 8;

/// Rhino `.3dm` files do not expose a renderable preview; some carry a stored
/// bitmap. The scan widens from the header outwards and returns `Err` when the
/// file has no preview, so the caller can fall back to the frontend mesh render.
pub(crate) fn extract_3dm_thumb(source: &Path, output: &Path) -> Result<()> {
    for window in RHINO_SCAN_WINDOWS {
        let head = read_prefix(source, window)?;
        // The compressed chunk is parsed precisely, so it runs before the
        // signature scan that could mistake its header for a bare DIB.
        if let Some(img) = find_3dm_compressed_preview(&head)
            .or_else(|| find_rhino_preview(&head))
            .or_else(|| find_compressed_preview(&head))
        {
            return save_thumb(&img, output);
        }
    }

    Err(anyhow!("No embedded preview image found in Rhino file"))
}

/// Rhino 3dm version 2 and later store the preview as a classic
/// `BITMAPINFOHEADER` followed by a compressed buffer holding the palette and
/// the pixel rows, so the decompressed bytes carry no header of their own.
fn find_3dm_compressed_preview(bytes: &[u8]) -> Option<DynamicImage> {
    dib_offsets(bytes)
        .take(MAX_COMPRESSED_PREVIEWS)
        .find_map(|index| decode_compressed_preview(&bytes[index..]))
}

/// Rebuild the DIB from its header and the compressed palette/pixel record.
fn decode_compressed_preview(tail: &[u8]) -> Option<DynamicImage> {
    let header = tail.get(..PREVIEW_HEADER_BYTES)?;
    let sizes = PreviewSizes::parse(header)?;
    let data = decompress_buffer(tail.get(PREVIEW_HEADER_BYTES..)?)?;
    if data.len() != sizes.total() {
        return None;
    }

    let (palette, pixels) = data.split_at(sizes.palette_bytes);
    let bmp = dib_parts_to_bmp(header, palette, pixels).ok()?;
    image::load_from_memory(&bmp).ok()
}

/// Palette and pixel sizes declared by a preview's DIB header.
struct PreviewSizes {
    palette_bytes: usize,
    pixel_bytes: usize,
}

impl PreviewSizes {
    fn parse(header: &[u8]) -> Option<Self> {
        let width = i32::from_le_bytes(header.get(4..8)?.try_into().ok()?);
        let height = i32::from_le_bytes(header.get(8..12)?.try_into().ok()?);
        if width <= 0 || height == 0 {
            return None;
        }

        let bit_count = read_u16(header, 14)?;
        let size_image = read_u32(header, 20)? as usize;
        let colors_used = read_u32(header, 32)? as usize;
        let palette_bytes = palette_bytes(bit_count, colors_used);
        let pixel_bytes = if size_image > 0 {
            size_image
        } else {
            image_bytes(width as usize, height.unsigned_abs() as usize, bit_count)?
        };

        let total = palette_bytes.checked_add(pixel_bytes)?;
        (pixel_bytes > 0 && total as u64 <= MAX_DECOMPRESSED_BYTES).then_some(Self {
            palette_bytes,
            pixel_bytes,
        })
    }

    fn total(&self) -> usize {
        self.palette_bytes + self.pixel_bytes
    }
}

/// Palette size in bytes, honouring the explicit colour count when present.
fn palette_bytes(bit_count: u16, colors_used: usize) -> usize {
    let colors = if colors_used > 0 {
        colors_used
    } else {
        match bit_count {
            1 => 2,
            4 => 16,
            8 => 256,
            _ => 0,
        }
    };
    colors * 4
}

/// Row-padded pixel bytes for a DIB with the given dimensions.
fn image_bytes(width: usize, height: usize, bit_count: u16) -> Option<usize> {
    let row = width
        .checked_mul(bit_count as usize)?
        .div_ceil(32)
        .checked_mul(4)?;
    row.checked_mul(height)
}

/// Decode one `WriteCompressedBuffer` record: uncompressed size, CRC, method,
/// then either raw bytes or a zlib stream wrapped in an anonymous chunk.
fn decompress_buffer(bytes: &[u8]) -> Option<Vec<u8>> {
    let expected = usize::try_from(read_u32(bytes, 0)?).ok()?;
    if expected == 0 || expected > MAX_DECOMPRESSED_BYTES as usize {
        return None;
    }

    match *bytes.get(8)? {
        0 => bytes.get(9..9 + expected).map(<[u8]>::to_vec),
        1 => inflate(&bytes[9..], expected),
        _ => None,
    }
}

/// Inflate the first zlib stream in `bytes`, requiring exactly `expected` bytes.
fn inflate(bytes: &[u8], expected: usize) -> Option<Vec<u8>> {
    let start = zlib_header_offset(bytes)?;
    let mut data = Vec::with_capacity(expected);
    ZlibDecoder::new(&bytes[start..])
        .take(expected as u64 + 1)
        .read_to_end(&mut data)
        .ok()?;
    (data.len() == expected).then_some(data)
}

/// Offset of the first plausible zlib stream header.
fn zlib_header_offset(bytes: &[u8]) -> Option<usize> {
    const SECOND_BYTES: [u8; 4] = [0x01, 0x5E, 0x9C, 0xDA];
    bytes
        .windows(2)
        .position(|w| w[0] == 0x78 && SECOND_BYTES.contains(&w[1]))
}


/// Rhino may store the preview chunk zlib-compressed. At most a handful of
/// candidate streams are tried and decompression is bounded, so a malformed
/// file stays cheap.
fn find_compressed_preview(bytes: &[u8]) -> Option<DynamicImage> {
    let mut decoded = Vec::new();
    let mut candidates = 0;
    let mut search_from = 0;

    while let Some(offset) = zlib_header_offset(&bytes[search_from..]) {
        let index = search_from + offset;
        candidates += 1;
        if candidates > MAX_ZLIB_CANDIDATES {
            break;
        }
        search_from = index + 1;

        decoded.clear();
        let decompressed = ZlibDecoder::new(&bytes[index..])
            .take(MAX_DECOMPRESSED_BYTES)
            .read_to_end(&mut decoded);
        if decompressed.is_err() {
            continue;
        }
        if let Some(img) = find_rhino_preview(&decompressed) {
            return Some(img);
        }
    }

    None
}

/// Most Collada files have no embedded picture, which is a normal outcome: the
/// caller completes without a thumbnail and the frontend renders the mesh.
pub(crate) fn extract_dae_thumb(source: &Path, output: &Path) -> Result<()> {
    let head = read_prefix(source, COLLADA_WINDOW)?;
    let img = decode_first_image(&head)
        .ok_or_else(|| anyhow!("No embedded preview image found in Collada file"))?;
    save_thumb(&img, output)
}

/// Decode the earliest embedded image, preferring Rhino preview chunks.
///
/// Candidates are ordered by offset so a stored header preview wins over a
/// later incidental signature. A signature can match by accident, so a failed
/// decode falls through to the next candidate instead of giving up.
fn find_rhino_preview(bytes: &[u8]) -> Option<DynamicImage> {
    let mut candidates = signature_offsets(bytes);
    for marker in RHINO_MARKERS {
        candidates.extend(marker_preview_offsets(bytes, marker));
    }

    candidates.sort_unstable();
    candidates.dedup();

    candidates
        .into_iter()
        .take(MAX_CANDIDATES)
        .find_map(|index| decode_candidate(&bytes[index..]))
}

fn decode_first_image(bytes: &[u8]) -> Option<DynamicImage> {
    let mut candidates = signature_offsets(bytes);
    candidates.sort_unstable();
    candidates.dedup();

    candidates
        .into_iter()
        .find_map(|index| decode_candidate(&bytes[index..]))
}

/// Offsets of every image signature understood by [`decode_candidate`].
fn signature_offsets(bytes: &[u8]) -> Vec<usize> {
    let jpeg = bytes.windows(3).position(|w| w == [0xFF, 0xD8, 0xFF]);
    [find_png(bytes), jpeg, find_bmp(bytes), find_dib(bytes)]
        .into_iter()
        .flatten()
        .collect()
}

/// Image offsets immediately following occurrences of `marker`.
fn marker_preview_offsets(bytes: &[u8], marker: &[u8]) -> Vec<usize> {
    let mut offsets = Vec::new();
    let mut start = 0;

    for _ in 0..MAX_MARKER_HITS {
        if marker.is_empty() {
            break;
        }
        let Some(position) = bytes[start..]
            .windows(marker.len())
            .position(|window| window == marker)
        else {
            break;
        };
        let marker_end = start + position + marker.len();
        let probe_end = bytes.len().min(marker_end + MARKER_PROBE);

        if let Some(offset) = signature_offsets(&bytes[marker_end..probe_end])
            .into_iter()
            .min()
        {
            offsets.push(marker_end + offset);
        }
        start = marker_end;
    }

    offsets
}

fn decode_candidate(tail: &[u8]) -> Option<DynamicImage> {
    image::load_from_memory(tail).ok().or_else(|| {
        let bmp = dib_to_bmp(tail).ok()?;
        image::load_from_memory(&bmp).ok()
    })
}
