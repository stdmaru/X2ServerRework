#!/usr/bin/env bash
# Builds and tests the Windows (clang-cl, T26) presets on a Windows machine over SSH (docs/ferramentas/ambiente-build.md).
#   X2_WIN_HOST=user@host tools/win/winbuild.sh [preset] [jobs]      default: windows-debug, every core of the machine
# Sends the tracked files with their working-tree contents (uncommitted edits included) to $X2_WIN_DIR/src, removes
# there the ones git deleted, and runs
# cmake/ctest inside the newest Visual Studio found by vswhere that has the C++ and Clang components. The machine needs Visual Studio Build Tools
# (C++ workload, Clang and CMake components), Python 3 and an OpenSSH server with key login; no Git.
set -euo pipefail

# X2_WIN_HOST and X2_WIN_DIR may also come from docker/.env
# shellcheck source=../../docker/env.sh
source "$(dirname "${BASH_SOURCE[0]}")/../../docker/env.sh"
x2_load_env "$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

host=${X2_WIN_HOST:?set X2_WIN_HOST=user@host}
dir=${X2_WIN_DIR:-C:/x2}
preset=${1:-windows-debug}
jobs=${2:-%NUMBER_OF_PROCESSORS%}
repo=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
ssh_opts=(-o BatchMode=yes -o LogLevel=ERROR)
win=${dir//\//\\}

ssh "${ssh_opts[@]}" "$host" "if not exist $win\\src mkdir $win\\src"
git -C "$repo" ls-files -z | tar -C "$repo" --null -T - -cf - | ssh "${ssh_opts[@]}" "$host" "tar -xf - -C $dir/src"
# tar only adds: files deleted in the history and no longer tracked are removed there too (a CMake glob would still
# compile them). Files git never had (the machine's own) are left alone. Windows ignores case, so a deleted path that
# matches a tracked one in another case (soapClient.cpp / SoapClient.cpp) is the tracked file and stays.
awk 'NR == FNR { tracked[tolower($0)] = 1; next } $0 != "" && !(tolower($0) in tracked) && !seen[$0]++' \
    <(git -C "$repo" ls-files) <(git -C "$repo" log --diff-filter=D --name-only --pretty=format: HEAD) |
    ssh "${ssh_opts[@]}" "$host" "python -c \"import os,sys; [os.remove(p) for p in (os.path.join(r'$win\\src', l.strip()) for l in sys.stdin if l.strip()) if os.path.isfile(p)]\""

script=$(mktemp)
trap 'rm -f "$script"' EXIT
cat > "$script" <<CMD
@echo off
for /f "usebackq delims=" %%i in (\`"%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 Microsoft.VisualStudio.Component.VC.Llvm.Clang -property installationPath\`) do set VSDIR=%%i
call "%VSDIR%\\Common7\\Tools\\VsDevCmd.bat" -arch=amd64 -host_arch=amd64 -no_logo || exit /b 1
set PATH=%VSDIR%\\VC\\Tools\\Llvm\\x64\\bin;%PATH%
cd /d $win\\src
if not exist build\\$preset\\CMakeCache.txt ( cmake --preset $preset || exit /b 1 )
cmake --build build\\$preset -j $jobs -- -k 0 > $win\\build.log 2>&1
set BUILD=%ERRORLEVEL%
findstr /r /c:"warning" /c:"error" $win\\build.log | findstr /v /c:"D9025"
echo build exit: %BUILD%
if %BUILD% neq 0 exit /b %BUILD%
ctest --test-dir build\\$preset --output-on-failure
CMD
scp -q "${ssh_opts[@]}" "$script" "$host:$dir/winbuild.cmd"
ssh "${ssh_opts[@]}" "$host" "$win\\winbuild.cmd"
