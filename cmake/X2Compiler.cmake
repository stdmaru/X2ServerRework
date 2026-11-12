# Common compiler settings shared by every X2 target, applied through the
# x2::options interface library so third-party targets are left untouched.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# no C++20 modules: skip the dependency scan step (it also stops the whole build on the first scan error)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_library(x2_options INTERFACE)
add_library(x2::options ALIAS x2_options)

# Always defined (0/1) so code can use `#if X2_DEV_TOOLS`.
target_compile_definitions(x2_options INTERFACE
    X2_DEV_TOOLS=$<IF:$<BOOL:${X2_DEV_TOOLS}>,1,0>)

# The original is a Unicode build: TCHAR, LPCTSTR and the Win32 A/W macros are the wide ones.
if(WIN32)
    target_compile_definitions(x2_options INTERFACE UNICODE _UNICODE)
endif()

if(MSVC)
    # clang-cl (T26): the MSVC command line and ABI, Clang's diagnostics (/W4 is -Wall -Wextra).
    target_compile_options(x2_options INTERFACE /utf-8 /W4 /permissive- /Zc:__cplusplus)
    target_compile_definitions(x2_options INTERFACE NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
else()
    # -fsigned-char: the original assumes MSVC's signed `char`; it is unsigned by default on Linux arm64.
    target_compile_options(x2_options INTERFACE -Wall -Wextra -finput-charset=UTF-8 -fsigned-char)
endif()

# x2_windows_icon(<executable target> <icon.ico>): the application icon of the original executable (its <Server>.rc had
# `IDI_ICON1 ICON "icon.ico"`). Windows only; the .rc is generated next to the build files.
function(x2_windows_icon target icon)
    if(NOT WIN32)
        return()
    endif()
    set(rc "${CMAKE_CURRENT_BINARY_DIR}/${target}_icon.rc")
    file(TO_CMAKE_PATH "${icon}" icon_path)
    file(CONFIGURE OUTPUT "${rc}" CONTENT "IDI_ICON1 ICON \"${icon_path}\"\n")
    target_sources(${target} PRIVATE "${rc}")
    set_property(SOURCE "${rc}" APPEND PROPERTY OBJECT_DEPENDS "${icon}")
endfunction()

# x2_embed_file(<target> <file> <symbol>): the file's bytes compiled into the target, as
# `extern const unsigned char <symbol>[]` and `extern const std::size_t <symbol>_size` (a .cpp generated at configure
# time; CMake configures again when the file changes). For the few files the original kept as executable resources.
function(x2_embed_file target file symbol)
    file(READ "${file}" hex HEX)
    string(LENGTH "${hex}" length)
    math(EXPR length "${length} / 2")
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," bytes "${hex}")
    set(out "${CMAKE_CURRENT_BINARY_DIR}/${symbol}.cpp")
    file(CONFIGURE OUTPUT "${out}" CONTENT "#include <cstddef>\nextern const unsigned char ${symbol}[] = {${bytes}};\nextern const std::size_t ${symbol}_size = ${length};\n")
    target_sources(${target} PRIVATE "${out}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${file}")
endfunction()
