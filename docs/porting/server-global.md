[Home](../README.md) › [Porting](README.md) › Server: GlobalServer

# Server: GlobalServer (`src/server/GlobalServer`)

Provides data shared across all server groups: global missions (`GlobalMissionManager`) and world missions
(`WorldMissionManager`, crystal defense, `P_LDefense_Crystal_*`), random-enchantment and exchange-limit events (`GlobalEventDBThread`),
user count (`GlobalServerCCUManager`), blocklist (`BlockListManager`, `P_MPenalty_GET`), automatic party formation (`AutoParty*`),
PvP matchmaking (`Match*`), field boss (`FieldBossManager`), and server serial numbers (`ServerSNManager`). Accepts GameServer
connections (port 9500, `Statistics.LServerList`); GameServers send `EGB_*` messages, currently discarded because server was absent
([GameServer dependencies](server-game.md#dependencies-on-other-servers-survey-from-2026-10-31)).
Source: `KncWX2Server/GlobalServer`, imported on 2026-10-31 with [server flags](../tools/flags.md)
([report](../../tools/import/reports/server-global/REPORT.md)). Depends on the ported [Common](server-common.md) library.
The port preserves the original directory layout (`src/server/GlobalServer` beside `src/server/Common`).

## Import (2026-10-31)

- **Selection**: files belonging to `GlobalServer/` compiled by `GlobalServer.vcxproj` (`Release_US`) and their headers:
  **77 files** (34 `.cpp`, 43 `.h`), 16.8k written lines, 13.7k in `.cpp`. Largest: `MatchMakingManager.cpp` (1.3k),
  `GlobalUser.cpp` (1.1k), `AutoPartyMakingProcess.cpp` (1k). Folder's `PvpMatchScriptManager.cpp` is not in project.
  `Common` includes are not followed (`include_scope`); all exist in ported `Common` except
  `OnlyGlobal/ProcessCommuniationModule/ProcessCommunicationManager.h` (KSMS, removed from other servers).
- **Flags**: resolved 506 `#if` blocks, removed 4.2k lines (1.2k in `WorldMissionManager.cpp` alone). No block became `X2_DEV_TOOLS`.
- **Nothing excluded during import**, as in other servers: publisher/legacy code is removed at build with changes recorded here.
  Only `.dsn` files are omitted (runtime versions come from `runtime/`, T22).
- Encoding: 62 CP949, 15 ASCII.

## Dependencies (2026-10-31)

| Area | Files/usages | Plan |
|---|---:|---|
| KSMS (`ProcessCommunicationManager`, *named pipes*) | `GlobalServer.*` (23 usages) | remove, as in other servers |
| SMS (`GlobalSMSDBThread`, `DC_SMS`, `SMS.dbo.SMS_Send`) | 2 + `GlobalServer`, `GlobalSimLayer`, `GlobalScriptRefreshThread` | builds and remains, as in LoginServer and CenterServer (US config does not enable `DC_SMS`) |
| Anti-cheat, TBB, Mail, publisher | none in code (TBB and Mail DLLs only in production folder) | — |
| Boost | `timer` in 11 files (25 uses), `shared_ptr` in 4 (19), `wformat`/`str` in 2 (7), `bind` in 1 (2) | mechanical pass ([Mechanical porting](../tools/mechanical-porting.md)); manual `bind` |
| `windows.h`/Winsock/ATL/`intsafe.h` | 11 | `x2/platform/win32.h`, `net.h` |
| `CTime`, `lua_tinker` | 13 files (132 uses), 10 (50) | already covered by [Platform](platform.md) and KNCSDK |
| Database | 27 `DO_QUERY` in 4 files | `KODBC` already ported; US config uses only `DC_ACCOUNT` and `DC_LOG` (`Account_US_SERVICE.dsn`, `log_US_SERVICE.dsn`) |

## Status (2026-10-31, branch `server/global-run`)

**Runs on Linux** from the directory assembled by the build (`x2_run_global`, [Server package](../tools/runtime.md)): connects to two
databases (`Account` and `Statistics`), loads scripts (`Enum`, `DungeonEnum`, `WorldMissionConfig`, `PvpMatchData`...; load errors
are the already-listed dungeon and line-map errors), authenticates to DB, reaches `OnServerReadyComplete`, and accepts GameServer
connection (port 9500). Exits with code 0 (see [Run the servers](../running.md)). Three groups of procedures it calls do not exist
in backups (global missions, timed enchantment, exchange limit): these features start without data
([Original bugs](../original/bugs.md#data-serverresource-clientscript)).

**Tested with original client** (Wine, 2026-10-31, with the other five servers): login, town, field, and dungeon work as before, with no
GlobalServer errors; GameServer now delivers `EGB_*` (nothing is discarded once servers are running) and receives
`EGB_UPDATE_AUTO_PARTY_BONUS_INFO_NOT` from GlobalServer. With this, **all original servers are ported**.

### Build (2026-10-31, branch `server/global-build`)

**Builds and links without warnings** (target `x2_global_server`, executable `GlobalServer`, `main` uses `KServerMain` like other
servers, with original icon) with Clang Debug, GCC Debug, Clang Release, GCC Release, and clang-cl (Windows test VM); all 6 tests pass
on Linux and Windows. Without arguments it starts, rejects missing config (`Init` fails), shuts down all layers, and exits with code 1,
as other servers do.

Build effort: 50 Clang Debug errors (`-k 0`) after mechanical pass, almost all from two templates in `GlobalServer.h`, and 7 under GCC;
~40 distinct Linux warnings and 17 clang-cl warnings. Smallest server: most coupling had already been removed in `Common`.

## Changes from the original

| Where | Change | Reason |
|---|---|---|
| `KGlobalServer` | Remove KSMS calls (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\GlobalServer*`, reconnected every 60 s in `Tick`, and script-reloaded notice) | As in [CenterServer](server-center.md#changes-from-the-original) and other servers |
| `ON_DBE_SERVER_ON_ACK` (`GlobalServer.cpp`) | When DB rejects server, response calls `SetAuthenticateResult( false )` and `Init` fails; when accepted, sets `true` | As in [GameServer](server-game.md#changes-from-the-original): original called `::exit( 0 )` on that thread |
| `KGlobalAccountDBThread` | Remove `SendToNXGMToolTcpManager`/`SendToNXGMToolTcpServer` templates | Nexon publisher GM tool; never called, and its singletons do not exist in US build |
| `KBlockListManager` (IP blocking) | `inet_addr` → `x2::platform::IpFromString` (`inet_pton`) | `inet_addr` is deprecated in Winsock. Only dotted-quad addresses; short forms accepted by `inet_addr` (`127.1`, hexadecimal) no longer match a block |
| `Global*DBThread.h` | `ProcessEvent` is no longer `virtual inline` | Defined in `.cpp`: ODR-invalid (`-Wundefined-inline` from clang-cl), as in CenterServer and LoginServer |

Portability (no behavior change): mechanical pass (`mechanical.py`); declaration skipped by `goto end_proc` in
`GlobalGameDBThread` moved upward (`fix_goto.py`); includes required by templates but omitted by MSVC (`ActorManager.h` in
`GlobalServer.h`, `BaseServer.h` in DB threads); `intsafe.h` → `<climits>` (`BYTE_MAX` → `UCHAR_MAX`, same 255); `max` → `std::max`;
`virtual` destructors in singletons `KGlobalMissionManager`, `KWorldMissionManager`, and `KGlobalServerCCUManager`;
`using KThreadManager::Init` in `KGlobalScriptRefreshManager`; `default: break;` in incomplete `switch` statements; loops that copied
`shared_ptr` and structures by value now use `const` references; comparisons of `enum` with `double` (exchange deadlines) use
`static_cast`; removed `unsigned < 0` (also always false in MSVC) from `KMatchUserManager`; removed dead variables/statements
(`bChanged` in `KBlockData::UpdateAndChanged`, which returns via `goto`; `tCurrentTime` in `CheckExchangeLimit`; members only mentioned,
not assigned, in `KBlockListManager` and `KMatch::GetRoomInfoForPvpGame` now use constructor defaults, as original did).

### Original bugs fixed

| Where | Bug | Fix |
|---|---|---|
| `KWorldMissionManager::ToStringW` | Used `BUILD_LOG` (logger manipulators, `dbg::tab`/`dbg::endl`) on ordinary `std::wostream`: MSVC converted function pointers to addresses, so dump showed numbers instead of tabs and line breaks | Use `TOSTRINGW`, like the rest of function (2026-10-31, clang-cl `-Wmicrosoft-cast`) |

## Plan

1. ~~**`server/global-import`**~~ (2026-10-31): `tools/import/modules/server-global.toml` and this page.
2. ~~**`server/global-build`**~~ (2026-10-31): build without warnings with Clang, GCC (Debug and Release), and clang-cl; `main` uses
   `KServerMain` (as other servers) and original icon; remove KSMS.
3. ~~**`server/global-run`**~~ (2026-10-31): `runtime/GlobalServer` (from production `config_gb_US_SERVICE.lua`, `.dsn` for `Account` and
   `Statistics`/log), build-assembled directory, and GameServer `EGB_*` responses. Production `.lua` files are resolved by T28:
   code loads only `WorldMissionConfig.lua` (in `ServerResource`), as well as `Enum.lua`, `DungeonEnum.lua`, and `PvpMatchData.lua`;
   production folder's `GlobalMissionConfig.lua` and `MissEliosConfig.lua` are not loaded by anything (verified in imported code, 2026-10-31).
