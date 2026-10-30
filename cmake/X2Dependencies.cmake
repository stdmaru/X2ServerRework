# Third-party dependencies (decision T13, docs/projeto/decisoes.md):
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

# --- Crypto++ (no upstream CMake: cryptopp-cmake builds the pinned release) -----------------------
find_package(cryptopp CONFIG QUIET)
if(NOT TARGET cryptopp::cryptopp)
    set(CRYPTOPP_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CRYPTOPP_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(cryptopp-cmake
        GIT_REPOSITORY https://github.com/abdes/cryptopp-cmake.git
        GIT_TAG CRYPTOPP_8_9_0
        GIT_SHALLOW TRUE)
    # cryptopp-cmake 8.9 still calls FetchContent_Populate() (deprecated in CMake 3.30, policy CMP0169)
    set(CMAKE_POLICY_DEFAULT_CMP0169 OLD)
    FetchContent_MakeAvailable(cryptopp-cmake)
    unset(CMAKE_POLICY_DEFAULT_CMP0169)
    # cryptopp-cmake also exposes .../cryptopp itself, putting generic names (zlib.h, config.h, misc.h,
    # base64.h...) on everyone's include path. Keep only the parent directory: <cryptopp/xxx.h>.
    get_target_property(_x2_cryptopp_inc cryptopp INTERFACE_INCLUDE_DIRECTORIES)
    list(FILTER _x2_cryptopp_inc EXCLUDE REGEX "/cryptopp/?(>)?$")
    set_target_properties(cryptopp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${_x2_cryptopp_inc}")
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

# --- LuaJIT (Lua 5.1 API) ---------------------------------------------------------------------------
# Linux: system package via pkg-config. Windows: still to be done (LuaJIT builds with its own scripts).
if(NOT TARGET x2::luajit)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(LUAJIT IMPORTED_TARGET luajit)
    endif()
    if(TARGET PkgConfig::LUAJIT)
        add_library(x2::luajit ALIAS PkgConfig::LUAJIT)
    else()
        message(WARNING "LuaJIT not found: targets that need Lua are skipped (docs/porte/kncsdk.md)")
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
