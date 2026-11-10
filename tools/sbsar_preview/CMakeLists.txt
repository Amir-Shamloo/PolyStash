cmake_minimum_required(VERSION 3.16)
project(polystash_sbsar_preview CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SUBSTANCE_SDK_ROOT "$ENV{SUBSTANCE_SDK_ROOT}" CACHE PATH "Adobe Substance 3D SDK root")
if(NOT SUBSTANCE_SDK_ROOT)
  message(FATAL_ERROR "SUBSTANCE_SDK_ROOT must point at the Adobe Substance 3D SDK (substance-win-x64-*)")
endif()

add_executable(sbsar_preview
  main.cpp
  frameworktestddssave.cpp
)

target_include_directories(sbsar_preview PRIVATE
  "${SUBSTANCE_SDK_ROOT}/include"
  "${SUBSTANCE_SDK_ROOT}/include/substance"
)

target_link_libraries(sbsar_preview PRIVATE
  "${SUBSTANCE_SDK_ROOT}/lib/release/substance_framework.lib"
  "${SUBSTANCE_SDK_ROOT}/lib/release/tinyxml.lib"
)

if(MSVC)
  target_compile_definitions(sbsar_preview PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# The Substance engine DLLs must sit next to the executable at runtime.
add_custom_command(TARGET sbsar_preview POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SUBSTANCE_SDK_ROOT}/bin/release" "$<TARGET_FILE_DIR:sbsar_preview>"
)
