# Cineware SDK (vendored)

Static libraries and headers from the Maxon Cineware SDK (`Cineware_22_008`), used to read
Cinema 4D (`.c4d`) files and convert them to glTF for the PolyStash 3D preview.

- `lib/cinewarelib_release.lib` — `libraries/win/cinewarelib_2015_MT/cinewarelib_release.lib`
- `lib/jpeglib_release.lib` — `libraries/win/jpeglib_2015_MT/jpeglib_release.lib`
- `includes/` — the SDK's public headers

These are binary blobs from Maxon's SDK distribution. They are linked statically into the
`c4d_preview` helper built by CI, so the shipped application needs no SDK install at runtime.
Redistribution terms are those of the Maxon SDK licence.
