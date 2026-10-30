#!/usr/bin/env bash
# Runs a command in the Ubuntu build container (decision T24, docs/ferramentas/ambiente-build.md).
#   docker/build.sh cmake --preset linux-debug
#   docker/build.sh cmake --build --preset linux-debug
#   docker/build.sh ctest --preset linux-debug
#   docker/build.sh                                   interactive shell
# The repository is mounted at the same path, so build/ and compile_commands.json match the host.
# Runs as the calling user; the ccache cache lives in ${X2_BUILD_HOME:-~/.cache/x2-build}.
# Host network: tests and servers reach the SQL Server container on 127.0.0.1.
set -euo pipefail

repo=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
image=${X2_BUILD_IMAGE:-x2-build}
home=${X2_BUILD_HOME:-${XDG_CACHE_HOME:-$HOME/.cache}/x2-build}
mkdir -p "$home"

# Docker's layer cache makes this a no-op when the Dockerfile has not changed.
docker build -q -t "$image" "$repo/docker/build" >/dev/null

tty=()
[ -t 0 ] && [ -t 1 ] && tty=(-it)
[ $# -eq 0 ] && set -- bash

exec docker run --rm "${tty[@]}" --network host \
    --user "$(id -u):$(id -g)" -e HOME=/x2home -e CCACHE_DIR=/x2home/ccache \
    -v "$home:/x2home" -v "$repo:$repo" -w "$PWD" \
    "$image" "$@"
