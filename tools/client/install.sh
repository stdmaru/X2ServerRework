#!/usr/bin/env bash
# Assembles a runnable client folder (docs/tools/client-folder.md):
#   tools/client/install.sh <client folder> [preset]      default preset: linux-debug
# Copies the built x2 (x2.exe on Windows builds) into the folder and, when missing there, what the client reads from
# disk outside the archives (general.ess, movie/, music/) from X2_CLIENT_DATA (default _client/data). The .kom archives and the client_data
# (data/, loose files for X2_DEV_TOOLS builds) are the user's: packed with x2_assets, never touched here.
# On Linux it also installs the launcher entry x2.desktop and the game's icon under $XDG_DATA_HOME (~/.local/share), so
# taskbars show the icon for the window's app_id "x2" (Wayland; DXUTInit sets it).
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

case "$exe" in *.exe) exit 0 ;; esac

data_home="${XDG_DATA_HOME:-$HOME/.local/share}"
folder="$(cd "$dest" && pwd)"
icon_dir="$data_home/icons/hicolor/256x256/apps"
mkdir -p "$icon_dir" "$data_home/applications"
# The .ico's 256x256 entry is a PNG: copy its bytes out.
python3 - "$root/src/client/X2/Elsword.ico" "$icon_dir/x2.png" <<'PY'
import struct, sys
ico = open(sys.argv[1], "rb").read()
best = None
for i in range(struct.unpack_from("<H", ico, 4)[0]):
    w, h, _, _, _, bits, size, offset = struct.unpack_from("<BBBBHHII", ico, 6 + 16 * i)
    key = ((w or 256) * (h or 256), bits, size)  # Elsword.ico has a black 24-bit 256x256 entry before the 32-bit one
    if ico[offset:offset + 8] == b"\x89PNG\r\n\x1a\n" and (best is None or key > best[0]):
        best = (key, ico[offset:offset + size])
if best is None:
    sys.exit("no PNG image in " + sys.argv[1])
open(sys.argv[2], "wb").write(best[1])
PY
cat > "$data_home/applications/x2.desktop" <<DESKTOP
[Desktop Entry]
Type=Application
Name=Elsword
Exec="$folder/x2"
Path=$folder
Icon=x2
StartupWMClass=x2
Categories=Game;
DESKTOP
command -v update-desktop-database > /dev/null && update-desktop-database -q "$data_home/applications" 2> /dev/null || true
echo "installed $data_home/applications/x2.desktop and the icon x2"
