# Third-party dependencies (decision T13, docs/project/decisions.md):
# find_package() first, FetchContent fallback with pinned versions.

include(FetchContent)
set(FETCHCONTENT_QUIET ON)

# --- zlib ---------------------------------------------------------------------------------------
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)  # they register their own ctest entries
FetchContent_Declare(zlib
    URL https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz
    URL_HASH SHA256=9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23
    FIND_PACKAGE_ARGS NAMES ZLIB)
FetchContent_MakeAvailable(zlib)
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
    target_include_directories(zlibstatic INTERFACE ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
endif()

# --- Asio standalone (header-only, decision T20): only x2::platform includes it ------------------
find_path(X2_ASIO_INCLUDE_DIR asio.hpp)
if(NOT X2_ASIO_INCLUDE_DIR)
    FetchContent_Declare(asio
        URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz
        URL_HASH SHA256=755bd7f85a4b269c67ae0ea254907c078d408cce8e1a352ad2ed664d233780e8
        SOURCE_SUBDIR do-not-add)
    FetchContent_MakeAvailable(asio)
    set(X2_ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "" FORCE)
endif()
add_library(x2_asio INTERFACE)
target_include_directories(x2_asio SYSTEM INTERFACE ${X2_ASIO_INCLUDE_DIR})
target_compile_definitions(x2_asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
if(WIN32)
    target_compile_definitions(x2_asio INTERFACE _WIN32_WINNT=0x0A00)  # Windows 10
    target_link_libraries(x2_asio INTERFACE ws2_32 mswsock)
endif()
add_library(x2::asio ALIAS x2_asio)

# --- LuaJIT 2.1 (Lua 5.1 API, decision T21) ----------------------------------------------------------
# Upstream, unmodified (not the client's LuaJIT 2.0.2 copy). Linux: system package via pkg-config first.
# Otherwise (Windows, or no package): the source at a pinned commit, built by its own scripts (LuaJIT has no
# CMake): the Makefile, or msvcbuild.bat on Windows (T25; it compiles with cl, ABI-compatible with clang-cl).
set(X2_LUAJIT_COMMIT a4c064539ea931274d407e1249b4bb575424a0f4)  # v2.1, 2026-10-21 (2.1.1792588164)
if(NOT TARGET x2::luajit)
    if(NOT WIN32)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(LUAJIT IMPORTED_TARGET luajit)
        endif()
    endif()

    if(TARGET PkgConfig::LUAJIT)
        add_library(x2::luajit ALIAS PkgConfig::LUAJIT)
    else()
        FetchContent_Declare(luajit
            URL https://github.com/LuaJIT/LuaJIT/archive/${X2_LUAJIT_COMMIT}.tar.gz
            URL_HASH SHA256=15625df3ebfbd1f76371005bffae39b84c65e718e57acc9a9c57863c7c101541
            SOURCE_SUBDIR _no_cmake)   # only the sources; it is built below
        FetchContent_MakeAvailable(luajit)

        if(MSVC)
            # Needs the Visual Studio environment (cl, lib, INCLUDE), as the Ninja generator already does. The script
            # sets no CRT flag; CL passes the one of our targets (CMake's default /MD, /MDd in Debug), or link mixes them.
            # _CL_ goes after the script's own options, so its /w wins over the script's /W3: third-party warnings.
            set(_x2_luajit_lib ${luajit_SOURCE_DIR}/src/lua51.lib)
            add_custom_command(OUTPUT ${_x2_luajit_lib}
                COMMAND ${CMAKE_COMMAND} -E env "CL=/MD$<$<CONFIG:Debug>:d>" "_CL_=/w" cmd /c msvcbuild.bat static
                WORKING_DIRECTORY ${luajit_SOURCE_DIR}/src
                COMMENT "Building LuaJIT ${X2_LUAJIT_COMMIT} (msvcbuild.bat)"
                VERBATIM)
        else()
            set(_x2_luajit_make BUILDMODE=static CC=${CMAKE_C_COMPILER})

            find_program(X2_MAKE NAMES gmake make REQUIRED)
            set(_x2_luajit_lib ${luajit_SOURCE_DIR}/src/libluajit.a)
            add_custom_command(OUTPUT ${_x2_luajit_lib}
                COMMAND ${X2_MAKE} -C ${luajit_SOURCE_DIR}/src ${_x2_luajit_make} libluajit.a
                WORKING_DIRECTORY ${luajit_SOURCE_DIR}
                COMMENT "Building LuaJIT ${X2_LUAJIT_COMMIT}"
                VERBATIM)
        endif()
        add_custom_target(x2_luajit_build DEPENDS ${_x2_luajit_lib})

        add_library(x2_luajit STATIC IMPORTED GLOBAL)
        set_target_properties(x2_luajit PROPERTIES
            IMPORTED_LOCATION ${_x2_luajit_lib}
            INTERFACE_INCLUDE_DIRECTORIES ${luajit_SOURCE_DIR}/src)
        if(NOT WIN32)
            set_property(TARGET x2_luajit PROPERTY INTERFACE_LINK_LIBRARIES m ${CMAKE_DL_LIBS})
        endif()
        add_dependencies(x2_luajit x2_luajit_build)
        add_library(x2::luajit ALIAS x2_luajit)
    endif()
endif()

# --- doctest (tests only) ---------------------------------------------------------------------------
if(X2_BUILD_TESTS)
    # header-only; its own CMakeLists requires a CMake policy level that CMake 4 removed, so skip it
    FetchContent_Declare(doctest
        URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.11.tar.gz
        URL_HASH SHA256=632ed2c05a7f53fa961381497bf8069093f0d6628c5f26286161fbd32a560186
        SOURCE_SUBDIR do-not-add
        FIND_PACKAGE_ARGS NAMES doctest)
    FetchContent_MakeAvailable(doctest)
    if(NOT TARGET doctest::doctest)
        add_library(x2_doctest INTERFACE)
        target_include_directories(x2_doctest SYSTEM INTERFACE ${doctest_SOURCE_DIR})
        # doctest includes <ciso646> on Clang, which libstdc++ >= 15 flags with #warning
        target_compile_options(x2_doctest INTERFACE $<$<CXX_COMPILER_ID:Clang>:-Wno-\#warnings>)
        add_library(doctest::doctest ALIAS x2_doctest)
    endif()
endif()

# --- SDL3 and Dear ImGui (decision T32): the client and the tools only, never the servers ---------------
if(X2_BUILD_TOOLS OR X2_BUILD_CLIENT)
    # Always the pinned release, linked statically on every platform (T32): one self-contained executable, the same
    # SDL everywhere. On Linux it needs the X11/Wayland/input/audio development headers (docs/building-linux.md); SDL
    # loads those libraries at run time, so the executable still runs on a machine with only one of them.
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL3
        URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.18/SDL3-3.4.18.tar.gz
        URL_HASH SHA256=9c75cf16330322c217dedd2e0609f1124f1b54b8633e763467b4684d0f4334a3)
    FetchContent_MakeAvailable(SDL3)

    # Compiled into our own target from the sources (ImGui ships no CMake), with the SDL3 platform and SDL_GPU renderer
    # backends. Pinned; packages do not reliably ship the sdlgpu3 backend.
    FetchContent_Declare(imgui
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.93.0.tar.gz
        URL_HASH SHA256=f7bf6bc76e5d79d40d751096663e6c5d18daf08129dd52e7f7b09e4251c8eeee
        SOURCE_SUBDIR do-not-add)
    FetchContent_MakeAvailable(imgui)
    add_library(x2_imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_null.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)
    target_include_directories(x2_imgui SYSTEM PUBLIC
        ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends ${imgui_SOURCE_DIR}/misc/cpp)
    target_compile_definitions(x2_imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
    target_link_libraries(x2_imgui PUBLIC SDL3::SDL3)
    set_target_properties(x2_imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)
    add_library(x2::imgui ALIAS x2_imgui)
endif()

# --- pl_mpeg: the client's MPEG-1 movies (docs/porting/client-movie.md), replacing BandiVideo ---------------
# Single-header C library with no releases: pinned to a commit. Compiled once into its own target without the project's
# warning options, like the other third-party code.
if(X2_BUILD_CLIENT)
    FetchContent_Declare(pl_mpeg
        URL https://github.com/phoboslab/pl_mpeg/archive/c871f2be022ece7ef4f64230b4fb8e1fb9eb6023.tar.gz
        URL_HASH SHA256=7caee5a96118afde256146556f3f207b41d85b8c4f55fa5fb6b4759b06470fd1
        SOURCE_SUBDIR do-not-add)
    FetchContent_MakeAvailable(pl_mpeg)
    set(X2_PL_MPEG_IMPL ${CMAKE_BINARY_DIR}/generated/pl_mpeg_impl.c)
    file(GENERATE OUTPUT ${X2_PL_MPEG_IMPL}
        CONTENT "#include <stddef.h>\n#define PL_MPEG_IMPLEMENTATION\n#include \"pl_mpeg.h\"\n")
    add_library(x2_pl_mpeg STATIC ${X2_PL_MPEG_IMPL})
    target_include_directories(x2_pl_mpeg SYSTEM PUBLIC ${pl_mpeg_SOURCE_DIR})
    # Only the video decoder is used and the movies are read from memory. With PLM_NO_STDIO the header no longer gets
    # size_t from <stdio.h>: includers include <stddef.h> first.
    target_compile_definitions(x2_pl_mpeg PUBLIC PLM_NO_STDIO)
    if(NOT MSVC)
        target_link_libraries(x2_pl_mpeg PRIVATE m)
    endif()
    add_library(x2::pl_mpeg ALIAS x2_pl_mpeg)
endif()

# --- stb: the client's .tga/.png/.jpg/.bmp textures and the BC encoder (docs/porting/client-d3d9.md, T37) -----------
# stb_image 2.30 and stb_dxt 1.12, single-header C libraries with no releases: pinned to a commit (2026-08-02) and
# compiled once into their own target, like pl_mpeg. Only the decoders the assets need, from memory.
if(X2_BUILD_CLIENT)
    FetchContent_Declare(stb
        URL https://github.com/nothings/stb/archive/2c980bb59875b0d32144a71867fbdebb2f77cd20.tar.gz
        URL_HASH SHA256=9a955b1b49a4410088a2e0ee2a9c057c3c907d0c1d75454144cb980aca0ba515
        SOURCE_SUBDIR do-not-add)
    FetchContent_MakeAvailable(stb)
    set(X2_STB_IMPL ${CMAKE_BINARY_DIR}/generated/stb_impl.c)
    file(GENERATE OUTPUT ${X2_STB_IMPL}
        CONTENT "#define STB_IMAGE_IMPLEMENTATION\n#include \"stb_image.h\"\n#define STB_DXT_IMPLEMENTATION\n#include \"stb_dxt.h\"\n")
    add_library(x2_stb STATIC ${X2_STB_IMPL})
    target_include_directories(x2_stb SYSTEM PUBLIC ${stb_SOURCE_DIR})
    target_compile_definitions(x2_stb PUBLIC STBI_NO_STDIO STBI_ONLY_TGA STBI_ONLY_PNG STBI_ONLY_JPEG STBI_ONLY_BMP)
    if(NOT MSVC)
        target_link_libraries(x2_stb PRIVATE m)
    endif()
    add_library(x2::stb ALIAS x2_stb)
endif()

# --- miniaudio: the client's audio (T45, docs/porting/client-ktdxlib.md#sound), replacing FMOD Ex ---------------------
# Single-file C library, pinned to a release (0.11.22) and compiled once into its own target with stb_vorbis from the
# pinned stb above, without the project's warning options, like the other third-party code. Only what the client uses:
# no device layer (SDL3 plays the mix), no resource manager (the client decodes from memory), no MP3/FLAC, no encoders.
if(X2_BUILD_CLIENT)
    FetchContent_Declare(miniaudio
        URL https://github.com/mackron/miniaudio/archive/refs/tags/0.11.22.tar.gz
        URL_HASH SHA256=bcb07bfb27e6fa94d34da73ba2d5642d4940b208ec2a660dbf4e52e6b7cd492f
        SOURCE_SUBDIR do-not-add)
    FetchContent_MakeAvailable(miniaudio)
    set(X2_MINIAUDIO_IMPL ${CMAKE_BINARY_DIR}/generated/miniaudio_impl.c)
    file(GENERATE OUTPUT ${X2_MINIAUDIO_IMPL}
        CONTENT "#define STB_VORBIS_HEADER_ONLY\n#include \"stb_vorbis.c\"\n#define MINIAUDIO_IMPLEMENTATION\n#include \"miniaudio.h\"\n#undef STB_VORBIS_HEADER_ONLY\n#include \"stb_vorbis.c\"\n")
    add_library(x2_miniaudio STATIC ${X2_MINIAUDIO_IMPL})
    target_include_directories(x2_miniaudio SYSTEM PUBLIC ${miniaudio_SOURCE_DIR})
    target_include_directories(x2_miniaudio SYSTEM PRIVATE ${stb_SOURCE_DIR})
    target_compile_definitions(x2_miniaudio PUBLIC
        MA_NO_DEVICE_IO MA_NO_RESOURCE_MANAGER MA_NO_MP3 MA_NO_FLAC MA_NO_ENCODING)
    find_package(Threads REQUIRED)
    target_link_libraries(x2_miniaudio PRIVATE Threads::Threads)
    if(NOT MSVC)
        target_link_libraries(x2_miniaudio PRIVATE m)
    endif()
    add_library(x2::miniaudio ALIAS x2_miniaudio)
endif()

# --- FreeType: the client's fonts (KTDXLIB, KTDGFreeTypeFont) ---------------------------------------------------
# Upstream, pinned and linked statically (the original linked freetype2411.lib, docs/original/libraries.md). Only the
# core: no zlib/bzip2/PNG/Brotli/HarfBuzz (the client's fonts are plain TrueType).
if(X2_BUILD_CLIENT)
    set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
    set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
    FetchContent_Declare(freetype
        URL https://github.com/freetype/freetype/archive/refs/tags/VER-2-13-3.tar.gz
        URL_HASH SHA256=bc5c898e4756d373e0d991bab053036c5eb2aa7c0d5c67e8662ddc6da40c4103
        EXCLUDE_FROM_ALL)
    set(X2_SAVED_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS OFF)
    FetchContent_MakeAvailable(freetype)
    set(BUILD_SHARED_LIBS ${X2_SAVED_BUILD_SHARED_LIBS})
    set_target_properties(freetype PROPERTIES POSITION_INDEPENDENT_CODE ON)
    if(NOT TARGET Freetype::Freetype)
        add_library(Freetype::Freetype ALIAS freetype)
    endif()
endif()

# ODBC (decision T17): unixODBC outside Windows, odbc32 on Windows. Only code that includes
# x2/platform/odbc.h links it, so x2_platform itself does not depend on ODBC.
add_library(x2_odbc INTERFACE)
if(WIN32)
    target_link_libraries(x2_odbc INTERFACE odbc32 x2::platform)
else()
    find_package(ODBC REQUIRED)
    target_link_libraries(x2_odbc INTERFACE ODBC::ODBC x2::platform)
endif()
add_library(x2::odbc ALIAS x2_odbc)
