[Home](../README.md) › Porting

# Porting

One page (or subfolder) per `src/` module: status, changes from the original, local decisions, and pending work.
The overall plan is in [Project › Roadmap](../project/roadmap.md).

Linux builds and tests are verified on x86_64 and arm64. ARM64 runtime validation currently covers LoginServer authentication.

| Module | Status | Page |
|---|---|---|
| `src/platform` | Linux (Clang, GCC) and Windows (clang-cl) build and tests pass | [Platform](platform.md) |
| `src/kncsdk` | Linux (Clang, GCC) and Windows (clang-cl) build and tests pass; custom crypto (T27); protocol validation pending | [KNCSDK](kncsdk.md) |
| `src/app` | SDL3 + ImGui foundation for the client and tools (T32): Linux (Clang, GCC) and Windows (clang-cl), headless tests (2026-11-04) | [App foundation](app.md) |
| `src/kncsdk/ServerConsole`, `src/server/x2ctl` | Server terminal (T15) and control protocol (T30), Linux and Windows (2026-10-31) | [Server console](console.md) |
| `src/server/Common` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings, with `Socket/` (TCP and UDP) and `ODBC/`; packet, session, UDP, and database tests (real SQL Server, FreeTDS; Linux only) | [Server: Common](server-common.md); [ODBC database](server-common-db.md) |
| `src/server/EPublisherServer` | DEB AuthServer reimplementation: builds without warnings, tests on Linux and Windows, runs against the database | [Server: EPublisherServer](server-epublisher.md) |
| `src/server/GameServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_game`); original client reaches town (2026-10-31) | [Server: GameServer](server-game.md) |
| `src/server/ChannelServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_channel`); original client channel login works (2026-10-30) | [Server: ChannelServer](server-channel.md) |
| `src/server/LoginServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_login`); original client reaches town (2026-10-31); ARM64 authentication crash fixed and verified | [Server: LoginServer](server-login.md) |
| `src/server/CenterServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_center`); original client reaches a field and dungeon (2026-10-31) | [Server: CenterServer](server-center.md) |
| `src/client` | Imported (849 files: `X2`, `X2Lib`, `KTDXLIB`, `X2ServerProtocol`, `LuaCommon`); skeleton (window, loop, `MsgProc`) and `LuaCommon` build on Linux (Clang, GCC) and Windows (clang-cl) with headless tests; the engine and game are not compiled yet (2026-11-05) | [Client](client.md): [X2](client-x2.md), [X2Lib](client-x2lib.md), [KTDXLIB](client-ktdxlib.md), [X2ServerProtocol](client-serverprotocol.md), [LuaCommon](client-luacommon.md), [DXUT](client-dxut.md) |
| `src/server/GlobalServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_global`); tested with original client and other servers (2026-10-31) | [Server: GlobalServer](server-global.md) |

**Windows icons** (2026-10-31): each server executable uses the original icon (`icon.ico` in the module directory, copied from
`_reference` through git; EPublisherServer's comes from the DEB `AuthServer`). `x2_windows_icon` (`cmake/X2Compiler.cmake`) generates
the `.rc` with the same `IDI_ICON1 ICON "icon.ico"` as the original `<Server>.rc`; MFC dialogs from those `.rc` files are not included (T15).
It does nothing on Linux.
