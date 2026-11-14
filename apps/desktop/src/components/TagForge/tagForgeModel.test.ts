import test from 'node:test';
import assert from 'node:assert/strict';
import {
  applyColor,
  applyIcon,
  collectIds,
  duplicateTag,
  findParentId,
  findTag,
  flattenTree,
  insertTag,
  moveTag,
  nestTags,
  removeTag,
  renameTag,
  stagePredefined,
  createCustomTag,
} from './tagForgeModel.ts';
import type { ForgeTag, PredefinedNode } from './tagForgeModel.ts';

function tag(id: string, children: ForgeTag[] = [], parentId: string | null = null): ForgeTag {
  return { id, name: id, color: null, iconId: null, parentId, children };
}

function sample(): ForgeTag[] {
  return [
    tag('a', [tag('a1', [tag('a1x', [], 'a1')], 'a'), tag('a2', [], 'a')]),
    tag('b'),
  ];
}

function pre(guid: string, children: PredefinedNode[] = []): PredefinedNode {
  return {
    guid, name: guid, path: guid, parentGuid: null, isRoot: false, color: '#FFAABBCC',
    colorApplyOnChildren: false, iconId: 'i1', iconApplyOnChildren: false,
    orderingPriority: 0, aliases: [], children,
  };
}

const ids = (f: ForgeTag[]) => f.map((n) => n.id);

test('nest/flatten round-trip', () => {
  const flat = flattenTree(sample());
  assert.deepEqual(flat.map((t) => t.id), ['a', 'a1', 'a1x', 'a2', 'b']);
  assert.equal(flat[2].parentId, 'a1');
  assert.ok(flat.every((t) => t.children.length === 0));
  assert.deepEqual(nestTags(flat), sample());
});

test('nestTags tolerates unknown parents and cycles', () => {
  const f = nestTags([
    { id: 'x', name: 'x', parentId: 'ghost' },
    { id: 'c1', name: 'c1', parentId: 'c2' },
    { id: 'c2', name: 'c2', parentId: 'c1' },
    { id: 'self', name: 'self', parentId: 'self' },
    { id: 'tail', name: 'tail', parentId: 'c1' },
  ]);
  assert.equal(collectIds(f).size, 5);
  assert.deepEqual(ids(f), ['x', 'c1', 'c2', 'self', 'tail']);
});

test('nestTags preserves order when children precede parent', () => {
  const f = nestTags([
    { id: 'child1', name: 'child1', parentId: 'root' },
    { id: 'child2', name: 'child2', parentId: 'root' },
    { id: 'root', name: 'root', parentId: null },
  ]);
  assert.deepEqual(ids(f), ['root']);
  assert.deepEqual(ids(f[0].children), ['child1', 'child2']);
});

test('stagePredefined copies subtree', () => {
  const s = stagePredefined(pre('r', [pre('c1', [pre('g')]), pre('c2')]), 'p');
  assert.equal(collectIds([s]).size, 4);
  assert.equal(s.id, 'r');
  assert.equal(s.parentId, 'p');
  assert.equal(s.sourceGuid, 'r');
  assert.equal(s.color, '#FFAABBCC');
  assert.equal(s.children[0].children[0].parentId, 'c1');
  assert.equal(stagePredefined(pre('z')).parentId, null);
});

test('stagePredefined drops already-staged descendants when ancestor is staged', () => {
  const child = pre('c1', [pre('g')]);
  const ancestor = pre('r', [child, pre('c2')]);
  let forest = insertTag([], null, stagePredefined(child));
  forest = insertTag(forest, null, stagePredefined(ancestor, null, collectIds(forest)));
  const flat = flattenTree(forest);
  assert.equal(flat.filter((t) => t.id === 'c1').length, 1);
  assert.equal(flat.filter((t) => t.id === 'g').length, 1);
  assert.deepEqual(flat.map((t) => t.id), ['c1', 'g', 'r', 'c2']);
});

test('createCustomTag shape', () => {
  const t = createCustomTag('p');
  assert.equal(t.name, 'New Tag');
  assert.equal(t.isCustom, true);
  assert.equal(t.parentId, 'p');
  assert.equal(t.color, null);
});

test('renameTag is immutable', () => {
  const f = sample();
  const r = renameTag(f, 'a1x', 'Z');
  assert.equal(findTag(r, 'a1x')?.name, 'Z');
  assert.equal(findTag(f, 'a1x')?.name, 'a1x');
});

test('removeTag drops subtree', () => {
  const r = removeTag(sample(), 'a1');
  assert.equal(findTag(r, 'a1x'), null);
  assert.equal(collectIds(r).size, 3);
});

test('insertTag honours index', () => {
  const r = insertTag(sample(), 'a', tag('n'), 1);
  assert.deepEqual(ids(findTag(r, 'a')!.children), ['a1', 'n', 'a2']);
  assert.equal(findTag(r, 'n')?.parentId, 'a');
  assert.deepEqual(ids(insertTag(sample(), null, tag('n'), 0)), ['n', 'a', 'b']);
  assert.deepEqual(ids(insertTag(sample(), null, tag('n'))), ['a', 'b', 'n']);
});

test('moveTag reorder, nest, refuse into descendant', () => {
  const reordered = moveTag(sample(), 'b', null, 0);
  assert.deepEqual(ids(reordered), ['b', 'a']);
  const nested = moveTag(sample(), 'b', 'a2', 0);
  assert.deepEqual(ids(nested), ['a']);
  assert.equal(findParentId(nested, 'b'), 'a2');
  const f = sample();
  assert.equal(moveTag(f, 'a', 'a1x', 0), f);
  assert.equal(moveTag(f, 'a', 'a', 0), f);
});

test('duplicateTag fresh ids after source', () => {
  const f = sample();
  f[0].children[0].sourceGuid = 'g';
  const d = duplicateTag(f, 'a1');
  const kids = findTag(d, 'a')!.children;
  assert.equal(kids.length, 3);
  assert.equal(kids[0].id, 'a1');
  assert.notEqual(kids[1].id, 'a1');
  assert.equal(kids[1].sourceGuid, 'g');
  assert.equal(kids[1].children.length, 1);
  assert.equal(kids[1].children[0].parentId, kids[1].id);
  assert.equal(collectIds(d).size, 7);
});

test('applyIcon/applyColor recursive vs single', () => {
  const single = applyIcon(sample(), 'a', 'ic', false);
  assert.equal(findTag(single, 'a')?.iconId, 'ic');
  assert.equal(findTag(single, 'a1')?.iconId, null);
  const rec = applyColor(sample(), 'a', '#FF000000', true);
  assert.equal(findTag(rec, 'a1x')?.color, '#FF000000');
  assert.equal(findTag(rec, 'b')?.color, null);
  assert.equal(findTag(applyColor(sample(), 'a', '#1', false), 'a2')?.color, null);
});

test('collectIds', () => {
  assert.deepEqual([...collectIds(sample())].sort(), ['a', 'a1', 'a1x', 'a2', 'b']);
  assert.equal(collectIds([]).size, 0);
});
