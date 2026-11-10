use serde_json::{json, Map, Value};
use std::path::Path;
use std::process::Stdio;

/// Page count, page size, and authorship from `pdfinfo`, when it is installed.
/// A missing tool is not an error; the preview itself does not depend on it.
pub(crate) async fn pdf_metadata(source: &Path) -> Option<Value> {
    let out = super::spawn_command("pdfinfo")
        .arg(source)
        .stdin(Stdio::null())
        .output()
        .await
        .ok()?;
    if !out.status.success() {
        return None;
    }

    let text = String::from_utf8_lossy(&out.stdout);
    let mut fields = Map::new();
    for line in text.lines() {
        let Some((key, value)) = line.split_once(':') else {
            continue;
        };
        insert_pdf_field(&mut fields, key.trim(), value.trim());
    }

    (!fields.is_empty()).then_some(Value::Object(fields))
}

fn insert_pdf_field(fields: &mut Map<String, Value>, key: &str, value: &str) {
    if value.is_empty() {
        return;
    }

    match key {
        "Pages" => {
            if let Ok(pages) = value.parse::<u64>() {
                fields.insert("pages".to_string(), json!(pages));
            }
        }
        "Page size" => {
            fields.insert("pageSize".to_string(), json!(value));
        }
        "Author" => {
            fields.insert("author".to_string(), json!(value));
        }
        "Creator" => {
            fields.insert("creator".to_string(), json!(value));
        }
        "Producer" => {
            fields.insert("producer".to_string(), json!(value));
        }
        _ => {}
    }
}
