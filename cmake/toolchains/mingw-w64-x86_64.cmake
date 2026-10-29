# Cross-compiles for Windows x64 with MinGW-w64 (GCC, UCRT runtime) from Linux; tests run under Wine.
# Decision T18 (docs/projeto/decisoes.md). This is a check of the Windows code paths, not a release
# toolchain: the shipped Windows build stays MSVC.

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(X2_MINGW_TRIPLE x86_64-w64-mingw32 CACHE STRING "MinGW-w64 target triple (compiler prefix)")
set(CMAKE_C_COMPILER   ${X2_MINGW_TRIPLE}-gcc)
set(CMAKE_CXX_COMPILER ${X2_MINGW_TRIPLE}-g++)
set(CMAKE_RC_COMPILER  ${X2_MINGW_TRIPLE}-windres)

# Search libraries/headers only in the target sysroot, never in the Linux host's /usr.
set(CMAKE_FIND_ROOT_PATH /usr/${X2_MINGW_TRIPLE})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
# Same for pkg-config (otherwise it returns the host's LuaJIT).
set(ENV{PKG_CONFIG_LIBDIR} /usr/${X2_MINGW_TRIPLE}/lib/pkgconfig)
set(ENV{PKG_CONFIG_SYSROOT_DIR} /)

# Static runtime (libgcc, libstdc++, winpthreads): the .exe runs under Wine without extra DLLs on the path.
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static")

# add_test() commands are run through Wine by ctest.
find_program(X2_WINE NAMES wine wine64)
if(X2_WINE)
    set(CMAKE_CROSSCOMPILING_EMULATOR ${X2_WINE})
endif()
