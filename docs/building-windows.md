[Home](README.md) › Build on Windows

# Build on Windows

Windows builds use **clang-cl** (Clang in MSVC mode) with Visual Studio's headers, standard library, and linker (decision T26).
Builds target x64 only. As of 2026-10-31, builds are warning-free and tests pass, but **the servers have only been run on Linux**.

## 1. Install

| What | Purpose |
|---|---|
| **Visual Studio 2026** (Community) or **Build Tools for Visual Studio 2026** | compiler, CMake, Ninja |
| **Git** | download code |
| **Python 3** (optional) | generate `ClientScript` from the client (see step 4) |

In the Visual Studio Installer, select:

- **Desktop development with C++** workload;
- under *Individual components*: **C++ Clang Compiler for Windows** and **C++ CMake tools for Windows**;
- a **Windows 11 SDK** (included with workload).

To install the same components from the command line, use the Build Tools installer (`vs_BuildTools.exe`, available from Microsoft):

```bat
vs_BuildTools.exe --passive --wait ^
  --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended ^
  --add Microsoft.VisualStudio.Component.VC.Llvm.Clang ^
  --add Microsoft.VisualStudio.Component.VC.CMake.Project
```

**Note:** if you have multiple Visual Studio installations, open the prompt **for the installation with these components**.
An installation without C++ reports `fatal error: 'cstdio' file not found` even for standard headers.

## 2. Get the code

```bat
git clone https://github.com/stdmaru/X2ServerRework.git
cd X2ServerRework
```

The original client (`_client\`) and database backups (`_database\`) are not included ([Run the servers](running.md)).

## 3. Build and test

Open **x64 Native Tools Command Prompt for VS 2026** (or *Developer Command Prompt* with `-arch=amd64`).
Visual Studio's clang-cl is not in `PATH` by default, so add its directory:

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
- To switch prompts or Visual Studio installations, remove `build\<preset>` and configure again.

## 4. Output

- Executables in `build\<preset>\src\server\<Server>\`.
- Assembled folders in `build\<preset>\run\<Server>\`, as on Linux ([Run the servers](running.md)). Differences on Windows:
  - `ClientScript` is generated only if Python 3 and `luajit.exe` are available in `PATH`. The Windows build compiles the LuaJIT
    library, not the interpreter. If either requirement is missing, the build emits a warning and continues. You can also generate
    `ClientScript` on Linux and copy it over.
  - Configs use **Microsoft ODBC Driver 18 for SQL Server**, which must be installed for servers to access the database.

## Build from another machine (optional)

Developers on Linux can build and run tests on a Windows machine over SSH without leaving Linux:

```bash
X2_WIN_HOST=user@host tools/win/winbuild.sh
```

See [Build environment](tools/build-environment.md#windows-clang-cl-over-ssh) for machine requirements and setup details.
