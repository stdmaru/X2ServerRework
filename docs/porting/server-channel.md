[Home](../README.md) › [Porting](README.md) › Server: ChannelServer

# Server: ChannelServer (`src/server/ChannelServer`)

Channel server: server list, account login (through DEB, `_DEB_AUTH`), and user handoff to GameServer.
Source: `KncWX2Server/ChannelServer`, imported on 2026-10-30 with [server flags](../tools/flags.md)
([report](../../tools/import/reports/server-channel/REPORT.md)). Depends on the ported [Common](server-common.md) library.
The port preserves the original directory layout (`src/server/ChannelServer` beside `src/server/Common`) because source files include
headers as `"../Common/x.h"`.

## Import (2026-10-30)

- **Selection**: files belonging to `ChannelServer/` compiled by `ChannelServer.vcxproj` (`Release_US`), plus their headers:
  **36 files** (17 `.cpp`), 6,071 written lines (4,913 in `.cpp`). `Common` includes are not followed (`include_scope`):
  report lists them, and all exist in `src/server/Common`.
- **Outside selection**: `OnlyGlobal/ChannelUserAuth_<country>.cpp` files (BR, CN, EU, ID, IN, JP, PH, TH, TW, US), not compiled by vcxproj;
  only `OnlyGlobal/ChannelUserAuth.cpp` is included. Configs (`config_ch_*.lua`, `.ini`), `.dsn`, `.rc`, icon, and DLLs are also not source.
- **Flags**: resolved 370 `#if` blocks, removed 2,898 lines. No block became `X2_DEV_TOOLS`.
- Encoding: 29 CP949, 3 UTF-8, 4 ASCII. Include spelling fixed: `odbc/` → `ODBC/`, `socket/` → `Socket/`, `\` → `/`.

## Status (2026-10-30, branch `server/channel-run`)

Runs on Linux from the directory assembled by `x2_run_channel` ([Run directories](../tools/runtime.md)): connects to the Docker SQL Server
(FreeTDS), loads scripts (`Enum.lua`, `DungeonEnum.lua`, `GameSysValTable.lua`, `WaitQueueData.lua`, `LogConfig.lua`...),
authenticates to DB (UID 59, "Channel"), reads channel list, and reaches ready state (`OnServerReadyComplete`), with Lua prompt responding.
Exits with code 0 (see [Run the servers](../running.md)).

**Tested with original client** (`x2_DEBUG.exe` and `x2.exe`, under Wine, 2026-10-30), with [EPublisherServer](server-epublisher.md):
login is accepted and client receives server-group and channel lists. It then connects to GameServer (`LChannelList`: `127.0.0.1:9300`),
which does not exist yet, and displays "The server is full. Please try again.", as expected. First test failed because of KNCSDK default keys
([KNCSDK](kncsdk.md#portability-no-behavior-change)).

## Build (2026-10-30, branch `server/channel-build`)

`ChannelServer` executable (target `x2_channel_server`) **builds and links without warnings** with GCC Debug, Clang, and MinGW (clean build).
Without arguments it starts, rejects missing arguments like original (`Init` fails), shuts down all layers, and exits with code 1,
on Linux and Wine. It had not yet run with config, database, and client: that was `server/channel-run`.

## Executable and command line

- `main.cpp` calls `KServerMain` (`Common/ServerMain.{h,cpp}`), replacing original MFC window (`Common/ui`: `CServerApp`/`CServerUIDlg`)
  with same sequence: `CreateKNC()`, `SetCmdArgs`, `Init`, `Run`, `ShutDown`, `ReleaseKObj`. Window's Lua input field is the
  `lua_interpreter` prompt (`KBaseServer::Run( true )`). Since 2026-10-31 this is the T15 terminal ([Server console](console.md)).
- Arguments as in original: `ChannelServer <tag> <server group> [<database id> <database password>]`; tag selects
  `config_ch_<tag>.lua` (e.g. `US_SERVICE`).
- Exit code is 1 when `Init` fails (original window stayed open until closed).
- `g_szConfigFile` and `CreateDBThread`, required by `Common` from each server, come from `ChannelServer.cpp` (as in original);
  `Common` tests continue to use stubs from `server_hooks.cpp`.

## Changes from the original

| Where | Change | Reason |
|---|---|---|
| `KChannelServer`, `KChannelSimLayer` | Remove calls to `ProcessCommunicationManager` (KSMS: *named pipes* `\\.\pipe\ChannelServerRead`/`Write`, reconnected every 60 s in `Tick`, and maintenance-mode messages) | KOG management tool, excluded from `Common`. Worked only with `GameSysVal:SetProcessCommunication(true)` |
| `ON_DBE_SERVER_ON_ACK` | When DB rejects server, response calls `SetAuthenticateResult( false )` and releases `Authenticate`; `Init` fails and server exits via `ShutDown` (code 1) | Original called `::exit( 0 )` on this thread while others kept running: static destructors freed event pool in use (segfault found with ASan on 2026-10-30) |
| `config_ch_DEV.lua` | `SetLocalIP( ... )` sets address server identifies itself with | Original used first address for host name, which on Linux could be any interface (here, VPN) |
| `-popup` | Rejected with message; DB credentials are arguments | Was MFC dialog asking for ID and password |
| `ChannelAccountDBThread` | `boost::variate_generator` (OTP) → lambda over same distribution; `_itow` → `_itow_s` | Same result |
| `ChannelLogDBThread`, `ChannelAccountDBThread`, `ChannelSMSDBThread` | `ProcessEvent` is no longer `virtual inline` (defined in `.cpp`) | `inline` without header definition violates ODR; clang-cl reports (`-Wundefined-inline`). GameServer has same pattern |

Portability (no behavior change): mechanical pass (Boost → `std`/`x2::`, platform includes), `goto end_proc` crossing declaration in
`ON_DBE_SERVER_OFF_NOT` (declaration moved up), include `BaseServer.h` in headers whose templates use `KBaseServer::GetKObj()`,
`virtual` destructors in `KCCUToolManager`, `KMachineBlockManager`, and `KWaitQueueManager` (deleted through base-class singleton pointer),
`using` declarations for hidden `RoutePacket`/`Init` overloads. Changes in `Common`: [Server: Common › Changes](server-common.md#changes-from-the-original).

## Pending

- `KAccepter` `cerr` "INVALID_SOCKET" during shutdown comes from closing socket waiting in `accept`; also happens in original (retained).
