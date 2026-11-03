# Build environment of the project (decision T24, docs/tools/build-environment.md): the same image runs the
# local builds (docker/build.sh) and the CI (.github/workflows/ci.yml). Linux only: Windows is built with
# clang-cl on a Windows CI runner (T26).
# Ubuntu LTS: Clang (shipped) and GCC (check), CMake, Ninja, unixODBC/FreeTDS, Python.
# No LuaJIT package: Ubuntu ships the OpenResty fork, so CMake builds the pinned upstream commit (T21).
FROM ubuntu:26.04

ARG DEBIAN_FRONTEND=noninteractive
ARG DEV_USER=vscode
ARG DEV_UID=1000
ARG DEV_GID=1000
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        build-essential clang cmake ninja-build pkg-config ccache git ca-certificates python3 \
        unixodbc-dev tdsodbc \
 && rm -rf /var/lib/apt/lists/* \
 && groupadd --gid "$DEV_GID" "$DEV_USER" \
 && useradd --uid "$DEV_UID" --gid "$DEV_GID" --create-home --shell /bin/bash "$DEV_USER" \
 && mkdir -p "/home/$DEV_USER/.cache/x2-build/ccache" \
 && chown -R "$DEV_UID:$DEV_GID" "/home/$DEV_USER/.cache"

# CMake reads these when a build directory is configured: every preset goes through ccache.
ENV CMAKE_C_COMPILER_LAUNCHER=ccache \
    CMAKE_CXX_COMPILER_LAUNCHER=ccache \
    CCACHE_DIR=/home/vscode/.cache/x2-build/ccache
