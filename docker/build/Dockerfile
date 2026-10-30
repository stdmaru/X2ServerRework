# Build environment of the project (decision T24, docs/ferramentas/ambiente-build.md): the same image runs the
# local builds (docker/build.sh) and the CI (.github/workflows/ci.yml).
# Ubuntu LTS: GCC, Clang, CMake, Ninja, MinGW-w64 (posix threads) + Wine, unixODBC/FreeTDS, Python.
# No LuaJIT package: Ubuntu ships the OpenResty fork, so CMake builds the pinned upstream commit (T21).
FROM ubuntu:26.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        build-essential clang cmake ninja-build pkg-config ccache git ca-certificates python3 \
        unixodbc-dev tdsodbc \
        mingw-w64 g++-mingw-w64-x86-64-posix gcc-mingw-w64-x86-64-posix wine wine64 \
 && rm -rf /var/lib/apt/lists/* \
 # MinGW: winpthreads (std::thread, std::mutex), as in T18; Ubuntu defaults to the win32 thread model
 && update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix \
 && update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

# CMake reads these when a build directory is configured: every preset goes through ccache.
ENV CMAKE_C_COMPILER_LAUNCHER=ccache \
    CMAKE_CXX_COMPILER_LAUNCHER=ccache \
    WINEDEBUG=-all
