import { api } from '../../api';
import { useLibraryStore, parseFolderId, type ActiveFolder } from '../../store';

/** `lib:<id>` and `dir:<id>:<path>` both key off the library; this reads it back, or null. */
function itemLibraryId(id: string): string | null {
  if (!id.startsWith('lib:') && !id.startsWith('dir:')) return null;
  return id.slice(4).split(':')[0];
}

export function pruneStaleItems(items: string[], libraryIds: string[]): string[] {
  return items.filter((id) => {
    const libraryId = itemLibraryId(id);
    return libraryId === null || libraryIds.includes(libraryId);
  });
}

export function resolveItemName(
  id: string,
  libraries: { id: string; name: string }[],
  fallbackData: string
): string {
  if (id === 'root') return 'All Libraries';
  if (id.startsWith('lib:')) {
    const libId = id.slice(4);
    return libraries.find((l) => l.id === libId)?.name ?? 'Library';
  }
  if (id.startsWith('dir:')) {
    return parseFolderId(id).relativePath.split('/').filter(Boolean).pop() ?? fallbackData;
  }
  return fallbackData;
}

async function fetchLibraryChildren(libId: string): Promise<string[]> {
  try {
    const dirs = await api.listLibraryDirectories(libId);
    return dirs.filter((d) => Boolean(d) && d !== '.').map((d) => `dir:${libId}:${d}`);
  } catch (err) {
    console.error(`Failed to load directories for library ${libId}:`, err);
    return [];
  }
}

async function fetchDirectoryChildren(id: string): Promise<string[]> {
  const { libraryId, relativePath } = parseFolderId(id);
  if (!libraryId) return [];
  try {
    const subdirs = await api.listSubdirectories(libraryId, relativePath);
    return subdirs
      .filter((d) => Boolean(d) && d !== '.' && d !== relativePath)
      .map((d) => `dir:${libraryId}:${d}`);
  } catch (err) {
    console.error(`Failed to load subdirectories for ${id}:`, err);
    return [];
  }
}

export async function fetchTreeChildren(id: string): Promise<string[]> {
  if (id === 'root') {
    return useLibraryStore.getState().libraries.map((l) => `lib:${l.id}`);
  }
  if (id.startsWith('lib:')) return fetchLibraryChildren(id.slice(4));
  if (id.startsWith('dir:')) return fetchDirectoryChildren(id);
  return [];
}

export type { ActiveFolder };
