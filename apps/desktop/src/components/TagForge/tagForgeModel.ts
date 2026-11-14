export interface PredefinedTag {
  guid: string;
  name: string;
  path: string;
  parentGuid: string | null;
  isRoot: boolean;
  color: string;
  colorApplyOnChildren: boolean;
  iconId: string | null;
  iconApplyOnChildren: boolean;
  orderingPriority: number;
  aliases: string[];
}

export interface PredefinedNode extends PredefinedTag {
  children: PredefinedNode[];
}

export interface ForgeTag {
  id: string;
  name: string;
  color: string | null;
  iconId: string | null;
  parentId: string | null;
  children: ForgeTag[];
  sourceGuid?: string;
  isCustom?: boolean;
}

interface FlatTag {
  id: string;
  name: string;
  color?: string | null;
  iconId?: string | null;
  parentId?: string | null;
}

function toForgeNode(flatTag: FlatTag): ForgeTag {
  return {
    id: flatTag.id,
    name: flatTag.name,
    color: flatTag.color ?? null,
    iconId: flatTag.iconId ?? null,
    parentId: null,
    children: [],
  };
}

interface NestIndex {
  byId: Map<string, ForgeTag>;
  parentOf: Map<string, string | null>;
}

function buildNestIndex(flat: FlatTag[]): NestIndex {
  const byId = new Map<string, ForgeTag>();
  const parentOf = new Map<string, string | null>();
  for (const t of flat) {
    byId.set(t.id, toForgeNode(t));
    parentOf.set(t.id, t.parentId ?? null);
  }
  return { byId, parentOf };
}

function hasCycle(node: ForgeTag, index: NestIndex): boolean {
  const seen = new Set<string>([node.id]);
  let cur = index.parentOf.get(node.id) ?? null;
  while (cur !== null && index.byId.has(cur)) {
    if (seen.has(cur)) return true;
    seen.add(cur);
    cur = index.parentOf.get(cur) ?? null;
  }
  return false;
}

function resolveParent(node: ForgeTag, index: NestIndex): ForgeTag | null {
  const parentId = index.parentOf.get(node.id);
  if (!parentId || parentId === node.id) return null;
  const parent = index.byId.get(parentId);
  if (!parent || hasCycle(node, index)) return null;
  return parent;
}

export function nestTags(flat: FlatTag[]): ForgeTag[] {
  const index = buildNestIndex(flat);
  const roots: ForgeTag[] = [];
  for (const t of flat) {
    const node = index.byId.get(t.id);
    if (!node) continue;
    const parent = resolveParent(node, index);
    if (parent) {
      node.parentId = parent.id;
      parent.children.push(node);
    } else {
      roots.push(node);
    }
  }
  return roots;
}

export function flattenTree(roots: ForgeTag[]): ForgeTag[] {
  const out: ForgeTag[] = [];
  const walk = (nodes: ForgeTag[], parent: ForgeTag | null): void => {
    const parentId = parent ? parent.id : null;
    for (const n of nodes) {
      out.push({ ...n, parentId, children: [] });
      walk(n.children, n);
    }
  };
  walk(roots, null);
  return out;
}

export function findTag(roots: ForgeTag[], id: string): ForgeTag | null {
  for (const n of roots) {
    if (n.id === id) return n;
    const hit = findTag(n.children, id);
    if (hit) return hit;
  }
  return null;
}

export function findParentId(roots: ForgeTag[], id: string): string | null {
  const walk = (nodes: ForgeTag[], parent: ForgeTag | null): string | null | undefined => {
    for (const n of nodes) {
      if (n.id === id) return parent ? parent.id : null;
      const hit = walk(n.children, n);
      if (hit !== undefined) return hit;
    }
    return undefined;
  };
  return walk(roots, null) ?? null;
}

export function stagePredefined(
  node: PredefinedNode,
  parentId: string | null = null,
  skipIds?: ReadonlySet<string>,
): ForgeTag {
  const children = skipIds
    ? node.children.filter((c) => !skipIds.has(c.guid))
    : node.children;
  return {
    id: node.guid,
    name: node.name,
    color: node.color,
    iconId: node.iconId,
    parentId,
    children: children.map((c) => stagePredefined(c, node.guid, skipIds)),
    sourceGuid: node.guid,
  };
}

export function createCustomTag(parentId: string | null = null): ForgeTag {
  return {
    id: crypto.randomUUID(),
    name: 'New Tag',
    color: null,
    iconId: null,
    parentId,
    children: [],
    isCustom: true,
  };
}

function mapTree(roots: ForgeTag[], fn: (n: ForgeTag) => ForgeTag | null): ForgeTag[] {
  const out: ForgeTag[] = [];
  for (const n of roots) {
    const mapped = fn(n);
    if (mapped === null) continue;
    out.push({ ...mapped, children: mapTree(mapped.children, fn) });
  }
  return out;
}

export function renameTag(roots: ForgeTag[], id: string, name: string): ForgeTag[] {
  return mapTree(roots, (n) => (n.id === id ? { ...n, name } : n));
}

export function removeTag(roots: ForgeTag[], id: string): ForgeTag[] {
  return mapTree(roots, (n) => (n.id === id ? null : n));
}

function spliceInto(
  list: ForgeTag[],
  placement: { node: ForgeTag; index?: number },
): ForgeTag[] {
  const { node, index } = placement;
  const at = index === undefined ? list.length : Math.max(0, Math.min(index, list.length));
  return [...list.slice(0, at), node, ...list.slice(at)];
}

export function insertTag(
  roots: ForgeTag[],
  parentId: string | null,
  node: ForgeTag,
  index?: number,
): ForgeTag[] {
  const placed = { ...node, parentId };
  if (parentId === null) return spliceInto(roots, { node: placed, index });
  if (!findTag(roots, parentId)) return roots;
  return mapTree(roots, (n) =>
    n.id === parentId ? { ...n, children: spliceInto(n.children, { node: placed, index }) } : n,
  );
}

function isValidMoveTarget(roots: ForgeTag[], node: ForgeTag, targetId: string | null): boolean {
  if (targetId === null) return true;
  if (targetId === node.id) return false;
  if (findTag(node.children, targetId)) return false;
  return findTag(roots, targetId) !== null;
}

export function moveTag(
  roots: ForgeTag[],
  id: string,
  newParentId: string | null,
  index: number,
): ForgeTag[] {
  const node = findTag(roots, id);
  if (!node) return roots;
  if (!isValidMoveTarget(roots, node, newParentId)) return roots;
  return insertTag(removeTag(roots, id), newParentId, node, index);
}

function cloneFresh(node: ForgeTag, parent: ForgeTag | null): ForgeTag {
  const id = crypto.randomUUID();
  const clone: ForgeTag = {
    ...node,
    id,
    parentId: parent ? parent.id : null,
    children: [],
  };
  return { ...clone, children: node.children.map((c) => cloneFresh(c, clone)) };
}

export function duplicateTag(roots: ForgeTag[], id: string): ForgeTag[] {
  const source = findTag(roots, id);
  if (!source) return roots;
  const parentId = findParentId(roots, id);
  const parent = parentId === null ? null : findTag(roots, parentId);
  const siblings = parent ? parent.children : roots;
  const index = siblings.findIndex((s) => s.id === id);
  return insertTag(roots, parentId, cloneFresh(source, parent), index + 1);
}

interface FieldPatch {
  id: string;
  patch: Partial<Pick<ForgeTag, 'iconId' | 'color'>>;
  recursive: boolean;
}

function applyField(roots: ForgeTag[], update: FieldPatch): ForgeTag[] {
  const { id, patch, recursive } = update;
  const paint = (nodes: ForgeTag[]): ForgeTag[] =>
    nodes.map((n) => ({ ...n, ...patch, children: paint(n.children) }));
  return mapTree(roots, (n) => {
    if (n.id !== id) return n;
    return recursive ? { ...n, ...patch, children: paint(n.children) } : { ...n, ...patch };
  });
}

export function applyIcon(
  roots: ForgeTag[],
  id: string,
  iconId: string | null,
  recursive: boolean,
): ForgeTag[] {
  return applyField(roots, { id, patch: { iconId }, recursive });
}

export function applyColor(
  roots: ForgeTag[],
  id: string,
  color: string | null,
  recursive: boolean,
): ForgeTag[] {
  return applyField(roots, { id, patch: { color }, recursive });
}

export function collectIds(roots: ForgeTag[]): Set<string> {
  const ids = new Set<string>();
  const walk = (nodes: ForgeTag[]): void => {
    for (const n of nodes) {
      ids.add(n.id);
      walk(n.children);
    }
  };
  walk(roots);
  return ids;
}
