//! Preview queue: drains queued jobs concurrently, user-requested rows first.

use std::path::PathBuf;
use std::sync::Arc;
use tokio::sync::Semaphore;

use super::{fetch_queued_jobs, process_job, JobRef, LibraryJobs};
use crate::db::LibraryHandle;
use crate::AppState;

/// Jobs run concurrently per batch. The SBSAR render path has its own, smaller
/// limit inside `archive::render_sbsar_maps`.
const MAX_CONCURRENT_JOBS: usize = 4;

/// Owned inputs for one spawned job, so the task outlives the batch loop.
struct JobTask {
    state: Arc<AppState>,
    pool: sqlx::Pool<sqlx::Sqlite>,
    root_path: PathBuf,
    thumb_dir: PathBuf,
    slots: Arc<Semaphore>,
    id: String,
    asset_id: String,
    file_path: String,
}

/// Claim one queued job, then generate its preview while holding a slot.
async fn run_job(task: JobTask) {
    let Ok(_slot) = task.slots.acquire().await else {
        return;
    };
    let job = JobRef {
        id: &task.id,
        asset_id: &task.asset_id,
        file_path: &task.file_path,
    };
    let jobs = LibraryJobs {
        state: &task.state,
        pool: &task.pool,
        root_path: &task.root_path,
        thumb_dir: &task.thumb_dir,
    };

    let claimed = sqlx::query("UPDATE preview_jobs SET status = 'processing' WHERE id = ?")
        .bind(job.id)
        .execute(jobs.pool)
        .await;
    match claimed {
        // No row means `request_preview` replaced the job after this batch was
        // fetched; the replacement is already queued.
        Ok(result) if result.rows_affected() == 0 => {}
        Ok(_) => process_job(&jobs, &job).await,
        Err(e) => tracing::error!("Failed to update job status to processing: {}", e),
    }
}

/// Drain one batch of queued jobs for a library; true when any job was seen.
pub(crate) async fn process_library(state: &Arc<AppState>, lib: &LibraryHandle) -> bool {
    let thumb_dir = lib.root_path.join(".polystash").join("thumbs");
    let _ = tokio::fs::create_dir_all(&thumb_dir).await;

    let jobs = fetch_queued_jobs(&lib.pool).await;
    if jobs.is_empty() {
        return false;
    }

    let slots = Arc::new(Semaphore::new(MAX_CONCURRENT_JOBS));
    let mut batch = Vec::with_capacity(jobs.len());

    for (id, asset_id, file_path) in jobs {
        batch.push(tokio::spawn(run_job(JobTask {
            state: Arc::clone(state),
            pool: lib.pool.clone(),
            root_path: lib.root_path.clone(),
            thumb_dir: thumb_dir.clone(),
            slots: Arc::clone(&slots),
            id,
            asset_id,
            file_path,
        })));
    }

    for task in batch {
        if let Err(error) = task.await {
            tracing::error!("Preview job task failed: {}", error);
        }
    }
    true
}
