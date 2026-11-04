# Build environment of the project (decision T24, docs/tools/build-environment.md): the same image runs the
# local builds (docker/build.sh) and the CI (.github/workflows/ci.yml). Linux only: Windows is built with
# clang-cl on a Windows CI runner (T26).
# Ubuntu LTS: Clang (shipped) and GCC (check), CMake, Ninja, unixODBC/FreeTDS, Python.
# No LuaJIT package: Ubuntu ships the OpenResty fork, so CMake builds the pinned upstream commit (T21).
FROM ubuntu:26.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
       build-essential nano sudo \
       clang clangd clang-format cmake ninja-build pkg-config ccache git ca-certificates python3 gdb shellcheck \
       unixodbc-dev tdsodbc \
 && rm -rf /var/lib/apt/lists/* \
 && mkdir -p /home/ubuntu/.cache/x2-build/ccache \
 && chown -R ubuntu:ubuntu /home/ubuntu/.cache \
 && echo 'ubuntu ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/ubuntu \
 && chmod 0440 /etc/sudoers.d/ubuntu

# CMake reads these when a build directory is configured: every preset goes through ccache.
ENV CMAKE_C_COMPILER_LAUNCHER=ccache \
    CMAKE_CXX_COMPILER_LAUNCHER=ccache \
    CCACHE_DIR=/home/ubuntu/.cache/x2-build/ccache
