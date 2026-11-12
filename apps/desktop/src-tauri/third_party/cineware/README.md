# Cineware SDK (vendored)

Static libraries and headers from the Maxon Cineware SDK (`Cineware_22_008`), used to read
Cinema 4D (`.c4d`) files and convert them to glTF for the PolyStash 3D preview.

- `lib/cinewarelib_release.lib` — `libraries/win/cinewarelib_2015_MT/cinewarelib_release.lib`
- `lib/jpeglib_release.lib` — `libraries/win/jpeglib_2015_MT/jpeglib_release.lib`
- `includes/` — the SDK's public headers

These are binary blobs from Maxon's SDK distribution. They are linked statically into the
`c4d_preview` helper built by CI, so the shipped application needs no SDK install at runtime.
Redistribution terms are those of the Maxon SDK licence.

The helper source is `tools/c4d_preview/main.cpp`. `apps/desktop/src-tauri/build.rs` compiles
it with MSVC (`/MT`) when these libraries are present, embeds the exe in the app, and the
preview worker extracts it on demand. See `tools/c4d_preview/README.md` for the runtime lookup
order and the `<library>/.polystash/thumbs/<stem>_preview.glb` output path.
