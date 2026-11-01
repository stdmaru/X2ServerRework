[Home](../README.md) › Porting

# Porting

One page (or subfolder) per `src/` module: status, changes from the original, local decisions, and pending work.
The overall plan is in [Project › Roadmap](../project/roadmap.md).

| Module | Status | Page |
|---|---|---|
| `src/platform` | Linux (Clang, GCC) and Windows (clang-cl) build and tests pass | [Platform](platform.md) |
| `src/kncsdk` | Linux (Clang, GCC) and Windows (clang-cl) build and tests pass; custom crypto (T27); protocol validation pending | [KNCSDK](kncsdk.md) |
| `src/kncsdk/ServerConsole`, `src/server/x2ctl` | Server terminal (T15) and control protocol (T30), Linux and Windows (2026-10-31) | [Server console](console.md) |
| `src/server/Common` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings, with `Socket/` (TCP and UDP) and `ODBC/`; packet, session, UDP, and database tests (real SQL Server, FreeTDS; Linux only) | [Server: Common](server-common.md); [ODBC database](server-common-db.md) |
| `src/server/EPublisherServer` | DEB AuthServer reimplementation: builds without warnings, tests on Linux and Windows, runs against the database | [Server: EPublisherServer](server-epublisher.md) |
| `src/server/GameServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_game`); original client reaches town (2026-10-31) | [Server: GameServer](server-game.md) |
| `src/server/ChannelServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_channel`); original client channel login works (2026-10-30) | [Server: ChannelServer](server-channel.md) |
| `src/server/LoginServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_login`); original client reaches town (2026-10-31); ARM64 authentication crash fixed and verified | [Server: LoginServer](server-login.md) |
| `src/server/CenterServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_center`); original client reaches a field and dungeon (2026-10-31) | [Server: CenterServer](server-center.md) |
| `src/server/GlobalServer` | Linux (Clang, GCC) and Windows (clang-cl) build without warnings; runs on Linux (`x2_run_global`); tested with original client and other servers (2026-10-31) | [Server: GlobalServer](server-global.md) |

**Windows icons** (2026-10-31): each server executable uses the original icon (`icon.ico` in the module directory, copied from
`_reference` through git; EPublisherServer's comes from the DEB `AuthServer`). `x2_windows_icon` (`cmake/X2Compiler.cmake`) generates
the `.rc` with the same `IDI_ICON1 ICON "icon.ico"` as the original `<Server>.rc`; MFC dialogs from those `.rc` files are not included (T15).
It does nothing on Linux.
