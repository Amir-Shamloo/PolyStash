//! Structural metadata read from container headers rather than decoded data.
//!
//! `image`/`ffprobe` describe the decoded stream; these parsers describe what
//! the file itself declares: JPEG frame/JFIF/Huffman markers, PNG chunks, and
//! ISO base media (MP4/MOV) boxes. Every parser is best effort and returns an
//! empty map for a file it cannot read, so a broken header never fails a job.

use std::fs::File;
use std::io::{BufReader, Read, Seek, SeekFrom};
use std::path::Path;

use flate2::read::ZlibDecoder;
use serde_json::{json, Map, Value};

use super::round_two;

/// Text chunks larger than this are skipped instead of buffered.
const MAX_TEXT_CHUNK: usize = 1024 * 1024;

/// Seconds between the ISO base media epoch (1904-01-01) and the Unix epoch.
const MOVIE_EPOCH_OFFSET: i64 = 2_082_844_800;

const PNG_SIGNATURE: [u8; 8] = [0x89, b'P', b'N', b'G', 0x0d, 0x0a, 0x1a, 0x0a];

/// Indexed by `marker - 0xC0`; C4/C8/CC are not frame headers.
const JPEG_COMPRESSION_TYPES: [&str; 16] = [
    "Baseline",
    "Extended sequential",
    "Progressive",
    "Lossless",
    "Unknown",
    "Differential sequential",
    "Differential progressive",
    "Differential lossless",
    "Unknown",
    "Extended sequential, arithmetic",
    "Progressive, arithmetic",
    "Lossless, arithmetic",
    "Unknown",
    "Differential sequential, arithmetic",
    "Differential progressive, arithmetic",
    "Differential lossless, arithmetic",
];

const JFIF_RESOLUTION_UNITS: [&str; 3] = ["none", "inches", "cm"];

const PNG_COLOR_TYPES: [&str; 7] = [
    "Grayscale",
    "Unknown",
    "Truecolor",
    "Indexed color",
    "Grayscale with alpha",
    "Unknown",
    "Truecolor with alpha",
];

/// Structural fields for `source`, empty when this module does not parse it.
pub(crate) fn extract(source: &Path) -> Map<String, Value> {
    let ext = source
        .extension()
        .and_then(|extension| extension.to_str())
        .unwrap_or("")
        .to_ascii_lowercase();

    match ext.as_str() {
        "jpg" | "jpeg" => jpeg(source),
        "png" => png(source),
        "mp4" | "mov" => movie(source),
        _ => Map::new(),
    }
}

fn read_u8(reader: &mut impl Read) -> Option<u8> {
    let mut byte = [0_u8; 1];
    reader.read_exact(&mut byte).ok()?;
    Some(byte[0])
}

fn read_u16(reader: &mut impl Read) -> Option<u16> {
    let mut bytes = [0_u8; 2];
    reader.read_exact(&mut bytes).ok()?;
    Some(u16::from_be_bytes(bytes))
}

fn read_u32(reader: &mut impl Read) -> Option<u32> {
    let mut bytes = [0_u8; 4];
    reader.read_exact(&mut bytes).ok()?;
    Some(u32::from_be_bytes(bytes))
}

fn read_u64(reader: &mut impl Read) -> Option<u64> {
    let mut bytes = [0_u8; 8];
    reader.read_exact(&mut bytes).ok()?;
    Some(u64::from_be_bytes(bytes))
}

fn be_u16(bytes: &[u8], offset: usize) -> Option<u16> {
    let slice: [u8; 2] = bytes.get(offset..offset + 2)?.try_into().ok()?;
    Some(u16::from_be_bytes(slice))
}

fn be_u32(bytes: &[u8], offset: usize) -> Option<u32> {
    let slice: [u8; 4] = bytes.get(offset..offset + 4)?.try_into().ok()?;
    Some(u32::from_be_bytes(slice))
}

fn be_u64(bytes: &[u8], offset: usize) -> Option<u64> {
    let slice: [u8; 8] = bytes.get(offset..offset + 8)?.try_into().ok()?;
    Some(u64::from_be_bytes(slice))
}

fn skip_bytes(reader: &mut impl Seek, length: usize) -> Option<()> {
    let distance = i64::try_from(length).ok()?;
    reader.seek(SeekFrom::Current(distance)).ok()?;
    Some(())
}

// ---------------------------------------------------------------------------
// JPEG
// ---------------------------------------------------------------------------

/// Frame description from a SOFn marker.
struct JpegFrame {
    marker: u8,
    precision: u8,
    width: u16,
    height: u16,
    /// (component id, sampling factors, quantization table) per component.
    components: Vec<(u8, u8, u8)>,
}

#[derive(Default)]
struct JpegHeader {
    fields: Map<String, Value>,
    frame: Option<JpegFrame>,
    huffman_tables: u32,
    adobe_transform: Option<u8>,
}

fn jpeg(source: &Path) -> Map<String, Value> {
    let Ok(file) = File::open(source) else {
        return Map::new();
    };
    let mut reader = BufReader::new(file);
    if read_u16(&mut reader) != Some(0xFFD8) {
        return Map::new();
    }

    let mut header = JpegHeader::default();
    while let Some((marker, payload)) = next_jpeg_segment(&mut reader) {
        absorb_jpeg_segment(marker, &payload, &mut header);
    }
    finish_jpeg(header)
}

/// Next marker that carries a payload; `None` at EOI/SOS or on a read failure.
fn next_jpeg_segment(reader: &mut impl Read) -> Option<(u8, Vec<u8>)> {
    loop {
        let marker = next_jpeg_marker(reader)?;
        // EOI, and the compressed scan payload begins at SOS: no structural
        // marker we care about follows it.
        if matches!(marker, 0xD9 | 0xDA) {
            return None;
        }
        // TEM, RSTn, and a repeated SOI carry no length field.
        if matches!(marker, 0x01 | 0xD0..=0xD8) {
            continue;
        }
        let length = read_u16(reader)?;
        if length < 2 {
            return None;
        }
        let mut payload = vec![0_u8; usize::from(length - 2)];
        reader.read_exact(&mut payload).ok()?;
        return Some((marker, payload));
    }
}

fn absorb_jpeg_segment(marker: u8, payload: &[u8], header: &mut JpegHeader) {
    match marker {
        0xC4 => header.huffman_tables += huffman_table_count(payload),
        0xE0 => parse_jfif(payload, &mut header.fields),
        0xEE => header.adobe_transform = adobe_transform_of(payload),
        0xC0..=0xCF if !matches!(marker, 0xC4 | 0xC8 | 0xCC) => {
            header.frame = parse_jpeg_frame(marker, payload);
        }
        _ => {}
    }
}

fn finish_jpeg(header: JpegHeader) -> Map<String, Value> {
    let JpegHeader {
        mut fields,
        frame,
        huffman_tables,
        adobe_transform,
    } = header;

    if let Some(frame) = frame {
        write_jpeg_frame(&frame, adobe_transform, &mut fields);
    }
    if huffman_tables > 0 {
        fields.insert("huffmanTableCount".to_string(), json!(huffman_tables));
    }
    fields
}

/// Advance to the next marker byte, skipping padding `0xFF` fill bytes.
fn next_jpeg_marker(reader: &mut impl Read) -> Option<u8> {
    loop {
        if read_u8(reader)? != 0xFF {
            continue;
        }
        let mut marker = read_u8(reader)?;
        while marker == 0xFF {
            marker = read_u8(reader)?;
        }
        // 0x00 is a stuffed data byte, not a marker.
        if marker != 0x00 {
            return Some(marker);
        }
    }
}

fn parse_jpeg_frame(marker: u8, payload: &[u8]) -> Option<JpegFrame> {
    let precision = *payload.first()?;
    let height = be_u16(payload, 1)?;
    let width = be_u16(payload, 3)?;
    let count = usize::from(*payload.get(5)?);

    let components = (0..count)
        .map(|index| {
            let base = 6 + index * 3;
            Some((
                *payload.get(base)?,
                *payload.get(base + 1)?,
                *payload.get(base + 2)?,
            ))
        })
        .collect::<Option<Vec<_>>>()?;

    Some(JpegFrame {
        marker,
        precision,
        width,
        height,
        components,
    })
}

fn write_jpeg_frame(
    frame: &JpegFrame,
    adobe_transform: Option<u8>,
    fields: &mut Map<String, Value>,
) {
    let compression = usize::from(frame.marker.wrapping_sub(0xC0));
    fields.insert(
        "compressionType".to_string(),
        json!(JPEG_COMPRESSION_TYPES
            .get(compression)
            .copied()
            .unwrap_or("Unknown")),
    );
    fields.insert(
        "dataPrecision".to_string(),
        json!(format!("{} bits", frame.precision)),
    );
    fields.insert("imageWidth".to_string(), json!(frame.width));
    fields.insert("imageHeight".to_string(), json!(frame.height));
    fields.insert(
        "numberOfComponents".to_string(),
        json!(frame.components.len()),
    );

    let names = frame.component_names(adobe_transform);
    for (index, ((_, sampling, quant), name)) in frame.components.iter().zip(names).enumerate() {
        fields.insert(
            format!("component{}", index + 1),
            json!(format!(
                "{name} component: Quantization table {quant}, Sampling factors {} horiz/{} vert",
                sampling >> 4,
                sampling & 0x0F
            )),
        );
    }
}

impl JpegFrame {
    /// Conventional component labels for the frame's component count. Adobe's
    /// transform 2 marks four-component data as YCCK rather than CMYK.
    fn component_names(&self, adobe_transform: Option<u8>) -> Vec<&'static str> {
        match self.components.len() {
            1 => vec!["Y"],
            3 => vec!["Y", "Cb", "Cr"],
            4 if adobe_transform == Some(2) => vec!["Y", "Cb", "Cr", "K"],
            4 => vec!["C", "M", "Y", "K"],
            count => vec!["Component"; count],
        }
    }
}

fn parse_jfif(payload: &[u8], fields: &mut Map<String, Value>) {
    if !payload.starts_with(b"JFIF\0") || payload.len() < 14 {
        return;
    }

    fields.insert(
        "jfifVersion".to_string(),
        json!(format!("{}.{}", payload[5], payload[6])),
    );
    fields.insert(
        "resolutionUnits".to_string(),
        json!(JFIF_RESOLUTION_UNITS
            .get(usize::from(payload[7]))
            .copied()
            .unwrap_or("none")),
    );
    fields.insert(
        "xResolution".to_string(),
        json!(be_u16(payload, 8).unwrap_or_default()),
    );
    fields.insert(
        "yResolution".to_string(),
        json!(be_u16(payload, 10).unwrap_or_default()),
    );
    fields.insert("thumbnailWidthPixels".to_string(), json!(payload[12]));
    fields.insert("thumbnailHeightPixels".to_string(), json!(payload[13]));
}

/// APP14 Adobe marker: only the colour transform is relevant here.
fn adobe_transform_of(payload: &[u8]) -> Option<u8> {
    payload
        .starts_with(b"Adobe")
        .then(|| payload.get(11).copied())?
}

fn huffman_table_count(payload: &[u8]) -> u32 {
    let mut count = 0;
    let mut offset = 0;
    while offset < payload.len() {
        // One table: class/id byte, 16 code-length counts, then symbols.
        let Some(symbols) = payload.get(offset + 1..offset + 17).map(|counts| {
            counts
                .iter()
                .map(|&value| usize::from(value))
                .sum::<usize>()
        }) else {
            break;
        };
        offset += 17 + symbols;
        count += 1;
    }
    count
}

// ---------------------------------------------------------------------------
// PNG
// ---------------------------------------------------------------------------

fn png(source: &Path) -> Map<String, Value> {
    let Ok(file) = File::open(source) else {
        return Map::new();
    };
    let mut reader = BufReader::new(file);
    let mut signature = [0_u8; 8];
    if reader.read_exact(&mut signature).is_err() || signature != PNG_SIGNATURE {
        return Map::new();
    }

    let mut fields = Map::new();
    while let Some((kind, data)) = next_png_chunk(&mut reader) {
        parse_png_chunk(&kind, &data, &mut fields);
    }
    fields
}

/// Next chunk whose body we keep; `None` at IEND or on a read failure.
fn next_png_chunk(reader: &mut BufReader<File>) -> Option<([u8; 4], Vec<u8>)> {
    loop {
        let length = read_u32(reader)?;
        let mut kind = [0_u8; 4];
        reader.read_exact(&mut kind).ok()?;
        if &kind == b"IEND" {
            return None;
        }
        let length = usize::try_from(length).ok()?;
        if let Some(data) = chunk_body(reader, length, png_chunk_limit(&kind))? {
            return Some((kind, data));
        }
    }
}

/// Bodies up to this size are parsed; larger chunks are skipped by `chunk_body`.
fn png_chunk_limit(kind: &[u8; 4]) -> usize {
    match kind {
        b"IHDR" => 13,
        b"pHYs" => 9,
        b"tEXt" | b"zTXt" | b"iTXt" => MAX_TEXT_CHUNK,
        _ => 0,
    }
}

/// Read a chunk body plus its CRC. Oversized bodies are skipped and reported
/// as `Some(None)`, an I/O failure as `None` so the scan stops.
fn chunk_body(
    reader: &mut BufReader<File>,
    length: usize,
    limit: usize,
) -> Option<Option<Vec<u8>>> {
    let data = if length <= limit {
        let mut data = vec![0_u8; length];
        reader.read_exact(&mut data).ok()?;
        Some(data)
    } else {
        skip_bytes(reader, length)?;
        None
    };
    skip_bytes(reader, 4)?;
    Some(data)
}

fn parse_png_chunk(kind: &[u8; 4], data: &[u8], fields: &mut Map<String, Value>) {
    match kind {
        b"IHDR" => parse_ihdr(data, fields),
        b"pHYs" => parse_phys(data, fields),
        b"tEXt" | b"zTXt" | b"iTXt" => parse_png_text(kind, data, fields),
        _ => {}
    }
}

fn parse_ihdr(data: &[u8], fields: &mut Map<String, Value>) {
    let (Some(width), Some(height)) = (be_u32(data, 0), be_u32(data, 4)) else {
        return;
    };
    let Some(&[bit_depth, color_type, compression, filter, interlace]) = data.get(8..13) else {
        return;
    };

    fields.insert("imageWidth".to_string(), json!(width));
    fields.insert("imageHeight".to_string(), json!(height));
    fields.insert("bitDepth".to_string(), json!(bit_depth));
    fields.insert(
        "colorType".to_string(),
        json!(PNG_COLOR_TYPES
            .get(usize::from(color_type))
            .copied()
            .unwrap_or("Unknown")),
    );
    fields.insert(
        "compressionMethod".to_string(),
        json!(if compression == 0 {
            "Deflate"
        } else {
            "Unknown"
        }),
    );
    fields.insert(
        "filterMethod".to_string(),
        json!(if filter == 0 { "Adaptive" } else { "Unknown" }),
    );
    fields.insert(
        "interlaceMethod".to_string(),
        json!(match interlace {
            0 => "None",
            1 => "Adam7",
            _ => "Unknown",
        }),
    );
}

fn parse_phys(data: &[u8], fields: &mut Map<String, Value>) {
    let (Some(x), Some(y)) = (be_u32(data, 0), be_u32(data, 4)) else {
        return;
    };
    let unit = data.get(8).copied().unwrap_or_default();

    fields.insert("pixelDensityX".to_string(), json!(x));
    fields.insert("pixelDensityY".to_string(), json!(y));
    fields.insert(
        "pixelDensityUnit".to_string(),
        json!(if unit == 1 {
            "pixels per meter"
        } else {
            "unknown"
        }),
    );

    let density = PixelDensity {
        x,
        y,
        metres: unit == 1,
    };
    if let Some(dpi) = density.dpi() {
        fields.insert("dpi".to_string(), json!(dpi));
    }
}

/// `pHYs` density, which only converts to DPI when stored per metre.
struct PixelDensity {
    x: u32,
    y: u32,
    metres: bool,
}

impl PixelDensity {
    /// 1 inch = 0.0254 m; requires a square, non-zero density.
    fn dpi(self) -> Option<f64> {
        if !self.metres {
            return None;
        }
        (self.x == self.y && self.x > 0).then(|| round_two(f64::from(self.x) * 0.0254))
    }
}

fn parse_png_text(kind: &[u8; 4], data: &[u8], fields: &mut Map<String, Value>) {
    match kind {
        b"tEXt" => {
            let Some((keyword, text)) = split_nul(data) else {
                return;
            };
            let decoded: String = text.iter().map(|&byte| char::from(byte)).collect();
            insert_text_chunk(fields, keyword, decoded);
        }
        b"zTXt" => {
            let Some((keyword, rest)) = split_nul(data) else {
                return;
            };
            if let Some(text) = rest.get(1..).and_then(inflate) {
                insert_text_chunk(fields, keyword, text);
            }
        }
        b"iTXt" => parse_itxt(data, fields),
        _ => {}
    }
}

fn parse_itxt(data: &[u8], fields: &mut Map<String, Value>) {
    let Some((keyword, rest)) = split_nul(data) else {
        return;
    };
    let Some((&compressed, rest)) = rest.split_first() else {
        return;
    };
    // Compression method byte, then language tag and translated keyword.
    let Some((_method, rest)) = rest.split_first() else {
        return;
    };
    let Some((_language, rest)) = split_nul(rest) else {
        return;
    };
    let Some((_translated, text)) = split_nul(rest) else {
        return;
    };

    let value = if compressed == 1 {
        inflate(text)
    } else {
        Some(String::from_utf8_lossy(text).into_owned())
    };
    if let Some(value) = value {
        insert_text_chunk(fields, keyword, value);
    }
}

fn insert_text_chunk(fields: &mut Map<String, Value>, keyword: &[u8], text: String) {
    if keyword.is_empty() || text.trim().is_empty() {
        return;
    }
    let keyword = String::from_utf8_lossy(keyword);
    fields.insert(format!("text.{keyword}"), json!(text));
}

/// Split a `keyword\\0value` chunk at its first NUL byte.
fn split_nul(data: &[u8]) -> Option<(&[u8], &[u8])> {
    let index = data.iter().position(|&byte| byte == 0)?;
    Some((&data[..index], &data[index + 1..]))
}

fn inflate(data: &[u8]) -> Option<String> {
    let mut text = String::new();
    ZlibDecoder::new(data)
        .take(MAX_TEXT_CHUNK as u64)
        .read_to_string(&mut text)
        .ok()?;
    Some(text)
}

// ---------------------------------------------------------------------------
// ISO base media (MP4 / MOV)
// ---------------------------------------------------------------------------

struct BoxHeader {
    kind: [u8; 4],
    data_start: u64,
    end: u64,
}

impl BoxHeader {
    fn payload_length(&self) -> usize {
        usize::try_from(self.end.saturating_sub(self.data_start)).unwrap_or(0)
    }
}

fn movie(source: &Path) -> Map<String, Value> {
    let Ok(file) = File::open(source) else {
        return Map::new();
    };
    let Ok(file_len) = file.metadata().map(|meta| meta.len()) else {
        return Map::new();
    };
    let mut reader = BufReader::new(file);
    let mut fields = Map::new();

    walk_boxes(&mut reader, 0, file_len, |reader, header| {
        match &header.kind {
            b"ftyp" => read_ftyp(reader, header, &mut fields),
            b"moov" => read_moov(reader, header, &mut fields),
            _ => {}
        }
    });
    fields
}

/// Visit each child box in `start..end`, stopping at the first malformed header.
fn walk_boxes<R: Read + Seek>(
    reader: &mut R,
    start: u64,
    end: u64,
    mut visit: impl FnMut(&mut R, &BoxHeader),
) {
    let mut offset = start;
    while offset + 8 <= end {
        if reader.seek(SeekFrom::Start(offset)).is_err() {
            break;
        }
        let Some(header) = read_box_header(reader, end) else {
            break;
        };
        visit(reader, &header);
        if header.end <= offset {
            break;
        }
        offset = header.end;
    }
}

fn read_box_header(reader: &mut (impl Read + Seek), limit: u64) -> Option<BoxHeader> {
    let start = reader.stream_position().ok()?;
    if start + 8 > limit {
        return None;
    }
    let size = read_u32(reader)?;
    let mut kind = [0_u8; 4];
    reader.read_exact(&mut kind).ok()?;

    let (end, data_start) = match size {
        0 => (limit, start + 8),
        1 => (start.checked_add(read_u64(reader)?)?, start + 16),
        _ => (start.checked_add(u64::from(size))?, start + 8),
    };

    let end = end.min(limit);
    (end >= data_start).then_some(BoxHeader {
        kind,
        data_start,
        end,
    })
}

fn box_payload(
    reader: &mut (impl Read + Seek),
    header: &BoxHeader,
    limit: usize,
) -> Option<Vec<u8>> {
    let mut data = vec![0_u8; header.payload_length().min(limit)];
    reader.seek(SeekFrom::Start(header.data_start)).ok()?;
    reader.read_exact(&mut data).ok()?;
    Some(data)
}

fn read_ftyp(reader: &mut (impl Read + Seek), header: &BoxHeader, fields: &mut Map<String, Value>) {
    let Some(data) = box_payload(reader, header, 4096) else {
        return;
    };
    parse_ftyp(&data, fields);
}

fn parse_ftyp(data: &[u8], fields: &mut Map<String, Value>) {
    let (Some(major), Some(minor)) = (data.get(0..4), be_u32(data, 4)) else {
        return;
    };
    let major = fourcc(major);
    let compatible: Vec<String> = data
        .get(8..)
        .unwrap_or_default()
        .chunks_exact(4)
        .map(fourcc)
        .collect();

    insert_file_type(&major, fields);
    fields.insert("majorBrand".to_string(), json!(major));
    fields.insert("minorVersion".to_string(), json!(minor));
    fields.insert("compatibleBrands".to_string(), json!(compatible.join(", ")));
}

fn fourcc(bytes: &[u8]) -> String {
    let text: String = bytes.iter().map(|&byte| char::from(byte)).collect();
    text.trim_end_matches(|character| character == ' ' || character == '\0')
        .to_string()
}

/// MediaInfo-style identification derived from the `ftyp` major brand.
fn insert_file_type(major: &str, fields: &mut Map<String, Value>) {
    let (name, long_name, mime, extension) = match major {
        "qt" => ("QuickTime", "QuickTime", "video/quicktime", "mov"),
        "M4A" => ("M4A", "MPEG-4 Audio", "audio/mp4", "m4a"),
        "M4V" => ("M4V", "MPEG-4 Video", "video/x-m4v", "m4v"),
        "3gp4" | "3gp5" | "3gp6" | "3gpp" => ("3GPP", "3GPP Media", "video/3gpp", "3gp"),
        "3g2a" | "3g2b" | "3g2c" => ("3GPP2", "3GPP2 Media", "video/3gpp2", "3g2"),
        _ => ("MP4", "MPEG-4 Part 14", "video/mp4", "mp4"),
    };

    fields.insert("detectedFileTypeName".to_string(), json!(name));
    fields.insert("detectedFileTypeLongName".to_string(), json!(long_name));
    fields.insert("detectedMimeType".to_string(), json!(mime));
    fields.insert("expectedFileNameExtension".to_string(), json!(extension));
}

fn read_moov(reader: &mut (impl Read + Seek), moov: &BoxHeader, fields: &mut Map<String, Value>) {
    let mut track_index = 0_usize;
    walk_boxes(
        reader,
        moov.data_start,
        moov.end,
        |reader, header| match &header.kind {
            b"mvhd" => read_mvhd(reader, header, fields),
            b"trak" => {
                track_index += 1;
                read_trak(reader, header, track_index, fields);
            }
            _ => {}
        },
    );
}

fn read_mvhd(reader: &mut (impl Read + Seek), header: &BoxHeader, fields: &mut Map<String, Value>) {
    let Some(data) = box_payload(reader, header, 256) else {
        return;
    };
    parse_mvhd(&data, fields);
}

fn parse_mvhd(data: &[u8], fields: &mut Map<String, Value>) {
    let Some(full) = FullBox::read(data) else {
        return;
    };
    let Some((created, modified, timescale, duration, cursor)) = full.movie_times() else {
        return;
    };

    fields.insert("movieHeaderVersion".to_string(), json!(full.version));
    fields.insert("movieHeaderFlags".to_string(), json!(full.flags_text()));
    fields.insert(
        "movieCreated".to_string(),
        json!(MovieTimestamp(created).text()),
    );
    fields.insert(
        "movieModified".to_string(),
        json!(MovieTimestamp(modified).text()),
    );
    if timescale > 0 {
        let seconds = (duration as f64 / f64::from(timescale)).round() as u64;
        fields.insert(
            "movieDuration".to_string(),
            json!(format!(
                "{:02}:{:02}:{:02}",
                seconds / 3600,
                seconds % 3600 / 60,
                seconds % 60
            )),
        );
    }

    insert_movie_playback(data, cursor, fields);
}

/// A box payload that starts with the ISO full-box version and flags.
struct FullBox<'a> {
    version: u8,
    flags: u32,
    data: &'a [u8],
}

impl<'a> FullBox<'a> {
    fn read(data: &'a [u8]) -> Option<Self> {
        Some(Self {
            version: *data.first()?,
            flags: be_u24(data, 1)?,
            data,
        })
    }

    fn flags_text(&self) -> String {
        format!(
            "{} {} {}",
            (self.flags >> 16) & 0xFF,
            (self.flags >> 8) & 0xFF,
            self.flags & 0xFF
        )
    }

    /// Version-dependent `mvhd` times, plus the offset of `preferred rate`.
    fn movie_times(&self) -> Option<(u64, u64, u32, u64, usize)> {
        let data = self.data;
        if self.version == 1 {
            Some((
                be_u64(data, 4)?,
                be_u64(data, 12)?,
                be_u32(data, 20)?,
                be_u64(data, 24)?,
                32,
            ))
        } else {
            Some((
                u64::from(be_u32(data, 4)?),
                u64::from(be_u32(data, 8)?),
                be_u32(data, 12)?,
                u64::from(be_u32(data, 16)?),
                20,
            ))
        }
    }

    /// Version-dependent `tkhd` times, plus the offset of the reserved block.
    fn track_times(&self) -> Option<(u64, u64, u32, u64)> {
        let data = self.data;
        if self.version == 1 {
            Some((
                be_u64(data, 4)?,
                be_u64(data, 12)?,
                be_u32(data, 20)?,
                be_u64(data, 28)?,
            ))
        } else {
            Some((
                u64::from(be_u32(data, 4)?),
                u64::from(be_u32(data, 8)?),
                be_u32(data, 12)?,
                u64::from(be_u32(data, 20)?),
            ))
        }
    }
}

fn insert_movie_playback(data: &[u8], cursor: usize, fields: &mut Map<String, Value>) {
    if let Some(rate) = be_u32(data, cursor) {
        fields.insert(
            "preferredRate".to_string(),
            json!(round_two(f64::from(rate as i32) / 65_536.0)),
        );
    }
    if let Some(volume) = be_u16(data, cursor + 4) {
        fields.insert(
            "preferredVolume".to_string(),
            json!(round_two(f64::from(volume as i16) / 256.0)),
        );
    }
    if let Some(matrix) = parse_matrix(data, cursor + 16) {
        fields.insert("movieMatrix".to_string(), json!(matrix_string(&matrix)));
    }

    for (key, offset) in [
        ("previewTime", 52),
        ("previewDuration", 56),
        ("posterTime", 60),
        ("selectionTime", 64),
        ("selectionDuration", 68),
        ("currentTime", 72),
        ("nextTrackId", 76),
    ] {
        if let Some(value) = be_u32(data, cursor + offset) {
            fields.insert(key.to_string(), json!(value));
        }
    }
}

fn read_trak(
    reader: &mut (impl Read + Seek),
    trak: &BoxHeader,
    index: usize,
    fields: &mut Map<String, Value>,
) {
    walk_boxes(reader, trak.data_start, trak.end, |reader, header| {
        if &header.kind == b"tkhd" {
            if let Some(data) = box_payload(reader, header, 256) {
                parse_tkhd(&data, index, fields);
            }
        }
    });
}

fn parse_tkhd(data: &[u8], index: usize, fields: &mut Map<String, Value>) {
    let Some(full) = FullBox::read(data) else {
        return;
    };
    let Some((created, modified, track_id, duration)) = full.track_times() else {
        return;
    };
    let cursor = if full.version == 1 { 36 } else { 24 };

    fields.insert(format!("track{index}Version"), json!(full.version));
    fields.insert(format!("track{index}Flags"), json!(full.flags_text()));
    fields.insert(
        format!("track{index}Created"),
        json!(MovieTimestamp(created).text()),
    );
    fields.insert(
        format!("track{index}Modified"),
        json!(MovieTimestamp(modified).text()),
    );
    fields.insert(format!("track{index}Id"), json!(track_id));
    fields.insert(format!("track{index}Duration"), json!(duration));

    insert_track_layout(data, cursor, index, fields);
}

fn insert_track_layout(data: &[u8], cursor: usize, index: usize, fields: &mut Map<String, Value>) {
    if let Some(layer) = be_u16(data, cursor + 8) {
        fields.insert(format!("track{index}Layer"), json!(layer as i16));
    }
    if let Some(group) = be_u16(data, cursor + 10) {
        fields.insert(format!("track{index}AlternateGroup"), json!(group as i16));
    }
    if let Some(volume) = be_u16(data, cursor + 12) {
        fields.insert(
            format!("track{index}Volume"),
            json!(round_two(f64::from(volume as i16) / 256.0)),
        );
    }
    if let Some(matrix) = parse_matrix(data, cursor + 16) {
        fields.insert(format!("track{index}Matrix"), json!(matrix_string(&matrix)));
        fields.insert(
            format!("track{index}Rotation"),
            json!(rotation_degrees(&matrix)),
        );
    }
    if let Some(width) = be_u32(data, cursor + 52) {
        fields.insert(
            format!("track{index}Width"),
            json!(round_two(f64::from(width) / 65_536.0)),
        );
    }
    if let Some(height) = be_u32(data, cursor + 56) {
        fields.insert(
            format!("track{index}Height"),
            json!(round_two(f64::from(height) / 65_536.0)),
        );
    }
}

fn be_u24(data: &[u8], offset: usize) -> Option<u32> {
    let bytes: [u8; 3] = data.get(offset..offset + 3)?.try_into().ok()?;
    Some(u32::from_be_bytes([0, bytes[0], bytes[1], bytes[2]]))
}

fn parse_matrix(data: &[u8], offset: usize) -> Option<[f64; 9]> {
    let mut matrix = [0.0_f64; 9];
    for (index, value) in matrix.iter_mut().enumerate() {
        let raw = be_u32(data, offset + index * 4)? as i32;
        *value = round_two(f64::from(raw) / 65_536.0);
    }
    Some(matrix)
}

fn matrix_string(matrix: &[f64; 9]) -> String {
    matrix
        .iter()
        .map(|value| {
            if value.fract() == 0.0 {
                format!("{}", *value as i64)
            } else {
                format!("{value}")
            }
        })
        .collect::<Vec<_>>()
        .join(" ")
}

/// Track rotation in degrees, recovered from the display matrix.
fn rotation_degrees(matrix: &[f64; 9]) -> f64 {
    round_two(matrix[1].atan2(matrix[0]).to_degrees().rem_euclid(360.0))
}

/// ISO base media timestamps count seconds from 1904-01-01 UTC.
struct MovieTimestamp(u64);

impl MovieTimestamp {
    fn text(self) -> String {
        let unix = i64::try_from(self.0)
            .unwrap_or(i64::MAX)
            .saturating_sub(MOVIE_EPOCH_OFFSET);
        chrono::DateTime::from_timestamp(unix, 0)
            .map(|date| date.format("%a %b %d %H:%M:%S %Y").to_string())
            .unwrap_or_else(|| self.0.to_string())
    }
}
