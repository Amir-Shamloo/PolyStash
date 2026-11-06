//! Self-contained EULUMDAT (`.ldt`) to IESNA LM-63-2002 conversion.
//!
//! The converter implements the subset of the LDT format the IES viewer needs.
//! It is deliberately dependency-free so the desktop binary never links an
//! AGPL parser and the project can stay MIT licensed.

use std::fmt::Write as _;
use std::path::Path;

/// Upper bounds accepted from a file. Real LDT files stay far below these;
/// the limits keep a malformed header from driving large allocations.
const MAX_C_PLANES: usize = 720;
const MAX_GAMMA_ANGLES: usize = 361;
const MAX_LAMP_SETS: usize = 20;
const DIRECT_RATIO_COUNT: usize = 10;
const IES_VALUES_PER_LINE: usize = 10;
const DEFAULT_FLUX_LUMENS: f64 = 1000.0;
const MIN_HORIZONTAL_ANGLES: usize = 2;

/// One lamp group from the LDT header.
struct LampSet {
    count: i32,
    flux: f64,
    watts: f64,
}

/// Metadata that precedes the photometric block in an LDT file.
struct LdtHeader {
    symmetry: i32,
    num_c_planes: usize,
    num_gamma: usize,
    c_distance: f64,
    name: String,
    catalog: String,
    length_mm: f64,
    width_mm: f64,
    height_mm: f64,
    lamp_sets: Vec<LampSet>,
}

/// The C-plane, gamma and intensity block at the end of an LDT file.
struct RawPhotometry {
    c_angles: Vec<f64>,
    gamma_angles: Vec<f64>,
    intensities: Vec<Vec<f64>>,
}

struct ParsedLdt {
    header: LdtHeader,
    photometry: RawPhotometry,
}

/// Physical dimensions written into the IES header, in metres.
struct IesDimensions {
    width: f64,
    length: f64,
    height: f64,
}

/// The lamp and photometric line that follows `TILT=NONE`.
struct LampHeader<'a> {
    lamps: u32,
    lumens_per_lamp: f64,
    vertical: &'a [f64],
    horizontal: &'a [f64],
    dimensions: IesDimensions,
}

/// Line cursor that accepts CRLF, CR and LF files while preserving empty
/// lines: blank lines are meaningful fields in the LDT header.
struct LineCursor {
    lines: Vec<String>,
    index: usize,
}

impl LineCursor {
    fn new(text: &str) -> Self {
        let lines = text
            .replace("\r\n", "\n")
            .replace('\r', "\n")
            .lines()
            .map(str::to_string)
            .collect();
        Self { lines, index: 0 }
    }

    fn next_line(&mut self, field: &str) -> Result<String, String> {
        let line = self
            .lines
            .get(self.index)
            .ok_or_else(|| format!("LDT ended before the {field}"))?;
        self.index += 1;
        Ok(line.clone())
    }

    fn next_number(&mut self, field: &str) -> Result<f64, String> {
        let line = self.next_line(field)?;
        parse_number(&line).ok_or_else(|| format!("LDT has an invalid {field}: {:?}", line.trim()))
    }

    fn next_usize(&mut self, field: &str) -> Result<usize, String> {
        let value = self.next_number(field)?;
        usize_from_number(value, field)
    }

    fn next_i32(&mut self, field: &str) -> Result<i32, String> {
        let value = self.next_number(field)?;
        if value < f64::from(i32::MIN) || value > f64::from(i32::MAX) {
            return Err(format!("LDT {field} is out of range"));
        }
        Ok(value as i32)
    }

    /// All numeric tokens left in the file. Reading whitespace tokens rather
    /// than fixed line numbers keeps the photometric block usable when a file
    /// wraps angles or intensities differently than expected.
    fn remaining_numbers(&self) -> Vec<f64> {
        self.lines
            .iter()
            .skip(self.index)
            .flat_map(|line| line.split_whitespace())
            .filter_map(parse_number)
            .collect()
    }
}

fn parse_number(token: &str) -> Option<f64> {
    let cleaned = token.trim().replace(',', ".");
    cleaned.parse::<f64>().ok().filter(|value| value.is_finite())
}

fn usize_from_number(value: f64, field: &str) -> Result<usize, String> {
    if !fits_usize(value) {
        return Err(format!("LDT {field} is out of range"));
    }
    Ok(value as usize)
}

/// True when a parsed number can be cast to `usize` without going out of range.
fn fits_usize(value: f64) -> bool {
    if !value.is_finite() {
        return false;
    }
    value >= 0.0 && value <= usize::MAX as f64
}

/// Parse `source` and return the IESNA LM-63-2002 document as text.
pub fn convert_file(source: &Path) -> Result<String, String> {
    let bytes = std::fs::read(source)
        .map_err(|error| format!("Cannot read {}: {error}", source.display()))?;
    let text = decode_text(&bytes);
    let source_name = source
        .file_name()
        .and_then(|name| name.to_str())
        .unwrap_or("luminaire.ldt");
    convert_text(&text, source_name)
}

/// Many photometric files come from Windows tools and use Latin-1 rather than
/// UTF-8. Every byte is a valid Latin-1 character, so the fallback is lossless
/// for the header strings and irrelevant for the numeric block.
fn decode_text(bytes: &[u8]) -> String {
    match std::str::from_utf8(bytes) {
        Ok(text) => text.to_string(),
        Err(_) => bytes.iter().map(|&byte| char::from(byte)).collect(),
    }
}

fn convert_text(text: &str, source_name: &str) -> Result<String, String> {
    let parsed = parse_ldt(text)?;
    Ok(write_ies(&parsed, source_name))
}

fn parse_ldt(text: &str) -> Result<ParsedLdt, String> {
    let mut cursor = LineCursor::new(text);
    let header = parse_header(&mut cursor)?;
    let photometry = parse_photometry(&cursor, &header)?;
    Ok(ParsedLdt { header, photometry })
}

fn parse_header(cursor: &mut LineCursor) -> Result<LdtHeader, String> {
    let _identification = cursor.next_line("identification")?;
    let _type_indicator = cursor.next_i32("type indicator")?;
    let symmetry = cursor.next_i32("symmetry indicator")?;
    let num_c_planes = cursor.next_usize("C-plane count")?;
    let c_distance = cursor.next_number("C-plane distance")?;
    let num_gamma = cursor.next_usize("gamma angle count")?;
    let _gamma_distance = cursor.next_number("gamma angle distance")?;

    let _report = cursor.next_line("measurement report")?;
    let name = cursor.next_line("luminaire name")?;
    let catalog = cursor.next_line("luminaire number")?;
    let _file_name = cursor.next_line("file name")?;
    let _date = cursor.next_line("date")?;

    let length_mm = cursor.next_number("luminaire length")?;
    let width_mm = cursor.next_number("luminaire width")?;
    let height_mm = cursor.next_number("luminaire height")?;
    skip_numbers(cursor, 6, "luminous area dimensions")?;
    skip_numbers(cursor, 2, "flux fractions")?;
    let _conversion = cursor.next_number("intensity conversion factor")?;
    let _tilt = cursor.next_number("tilt angle")?;

    let lamp_sets = parse_lamp_sets(cursor)?;
    skip_numbers(cursor, DIRECT_RATIO_COUNT, "direct ratios")?;
    validate_positive(num_c_planes, MAX_C_PLANES, "C-planes")?;
    validate_positive(num_gamma, MAX_GAMMA_ANGLES, "gamma angles")?;

    Ok(LdtHeader {
        symmetry,
        num_c_planes,
        num_gamma,
        c_distance,
        name,
        catalog,
        length_mm,
        width_mm,
        height_mm,
        lamp_sets,
    })
}

fn skip_numbers(cursor: &mut LineCursor, count: usize, field: &str) -> Result<(), String> {
    for _ in 0..count {
        cursor.next_number(field)?;
    }
    Ok(())
}

fn parse_lamp_sets(cursor: &mut LineCursor) -> Result<Vec<LampSet>, String> {
    let declared = cursor.next_usize("lamp set count")?;
    validate_max(declared, MAX_LAMP_SETS, "lamp sets")?;

    let mut sets = Vec::with_capacity(declared);
    for _ in 0..declared {
        sets.push(read_lamp_set(cursor)?);
    }
    Ok(sets)
}

fn read_lamp_set(cursor: &mut LineCursor) -> Result<LampSet, String> {
    let count = cursor.next_i32("lamp count")?;
    let _type = cursor.next_line("lamp type")?;
    let flux = cursor.next_number("luminous flux")?;
    let _color = cursor.next_line("color appearance")?;
    let _rendering = cursor.next_line("color rendering group")?;
    let watts = cursor.next_number("lamp wattage")?;
    Ok(LampSet {
        count,
        flux,
        watts,
    })
}

fn parse_photometry(cursor: &LineCursor, header: &LdtHeader) -> Result<RawPhotometry, String> {
    let values = cursor.remaining_numbers();
    let (c_angles, rest) = take_values(&values, header.num_c_planes, "C angles")?;
    let (gamma_angles, rest) = take_values(rest, header.num_gamma, "gamma angles")?;
    let planes = stored_plane_count(header.symmetry, header.num_c_planes);
    let intensities = take_intensity_grid(rest, header.num_gamma, planes)?;

    Ok(RawPhotometry {
        c_angles: c_angles.to_vec(),
        gamma_angles: gamma_angles.to_vec(),
        intensities,
    })
}

fn take_values<'a>(
    values: &'a [f64],
    count: usize,
    field: &str,
) -> Result<(&'a [f64], &'a [f64]), String> {
    if values.len() < count {
        return Err(format!("LDT is missing {field}"));
    }
    Ok(values.split_at(count))
}

fn take_intensity_grid(
    values: &[f64],
    gamma_count: usize,
    planes: usize,
) -> Result<Vec<Vec<f64>>, String> {
    if gamma_count == 0 {
        return Err("LDT has no gamma angles".to_string());
    }
    let available = values.len() / gamma_count;
    if available == 0 {
        return Err("LDT has no luminous intensity data".to_string());
    }
    let rows = available.min(planes);
    Ok(values[..rows * gamma_count]
        .chunks_exact(gamma_count)
        .map(|chunk| chunk.to_vec())
        .collect())
}

fn validate_positive(value: usize, max: usize, label: &str) -> Result<(), String> {
    if value == 0 {
        return Err(format!("LDT has no {label}"));
    }
    validate_max(value, max, label)
}

fn validate_max(value: usize, max: usize, label: &str) -> Result<(), String> {
    if value > max {
        return Err(format!(
            "LDT declares {value} {label}, above the supported maximum of {max}"
        ));
    }
    Ok(())
}

/// Number of C-planes actually stored in the intensity grid for a symmetry.
fn stored_plane_count(symmetry: i32, c_planes: usize) -> usize {
    match symmetry {
        1 => 1,
        2 | 3 => c_planes / 2 + 1,
        4 => c_planes / 4 + 1,
        _ => c_planes,
    }
    .clamp(1, c_planes.max(1))
}

/// Build the horizontal angles and matching intensity rows at full 360°.
fn build_distribution(header: &LdtHeader, photometry: &RawPhotometry) -> (Vec<f64>, Vec<Vec<f64>>) {
    match header.symmetry {
        0 => direct_distribution(photometry),
        1 => rotational_distribution(photometry),
        _ => expanded_distribution(header, photometry),
    }
}

fn direct_distribution(photometry: &RawPhotometry) -> (Vec<f64>, Vec<Vec<f64>>) {
    let count = photometry.intensities.len().min(photometry.c_angles.len());
    (
        photometry.c_angles[..count].to_vec(),
        photometry.intensities[..count].to_vec(),
    )
}

/// Rotational symmetry needs two horizontal angles so the three.js loader can
/// build a web; both rows are identical.
fn rotational_distribution(photometry: &RawPhotometry) -> (Vec<f64>, Vec<Vec<f64>>) {
    let row = photometry
        .intensities
        .first()
        .cloned()
        .unwrap_or_else(|| vec![0.0; photometry.gamma_angles.len()]);
    (vec![0.0, 90.0], vec![row.clone(), row])
}

fn expanded_distribution(
    header: &LdtHeader,
    photometry: &RawPhotometry,
) -> (Vec<f64>, Vec<Vec<f64>>) {
    let stored = stored_angles(&photometry.c_angles, header.symmetry, photometry.intensities.len());
    let angles = full_c_angles(header, &photometry.c_angles);
    let rows = angles
        .iter()
        .map(|&angle| {
            let folded = fold_angle(header.symmetry, angle);
            nearest_row(&photometry.intensities, &stored, folded)
        })
        .collect();
    (angles, rows)
}

/// C angles covered by the stored intensity rows.
fn stored_angles(c_angles: &[f64], symmetry: i32, rows: usize) -> Vec<f64> {
    if rows == 0 {
        return Vec::new();
    }
    let (start, end) = stored_domain(symmetry);
    let filtered: Vec<f64> = c_angles
        .iter()
        .copied()
        .filter(|&angle| angle >= start - 1e-9 && angle <= end + 1e-9)
        .collect();
    if filtered.len() == rows {
        return filtered;
    }
    if c_angles.len() >= rows {
        return c_angles[..rows].to_vec();
    }
    evenly_spaced(rows, start, end)
}

fn stored_domain(symmetry: i32) -> (f64, f64) {
    match symmetry {
        2 => (0.0, 180.0),
        3 => (90.0, 270.0),
        4 => (0.0, 90.0),
        _ => (0.0, 360.0),
    }
}

/// Fold a full-circle C angle into the domain covered by the stored planes.
fn fold_angle(symmetry: i32, angle: f64) -> f64 {
    let c = angle.rem_euclid(360.0);
    match symmetry {
        1 => 0.0,
        2 => {
            if c <= 180.0 {
                c
            } else {
                360.0 - c
            }
        }
        3 => {
            if (90.0..=270.0).contains(&c) {
                c
            } else {
                (180.0 - c).rem_euclid(360.0)
            }
        }
        4 => {
            let half = if c <= 180.0 { c } else { 360.0 - c };
            if half <= 90.0 {
                half
            } else {
                180.0 - half
            }
        }
        _ => c,
    }
}

fn full_c_angles(header: &LdtHeader, c_angles: &[f64]) -> Vec<f64> {
    let complete = c_angles.len() >= header.num_c_planes
        && c_angles.last().is_some_and(|angle| *angle > 270.0);
    if complete {
        return c_angles[..header.num_c_planes].to_vec();
    }
    let step = valid_step(header.c_distance, header.num_c_planes);
    (0..header.num_c_planes)
        .map(|index| (index as f64 * step).rem_euclid(360.0))
        .collect()
}

fn valid_step(distance: f64, count: usize) -> f64 {
    if is_usable_step(distance) {
        distance
    } else {
        360.0 / count.max(1) as f64
    }
}

/// A step is usable when it is a finite, positive angle within one full circle.
fn is_usable_step(distance: f64) -> bool {
    if !distance.is_finite() {
        return false;
    }
    distance > 0.0 && distance <= 360.0
}

fn evenly_spaced(count: usize, start: f64, end: f64) -> Vec<f64> {
    if count == 0 {
        return Vec::new();
    }
    if count == 1 {
        return vec![start];
    }
    let step = (end - start) / (count as f64 - 1.0);
    (0..count)
        .map(|index| start + step * index as f64)
        .collect()
}

fn full_spaced(count: usize) -> Vec<f64> {
    if count == 0 {
        return Vec::new();
    }
    let step = 360.0 / count as f64;
    (0..count).map(|index| index as f64 * step).collect()
}

fn nearest_row(rows: &[Vec<f64>], stored: &[f64], angle: f64) -> Vec<f64> {
    let mut best = 0;
    let mut best_distance = f64::INFINITY;
    for (index, &stored_angle) in stored.iter().enumerate() {
        let distance = (stored_angle - angle).abs();
        if distance < best_distance {
            best_distance = distance;
            best = index;
        }
    }
    rows.get(best).cloned().unwrap_or_default()
}

fn total_flux(lamp_sets: &[LampSet]) -> f64 {
    let total: f64 = lamp_sets
        .iter()
        .map(|set| set.flux)
        .filter(|flux| flux.is_finite() && *flux > 0.0)
        .sum();
    if total.is_finite() && total > 0.0 {
        total
    } else {
        DEFAULT_FLUX_LUMENS
    }
}

fn total_lamps(lamp_sets: &[LampSet]) -> u32 {
    let total: i64 = lamp_sets
        .iter()
        .map(|set| i64::from(set.count.max(0)))
        .sum();
    total.clamp(1, i64::from(u32::MAX)) as u32
}

fn total_watts(lamp_sets: &[LampSet]) -> f64 {
    let total: f64 = lamp_sets
        .iter()
        .map(|set| set.watts)
        .filter(|watts| watts.is_finite() && *watts > 0.0)
        .sum();
    if total.is_finite() && total > 0.0 {
        total
    } else {
        0.0
    }
}

fn sanitize_candela(value: f64) -> f64 {
    if value.is_finite() && value > 0.0 {
        value
    } else {
        0.0
    }
}

fn to_candela(rows: &[Vec<f64>], flux: f64) -> Vec<Vec<f64>> {
    let factor = flux / 1000.0;
    rows.iter()
        .map(|row| {
            row.iter()
                .map(|&value| sanitize_candela(value * factor))
                .collect()
        })
        .collect()
}

/// The three.js loader cannot interpolate a single-angle axis, so a one-value
/// gamma list is expanded to a visible pair with the same intensity.
fn normalize_vertical_angles(gamma_angles: Vec<f64>, rows: &mut Vec<Vec<f64>>) -> Vec<f64> {
    if rows.is_empty() {
        rows.push(Vec::new());
    }
    if gamma_angles.len() >= 2 {
        for row in rows.iter_mut() {
            row.resize(gamma_angles.len(), 0.0);
        }
        return gamma_angles;
    }

    let value = rows
        .first()
        .and_then(|row| row.first())
        .copied()
        .unwrap_or(0.0);
    for row in rows.iter_mut() {
        *row = vec![value, value];
    }
    vec![0.0, 180.0]
}

fn normalize_horizontal_angles(
    mut angles: Vec<f64>,
    mut rows: Vec<Vec<f64>>,
) -> (Vec<f64>, Vec<Vec<f64>>) {
    if rows.is_empty() {
        rows.push(Vec::new());
    }
    if angles.len() < rows.len() {
        angles = full_spaced(rows.len());
    } else if angles.len() > rows.len() {
        angles.truncate(rows.len());
    }
    if angles.len() < MIN_HORIZONTAL_ANGLES {
        let row = rows.first().cloned().unwrap_or_default();
        return (vec![0.0, 90.0], vec![row.clone(), row]);
    }
    (angles, rows)
}

fn meters(millimeters: f64) -> f64 {
    if millimeters.is_finite() && millimeters > 0.0 {
        millimeters / 1000.0
    } else {
        0.0
    }
}

fn sanitize_ascii(input: &str) -> String {
    input
        .chars()
        .map(|character| {
            if character.is_ascii_graphic() || character == ' ' {
                character
            } else {
                '?'
            }
        })
        .collect::<String>()
        .trim()
        .to_string()
}

fn write_ies(parsed: &ParsedLdt, source_name: &str) -> String {
    let flux = total_flux(&parsed.header.lamp_sets);
    let (horizontal, rows) = build_distribution(&parsed.header, &parsed.photometry);
    let mut intensities = to_candela(&rows, flux);
    let vertical = normalize_vertical_angles(parsed.photometry.gamma_angles.clone(), &mut intensities);
    let (horizontal, intensities) = normalize_horizontal_angles(horizontal, intensities);

    let mut output = String::new();
    output.push_str("IESNA:LM-63-2002\n");
    write_keyword(&mut output, "TEST", source_name);
    write_keyword(&mut output, "MANUFAC", "PolyStash");
    write_keyword(&mut output, "LUMINAIRE", &parsed.header.name);
    write_keyword(&mut output, "LUMCAT", &parsed.header.catalog);
    output.push_str("TILT=NONE\n");

    let lamps = total_lamps(&parsed.header.lamp_sets);
    let dimensions = IesDimensions {
        width: meters(parsed.header.width_mm),
        length: meters(parsed.header.length_mm),
        height: meters(parsed.header.height_mm),
    };
    let lamp_header = LampHeader {
        lamps,
        lumens_per_lamp: flux / f64::from(lamps),
        vertical: &vertical,
        horizontal: &horizontal,
        dimensions,
    };
    write_lamp_header(&mut output, &lamp_header);
    write_factors(&mut output, total_watts(&parsed.header.lamp_sets));
    write_values(&mut output, &vertical);
    write_values(&mut output, &horizontal);
    for row in &intensities {
        write_values(&mut output, row);
    }
    output
}

fn write_keyword(output: &mut String, keyword: &str, value: &str) {
    let clean = sanitize_ascii(value);
    if clean.is_empty() {
        return;
    }
    let _ = writeln!(output, "[{keyword}] {clean}");
}

fn write_lamp_header(output: &mut String, header: &LampHeader<'_>) {
    let _ = writeln!(
        output,
        "{} {:.1} 1.000000 {} {} 1 2 {:.4} {:.4} {:.4}",
        header.lamps,
        header.lumens_per_lamp,
        header.vertical.len(),
        header.horizontal.len(),
        header.dimensions.width,
        header.dimensions.length,
        header.dimensions.height,
    );
}

fn write_factors(output: &mut String, watts: f64) {
    let _ = writeln!(output, "1.0 1.0 {watts:.1}");
}

fn write_values(output: &mut String, values: &[f64]) {
    for (index, value) in values.iter().enumerate() {
        if index > 0 {
            let separator = if index % IES_VALUES_PER_LINE == 0 {
                '\n'
            } else {
                ' '
            };
            output.push(separator);
        }
        let _ = write!(output, "{value:.2}");
    }
    output.push('\n');
}

#[cfg(test)]
mod tests {
    use super::*;

    fn minimal_ldt() -> String {
        let mut lines = vec![
            "PolyStash",
            "1",
            "0",
            "4",
            "90",
            "3",
            "90",
            "",
            "Test Luminaire",
            "TEST-1",
            "test.ldt",
            "",
            "100",
            "100",
            "100",
            "0",
            "0",
            "0",
            "0",
            "0",
            "0",
            "100",
            "100",
            "1",
            "0",
            "1",
            "1",
            "LED",
            "1000",
            "3000K",
            "CRI80",
            "10",
        ];
        lines.extend(std::iter::repeat_n("0", DIRECT_RATIO_COUNT));
        lines.extend(["0", "90", "180", "270"]);
        lines.extend(["0", "45", "90"]);
        for _ in 0..4 {
            lines.extend(["100", "200", "300"]);
        }
        lines.join("\n")
    }

    #[test]
    fn converts_minimal_ldt_to_ies() {
        let ies = convert_text(&minimal_ldt(), "test.ldt").expect("minimal LDT should convert");

        assert!(ies.starts_with("IESNA:LM-63-2002\n"));
        assert!(ies.contains("TILT=NONE"));
        assert!(ies.contains("[MANUFAC] PolyStash"));
        assert!(ies.is_ascii());
        assert!(ies.contains("100.00"));
        assert!(ies.contains("200.00"));
        assert!(ies.contains("300.00"));
    }

    #[test]
    fn rejects_garbage() {
        assert!(convert_text("not a valid ldt", "bad.ldt").is_err());
    }
}
