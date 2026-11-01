[Home](../README.md) › [Tools](README.md) › Build environment

# Build environment (Ubuntu in Docker)

Since 2026-10-30, Linux builds and tests run in a custom **Ubuntu** image (decision T24), shared by development and
[CI](ci.md). The host (any distribution with Docker) only mounts the repository. Windows (clang-cl, T26) is not built here:
only in the Windows CI job.

- Image: `docker/build/Dockerfile`, based on `ubuntu:26.04` (26.04.1 LTS). As of 2026-10-30: Clang 21.1 (`linux-*` presets), GCC 15.2
  (`linux-gcc` preset, verification), CMake 4.2, Ninja 1.13, unixODBC 2.3 + FreeTDS (`tdsodbc`), Python 3, ccache.
- No LuaJIT package: Ubuntu's is the OpenResty fork, so CMake also builds a pinned upstream commit (T21) on Linux.
- Every preset goes through `ccache` (`CMAKE_<LANG>_COMPILER_LAUNCHER` in image).

## Usage

`docker/build.sh` runs a command inside container (without arguments, opens a shell). Builds image the first time
(and when `Dockerfile` changes; otherwise Docker cache makes it immediate).

```bash
docker/build.sh cmake --preset linux-debug
docker/build.sh cmake --build --preset linux-debug
docker/build.sh ctest --preset linux-debug
docker/build.sh cmake --preset linux-gcc && docker/build.sh cmake --build --preset linux-gcc   # GCC check
```

- Repository is mounted **at the same path** as host: `build/` and `compile_commands.json` are available to host editor.
- Runs as host user (files under `build/` are not root-owned). Container `HOME`, with ccache, is `~/.cache/x2-build` (`X2_BUILD_HOME` changes it).
- Passes host and `docker/.env` `X2_*` variables into container (shell variable wins; template at `docker/.env.example`),
  e.g. DB password for CMake ([Run directories](runtime.md#template-values-cmake-cache)). `winbuild.sh` also reads `docker/.env`
  (`X2_WIN_HOST`, `X2_WIN_DIR`).
- Host networking (`--network host`): `db` test suite and servers reach [development DB](development-database.md) at `127.0.0.1`;
  original client reaches servers as before.
- stdin is always connected (`-i`, and `-it` in a terminal): without terminal, server Lua prompt exits at EOF, so a server started here
  stays up while stdin remains open (e.g. `(sleep 600) | docker/build.sh ...`) or with `--no-stdin`.
- A `build/` directory configured outside container (another compiler) should be deleted before use: Ninja does not rebuild objects only because compiler version changed.

## Windows (clang-cl) over SSH

Windows does not run in the image. Besides [CI](ci.md), there is a **Windows test machine** (user VM, since 2026-10-30)
where `tools/win/winbuild.sh` builds and tests without spending Actions minutes:

```bash
X2_WIN_HOST=user@host tools/win/winbuild.sh                        # windows-debug, all machine cores
X2_WIN_HOST=user@host tools/win/winbuild.sh windows-release 4
```

- Sends tracked files **including uncommitted edits** (`git ls-files` + `tar` over SSH) to `C:/x2/src` (`X2_WIN_DIR` changes it)
  and runs `cmake`/`ctest` in newest Visual Studio environment with C++ and Clang components (`vswhere -requires`: on 2026-10-31
  VM gained Visual Studio Community without C++, and `-latest` alone began selecting it). `build/` stays on machine and is incremental.
- There, deletes files removed from git history and no longer tracked. `tar` only adds files, and CMake glob kept compiling
  `stdsoap2.cpp` after gSOAP was removed (2026-10-31). Comparison is case-insensitive, like Windows: removed `soapClient.cpp` and new
  `SoapClient.cpp` are the same file there. Files never tracked in git are untouched.
- Machine needs Visual Studio Build Tools (C++ workload, Clang and CMake components, which bring CMake and Ninja), Python 3,
  and OpenSSH with key login; no Git required. As of 2026-10-30: Windows Server 2022, Build Tools 18.11.3 (MSVC 14.51), 12 cores, 8 GB.
  VM is on dedicated Proxmox server: default is all cores (`%NUMBER_OF_PROCESSORS%`, 2026-10-31).
- Only for build and tests.

## What changed when moving from Arch to Ubuntu (2026-10-30)

| Finding | Handling |
|---|---|
| Ubuntu MinGW: `win32` threads by default and "file too big" (COFF section limit) in packets | fixed with `-posix` and `-Wa,-mbig-obj`; MinGW later removed (T26) |
| GCC 15 Release: false `-Wmaybe-uninitialized` for `KTCInfo::m_wstrUDPRelayIP` in round-trip tests | pragma only around `CheckRoundTrip` (`src/server/Common/tests/packet_test.h`); pragma at call site does not work |
| LuaJIT package is OpenResty fork | not installed; use pinned-commit recipe (T21) |
