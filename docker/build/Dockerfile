# Build environment of the project (decision T24, docs/tools/build-environment.md): the same image runs the
# local builds (docker/build.sh) and the CI (.github/workflows/ci.yml). Linux only: Windows is built with
# clang-cl on a Windows CI runner (T26).
# Ubuntu LTS: Clang (shipped) and GCC (check), CMake, Ninja, unixODBC/FreeTDS, the headers SDL3 is built
# against (it is linked statically: tools, T32), Python.
# No LuaJIT package: Ubuntu ships the OpenResty fork, so CMake builds the pinned upstream commit (T21).
# Shader compiler (decision T39): SDL_shadercross on Microsoft's prebuilt DXC, for tools/shaders. Shaders are compiled
# offline and their outputs committed, so builds never need it; DXC is only published for x86_64, so other
# architectures get no shadercross.
FROM ubuntu:26.04 AS shadercross
ARG DEBIAN_FRONTEND=noninteractive
ARG SHADERCROSS_COMMIT=1ff05bec573988a98ef9e0260b4da44f512b8367
ARG DXC_URL=https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2609/linux_dxc_2026_09_28.x86_x64.tar.gz
ARG DXC_SHA256=96faadc7f5c282d2ffda49804beb4c3ee38127bc252b723234e3c5cdf7aa39a1
RUN mkdir -p /opt/shadercross \
 && if [ "$(uname -m)" = x86_64 ]; then \
      apt-get update \
      && apt-get install -y --no-install-recommends build-essential cmake ninja-build git ca-certificates curl \
           libsdl3-dev libspirv-cross-c-shared-dev \
      && git clone -q https://github.com/libsdl-org/SDL_shadercross /src && git -C /src checkout -q "$SHADERCROSS_COMMIT" \
      && mkdir -p /src/external/DirectXShaderCompiler-binaries \
      && curl -fsSL -o /tmp/dxc.tgz "$DXC_URL" && echo "$DXC_SHA256  /tmp/dxc.tgz" | sha256sum -c - \
      && tar xzf /tmp/dxc.tgz -C /src/external/DirectXShaderCompiler-binaries \
      && cmake -S /src -B /build -G Ninja -DCMAKE_BUILD_TYPE=Release -DSDLSHADERCROSS_VENDORED=OFF \
           -DSDLSHADERCROSS_INSTALL=OFF -DSDLSHADERCROSS_SHARED=ON -DSDLSHADERCROSS_STATIC=OFF \
           -DCMAKE_BUILD_RPATH='$ORIGIN' \
      && cmake --build /build \
      && cp /build/shadercross /build/libSDL3_shadercross.so* /src/external/DirectXShaderCompiler-binaries/lib/*.so \
           /opt/shadercross/; \
    fi

FROM ubuntu:26.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
       build-essential nano sudo \
       clang clangd clang-format cmake ninja-build pkg-config ccache git ca-certificates python3 gdb shellcheck \
       unixodbc-dev tdsodbc \
       libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
       libxkbcommon-dev libwayland-dev libdecor-0-dev libdrm-dev libgbm-dev libegl-dev \
       libdbus-1-dev libibus-1.0-dev libudev-dev libasound2-dev libpulse-dev libpipewire-0.3-dev \
       libsdl3-0 libspirv-cross-c-shared0 \
 && rm -rf /var/lib/apt/lists/* \
 && mkdir -p /home/ubuntu/.cache/x2-build/ccache \
 && chown -R ubuntu:ubuntu /home/ubuntu/.cache \
 && echo 'ubuntu ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/ubuntu \
 && chmod 0440 /etc/sudoers.d/ubuntu

COPY --from=shadercross /opt/shadercross /opt/shadercross
ENV PATH=/opt/shadercross:$PATH

# CMake reads these when a build directory is configured: every preset goes through ccache.
ENV CMAKE_C_COMPILER_LAUNCHER=ccache \
    CMAKE_CXX_COMPILER_LAUNCHER=ccache \
    CCACHE_DIR=/home/ubuntu/.cache/x2-build/ccache
