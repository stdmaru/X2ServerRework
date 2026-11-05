[Home](../README.md) › [Project](README.md) › Roadmap

# Roadmap

The phases are ordered to produce something **testable** as early as possible. The key is that the original client
(`_client/elsword.exe`, PE32 x86) already exists and works — so the ported server can be validated against it
**before** we touch the client. This requires preserving the network protocol exactly
([Network protocol and serialization](../original/protocol.md)).

```text
Phase 0  New repo + automatic importer           (clean copy from _reference; no logic changes)
Phase 1  Libraries: replace third party, port KNCSDK
Phase 2  Server  → Linux/Windows, tested with original client
Phase 3  Client  → platform layer (SDL) + graphics abstraction
Phase 4  Client  → new graphics backend (Vulkan / SDL_GPU)
Phase 5  Deep cleanup / reorganization
Phase 6  Server  → compile-time compatibility with selected historical clients
```

## Phase 0 — New repo + importer

`_reference` is **read-only**. New code starts in `src/`, but retained source files are **not copied manually**: the importer
(`tools/import/`) reads `_reference` and writes normalized files to `src/`. See [decision T9](decisions.md#accepted).

Importer steps:

1. **File selection**: read `.vcxproj` files in `Release_US|Win32` and select only sources that actually compile
   (respecting `ExcludedFromBuild`) + headers they include. Files not in the build are not imported.
2. **Encoding**: CP949 → UTF-8, LF line endings. See [Original code encoding and conventions](../original/encoding.md).
3. **Freeze feature flags** with `coan` using the final US/`_SERVICE_` macro set
   (~1,175 `SERV_*` flags, ~58,000 `#if*` lines). See [Feature flags and variants](../original/feature-flags.md).
   **Exception:** whitelisted debug/dev macros and direct environment checks (`#ifndef _SERVICE_`, `_IN_HOUSE_`)
   become `#if X2_DEV_TOOLS` and are included in the report for triage. See [Debug, development, and administration tools](../original/debug-and-development.md).
4. **Exclusions**: explicit list of folders/files not to bring in (other countries, anti-cheat, SOAP, proprietary binaries, DXUT...).
5. **Destination map**: path in `_reference` → path in `src/` (where folder reorganization happens).
6. Report imported/discarded files and resolved flags.

Rules:
- Fixes that require a human (port IOCP, replace types, etc.) happen **after** import, in regular commits to `src/`.
- The importer runs **per module** (first KNCSDK, then `Common`, then each server), as required by each phase — we do not import everything at once.
- Re-running the importer over a manually edited module is not allowed; if we discover a flag error later, fix it manually in `src/`.
  (Before the first manual edit to a module, re-running is fine.)
- Unifying duplicates (`KNCSDK_Server`×`KNCSDK_Client`, the two `Socket` folders, the 3 copies of `lua_tinker`) happens in the destination map:
  choose one source and manually review differences from the other.

Tasks:
- [x] Repository layout (`src/`, `cmake/`, `tools/import/`), root CMake, Linux/Windows x64 presets, `.gitattributes`, `.editorconfig`.
- [x] Importer ([Importer](../tools/importer.md)): vcxproj selection, encoding, custom `#if` resolver
      (instead of `coan`, because of the `X2_DEV_TOOLS` rule), exclusions, destination map, include fixes, report, overwrite protection.
- [x] `kncsdk` module imported: 62 of 742 files ([report](../../tools/import/reports/kncsdk/REPORT.md)).
- [x] **Server** flags generated from `ServerDefine.h` (736 true / 423 false): [Flags and verification](../tools/flags.md).
- [x] Resolver validated at scale by differential check: 1,415/1,423 server files equivalent (the remaining 8 are configuration headers).
- [x] Importer checks each file's integrity against git (the HDD copy had 90 corrupted files; see [Inventory](../original/inventory.md#copy-integrity)).
- [ ] **Client** flags (`KTDX.h`/`Always.h`) and conflicts between server and client in shared code (`Common`): handle these during the client import.
- [x] `server-common` imported: 414 files ([Server: Common](../porting/server-common.md)).
- [x] `server-x2lib` imported: `X2Lib/X2UDPPacket.*` (client UDP structs compiled by the server) into `src/server/X2Lib` (2026-10-29).
- [x] `server-channel` imported: 36 files ([Server: ChannelServer](../porting/server-channel.md), 2026-10-30).
- [x] `server-game` imported: 298 files ([Server: GameServer](../porting/server-game.md), 2026-10-30).
- [x] `server-login` imported: 86 files ([Server: LoginServer](../porting/server-login.md), 2026-10-31).
- [x] `server-center` imported: 123 files ([Server: CenterServer](../porting/server-center.md), 2026-10-31).
- [x] GlobalServer imported (2026-10-31).

## Phase 1 — Libraries

Most of the work is not "porting"; it is **replacing with upstream versions** through CMake. KNCSDK is the only custom library.
Full table: [Libraries](../original/libraries.md).

Status on 2026-10-29: platform and KNCSDK compiled on Linux (GCC and Clang) and MinGW-w64 (tested with Wine), with tests. Details in [Porting](../porting/README.md).

- [x] `x2::platform` layer (types, sync, time, `CTime`, `CString`, CRT, Unicode), with tests. See [Platform](../porting/platform.md).
- [x] Dependencies via CMake (T13): zlib, LuaJIT 2.1 (pkg-config or source at a pinned commit built by its Makefile: T21), doctest.
- [x] KNCSDK ported: logging, `Serializer` (fixed network format), `KncSecurity`, `Thread`, `ObjectPool`, `KncUtil`, merged with `KNCSDK_Client`. See [KNCSDK](../porting/kncsdk.md).
- [x] Server `lua_tinker` compiles with GCC/Clang on LuaJIT 2.1; luabind removed.
- [ ] **Validate protocol against real bytes** from the original client/server. On 2026-10-30, login to the channel worked with `x2_DEBUG.exe` and `x2.exe` (key exchange, encryption, `ECH_VERIFY_ACCOUNT`, lists); on 2026-10-31, the client reached town through GameServer and LoginServer (character selection, channel, town), then the field and a dungeon with CenterServer; with GlobalServer, all servers run together.
- [x] Compile and test Windows paths with **MinGW-w64 + Wine** (T18): platform and KNCSDK without warnings, tests passing (2026-10-29).
- [x] LuaJIT 2.1 on MinGW (T21): `kncsdk_lua` and server `Common` compile without warnings and tests pass in Wine (2026-10-30).
- [x] Crypto++ replaced by `x2::crypto` (DES-CBC, HMAC-MD5, MD5, SHA-256), with official test vectors (T27, 2026-10-30).
- [x] Unified build environment: Docker Ubuntu 26.04 image for local builds and CI (T24, `build/ubuntu-env`, 2026-10-30). See [Build environment](../tools/build-environment.md).
- [x] Compile and test on **Windows with clang-cl**, including LuaJIT with MSVC (T25/T26).
- [ ] Boost: KNCSDK no longer uses it, but the server still does in ~230 files. Reduce its use as server modules are ported.
- [ ] Intel TBB 2.2 → `std` (2 uses in GameServer).
- [ ] Upstream libxml2 and FreeType: used only by the client (Phase 3).

## Phase 2 — Server

Detailed `Common` plan: [Server: Common › Plan](../porting/server-common.md#manual-port-plan-one-branch-per-step).

See [Server — Windows dependencies](../original/server.md).

- [x] `server/common-build`: `Common` compiles on Linux (GCC/Clang) without warnings, except `Socket/`/`ODBC/` (2026-10-29). See [Server: Common](../porting/server-common.md).
- [x] `server/common-packets`: round-trip all 1,964 serialized types + exact bytes for the start of login (2026-10-29). Found 2 `GET`s that called `PUT` in the original.
- [x] `server/common-net`: TCP `Socket/` on Asio (T20), with `KSession` loopback test (2026-10-29).
- [x] `server/common-udp`: UDP `Socket/` (`KTDNUDP`, `UdpEcho`, `LBSUdpEcho`) without completion routines, with loopback test (2026-10-29).
- [x] `server/common-db`: portable ODBC (T17), tested against SQL Server 2022 in Docker with game databases via FreeTDS (2026-10-30): [ODBC database](../porting/server-common-db.md). Driver 18 still untested.
- [x] `server/channel-build`: ChannelServer compiles without warnings (GCC, Clang, MinGW), `main` without MFC (`KServerMain`), 2026-10-30. See [Server: ChannelServer](../porting/server-channel.md).
- [x] EPublisherServer: DEB AuthServer reimplementation (T23), tested with the servers' gSOAP client and against the database (2026-10-30). See [Server: EPublisherServer](../porting/server-epublisher.md).
- [x] `server/channel-run`: ChannelServer on Linux with original `elsword.exe` through the end of channel login (2026-10-30): login through EPublisherServer, server-group and channel lists received; the client then tries GameServer (port 9300), which does not exist yet, and shows "Server is full". `dist/` package generated by CMake (T22).
- [x] `server/game-build`: GameServer compiles without warnings (Clang, GCC, clang-cl), `main` without MFC, no anti-cheat or TBB (2026-10-30). See [Server: GameServer](../porting/server-game.md).
- [x] `server/game-run`: GameServer reaches the ready state on Linux while the original client is connected (2026-10-31): "server full" disappears, but login stops at `EGS_VERIFY_ACCOUNT_REQ` until LoginServer is available ([dependency survey](../porting/server-game.md#dependencies-on-other-servers-survey-from-2026-10-31)).
- [x] `server/login-import`: LoginServer imported (2026-10-31). See [Server: LoginServer](../porting/server-login.md).
- [x] `server/login-build`: LoginServer compiles without warnings (Clang, GCC, clang-cl), without Nexon (KR) or country-specific files (2026-10-31). See [Server: LoginServer](../porting/server-login.md).
- [x] `server/login-run`: LoginServer on Linux (`x2_run_login`), original client from character list through **town** (2026-10-31). Leaving for a field stops at `ERM_OPEN_BATTLE_FIELD_REQ`, which goes to CenterServer.
- [x] `server/login-arm64-alignment`: LoginServer SIGBUS diagnosed in `KUserList::RegUser`; packing that misaligned the mutex removed, regression test passes on Linux x86_64 Release (2026-11-01). ARM64 validation also passed: the server no longer crashes during authentication. See [Server: LoginServer](../porting/server-login.md).
- [x] `build/linux-arm64`: Linux ARM64 build and full test suite pass locally; CI includes a native ARM64 build/test job. See [Continuous integration](../tools/ci.md).
- [x] `server/center-import`: CenterServer imported (2026-10-31). See [Server: CenterServer](../porting/server-center.md).
- [x] `server/center-build`: CenterServer compiles without warnings (Clang, GCC, clang-cl), without KSMS (2026-10-31). See [Server: CenterServer](../porting/server-center.md).
- [x] `server/center-run`: CenterServer on Linux (`x2_run_center`), original client reaches a **field** and a **dungeon** (2026-10-31). See [Server: CenterServer](../porting/server-center.md).
- [x] `server/global-import`: GlobalServer imported (2026-10-31). See [Server: GlobalServer](../porting/server-global.md).
- [x] `server/global-build`: GlobalServer compiles without warnings (Clang, GCC, clang-cl), without KSMS (2026-10-31). See [Server: GlobalServer](../porting/server-global.md).
- [x] `server/global-run`: GlobalServer on Linux (`x2_run_global`), GameServer delivers `EGB_*`, and the original client was tested with all six servers (2026-10-31). See [Server: GlobalServer](../porting/server-global.md).
- [x] MFC UI (`Common/ui/ServerUIDlg`) → terminal on all platforms (T15): live log, Lua prompt with history, status in title, shutdown confirmation, service mode without stdin (2026-10-31, `server/terminal`, see [Server console](../porting/console.md)).
- [x] Logging (T16): structured metadata in `START_LOG`; file retains full original content, console is compact and colored with its own level (2026-10-31, `kncsdk/log-structured`, see [KNCSDK › Logging](../porting/kncsdk.md#logging-t16)).
- [x] Localize Korean server console log messages to English; leave code comments and non-log strings unchanged (2026-11-02).
- [x] Machine-readable output mode and local socket with `x2ctl attach` (T30), after terminal and logging (2026-10-31, `server/control`, see [Server console](../porting/console.md)).
- [ ] ServerManager (T29, T30, T32): SDL3 + ImGui tool to orchestrate and monitor servers locally and over SSH. Started on
      2026-11-04, before the client, so it builds the SDL3 foundation the client reuses (T32). Startup order: EPublisherServer →
      LoginServer, CenterServer, GlobalServer → GameServer → ChannelServer, each after the previous reports `ready` (the original
      `KX2ServerLauncher` waited for the `CenterServer_Ready` window before starting GameServer); stop in reverse. One branch per step:
  - [x] `tools/sdl3-imgui-base`: SDL3 and ImGui dependencies, `x2::app` (window, events, SDL_GPU) and `x2::app_imgui`, demo, headless
        tests; Clang, GCC, and clang-cl without warnings (2026-11-04). See [App foundation](../porting/app.md).
  - [x] `tools/servermanager-protocol`: full JSON parser (`x2::platform`), protocol v1 client (`hello`/`log`/`output`/`status`/`result`,
        `id` correlation), server state, log store with filter and search; tests with recorded lines and the real control socket
        (2026-11-04). See [ServerManager](../tools/servermanager.md).
  - [x] `tools/servermanager-local`: `x2::platform::Process` (child with pipes, Linux and Windows), `--machine` transport, attach to a
        local control socket (through `x2ctl attach`), orchestrator (profile with dependencies, ordered start waiting for `ready`,
        reverse stop, crash detection); tested with the six real servers (2026-11-05). Fixed on the way: a `--machine` server stopped
        through the protocol waited for the end of its stdin to exit.
  - [x] `server/control-history`: numbered `log`/`output` lines (`seq`), a bounded backlog in the console (10,000 lines / 4 MB)
        replayed on `{"history":true,"upto":n}` (additive to protocol v1; no new port); the session asks for it on attach and merges
        it with the live lines by `seq`; tested with a real EPublisherServer (2026-11-05). See [Server console](../porting/console.md#history).
  - [ ] `tools/servermanager-ssh`: `ssh host x2ctl attach <name>` transport (configurable command, BatchMode, ControlMaster), remote
        `x2ctl list` discovery and `x2ctl stop`.
  - [ ] `tools/servermanager-ui`: server list and status, log view (level/function filter, search), Lua prompt with history and
        results, start/stop all; GUI tested on the user's desktop.
- [x] Remove SOAP/gSOAP, publisher billing/auth, Mail (MIME/SMTP), CrashRpt; stub where game logic depends on them (2026-10-31). The DEB gSOAP client became custom SOAP/HTTP, with DEB login and billing unchanged (T31, `server/cleanup-gsoap`, see [Common › DEB client](../porting/server-common.md#deb-client-t31)). `KMailManager` was removed, and script error reports go to the log (`server/cleanup-mail`). CrashRpt, other-country publishers, and `Common/SOAP` were never imported into `src/`: the importer excludes them. Only their `NetError`s/event IDs remain as protocol, along with comments.
- [x] Milestone: login + town + dungeon with original `elsword.exe` against servers running on Linux (2026-10-31).

## Phase 3 — Client: platform

See [Client — Windows / Direct3D 9 dependencies](../original/client.md).

- [ ] Window/input/IME: Win32 + DirectInput → SDL3. For now, retain the `MsgProc(HWND, UINT, WPARAM, LPARAM)` model
      by translating SDL events into Win32-style messages (`X2Lib` uses `HWND` in ~272 files — mostly through the `MsgProc` signature).
- [ ] D3DX math (`D3DXVECTOR3` ~8,000 uses) → header-compatible shim over GLM or custom implementation.
- [ ] Audio: FMOD Ex / DirectSound → miniaudio/SDL3 audio (or current cross-platform FMOD Core).
- [ ] Video (Bandi), embedded browser, CxImage, Steam, anti-cheat → remove/replace.
- [ ] `.kom` (zlib + header "KOG GC TEAM MASSFILE V.0.3") — portable loader.

## Phase 4 — Client: graphics

- [ ] Introduce graphics abstraction within `KTDXLIB` (D3D9 is concentrated there: ~56 files use the device; X2Lib mostly uses math).
- [ ] Reimplement what currently comes from D3DX: `.x` loader (mesh + skin + animation), `.fx` effects (`x2.fx`), `.dds` textures, fonts, sprites.
- [ ] New backend: SDL_GPU ([decisions.md](decisions.md), T19).

## Phase 5 — Deep cleanup

Real refactoring (remove dead systems, reorganize folders and names), guided by tests/smoke tests
from the previous phases. Only after everything runs.

- [ ] Fix the [original bugs](../original/bugs.md) retained in the port, each with a decision about the new behavior.

## Phase 6 — Server: historical client compatibility

Goal: keep one codebase that can be built for one selected client/protocol revision at a time, starting with the current
2014 baseline and adding the selected 2015 revision. This does not require one running server binary to support both
protocols simultaneously. See [Client compatibility](client-compatibility.md) for the preliminary comparison and open
investigation.

- [ ] Identify and normalize the exact 2014 and 2015 US / `_SERVICE_` source configurations; separate version changes
  from region, publisher, debug, and unrelated feature-flag differences.
- [ ] Inventory protocol differences, including handshake, packet IDs, layouts, serialization, and client version
  validation.
- [ ] Trace the new character and related features across shared code, servers, scripts/assets, and database schema.
- [ ] Define a CMake-selected protocol revision and version-specific runtime data layout.
- [ ] Port selected 2015 changes into the current codebase while preserving the 2014 build.
- [ ] Build both configurations and validate each against its matching client.
