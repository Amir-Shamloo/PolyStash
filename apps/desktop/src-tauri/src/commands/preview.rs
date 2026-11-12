use std::sync::Arc;
use base64::Engine as _;
use tauri::State;
use uuid::Uuid;
use chrono::Utc;

use crate::AppState;
use crate::models::PreviewJob;

type Result<T> = std::result::Result<T, String>;

#[tauri::command]
pub async fn request_preview(
    asset_ids: Vec<String>,
    state: State<'_, Arc<AppState>>,
) -> Result<()> {
    let libs = state.db.get_all_libraries().await;
    let now = Utc::now().timestamp_millis();

    for asset_id in &asset_ids {
        let mut target_pool = None;
        for lib in &libs {
            let exists: Option<(String,)> = sqlx::query_as("SELECT id FROM assets WHERE id = ?")
                .bind(asset_id)
                .fetch_optional(&lib.pool)
                .await
                .unwrap_or(None);

            if exists.is_some() {
                target_pool = Some(lib.pool.clone());
                break;
            }
        }

        let pool = match target_pool {
            Some(p) => p,
            None => state.db.get_active_pool().await,
        };

        // Delete any existing preview job so a new one is queued cleanly
        let _ = sqlx::query("DELETE FROM preview_jobs WHERE asset_id = ?")
            .bind(asset_id)
            .execute(&pool)
            .await;

        let id = Uuid::new_v4().to_string();
        let _ = sqlx::query(
            "INSERT INTO preview_jobs (id, asset_id, status, created_at) VALUES (?, ?, 'queued', ?)"
        )
        .bind(&id).bind(asset_id).bind(now)
        .execute(&pool).await;

        // Mark asset as generating / pending
        let _ = sqlx::query("UPDATE assets SET preview_status = 'pending' WHERE id = ?")
            .bind(asset_id).execute(&pool).await;
    }
    Ok(())
}

/// Persist a GLB produced in the webview by the USD converter, under the asset
/// library's `.polystash/models/` directory, and point the asset's
/// `preview_model` at it so the 3D viewer loads the cached file.
#[tauri::command]
pub async fn save_converted_usd_glb(
    asset_id: String,
    file_name: String,
    base64_bytes: String,
    state: State<'_, Arc<AppState>>,
) -> Result<String> {
    if file_name.is_empty() || file_name.contains(['/', '\\']) {
        return Err("invalid file name".to_string());
    }

    let libs = state.db.get_all_libraries().await;
    let mut target: Option<(std::path::PathBuf, sqlx::Pool<sqlx::Sqlite>)> = None;
    for lib in &libs {
        let row: Option<(String,)> = sqlx::query_as("SELECT id FROM assets WHERE id = ?")
            .bind(&asset_id)
            .fetch_optional(&lib.pool)
            .await
            .unwrap_or(None);
        if row.is_some() {
            target = Some((lib.root_path.clone(), lib.pool.clone()));
            break;
        }
    }
    let (root, pool) = target.ok_or_else(|| "asset not found".to_string())?;

    let bytes = base64::engine::general_purpose::STANDARD
        .decode(base64_bytes.as_bytes())
        .map_err(|error| error.to_string())?;
    let dir = root.join(".polystash").join("models");
    tokio::fs::create_dir_all(&dir)
        .await
        .map_err(|error| error.to_string())?;
    let destination = dir.join(&file_name);
    tokio::fs::write(&destination, &bytes)
        .await
        .map_err(|error| error.to_string())?;

    let relative = format!(".polystash/models/{file_name}");
    let existing: Option<(String,)> = sqlx::query_as("SELECT metadata FROM assets WHERE id = ?")
        .bind(&asset_id)
        .fetch_optional(&pool)
        .await
        .unwrap_or(None);
    let mut metadata: serde_json::Map<String, serde_json::Value> = existing
        .and_then(|(raw,)| serde_json::from_str(&raw).ok())
        .unwrap_or_default();
    metadata.insert(
        "preview_model".to_string(),
        serde_json::Value::String(relative),
    );
    let _ = sqlx::query("UPDATE assets SET metadata = ? WHERE id = ?")
        .bind(serde_json::Value::Object(metadata).to_string())
        .bind(&asset_id)
        .execute(&pool)
        .await;

    Ok(destination.to_string_lossy().to_string())
}

#[tauri::command]
pub async fn get_preview_jobs(state: State<'_, Arc<AppState>>) -> Result<Vec<PreviewJob>> {
    let libs = state.db.get_all_libraries().await;
    let mut all_jobs = Vec::new();

    for lib in &libs {
        if let Ok(rows) = sqlx::query_as::<_, (String, String, String, Option<i64>, Option<String>, i64)>(
            "SELECT id, asset_id, status, progress, error_message, created_at FROM preview_jobs ORDER BY created_at DESC LIMIT 50"
        )
        .fetch_all(&lib.pool)
        .await {
            for (id, asset_id, status, progress, error_message, created_at) in rows {
                all_jobs.push(PreviewJob { id, asset_id, status, progress, error_message, created_at });
            }
        }
    }

    all_jobs.sort_by_key(|b| std::cmp::Reverse(b.created_at));
    all_jobs.truncate(100);
    Ok(all_jobs)
}
