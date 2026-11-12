#!/usr/bin/env bash
# Assembles a runnable client folder (docs/tools/client-folder.md):
#   tools/client/install.sh <client folder> [preset]      default preset: linux-debug
# Copies the built x2 (x2.exe on Windows builds) into the folder and, when missing there, what the client reads from
# disk outside the archives (general.ess, movie/, music/) from X2_CLIENT_DATA (default _client/data). The .kom archives and the client_data
# (data/, loose files for X2_DEV_TOOLS builds) are the user's: packed with x2_assets, never touched here.
set -euo pipefail

root="$(cd "$(dirname "$0")/../.." && pwd)"
dest="${1:?usage: $0 <client folder> [preset]}"
preset="${2:-linux-debug}"
source_data="${X2_CLIENT_DATA:-$root/_client/data}"
case "$source_data" in /*) ;; *) source_data="$root/$source_data" ;; esac

exe="$root/build/$preset/src/client/x2"
[ -f "$exe" ] || exe="$exe.exe"
if [ ! -f "$exe" ]; then
    echo "no client executable in build/$preset/src/client (build the x2_client target first)" >&2
    exit 1
fi
if ! compgen -G "$dest/data*.kom" > /dev/null; then
    echo "warning: no data*.kom in $dest (x2_assets pack <client_data> $dest)" >&2
fi

mkdir -p "$dest"
cp -f "$exe" "$dest/"
echo "installed $(basename "$exe") ($preset)"

if [ ! -f "$dest/general.ess" ]; then
    if [ -f "$source_data/general.ess" ]; then
        cp --preserve=timestamps "$source_data/general.ess" "$dest/" && chmod a-x "$dest/general.ess"
        echo "copied general.ess (string table) from $source_data"
    else
        echo "warning: no general.ess in $source_data" >&2
    fi
fi

for folder in movie music; do
    if [ -d "$dest/$folder" ]; then
        continue
    fi
    if [ -d "$source_data/$folder" ]; then
        cp -r --preserve=timestamps "$source_data/$folder" "$dest/"
        chmod -R a-x+X "$dest/$folder"
        echo "copied $folder/ from $source_data"
    else
        echo "warning: no $folder/ in $source_data" >&2
    fi
done
