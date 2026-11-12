# c4d_preview

Cineware-based `.c4d` → GLB converter used by the PolyStash desktop preview worker.

## What it does

`c4d_preview.exe <input.c4d> <output.glb>` loads a Cinema 4D scene with the vendored
Maxon Cineware SDK, walks the object/deform-cache hierarchy, and writes a
self-contained binary glTF 2.0 file:

- one mesh per polygon object (triangles and quads, indices, glTF-required bounds),
- one PBR material per Cinema 4D material using the material colour and roughness,
- left-handed C4D coordinates converted to right-handed glTF (Z negated, winding reversed).

Textures and shader networks are not baked; materials fall back to their base
colour. Scenes saved with the C4D option **Save Polygons for Cineware** include
polygon caches for generators (cubes, sweeps, boolean results, …). Without those
caches only real polygon objects can be exported, so the backend falls back to
the embedded `THMB` preview image.

## Build

Only built on Windows with the two vendored static libraries present:

```
third_party/cineware/lib/cinewarelib_release.lib
third_party/cineware/lib/jpeglib_release.lib
```

`apps/desktop/src-tauri/build.rs` compiles this source with `cl.exe` (`/MT` static
CRT, matching the `_MT` libraries) and embeds the resulting exe in the PolyStash
binary. CI sets up MSVC with `ilammy/msvc-dev-cmd` and pre-builds the helper into
`apps/desktop/src-tauri/bin/c4d/c4d_preview.exe`; `build.rs` picks that up and
embeds it. The `bin/c4d/` directory is not committed.

Manual build from `apps/desktop/src-tauri` in a Visual Studio developer prompt:

```
cl /nologo /std:c++17 /EHsc /MT /O2 /DNDEBUG /Ithird_party\cineware\includes ^
   /Fe:bin\c4d\c4d_preview.exe ..\..\..\tools\c4d_preview\main.cpp ^
   /link /LIBPATH:third_party\cineware\lib cinewarelib_release.lib jpeglib_release.lib ws2_32.lib winmm.lib
```

## Runtime lookup

`apps/desktop/src-tauri/src/preview/cineware.rs` resolves the helper in this order:

1. `POLYSTASH_C4D_PREVIEW` environment variable (absolute path to `c4d_preview.exe`).
2. `c4d_preview.exe` next to the running PolyStash executable.
3. `<PolyStash exe dir>\c4d_preview\c4d_preview.exe`.
4. The copy embedded at build time, extracted on first use to
   `%TEMP%\polystash_c4d\c4d_preview.exe`.

The converted GLB is written to `<library>/.polystash/thumbs/<stem>_preview.glb`,
stored as the asset's `preview_model`, and used by the frontend to render the
card thumbnail through the existing offscreen GLB snapshot path.
