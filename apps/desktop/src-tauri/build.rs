use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::{Command, Output};

/// Repo-root helper source, relative to this crate.
const HELPER_SOURCE: &str = "../../../tools/c4d_preview/main.cpp";
const CINEWARE_INCLUDES: &str = "third_party/cineware/includes";
const CINEWARE_LIB_DIR: &str = "third_party/cineware/lib";
/// CI builds the helper here before `tauri build`; the binary is then embedded.
const PREBUILT_HELPER: &str = "bin/c4d/c4d_preview.exe";

const C4D_INCLUDE_DIRS: [&str; 1] = [CINEWARE_INCLUDES];
const C4D_LIBRARIES: [&str; 5] = [
    "cinewarelib_release.lib",
    "jpeglib_release.lib",
    "ws2_32.lib",
    "winmm.lib",
    "ole32.lib",
];

/// SBSAR renderer source and its vendored SDK inputs.
const SBSAR_HELPER_SOURCE: &str = "../../../tools/sbsar_render/main.cpp";
const SBSAR_INCLUDE_DIRS: [&str; 2] = [
    "third_party/substance/include",
    "third_party/substance/stb",
];
const SBSAR_LIB_DIR: &str = "third_party/substance/lib";
const SBSAR_BIN_DIR: &str = "third_party/substance/bin";
const PREBUILT_SBSAR_HELPER: &str = "bin/sbsar/sbsar_render.exe";
const SBSAR_LIBRARIES: [&str; 3] =
    [
        "substance_framework.lib",
        "substance_linker.lib",
        "substance_sse2_blend.lib",
        "tinyxml.lib",
    ];
const SBSAR_DLLS: [&str; 2] = ["substance_linker.dll", "substance_sse2_blend.dll"];

fn main() {
    for path in [
        "tauri.conf.json",
        HELPER_SOURCE,
        CINEWARE_INCLUDES,
        CINEWARE_LIB_DIR,
        PREBUILT_HELPER,
        SBSAR_HELPER_SOURCE,
        SBSAR_INCLUDE_DIRS[0],
        SBSAR_INCLUDE_DIRS[1],
        SBSAR_LIB_DIR,
        SBSAR_BIN_DIR,
        PREBUILT_SBSAR_HELPER,
    ] {
        println!("cargo:rerun-if-changed={path}");
    }
    println!("cargo:rerun-if-env-changed=CXX");
    println!("cargo:rerun-if-env-changed=CC");

    build_c4d_preview();
    build_sbsar_renderer();
    tauri_build::build()
}

/// Inputs shared by every MSVC helper build.
struct NativeHelper<'a> {
    source: &'a Path,
    exe: &'a Path,
    include_dirs: &'a [&'a str],
    lib_dir: &'a str,
    libraries: &'a [&'a str],
    windows_defines: bool,
    /// The vendored SDK libs are built against the dynamic CRT (`/MD`).
    dynamic_runtime: bool,
}

/// Everything that differs between the embedded native helpers.
struct HelperBuild<'a> {
    label: &'a str,
    source: &'a str,
    prebuilt: &'a str,
    exe_name: &'a str,
    generated_name: &'a str,
    include_dirs: &'a [&'a str],
    lib_dir: &'a str,
    libraries: &'a [&'a str],
    windows_defines: bool,
    dynamic_runtime: bool,
    embed: fn(&Path, &Path),
    stub: fn(&Path),
}

/// Build one helper and emit a small Rust source file that embeds it in the
/// application binary. Non-Windows builds and builds without the vendored SDK
/// libraries get an empty stub, so they never fail or change.
fn build_helper(build: &HelperBuild<'_>, ready: bool) {
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").expect("OUT_DIR is set by cargo"));
    let generated = out_dir.join(build.generated_name);

    if !targets_windows() {
        (build.stub)(&generated);
        return;
    }
    if !ready {
        (build.stub)(&generated);
        return;
    }

    let source = PathBuf::from(build.source);
    let prebuilt = PathBuf::from(build.prebuilt);
    if prebuilt.is_file() && is_fresh(&prebuilt, &source) {
        (build.embed)(&generated, &prebuilt);
        return;
    }

    let exe = out_dir.join(build.exe_name);
    let helper = NativeHelper {
        source: &source,
        exe: &exe,
        include_dirs: build.include_dirs,
        lib_dir: build.lib_dir,
        libraries: build.libraries,
        windows_defines: build.windows_defines,
        dynamic_runtime: build.dynamic_runtime,
    };
    match compile_helper(&helper) {
        Ok(()) => (build.embed)(&generated, &exe),
        Err(reason) => {
            println!("cargo:warning={} was not built: {reason}", build.label);
            (build.stub)(&generated);
        }
    }
}

/// Build the Cineware GLB helper.
fn build_c4d_preview() {
    build_helper(&C4D_BUILD, c4d_libraries_ready());
}

const C4D_BUILD: HelperBuild<'static> = HelperBuild {
    label: "C4D preview helper",
    source: HELPER_SOURCE,
    prebuilt: PREBUILT_HELPER,
    exe_name: "c4d_preview.exe",
    generated_name: "c4d_preview_exe.rs",
    include_dirs: &C4D_INCLUDE_DIRS,
    lib_dir: CINEWARE_LIB_DIR,
    libraries: &C4D_LIBRARIES,
    windows_defines: true,
    dynamic_runtime: false,
    embed: write_embed,
    stub: write_stub,
};

fn c4d_libraries_ready() -> bool {
    ["cinewarelib_release.lib", "jpeglib_release.lib"]
        .iter()
        .all(|library| Path::new(CINEWARE_LIB_DIR).join(library).is_file())
}

/// Build the native SBSAR renderer and embed it together with its two runtime
/// DLLs. The DLLs live next to the executable when it is extracted to
/// `%TEMP%\polystash_sbsar`, so Windows resolves them without a PATH entry.
fn build_sbsar_renderer() {
    build_helper(&SBSAR_BUILD, sbsar_ready());
}

const SBSAR_BUILD: HelperBuild<'static> = HelperBuild {
    label: "SBSAR renderer",
    source: SBSAR_HELPER_SOURCE,
    prebuilt: PREBUILT_SBSAR_HELPER,
    exe_name: "sbsar_render.exe",
    generated_name: "sbsar_render_exe.rs",
    include_dirs: &SBSAR_INCLUDE_DIRS,
    lib_dir: SBSAR_LIB_DIR,
    libraries: &SBSAR_LIBRARIES,
    windows_defines: false,
    dynamic_runtime: true,
    embed: write_sbsar_embed,
    stub: write_sbsar_stub,
};

fn sbsar_ready() -> bool {
    if !sbsar_libraries_ready() {
        return false;
    }
    sbsar_dlls_ready()
}

fn sbsar_libraries_ready() -> bool {
    SBSAR_LIBRARIES
        .iter()
        .all(|library| Path::new(SBSAR_LIB_DIR).join(library).is_file())
}

fn sbsar_dlls_ready() -> bool {
    SBSAR_DLLS
        .iter()
        .all(|library| Path::new(SBSAR_BIN_DIR).join(library).is_file())
}

fn targets_windows() -> bool {
    env::var("CARGO_CFG_TARGET_OS").as_deref() == Ok("windows")
}

fn find_cl() -> Option<PathBuf> {
    for variable in ["CXX", "CC"] {
        let Ok(value) = env::var(variable) else { continue };
        let Some(first) = value.split_whitespace().next() else { continue };
        let candidate = PathBuf::from(first);
        let name = candidate
            .file_name()
            .and_then(|name| name.to_str())
            .unwrap_or_default();
        if name.eq_ignore_ascii_case("cl") || name.eq_ignore_ascii_case("cl.exe") {
            return Some(candidate);
        }
    }

    let output = Command::new("where").arg("cl.exe").output().ok()?;
    if !output.status.success() {
        return None;
    }
    let stdout = String::from_utf8_lossy(&output.stdout);
    let first = stdout.lines().map(str::trim).find(|line| !line.is_empty())?;
    let path = PathBuf::from(first);
    path.is_file().then_some(path)
}

fn compile_helper(helper: &NativeHelper<'_>) -> Result<(), String> {
    let cl = find_cl().ok_or_else(|| {
        "cl.exe not found; run from a Visual Studio developer prompt or set up MSVC".to_string()
    })?;
    let out_dir = helper.exe.parent().expect("helper output has a parent directory");
    let stem = helper
        .exe
        .file_stem()
        .and_then(|stem| stem.to_str())
        .unwrap_or("helper");
    let object = out_dir.join(format!("{stem}.obj"));
    let pdb = out_dir.join(format!("{stem}.pdb"));

    let mut command = Command::new(cl);
    command
        .arg("/nologo")
        .arg("/std:c++17")
        .arg("/EHsc")
        .arg("/O2")
        .arg("/DNDEBUG")
        .arg("/D_CRT_SECURE_NO_WARNINGS");
    command.arg(if helper.dynamic_runtime { "/MD" } else { "/MT" });
    if helper.windows_defines {
        command.arg("/DWIN32").arg("/D_WINDOWS");
    }
    for include_dir in helper.include_dirs {
        command.arg(format!("/I{include_dir}"));
    }
    command
        .arg(format!("/Fo{}", object.display()))
        .arg(format!("/Fd{}", pdb.display()))
        .arg(format!("/Fe{}", helper.exe.display()))
        .arg(helper.source)
        .arg("/link")
        .arg(format!("/LIBPATH:{}", helper.lib_dir));
    for library in helper.libraries {
        command.arg(library);
    }

    let output = command
        .output()
        .map_err(|error| format!("failed to run cl.exe: {error}"))?;
    if output.status.success() && helper.exe.is_file() {
        return Ok(());
    }
    Err(format!("cl.exe failed: {}", summarize(&output)))
}

fn summarize(output: &Output) -> String {
    let stderr = String::from_utf8_lossy(&output.stderr);
    let stdout = String::from_utf8_lossy(&output.stdout);
    let detail = if stderr.trim().is_empty() { stdout } else { stderr };
    detail
        .trim()
        .lines()
        .take(10)
        .collect::<Vec<_>>()
        .join(" | ")
}

fn is_fresh(exe: &Path, source: &Path) -> bool {
    let exe_time = fs::metadata(exe).and_then(|metadata| metadata.modified()).ok();
    let source_time = fs::metadata(source).and_then(|metadata| metadata.modified()).ok();
    matches!((exe_time, source_time), (Some(exe_time), Some(source_time)) if exe_time >= source_time)
}

fn absolute(path: &Path) -> PathBuf {
    fs::canonicalize(path).unwrap_or_else(|_| path.to_path_buf())
}

fn write_embed(generated: &Path, exe: &Path) {
    let literal = format!("{:?}", absolute(exe));
    let contents = format!(
        "// Generated by build.rs. Do not edit.\n\
         pub const C4D_PREVIEW_EXE: Option<&'static [u8]> = Some(include_bytes!({literal}));\n"
    );

    if let Err(error) = fs::write(generated, contents) {
        println!("cargo:warning=failed to write C4D helper embed source: {error}");
    }
}

fn write_sbsar_embed(generated: &Path, exe: &Path) {
    let exe = format!("{:?}", absolute(exe));
    let linker = format!("{:?}", absolute(&Path::new(SBSAR_BIN_DIR).join(SBSAR_DLLS[0])));
    let sse2 = format!("{:?}", absolute(&Path::new(SBSAR_BIN_DIR).join(SBSAR_DLLS[1])));
    let contents = format!(
        "// Generated by build.rs. Do not edit.\n\
         pub const SBSAR_RENDER_EXE: Option<&'static [u8]> = Some(include_bytes!({exe}));\n\
         pub const SBSAR_LINKER_DLL: Option<&'static [u8]> = Some(include_bytes!({linker}));\n\
         pub const SBSAR_SSE2_DLL: Option<&'static [u8]> = Some(include_bytes!({sse2}));\n"
    );

    if let Err(error) = fs::write(generated, contents) {
        println!("cargo:warning=failed to write SBSAR helper embed source: {error}");
    }
}

fn write_stub(generated: &Path) {
    let contents = "// Generated by build.rs. No Cineware helper for this target.\n\
                    pub const C4D_PREVIEW_EXE: Option<&'static [u8]> = None;\n";
    if let Err(error) = fs::write(generated, contents) {
        println!("cargo:warning=failed to write C4D helper stub: {error}");
    }
}

fn write_sbsar_stub(generated: &Path) {
    let contents = "// Generated by build.rs. No SBSAR renderer for this target.\n\
                    pub const SBSAR_RENDER_EXE: Option<&'static [u8]> = None;\n\
                    pub const SBSAR_LINKER_DLL: Option<&'static [u8]> = None;\n\
                    pub const SBSAR_SSE2_DLL: Option<&'static [u8]> = None;\n";
    if let Err(error) = fs::write(generated, contents) {
        println!("cargo:warning=failed to write SBSAR helper stub: {error}");
    }
}
