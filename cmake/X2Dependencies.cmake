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
