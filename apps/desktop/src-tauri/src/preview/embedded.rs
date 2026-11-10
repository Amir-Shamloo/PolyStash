use anyhow::{anyhow, Result};
use flate2::read::ZlibDecoder;
use image::DynamicImage;
use std::io::Read;
use std::path::Path;

use super::archive::{find_bmp, read_prefix};
use super::ole::{dib_to_bmp, find_dib};
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

/// Rhino `.3dm` files do not expose a renderable preview; some carry a stored
/// bitmap. The scan widens from the header outwards and returns `Err` when the
/// file has no preview, so the caller can fall back to the frontend mesh render.
pub(crate) fn extract_3dm_thumb(source: &Path, output: &Path) -> Result<()> {
    for window in RHINO_SCAN_WINDOWS {
        let head = read_prefix(source, window)?;
        if let Some(img) = find_rhino_preview(&head).or_else(|| find_compressed_preview(&head)) {
            return save_thumb(&img, output);
        }
    }

    Err(anyhow!("No embedded preview image found in Rhino file"))
}

/// Rhino may store the preview chunk zlib-compressed. At most a handful of
/// candidate streams are tried and decompression is bounded, so a malformed
/// file stays cheap.
fn find_compressed_preview(bytes: &[u8]) -> Option<DynamicImage> {
    const SECOND_BYTES: [u8; 4] = [0x01, 0x5e, 0x9c, 0xda];
    let mut decoded = Vec::new();
    let mut candidates = 0;

    for index in 0..bytes.len().saturating_sub(1) {
        if bytes[index] != 0x78 || !SECOND_BYTES.contains(&bytes[index + 1]) {
            continue;
        }
        candidates += 1;
        if candidates > MAX_ZLIB_CANDIDATES {
            break;
        }

        decoded.clear();
        let decompressed = ZlibDecoder::new(&bytes[index..])
            .take(MAX_DECOMPRESSED_BYTES)
            .read_to_end(&mut decoded);
        if decompressed.is_err() {
            continue;
        }
        if let Some(img) = find_rhino_preview(&decoded) {
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
