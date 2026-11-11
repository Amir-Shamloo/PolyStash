use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

/// Repo-root helper source, relative to this crate.
const HELPER_SOURCE: &str = "../../../tools/c4d_preview/main.cpp";
const CINEWARE_INCLUDES: &str = "third_party/cineware/includes";
const CINEWARE_LIB_DIR: &str = "third_party/cineware/lib";
/// CI builds the helper here before `tauri build`; the binary is then embedded.
const PREBUILT_HELPER: &str = "bin/c4d/c4d_preview.exe";

fn main() {
    println!("cargo:rerun-if-changed=tauri.conf.json");
    println!("cargo:rerun-if-changed={HELPER_SOURCE}");
    println!("cargo:rerun-if-changed={CINEWARE_INCLUDES}");
    println!("cargo:rerun-if-changed={CINEWARE_LIB_DIR}");
    println!("cargo:rerun-if-changed={PREBUILT_HELPER}");
    println!("cargo:rerun-if-env-changed=CXX");
    println!("cargo:rerun-if-env-changed=CC");

    build_c4d_preview();
    tauri_build::build()
}

/// Build the Cineware GLB helper and emit a small Rust source file that embeds
/// it in the application binary. Non-Windows builds and builds without the
/// vendored SDK libraries get an empty stub, so they never fail or change.
fn build_c4d_preview() {
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").expect("OUT_DIR is set by cargo"));
    let generated = out_dir.join("c4d_preview_exe.rs");

    if env::var("CARGO_CFG_TARGET_OS").as_deref() != Ok("windows") {
        write_stub(&generated);
        return;
    }

    let cineware = Path::new(CINEWARE_LIB_DIR).join("cinewarelib_release.lib");
    let jpeg = Path::new(CINEWARE_LIB_DIR).join("jpeglib_release.lib");
    if !cineware.is_file() || !jpeg.is_file() {
        write_stub(&generated);
        return;
    }

    let source = PathBuf::from(HELPER_SOURCE);
    let prebuilt = PathBuf::from(PREBUILT_HELPER);
    if prebuilt.is_file() && is_fresh(&prebuilt, &source) {
        write_embed(&generated, &prebuilt);
        return;
    }

    let exe = out_dir.join("c4d_preview.exe");
    match compile_helper(&source, &exe, &cineware, &jpeg) {
        Ok(()) => write_embed(&generated, &exe),
        Err(reason) => {
            println!("cargo:warning=C4D preview helper was not built: {reason}");
            write_stub(&generated);
        }
    }
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

fn compile_helper(source: &Path, exe: &Path, cineware: &Path, jpeg: &Path) -> Result<(), String> {
    let cl = find_cl().ok_or_else(|| {
        "cl.exe not found; run from a Visual Studio developer prompt or set up MSVC".to_string()
    })?;
    let out_dir = exe.parent().expect("helper output has a parent directory");
    let object = out_dir.join("c4d_preview.obj");
    let pdb = out_dir.join("c4d_preview.pdb");

    let output = Command::new(cl)
        .arg("/nologo")
        .arg("/std:c++17")
        .arg("/EHsc")
        .arg("/MT")
        .arg("/O2")
        .arg("/DNDEBUG")
        .arg("/DWIN32")
        .arg("/D_WINDOWS")
        .arg("/D_CRT_SECURE_NO_WARNINGS")
        .arg(format!("/I{CINEWARE_INCLUDES}"))
        .arg(format!("/Fo{}", object.display()))
        .arg(format!("/Fd{}", pdb.display()))
        .arg(format!("/Fe{}", exe.display()))
        .arg(source)
        .arg("/link")
        .arg(format!("/LIBPATH:{CINEWARE_LIB_DIR}"))
        .arg(cineware)
        .arg(jpeg)
        .arg("ws2_32.lib")
        .arg("winmm.lib")
        .output()
        .map_err(|error| format!("failed to run cl.exe: {error}"))?;

    if output.status.success() && exe.is_file() {
        return Ok(());
    }

    let stderr = String::from_utf8_lossy(&output.stderr);
    let stdout = String::from_utf8_lossy(&output.stdout);
    let detail = if stderr.trim().is_empty() { stdout } else { stderr };
    let summary = detail
        .trim()
        .lines()
        .take(10)
        .collect::<Vec<_>>()
        .join(" | ");
    Err(format!("cl.exe failed: {summary}"))
}

fn is_fresh(exe: &Path, source: &Path) -> bool {
    let exe_time = fs::metadata(exe).and_then(|metadata| metadata.modified()).ok();
    let source_time = fs::metadata(source).and_then(|metadata| metadata.modified()).ok();
    matches!((exe_time, source_time), (Some(exe_time), Some(source_time)) if exe_time >= source_time)
}

fn write_embed(generated: &Path, exe: &Path) {
    let absolute = fs::canonicalize(exe).unwrap_or_else(|_| exe.to_path_buf());
    let literal = format!("{absolute:?}");
    let contents = format!(
        "// Generated by build.rs. Do not edit.\n\
         pub const C4D_PREVIEW_EXE: Option<&'static [u8]> = Some(include_bytes!({literal}));\n"
    );

    if let Err(error) = fs::write(generated, contents) {
        println!("cargo:warning=failed to write C4D helper embed source: {error}");
    }
}

fn write_stub(generated: &Path) {
    let contents = "// Generated by build.rs. No Cineware helper for this target.\n\
                    pub const C4D_PREVIEW_EXE: Option<&'static [u8]> = None;\n";
    if let Err(error) = fs::write(generated, contents) {
        println!("cargo:warning=failed to write C4D helper stub: {error}");
    }
}
