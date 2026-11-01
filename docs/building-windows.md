[Home](README.md) › Build on Windows

# Build on Windows

Windows builds use **clang-cl** (Clang in MSVC mode), with Visual Studio headers, STL, and linker (decision T26). x64 only.
As of 2026-10-31: builds without warnings and tests pass, but **servers have not yet been tested running on Windows**, only Linux.

## 1. Install

| What | Purpose |
|---|---|
| **Visual Studio 2026** (Community) or **Build Tools for Visual Studio 2026** | compiler, CMake, Ninja |
| **Git** | download code |
| **Python 3** (optional) | generate `ClientScript` from client (see step 4) |

In Visual Studio installer, select:

- **Desktop development with C++** workload;
- under *Individual components*: **C++ Clang Compiler for Windows** and **C++ CMake tools for Windows**;
- a **Windows 11 SDK** (included with workload).

Same from command line using Build Tools installer (`vs_BuildTools.exe`, downloaded from Microsoft):

```bat
vs_BuildTools.exe --passive --wait ^
  --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended ^
  --add Microsoft.VisualStudio.Component.VC.Llvm.Clang ^
  --add Microsoft.VisualStudio.Component.VC.CMake.Project
```

**Note:** if multiple Visual Studio installations exist, open prompt **for the one with these components**.
An installation without C++ reports `fatal error: 'cstdio' file not found` even for standard headers.

## 2. Get the code

```bat
git clone https://github.com/stdmaru/X2ServerRework.git
cd X2ServerRework
```

Original client (`_client\`) and DB backups (`_database\`) are not included ([Run the servers](running.md)).

## 3. Build and test

Open **x64 Native Tools Command Prompt for VS 2026** (or *Developer Command Prompt* with `-arch=amd64`).
Visual Studio clang-cl is not in `PATH`, so add it first:

```bat
set PATH=%VSINSTALLDIR%VC\Tools\Llvm\x64\bin;%PATH%
cmake --preset windows-debug
cmake --build --preset windows-debug
ctest --preset windows-debug
```

| Preset | Type | Dev tools (`X2_DEV_TOOLS`) |
|---|---|---|
| `windows-debug` | Debug | yes |
| `windows-development` | RelWithDebInfo | yes |
| `windows-release` | Release | no |

- Dependencies (zlib, LuaJIT, Asio, doctest) are downloaded by CMake on first configure, which requires internet.
  LuaJIT is built using its `msvcbuild.bat` (T25).
- Executables use original server icons.
- To switch prompt or Visual Studio installation, remove `build\<preset>` and configure again.

## 4. Output

- Executables in `build\<preset>\src\server\<Server>\`.
- Assembled folders in `build\<preset>\run\<Server>\`, as on Linux ([Run the servers](running.md)). Differences on Windows:
  - `ClientScript` is generated only with Python 3 and a `luajit.exe` in `PATH`. Windows build does not compile interpreter,
    only library. Without them, build warns and continues. It can also be generated on Linux and copied.
  - Configs use **Microsoft ODBC Driver 18 for SQL Server**, which must be installed for servers to access database.

## Build from another machine (optional)

Developers on Linux can build and test on Windows machine over SSH without leaving Linux:

```bash
X2_WIN_HOST=user@host tools/win/winbuild.sh
```

Machine requirements and details: [Build environment](tools/build-environment.md#windows-clang-cl-over-ssh).
