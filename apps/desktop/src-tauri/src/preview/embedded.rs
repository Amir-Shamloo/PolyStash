use anyhow::{anyhow, Result};
use image::DynamicImage;
use std::path::Path;

use super::archive::{find_bmp, read_prefix};
use super::ole::{dib_to_bmp, find_dib};
use super::{find_png, save_thumb};

/// Byte window and label for a raw-image scan of a format that cannot be
/// parsed headlessly.
struct ScanSpec {
    window: u64,
    label: &'static str,
}

/// Rhino keeps its preview chunk among the file header data, usually as a
/// Windows DIB; some exporters write PNG or JPEG instead.
const RHINO: ScanSpec = ScanSpec {
    window: 16 * 1024 * 1024,
    label: "Rhino",
};

/// Collada is XML text, so only a byte-embedded picture can serve as a preview.
const COLLADA: ScanSpec = ScanSpec {
    window: 4 * 1024 * 1024,
    label: "Collada",
};

impl ScanSpec {
    /// A miss returns `Err` so the caller can pick its own fallback.
    fn extract(&self, source: &Path, output: &Path) -> Result<()> {
        let head = read_prefix(source, self.window)?;
        let img = decode_first_image(&head)
            .ok_or_else(|| anyhow!("No embedded preview image found in {} file", self.label))?;
        save_thumb(&img, output)
    }
}

/// Rhino `.3dm` files do not expose a renderable preview; some carry a stored
/// bitmap. A file without one has no headless preview at all.
pub(crate) fn extract_3dm_thumb(source: &Path, output: &Path) -> Result<()> {
    RHINO.extract(source, output)
}

/// Most Collada files have no embedded picture, which is a normal outcome: the
/// caller completes without a thumbnail and the frontend renders the mesh.
pub(crate) fn extract_dae_thumb(source: &Path, output: &Path) -> Result<()> {
    COLLADA.extract(source, output)
}

/// Decode the earliest embedded PNG, JPEG, BMP, or bare DIB image.
///
/// Candidates are tried in file order so a stored preview wins over a later
/// incidental signature. A signature can match by accident, so a failed decode
/// falls through to the next candidate instead of giving up.
fn decode_first_image(bytes: &[u8]) -> Option<DynamicImage> {
    let jpeg = bytes.windows(3).position(|w| w == [0xFF, 0xD8, 0xFF]);
    let mut candidates: Vec<usize> = [find_png(bytes), jpeg, find_bmp(bytes), find_dib(bytes)]
        .into_iter()
        .flatten()
        .collect();
    candidates.sort_unstable();
    candidates.dedup();

    candidates
        .into_iter()
        .find_map(|index| decode_candidate(&bytes[index..]))
}

fn decode_candidate(tail: &[u8]) -> Option<DynamicImage> {
    image::load_from_memory(tail).ok().or_else(|| {
        let bmp = dib_to_bmp(tail).ok()?;
        image::load_from_memory(&bmp).ok()
    })
}
