use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use sqlx::{Pool, Sqlite};
use tauri::State;
use uuid::Uuid;

use crate::AppState;
use crate::models::{Tag, TagDraft, TagPatch};

type Result<T> = std::result::Result<T, String>;

type TagRow = (String, String, Option<String>, Option<String>, Option<String>);

// Both statements bind in the same order: name, color, parent_id, icon_id, id.
const UPSERT_TAG: &str =
    "INSERT INTO tags (name, color, parent_id, icon_id, id) VALUES (?, ?, ?, ?, ?) \
     ON CONFLICT(id) DO UPDATE SET name = excluded.name, color = excluded.color, \
     parent_id = excluded.parent_id, icon_id = excluded.icon_id";

const UPDATE_TAG: &str =
    "UPDATE tags SET name = ?, color = ?, parent_id = ?, icon_id = ? WHERE id = ?";

fn row_to_tag((id, name, color, parent_id, icon_id): TagRow) -> Tag {
    Tag { id, name, color, parent_id, icon_id }
}

#[tauri::command]
pub async fn list_tags(state: State<'_, Arc<AppState>>) -> Result<Vec<Tag>> {
    let pool = state.db.get_active_pool().await;
    let rows = sqlx::query_as::<_, TagRow>(
        "SELECT id, name, color, parent_id, icon_id FROM tags ORDER BY name ASC"
    )
    .fetch_all(&pool)
    .await
    .map_err(|e| e.to_string())?;

    Ok(rows.into_iter().map(row_to_tag).collect())
}

/// Builds a `Tag` from its identity plus the optional fields shared by the
/// create and update commands.
fn tag_from_patch(id: String, name: String, patch: TagPatch) -> Tag {
    Tag { id, name, color: patch.color, parent_id: patch.parent_id, icon_id: patch.icon_id }
}

#[tauri::command]
pub async fn create_tag(
    name: String,
    patch: TagPatch,
    state: State<'_, Arc<AppState>>,
) -> Result<Tag> {
    let tag = tag_from_patch(Uuid::new_v4().to_string(), name, patch);
    save_tag(&state, UPSERT_TAG, tag, false).await
}

#[tauri::command]
pub async fn update_tag(
    id: String,
    name: String,
    patch: TagPatch,
    state: State<'_, Arc<AppState>>,
) -> Result<Tag> {
    let tag = tag_from_patch(id, name, patch);
    save_tag(&state, UPDATE_TAG, tag, true).await
}

#[tauri::command]
pub async fn delete_tag(id: String, state: State<'_, Arc<AppState>>) -> Result<()> {
    let pool = state.db.get_active_pool().await;
    sqlx::query("DELETE FROM tags WHERE id = ?")
        .bind(&id).execute(&pool).await.map_err(|e| e.to_string())?;

    let libs = state.db.get_all_libraries().await;
    for lib in libs {
        let _ = sqlx::query("DELETE FROM tags WHERE id = ?")
            .bind(&id).execute(&lib.pool).await;
    }
    let _ = sqlx::query("DELETE FROM tags WHERE id = ?")
        .bind(&id).execute(&state.db.master_pool).await;

    Ok(())
}

/// Replaces the whole tag library with the staged forest.
///
/// Validation runs before any write, so a rejected payload leaves every pool
/// untouched. Rows missing from the payload are deleted (their `asset_tags`
/// links cascade), then every payload row is upserted parents-first so pools
/// that enforce foreign keys can resolve `parent_id`.
#[tauri::command]
pub async fn save_tag_tree(
    tags: Vec<TagDraft>,
    state: State<'_, Arc<AppState>>,
) -> Result<Vec<Tag>> {
    let ordered = validate_tag_tree(&tags)?;

    let pool = state.db.get_active_pool().await;
    replace_tag_tree(&pool, &ordered).await?;

    // Mirror the tree to every open library and the master pool, each in its
    // own transaction. A replica failure must not undo the active save.
    for lib in state.db.get_all_libraries().await {
        let _ = replace_tag_tree(&lib.pool, &ordered).await;
    }
    let _ = replace_tag_tree(&state.db.master_pool, &ordered).await;

    list_tags(state).await
}

/// Runs `sql` with the shared bind order: name, color, parent_id, icon_id, id.
async fn write_tag(pool: &Pool<Sqlite>, sql: &str, tag: &Tag) -> Result<u64> {
    let result = sqlx::query(sql)
        .bind(&tag.name).bind(&tag.color).bind(&tag.parent_id).bind(&tag.icon_id).bind(&tag.id)
        .execute(pool).await.map_err(|e| e.to_string())?;
    Ok(result.rows_affected())
}

/// Persists a tag on the active pool, then shares it with every other pool.
/// With `require_existing`, a write that matches no row (an update of a missing
/// id) fails instead of silently reporting success.
async fn save_tag(
    state: &AppState,
    sql: &str,
    tag: Tag,
    require_existing: bool,
) -> Result<Tag> {
    let pool = state.db.get_active_pool().await;
    let affected = write_tag(&pool, sql, &tag).await?;
    if require_existing && affected == 0 {
        return Err(format!("tag not found: {}", tag.id));
    }

    for lib in state.db.get_all_libraries().await {
        let _ = write_tag(&lib.pool, UPSERT_TAG, &tag).await;
    }
    let _ = write_tag(&state.db.master_pool, UPSERT_TAG, &tag).await;

    Ok(tag)
}

/// Replaces one pool's tag rows with `ordered` inside a single transaction.
/// The caller must pass a validated, parents-first payload.
async fn replace_tag_tree(pool: &Pool<Sqlite>, ordered: &[&TagDraft]) -> Result<()> {
    let mut tx = pool.begin().await.map_err(|e| e.to_string())?;

    if ordered.is_empty() {
        sqlx::query("DELETE FROM tags")
            .execute(&mut *tx).await.map_err(|e| e.to_string())?;
    } else {
        let placeholders = vec!["?"; ordered.len()].join(", ");
        let sql = format!("DELETE FROM tags WHERE id NOT IN ({placeholders})");
        let mut delete = sqlx::query(&sql);
        for tag in ordered {
            delete = delete.bind(&tag.id);
        }
        delete.execute(&mut *tx).await.map_err(|e| e.to_string())?;
    }

    for tag in ordered {
        sqlx::query(UPSERT_TAG)
            .bind(&tag.name).bind(&tag.color).bind(&tag.parent_id)
            .bind(&tag.icon_id).bind(&tag.id)
            .execute(&mut *tx).await.map_err(|e| e.to_string())?;
    }

    tx.commit().await.map_err(|e| e.to_string())
}

/// Validates a staged forest and returns it ordered parents-first.
///
/// Rejects blank names, duplicate ids, unknown parents and parent cycles.
/// Duplicate names are allowed: the hierarchical taxonomy reuses names across
/// categories.
fn validate_tag_tree(tags: &[TagDraft]) -> Result<Vec<&TagDraft>> {
    if tags.iter().any(|tag| tag.name.trim().is_empty()) {
        return Err("tag name cannot be blank".to_string());
    }
    if let Some(tag) = first_duplicate(tags, |tag| tag.id.as_str()) {
        return Err(format!("duplicate tag id: {}", tag.id));
    }

    let ids: HashSet<&str> = tags.iter().map(|tag| tag.id.as_str()).collect();
    let unknown_parent = tags
        .iter()
        .filter_map(|tag| tag.parent_id.as_deref())
        .find(|parent_id| !ids.contains(*parent_id));
    if let Some(parent_id) = unknown_parent {
        return Err(format!("unknown parent tag: {}", parent_id));
    }

    parents_first(tags)
}

/// Returns the first tag whose key was already seen.
fn first_duplicate<'a, K, F>(tags: &'a [TagDraft], key: F) -> Option<&'a TagDraft>
where
    K: Eq + std::hash::Hash,
    F: Fn(&'a TagDraft) -> K,
{
    let mut seen: HashSet<K> = HashSet::with_capacity(tags.len());
    tags.iter().find(|tag| !seen.insert(key(tag)))
}

fn parents_first(tags: &[TagDraft]) -> Result<Vec<&TagDraft>> {
    let by_id: HashMap<&str, &TagDraft> = tags.iter().map(|t| (t.id.as_str(), t)).collect();
    let mut depths: HashMap<&str, usize> = HashMap::with_capacity(tags.len());

    for tag in tags {
        let mut seen: HashSet<&str> = HashSet::new();
        let mut depth = 0;
        let mut current = Some(tag.id.as_str());
        while let Some(id) = current {
            if !seen.insert(id) {
                return Err("tag hierarchy contains a cycle".to_string());
            }
            current = by_id.get(id).and_then(|t| t.parent_id.as_deref());
            depth += 1;
        }
        depths.insert(tag.id.as_str(), depth);
    }

    let mut ordered: Vec<&TagDraft> = tags.iter().collect();
    ordered.sort_by_key(|tag| depths.get(tag.id.as_str()).copied().unwrap_or(0));
    Ok(ordered)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn draft(id: &str, name: &str, parent_id: Option<&str>) -> TagDraft {
        TagDraft {
            id: id.to_string(),
            name: name.to_string(),
            parent_id: parent_id.map(str::to_string),
            color: None,
            icon_id: None,
        }
    }

    #[test]
    fn rejects_blank_name() {
        let tags = vec![draft("a", "   ", None)];
        let err = validate_tag_tree(&tags).expect_err("blank name must be rejected");
        assert_eq!(err, "tag name cannot be blank");
    }

    #[test]
    fn rejects_duplicate_ids() {
        let tags = vec![draft("a", "Wood", None), draft("a", "Oak", None)];
        let err = validate_tag_tree(&tags).expect_err("duplicate ids must be rejected");
        assert_eq!(err, "duplicate tag id: a");
    }

    #[test]
    fn allows_duplicate_names() {
        let tags = vec![draft("child", "Wood", Some("parent")), draft("parent", "Wood", None)];

        let ordered = validate_tag_tree(&tags).expect("duplicate names are allowed");
        let position = |id: &str| {
            ordered.iter().position(|tag| tag.id == id).expect("tag present")
        };

        assert!(position("parent") < position("child"));
    }

    #[test]
    fn rejects_unknown_parent() {
        let tags = vec![draft("a", "Wood", Some("missing"))];
        let err = validate_tag_tree(&tags).expect_err("unknown parent must be rejected");
        assert_eq!(err, "unknown parent tag: missing");
    }

    #[test]
    fn rejects_cycles_including_self_parents() {
        let cycle = vec![draft("a", "A", Some("b")), draft("b", "B", Some("a"))];
        let err = validate_tag_tree(&cycle).expect_err("cycles must be rejected");
        assert_eq!(err, "tag hierarchy contains a cycle");

        let self_parent = vec![draft("a", "A", Some("a"))];
        let err = validate_tag_tree(&self_parent).expect_err("self parents must be rejected");
        assert_eq!(err, "tag hierarchy contains a cycle");
    }

    #[test]
    fn orders_parents_before_children() {
        let tags = vec![
            draft("child", "Child", Some("parent")),
            draft("grandchild", "Grandchild", Some("child")),
            draft("parent", "Parent", None),
        ];

        let ordered = validate_tag_tree(&tags).expect("acyclic payload");
        let position = |id: &str| {
            ordered.iter().position(|tag| tag.id == id).expect("tag present")
        };

        assert!(position("parent") < position("child"));
        assert!(position("child") < position("grandchild"));
    }
}
