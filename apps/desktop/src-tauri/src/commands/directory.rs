use std::collections::BTreeSet;
use std::path::{Component, Path, PathBuf};
use std::sync::Arc;

use tauri::State;

use crate::db::LibraryHandle;
use crate::AppState;
use crate::commands::asset::escape_like_pattern;

type Result<T> = std::result::Result<T, String>;

const ROOT: &str = ".";

#[tauri::command]
pub async fn list_library_directories(
    library_id: String,
    state: State<'_, Arc<AppState>>,
) -> Result<Vec<String>> {
    let handle = resolve_library(state.inner(), &library_id).await?;
    let mut directories = collect_children(&handle.pool, &handle.root_path, Path::new(ROOT)).await?;
    directories.insert(ROOT.to_string());
    Ok(directories.into_iter().collect())
}

#[tauri::command]
pub async fn list_subdirectories(
    library_id: String,
    parent: String,
    state: State<'_, Arc<AppState>>,
) -> Result<Vec<String>> {
    let parent = normalize_relative_dir(&parent)?;
    let handle = resolve_library(state.inner(), &library_id).await?;
    let directories =
        collect_children(&handle.pool, &handle.root_path, Path::new(&parent)).await?;
    Ok(directories.into_iter().collect())
}

async fn resolve_library(state: &AppState, library_id: &str) -> Result<LibraryHandle> {
    state
        .db
        .get_library_handle(Some(library_id))
        .await
        .filter(|handle| handle.id.as_str() == library_id)
        .ok_or_else(|| "library not found".to_string())
}

async fn collect_children(
    pool: &sqlx::Pool<sqlx::Sqlite>,
    root: &Path,
    parent: &Path,
) -> Result<BTreeSet<String>> {
    let mut children = BTreeSet::new();
    for directory in load_asset_directories(pool, root, parent).await? {
        if let Some(child) = immediate_child(parent, &directory) {
            children.insert(child);
        }
    }
    Ok(children)
}

async fn load_asset_directories(
    pool: &sqlx::Pool<sqlx::Sqlite>,
    root: &Path,
    parent: &Path,
) -> Result<Vec<PathBuf>> {
    let paths = if parent == Path::new(ROOT) {
        sqlx::query_scalar::<_, String>("SELECT file_path FROM assets")
            .fetch_all(pool)
            .await
            .map_err(|error| error.to_string())?
    } else {
        sqlx::query_scalar::<_, String>(
            "SELECT file_path FROM assets WHERE file_path LIKE ? ESCAPE '\\'",
        )
        .bind(like_prefix(parent))
        .fetch_all(pool)
        .await
        .map_err(|error| error.to_string())?
    };

    Ok(paths
        .into_iter()
        .filter_map(|path| {
            let unified = path.replace('\\', "/");
            asset_directory(Path::new(&unified), root)
        })
        .collect())
}

fn asset_directory(path: &Path, root: &Path) -> Option<PathBuf> {
    let relative = relative_to_root(path, root)?;
    if is_hidden_path(&relative) {
        return None;
    }

    let parent = relative.parent()?;
    Some(if parent.as_os_str().is_empty() {
        PathBuf::from(ROOT)
    } else {
        parent.to_path_buf()
    })
}

fn relative_to_root(path: &Path, root: &Path) -> Option<PathBuf> {
    let relative = if is_absolute_path(path) {
        path.strip_prefix(root).ok()?
    } else {
        path
    };
    normalize_path(relative)
}

fn normalize_path(path: &Path) -> Option<PathBuf> {
    let mut normalized = PathBuf::new();
    for component in path.components() {
        match component {
            Component::Normal(segment) => normalized.push(segment),
            Component::CurDir => {}
            _ => return None,
        }
    }

    Some(if normalized.as_os_str().is_empty() {
        PathBuf::from(ROOT)
    } else {
        normalized
    })
}

fn immediate_child(parent: &Path, directory: &Path) -> Option<String> {
    let relative = if parent == Path::new(ROOT) {
        directory
    } else {
        directory.strip_prefix(parent).ok()?
    };

    let child = relative.components().find_map(|component| match component {
        Component::Normal(name) => Some(name.to_string_lossy()),
        _ => None,
    })?;
    if child.starts_with('.') {
        return None;
    }

    let parent = path_to_posix(parent);
    Some(if parent.is_empty() {
        child.into_owned()
    } else {
        format!("{parent}/{child}")
    })
}

fn path_to_posix(path: &Path) -> String {
    path.components()
        .filter_map(|component| match component {
            Component::Normal(name) => Some(name.to_string_lossy().into_owned()),
            _ => None,
        })
        .collect::<Vec<_>>()
        .join("/")
}

fn is_hidden_path(path: &Path) -> bool {
    path.components().any(|component| match component {
        Component::Normal(name) => name.to_string_lossy().starts_with('.'),
        _ => false,
    })
}

pub(crate) fn normalize_relative_dir(input: &str) -> Result<String> {
    let unified = input.replace('\\', "/");
    if is_absolute_path(Path::new(&unified)) {
        return Err(format!("Invalid directory path: {}", input));
    }

    let mut segments = Vec::new();
    for segment in unified.split('/') {
        match segment {
            "" | "." => {}
            ".." => return Err(format!("Invalid directory path: {}", input)),
            _ => segments.push(segment),
        }
    }

    if segments.is_empty() {
        Ok(ROOT.to_string())
    } else {
        Ok(segments.join("/"))
    }
}

fn is_absolute_path(path: &Path) -> bool {
    if path.is_absolute() {
        return true;
    }
    let text = path.to_string_lossy();
    let mut characters = text.chars();
    matches!(
        (characters.next(), characters.next()),
        (Some(letter), Some(':')) if letter.is_ascii_alphabetic()
    )
}

fn like_prefix(directory: &Path) -> String {
    format!("{}/%", escape_like_pattern(&path_to_posix(directory)))
}
