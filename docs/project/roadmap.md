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
Phase 4  Client  → graphics boundary x2::render + SDL_GPU backend (T36)
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
- [x] **Client** flags generated from `ServerDefine.h` + `KTDX.h` (1,719 true / 890 false; 973/978 files equivalent), and server/client conflicts in shared code listed (two, neither on the wire, 2026-11-05): [Flags and verification](../tools/flags.md#client-results-2026-11-05). Client and server share one `Common` (T33).
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
- [x] ServerManager (T29, T30, T32): SDL3 + ImGui tool to orchestrate and monitor servers locally and over SSH. Started on
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
  - [x] `tools/servermanager-ssh`: profile `hosts` (destination, remote `x2ctl`, ssh command; default with BatchMode and
        ControlMaster), remote servers attached and never started, `x2ctl list` discovery in the background, stop of a listed
        server through attach + protocol stop; tested with a fake ssh and with the real ssh to localhost (2026-11-05). See
        [ServerManager](../tools/servermanager.md#remote-servers-over-ssh).
  - [x] `tools/servermanager-ui`: server list and status, log view (level/function filter, search), Lua prompt with history and
        results, start/stop all, quit that stops what it started; Windows GUI subsystem and a Linux `.desktop`; drawn headless in
        tests; tested by the user on Arch (2026-11-05). See [ServerManager](../tools/servermanager.md#window).
  - [x] `tools/control-dir`: control sockets in another folder (servers in containers with a bind mount): `X2_CONTROL_DIR`,
        `x2ctl --dir=`, and `controlDir` in the profile and per host; plus the ssh `port` of a host (2026-11-05). See
        [Server console](../porting/console.md#control-socket).
  - [x] `tools/servermanager-remote-start`: start and stop remote servers through the host's commands run over ssh
        (`docker compose up -d`/`stop`, `systemctl`), then attach (`x2ctl attach --wait`); asked for by the user on 2026-11-05.
        See [ServerManager](../tools/servermanager.md#start-and-stop-commands).
  - [x] `tools/servermanager-windows-host`: a ServerManager on Linux attaches over ssh to servers started on Windows: control
        sockets in `%LOCALAPPDATA%\x2` (the same for RDP and ssh sessions), `cmd.exe` quoting for Windows hosts
        (`"shell": "cmd"`), and the `\` drawn as `₩` in the window fixed (Segoe UI first, Malgun Gothic merged); attach from Linux to
        a server in the VM's RDP session tested (2026-11-05). See
        [ServerManager](../tools/servermanager.md#windows-hosts-2026-11-05-toolsservermanager-windows-host).
  - [x] `tools/servermanager-profile-editor`: a Profile tab to edit the profile before connecting (hosts, servers, an ssh key per
        host with `identityFile`, file and folder pickers, live validation, Save / Save and apply / Revert), saved as
        `profile.json` next to the executable; drawn headless in tests, Clang, GCC and clang-cl without warnings (2026-11-05).
        See [ServerManager](../tools/servermanager.md#profile-tab-2026-11-05-toolsservermanager-profile-editor).
  - [x] `tools/servermanager-askpass`: ssh's password, passphrase and host key questions in a dialog of the window through
        `SSH_ASKPASS` (ServerManager itself in askpass mode, and `AskpassBroker`; `BatchMode` only without it), with secrets remembered until it closes
        and cancelled prompts not asked again until Refresh; Windows' OpenSSH 9.5 checked on the test VM; Clang, GCC and
        clang-cl without warnings (2026-11-05). See [ServerManager](../tools/servermanager.md#ssh-prompts-2026-11-05-toolsservermanager-askpass).
  - [x] `tools/servermanager-askpass-feedback`: the dialog follows the ssh that asked (its pid): "Connecting..." with inputs
        disabled, closes when ssh gets in, shows ssh's complaint after a wrong answer or when it gives up; tested by hand by the
        user (2026-11-05).
- [x] Remove SOAP/gSOAP, publisher billing/auth, Mail (MIME/SMTP), CrashRpt; stub where game logic depends on them (2026-10-31). The DEB gSOAP client became custom SOAP/HTTP, with DEB login and billing unchanged (T31, `server/cleanup-gsoap`, see [Common › DEB client](../porting/server-common.md#deb-client-t31)). `KMailManager` was removed, and script error reports go to the log (`server/cleanup-mail`). CrashRpt, other-country publishers, and `Common/SOAP` were never imported into `src/`: the importer excludes them. Only their `NetError`s/event IDs remain as protocol, along with comments.
- [x] Milestone: login + town + dungeon with original `elsword.exe` against servers running on Linux (2026-10-31).

## Phase 3 — Client: platform

See [Client — Windows / Direct3D 9 dependencies](../original/client.md).

- [x] `client/import`: importer modules `client-ktdxlib`, `client-x2lib`, `client-x2`, `client-serverprotocol` and
      `client-luacommon` with `client.generated.toml`: 854 files in `src/client`, not compiled yet; shared `Common` files and
      `X2UDPPacket.*` are not imported again (T33), and `X2UDPPacket.*` got the client's NPC sync layout; nothing was missing from
      `KNCSDK_Client` (2026-11-05). See [Client](../porting/client.md).
- [x] `client/skeleton`: client CMake targets (`X2_BUILD_CLIENT`), DXUT replaced by `src/client/DXUT` on `x2::app` (T32), SDL3
      events translated to Win32 messages for `MsgProc` (keyboard, mouse, focus, size, text/IME), `x2` skeleton executable with
      the original icon, `LuaCommon` on upstream LuaJIT (T21), excluded integrations removed by hand and the client modules frozen;
      headless tests (2026-11-05). See [Client: DXUT](../porting/client-dxut.md).
- [x] `client/serverprotocol`: `X2ServerProtocol` builds as `x2::client_serverprotocol` on the servers' network core
      `x2::common_net` (T34: one `Socket/` and event model for both ends, `KSession` client side, 64 KB buffers), with
      loopback tests (handshake, encryption accepted with the sequence check on, 40 KB packet, connection lost, TCP relay)
      (2026-11-05). See [Client: X2ServerProtocol](../porting/client-serverprotocol.md).
- [x] `client/input`: DirectInput (`CKTDIManager`, `CKTDIDevice`) → SDL3 keyboard, mouse and joystick state with the
      original API, as `x2::client_ktdi`; `DIK_*` ↔ SDL scancode table checked against the 102 `DIK_*` names the client
      uses; joysticks through SDL's gamepad API in DirectInput's XInput layout, with hot plug; headless tests with SDL
      virtual joysticks (2026-11-05). See [Client: input](../porting/client-input.md).
- [x] `client/ime-candidates`: the IME edit box's IMM32 (`ImmGetCompositionStringW`, `ImmGetCandidateListW`,
      `ImmNotifyIME`...) emulated on SDL3's IME state, the same on Windows and Linux; `TEXT_EDITING_CANDIDATES` →
      `WM_IME_NOTIFY`, with SDL's Windows page/selection quirks normalized; IMM types in `x2/platform/ime.h`; the
      `MsgProc(HWND, UINT, WPARAM, LPARAM)` model stays; headless tests (2026-11-05). See [Client: IME](../porting/client-ime.md).
- [x] `client/d3dx-math`: the custom D3DX math of `x2::platform` (`d3dx_math.h`, D3DX layouts and formulas checked against
      Wine) extended to the client's subset: `D3DXQUATERNION`, `D3DXPLANE`, animation keys, plane/quaternion functions,
      `Vec3Project`, `Vec3CatmullRom`, `PerspectiveFovLH`, `OrthoLH`, `Transformation2D`, `ComputeBoundingSphere`, with tests
      (2026-11-05). See [Platform: D3DX math](../porting/platform.md#d3dx-math).
- Audio moved to the end of the client work ([Phase 4](#phase-4--client-graphics); user, 2026-11-05).
- [x] `client/video-browser-cximage`: Bandi, the embedded browser and CxImage (Steam and anti-cheat removed on 2026-11-05).
      The character creation movies on pl_mpeg (`x2::client_movie`, all 30 decode every frame), Bandicam's recording and
      screenshots removed, web pages in the system browser (`x2::client_browser`), CxImage's dead users deleted; a source
      scan keeps them out (2026-11-06). See [Client](../porting/client.md#bandi-embedded-browser-and-cximage-2026-11-06-clientvideo-browser-cximage).
- [ ] `.kom` archives (T35), one branch per step; see [`.kom` archives](../porting/kom.md):
  - [x] `build/kom-library`: `src/kom` (`x2::kom` read and mount, `x2::kom_write`): byte-identical rewrites, first-wins
        mounting with a warning, `ReadOnlyFile` in `x2::platform`; headless tests, byte-for-byte cross-check with `kom.py`,
        and all 173 client archives decoded and round-tripped (2026-11-05); survey of how the original client reads `.kom`
        and loose files.
  - [x] `tools/kom-import`: `x2_assets` (`src/tools/AssetTool`) imports the client into `client_data` (layout rules in
        `tools/assets/layout.txt`, 98.5% classified) + a name-keyed manifest, and packs it back byte for byte; KOG's packer
        used zlib level 6; Lua stays bytecode for now (2026-11-05). See [client_data](../tools/client-data.md).
  - [x] `tools/kom-lua`: readable Lua in `client_data`: `src/lua` (`x2::lua`, C++ port of `kom.py`'s decompiler and
        `kim.py`) decompiles all 4,553 bytecode scripts and turns `Item.kim` into `Item.lua` + `SetItem.lua`; unchanged
        files pack back from the originals (173/173 identical), edited ones as UTF-8 source with BOM, `Item.kim` compiled
        again (2026-11-05). See [Client Lua data](../porting/lua.md).
  - Packing `client_data` for distribution (rebuild only the changed archives, a rule for new files) moved to
    x2 studio's export to `.kom` (user, 2026-11-05; [Later](#later--x2-studio)).
  - [x] `client/kom-mass-file`: the client's `KGCMassFileManager` on `x2::kom` (`x2::client_massfile`): `CX2Main`'s fixed
        list in load order, first-wins, entries decoded by algorithm, full names; with `X2_DEV_TOOLS` the loose files of
        `/data/data` (a client_data works) preferred; nothing deleted at start, a missing or rejected archive named before
        exiting; date-range renames kept; headless tests and all 101,826 entries of `_client/data` decoded through it
        (2026-11-05). Fixed on the way: `KSession`'s session thread could wait on an uninitialized handle (a crash on Linux).
        See [Client: KTDXLIB › Mass files](../porting/client-ktdxlib.md#mass-files).

## Phase 4 — Client: graphics

- [x] `client/render-boundary`: `x2::render` (`src/render`, T36) with the SDL_GPU backend and a null/recording backend for
      headless tests; the SDL_GPU device moved from `x2::app` to it (ImGui through the `sdlgpu::Native` escape hatch), the DXUT
      replacement clears through it (`X2_RENDER_BACKEND`), and a source scan keeps the SDL_GPU API in
      `src/render/backends/sdlgpu` (2026-11-06). See [Render](../porting/render.md).
- [ ] D3D9 translation in `KTDXLIB`, above the boundary (T36), one branch per step:
  - [x] `client/ktdxlib-device`: KTDXLIB's own device interface, no facade (T38), and offline shaders (T39):
        `x2::d3d9::Device` (state cache → pipeline key → pipeline cache, fixed-function shaders through shadercross,
        half-pixel offset, fans as indexed lists, uploads on transfer buffers with D3D9's lock semantics, partial clears,
        render targets); DXUT passes it to the callbacks and presents, the skeleton draws a textured quad and a fan;
        headless tests and a GPU pixel test (2026-11-06). See [D3D9 device](../porting/client-d3d9.md).
  - [x] `client/ktdxlib-engine`: `KTDX.h` and all its headers on the device and `x2::platform`; `x2::client_ktdxlib` builds
        the state manager, the dynamic VB manager, textures, render targets and the silent sound backend, with
        null-backend tests (2026-11-06). See [KTDXLIB › Engine](../porting/client-ktdxlib.md#engine-2026-11-06-clientktdxlib-engine).
  - [x] `client/ktdxlib-build`: all of `KTDXLIB` (100 files) compiles and links, warning-free on GCC, Clang and clang-cl;
        the device callers rewritten onto the device, D3DX's mesh/animation/effect interfaces declared (creators fail
        until their steps), the material/light state and `UpdateTexture`; the tests run on the real `CKTDXApp`; X2Lib's
        32 loading-screen triplets became `CKTDXApp::RenderStageNow` (T38) (2026-11-06). See
        [KTDXLIB › Whole engine](../porting/client-ktdxlib.md#whole-engine-2026-11-06-clientktdxlib-build). What the device
        still lacks comes with the file that needs it ([list](../porting/client-d3d9.md#not-emulated-yet-2026-11-07)).
- [x] `client/effects`: `XRenderer.fx` (19 techniques, its 80 `vs_1_1` shaders as one HLSL program in 8 variants)
      rewritten by hand, compiled by shadercross, as `ID3DXEffect` on the device (vertex shaders, a third texture
      stage, vertex fog); `CKTDGXRenderer` draws the client's skinned meshes through it headless. The other three `.fx`
      have no caller in US/`_SERVICE_` and are not ported (28 techniques in all, recounted 2026-11-07). See
      [D3D9 device › Effects](../porting/client-d3d9.md#effects-2026-11-07-clienteffects).
- [ ] `client/x2lib-build`: `X2Lib` (270 `.cpp`, 547 files) and `X2.cpp` compile and link on KTDXLIB and the device,
      warning-free on GCC, Clang and clang-cl, split into sequential branches if large. See [Client: X2Lib](../porting/client-x2lib.md).
  - [x] `client/x2lib-headers`: the precompiled header chain (`stdafx.h` → `X2Main.h` and all it includes) compiles
        warning-free (target `x2::client_x2lib`); Themida/Virtualizer, WinINet, libxml (now `x2::platform::ParseXml`) and
        Boost gone from the headers; DXUT's `CFirstPersonCamera` and `x2::bimap` added (2026-11-07). See
        [Client: X2Lib](../porting/client-x2lib.md#header-chain-2026-11-07-clientx2lib-headers).
  - [x] `client/x2lib-build`: all 265 `.cpp` files compile warning-free in `x2::client_x2lib` (Clang, GCC; clang-cl in
        CI); Boost, Win32 window/string calls and x86 hooks gone, the anti-cheat scan on `x2::platform`, about 40 original
        bugs fixed (2026-11-11). See [Client: X2Lib](../porting/client-x2lib.md#cpp-files-2026-11-11-clientx2lib-build).
  - [x] `client/x2-link`: `X2.cpp` replaces `X2Skeleton.cpp`; the client links on Clang and GCC. DXUT gains the device
        callbacks; the `ElswordX2` mutex is `x2::platform::SingleInstance` (2026-11-12). See
        [Client: X2](../porting/client-x2.md#x2cpp-2026-11-12-clientx2-link).
  - [x] `client/x2-run`: headless start-up reaches `XS_LOGIN` in the [client folder](../tools/client-folder.md); with a
        window on Hyprland it shows the textured login screen, logs in to the Linux servers, draws the character select
        and the character creation screen with its movies (2026-11-12). See [Client: X2](../porting/client-x2.md#status).
  - [x] `client/frame-pacing`: fixed simulation ticks as in the original, rendering at any rate with the camera and
        skinned meshes interpolated between ticks; vsync at the monitor's rate (FIFO) or a frame limit ([T41](decisions.md));
        D3D9 fixes (loader thread, destination alpha, untextured stages) and GPU validation opt-in. Village ~600 fps,
        field ~350 (2026-11-12, PR #59).
  - [ ] `client/x2-world`: mass script tests, then character select → channel → town/world on the Linux servers.
        Open: a stray server-select disconnect while entering the village, seen once (2026-11-12,
        [Client: X2](../porting/client-x2.md#open-issues)).
- [ ] Reimplement the rest of D3DX on the original assets (T1, T37): `.x` loader (mesh + skin + animation), `.dds` (BC1–3 native,
      legacy formats to RGBA8). Fonts and sprites dropped: the engine uses neither `ID3DXFont` nor `ID3DXSprite`
      (2026-11-07).
  - [x] `client/d3dx-textures`: `.dds` (BC1–3 copied, legacy formats, mips), `.tga` and the disguised `.png`/`.jpg`/`.bmp`
        through stb_image, recognized by contents; BC decoding, D3DX's filters and stb_dxt encoding for the Medium/Low
        texture detail; all 57,793 textures of the client decode but 3 D3DX could not read either (2026-11-07). See
        [D3D9 device › Image files](../porting/client-d3d9.md#image-files-2026-11-07-clientd3dx-textures).
  - [x] `client/x-loader`: the `.x` reader (text, binary, MSZIP) and static meshes — `ID3DXMesh` on the device's new
        index buffers and `DrawIndexedPrimitive`, `D3DXLoadMeshFromXInMemory`, `D3DXCreateMeshFVF` (KME), clone,
        weld, optimize, adjacency, `D3DXIntersect`; all 25,679 `xof` files of the client read and the 25,588 with meshes load, clone, weld and optimize (2026-11-07). See
        [D3D9 device › Meshes](../porting/client-d3d9.md#meshes-2026-11-07-clientx-loader).
  - [x] `client/x-skin`: frame hierarchies (`D3DXLoadMeshHierarchyFromXInMemory`), `ID3DXSkinInfo`,
        `ConvertToIndexedBlendedMesh`, `D3DXFrameCalculateBoundingSphere`; all 7,600 skinned `.x` of the client load
        through `CKTDXDeviceXSkinMesh` (2026-11-07). See
        [D3D9 device › Skinned meshes](../porting/client-d3d9.md#skinned-meshes-2026-11-07-clientx-skin).
  - [x] `client/x-animation`: the animation controller (tracks, events, blending, clones), keyframed and compressed
        animation sets, `.x` animation sets; all 8,687 skinned meshes of the client load, the 1,087 KSM included, and
        every animation set of the 5,466 animated `.x` and the KSM files plays (2026-11-07). See
        [D3D9 device › Animation](../porting/client-d3d9.md#animation-2026-11-07-clientx-animation).
- [ ] ImGui renderer on `x2::render`, replacing the tools' `sdlgpu3` escape hatch (T36).
- [x] Silent sound backend (2026-11-06, `client/ktdxlib-engine`, [Sound](../porting/client-ktdxlib.md#sound)): `CKTDXDeviceSound`, `KTDSManager` and
      `CKTDXDeviceManager::OpenSound` keep their interface and play nothing, so the 149 files that use sound compile
      unchanged. The audio APIs (FMOD, DirectSound, MP3) are confined to 9 `KTDXLIB` files (`KTDXDeviceSound.*`,
      `KTDSManager.*`, `KTDSMP3Play.*`, `Always.h`, `KTDXErrorDefine.h`) and 3 in `X2Lib` (2026-11-05).
- [ ] Graphics improvements before audio (user, 2026-11-12; content may change, [T42](decisions.md)), in this order:
  - [x] `client/global-tick`: one simulation rate of 60 steps per second in every state ([T43](decisions.md), PR #61).
  - [x] `client/resolution`: choosing another resolution or full screen resets the game's device objects; full screen at
        the closest display mode (Hyprland refuses windowed resizes) (2026-11-12, PR #60).
  - [x] `client/interpolation-2` (2026-11-12, PR #62): particles, mesh players, world objects, dialogs (quest balloons)
        and monsters updated every few steps drawn between steps
        ([Frame pacing](../porting/client-ktdxlib.md#frame-pacing-2026-11-12-client-frame-pacing-t41)).
  - [x] `client/msaa` (2026-11-12, PR #63): MSAA 2x/4x/8x with alpha-to-coverage, anisotropic filtering up to 16x, SSAA 2x, and
        the graphics tab with them, the frame limit and vertical sync (merged with `client/graphics-options`, user).
        Also fixed there: the camera updated twice per step, and the interpolated camera turning in 4-pixel steps on
        fields far from the origin (2026-11-13, user confirmed smooth).
  - [x] `client/name-interpolation` (2026-11-13, PR #64): names, the dev overlay, small HP bars, talk balloons and
        attached equipment drawn between steps with their units (user confirmed in the field and the village).
  - [ ] `client/render-perf`: UI costs (`DrawPrimitiveUP` uploads; opening the options menu costs ~0.4 ms), descriptor
        rebinding per draw.
  - [ ] `client/font-rendering`, after widescreen (user, 2026-11-13): UI text looks too bold at 1080p, as it does in
        the original. FreeType hinting and weight (the `fontWeight` parameter is ignored) and the outline styles, which
        redraw the text at ±1 layout unit, so 1 or 2 pixels at 1080p (`CUKFont::DrawFontStyle`).
  - [ ] `server/add-character`, soon (user, 2026-11-13): enable Add (`UT_ADD` = 9; Nasod Ruler 9, Psychic Tracer 32,
        Lunatic Psyker 122). Investigated 2026-11-13. The code is in place: `CX2GUAdd`, the classes on both sides, and the
        creation slot. One limit gates it on both sides, `_CONST_LIMIT_UNIT_::iLimitType = 8` (Elesis) in
        `ServerDefine_Global.h`. Our client has the models, motions, effects, sounds, UI and `UnitTemplet.lua` entries.
        Missing data:
        - no Add skills or skill tree in `NewSkillTempletVer2.lua`;
        - 17 Add items in `Item.lua` (Elesis has 461);
        - nothing for classes 9/32/122 in `runtime/ServerResource` (`StatTable`, `ClassChange*`);
        - in `Game01`, `gup_create_unit` accepts only classes 1–8, `GSkill_Default` has no class 9, and `GBase_Item`'s
          class 9 rows are Ara items.
        The V5 client (`Elsword-V5.0318.6_3`, same `.kom` format) has the Add data, but it is a later version with third
        jobs. Take only these three classes, checking field by field.
  - [ ] **Widescreen** ([T44](decisions.md), user, 2026-11-13; moved ahead of audio): no distortion at any aspect
        ratio, ultrawide included, and 16:9 by default. Today the UI's 1024×768 layout is scaled by separate X and Y
        factors (about 290 uses in 63 files, plus about 130 literal 1024/512/384 positions in X2Lib, 2026-11-13), so it
        stretches; the 3D camera already uses the window's aspect ratio, the fonts scale by Y only. Open:
        [D9, D10](decisions.md#pending). Estimated at 7–10 PRs (2026-11-13). Steps:
    - [x] `client/widescreen-scale` (2026-11-13, PR #66): one uniform scale (screen height / 768) on a canvas as wide
          as the screen, through one set of conversions in `KTDXApp` (positions, sizes, canvas edges) used by dialogs,
          mouse input, world-attached UI, tooltips, `WordWrap`, the 2D orthographic mode and render targets. Everything
          is drawn in the centered column, with black bars at the sides, and the default resolution is 16:9
          ([KTDXLIB](../porting/client-ktdxlib.md#widescreen-canvas-2026-11-13-clientwidescreen-scale),
          [X2Lib](../porting/client-x2lib.md#widescreen-canvas-2026-11-13-clientwidescreen-scale)). Checked in game at
          1920×1080 with the user: login, character select, village, inventory and tooltips.
    - [ ] `tools/ui-capture`: the dev tool capturing the main screens (below). It was split from the step above
          (2026-11-13) because it has to render at sizes other than the window's, and Hyprland does not resize
          windowed clients.
    - [x] `client/widescreen-anchors` (2026-11-13, PR #67): the anchor model in `KTDGUIDialog`/`KTDGUIControl` (the UI
          origin moved while a dialog or control draws and takes the mouse) and the `UIAnchors.lua` loader; the file is
          in the client_data, empty ([KTDXLIB](../porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors)).
          Packing new client_data files is still needed before a release build reads it.
    - [x] `tools/ui-anchors` (2026-11-13, PR #68): the client records what each opened dialog draws
          (`X2_RECORD_UI_ANCHORS=1`) and `propose.py` proposes `UIAnchors.lua` ([UI anchors](../tools/ui-anchors.md)).
          Floating windows stay centered (user, 2026-11-13). Two play sessions of the user recorded 106 of 1,031 dialog
          scripts: 14 anchored (the HUD), 44 centered as windows, 29 to review; more is recorded during the migration.
    - [ ] `client/widescreen-migration`: `UIAnchors.lua` filled and reviewed, HUD first, then windows, then the rest;
          the C++ placement sites (about 224) sorted into layout and derived positions along the way. Two or three PRs.
          Part 1, the HUD, done 2026-11-13 (with the code-placed pieces it showed: quick slot items, minimap, party and
          PvP gauges, buffs). Part 2 (`client/widescreen-screens`, PR #72): character select and creation done
          (user checked, 2026-11-13), the NPC talk shade spans the screen, and dragged dialogs follow the pointer.
          **Paused 2026-11-13** (user): merged as it stands; the migration resumes with the UI editor
          (`tools/ui-editor`, below), with the user reviewing screens and the agent taking what needs code.
          Left: PvP rooms, cash shop, boss bars, the character select background fix and the review list.
          Full-screen black shades found by a scan (2026-11-13), to stretch with their screens: cash shop popups,
          minimap item choice, event scenes and slideshow, world map, login keyboard, bingo.
          This includes the content fixes for full-screen images whose texture does not match its rectangle (the
          character select background: 512×512 drawn at 1024×768, also stretched in the original), found by comparing
          `.tet` UVs with script rectangles.
    - [x] `client/widescreen-stretch` (2026-11-13, PR #71): `STRETCH` controls span the screen through a
          piecewise-linear map whose `STRETCH_ZONES` (the plain middles) grow while end pieces keep their size; text is
          translated, not stretched ([KTDXLIB](../porting/client-ktdxlib.md#stretched-pieces-2026-11-13-clientwidescreen-stretch-t44)).
          The EXP/vigor bar and the loading bars stretched (user checked in game); loading frames now clear the screen
          and the vigor bar's 1-unit overlap (in the original too) is fixed in the client_data. Other spanning pieces
          (boss bars, the NPC talk shade) go with the migration's next part.
    - [ ] `client/widescreen-backgrounds`, art permitting: full-screen images (login, loading screens, cut-ins)
          redrawn for wider screens, replacing the side bars; same formats (T1).
    - [ ] `client/ui-scale`: a "UI scale" option (automatic, 100–200%) and a pixel-perfect mode (integer scale,
          nearest texels: 1× at 1080p, 2× at 4K), since a fractional scale (1.41 at 1080p) blurs the UI a little.
    - [ ] `client/ui-hd-assets`, later: 2× and 4× UI textures, same formats and names in a separate package (T42);
          sizes stay in canvas units, the engine divides the `.tet` UV keys by the loaded texture's scale; the
          original as fallback.
    - Validation for every step: a dev tool that captures the main screens (login, character select, village, field,
          dungeon, PvP, inventory, shop) at 4:3, 16:9, 16:10 and 21:9 to compare across PRs; at 4:3 a migrated
          screen must match the original.
- [ ] **Audio, next** (user, 2026-11-05; **moved ahead of the rest of the widescreen migration**, user, 2026-11-13: a
      long-pending, self-contained task): replace the silent backend with a real one, FMOD Ex / DirectSound →
      miniaudio mixing on an SDL3 audio stream ([T45](decisions.md)); only those files change. `client/audio`
      (2026-11-13, PR #73): music (134 `.ogg` files in `data/music`, streamed) and effects (6,481 sounds, `.ogg` and
      PCM `.wav`, in the `.kom` archives), 3D with linear roll-off; every file decodes in the tests
      ([Sound](../porting/client-ktdxlib.md#sound)). Waiting for the user to listen in game.
- [ ] `tools/ui-editor`, after audio (user, 2026-11-13): a UI editor for the user, under the client development suite
      ([T29](decisions.md)), built from the client's own GUI code so it renders dialogs exactly as the game does: view
      any dialog at any resolution and aspect, select a control, and see and edit its anchor, position, texture and
      UV in a side panel. It replaces the agent reviewing each screen by captures. Open when it starts: how it places
      the dialogs the C++ positions at run time (the positions recorded by `X2_RECORD_UI_ANCHORS`, or moving that
      placement out of the game states), and where position/texture edits are saved (anchors go to `UIAnchors.lua`).

## Phase 5 — Deep cleanup

Real refactoring (remove dead systems, reorganize folders and names), guided by tests/smoke tests
from the previous phases. Only after everything runs.

- [ ] Fix the [original bugs](../original/bugs.md) retained in the port, each with a decision about the new behavior.
- [ ] Optional: rewrite the engine's D3D9-style calls onto `x2::render`'s own concepts and drop the D3D9 vocabulary
      (`D3D9Types.h`, T38), file by file, once everything renders through the [D3D9 device](../porting/client-d3d9.md).

## Later — x2 studio

The definitive game editing tool (user, 2026-11-05): edit the game, maps and everything else on `client_data`
([client_data](../tools/client-data.md)), on `x2::kom` and the SDL3 + ImGui foundation (T32). Not planned in detail yet.

- [ ] **Export to `.kom`**: pack `client_data` for distribution, rebuilding only the archives that changed; new files
      go to archives chosen by a rule (2 GB limit; today the client loads a fixed list of 173 names). Builds on
      `x2_assets pack`, which already rewrites every archive byte for byte when nothing changed.

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
