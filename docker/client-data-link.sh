#!/usr/bin/env bash
# Runs on the host before the Dev Container is created (initializeCommand in .devcontainer/devcontainer.json).
# Points .devcontainer/.client_data (not versioned) at X2_CLIENT_DATA_MOUNT from docker/.env or the environment, or at
# an empty folder when it is unset. The container bind-mounts that link (Docker resolves it on the host) read-only at
# /x2/client_data, as docker/build.sh does. See docs/tools/build-environment.md.
set -euo pipefail

repo=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
# shellcheck source=env.sh
source "$repo/docker/env.sh"
x2_load_env "$repo"

link="$repo/.devcontainer/.client_data"
empty="$repo/.devcontainer/.client_data_empty"
target=${X2_CLIENT_DATA_MOUNT:-}
if [ -n "$target" ] && [ ! -d "$target" ]; then
    echo "warning: X2_CLIENT_DATA_MOUNT=$target is not a folder; /x2/client_data will be empty" >&2
    target=
fi
if [ -z "$target" ]; then
    mkdir -p "$empty"
    target=$empty
fi
ln -sfn "$target" "$link"
