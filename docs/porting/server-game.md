[Home](../README.md) › [Porting](README.md) › Server: GameServer

# Server: GameServer (`src/server/GameServer`)

Game server: characters, towns, fields, dungeons, PvP, items, quests, guilds, cash (through [EPublisherServer](server-epublisher.md)).
Source: `KncWX2Server/GameServer`, imported on 2026-10-30 with [server flags](../tools/flags.md)
([report](../../tools/import/reports/server-game/REPORT.md)). Depends on the ported [Common](server-common.md) library.
The port preserves the original directory layout (`src/server/GameServer` beside `src/server/Common`).

## Import (2026-10-30)

- **Selection**: files belonging to `GameServer/` compiled by `GameServer.vcxproj` (`Release_US`) and their headers:
  **298 files** (145 `.cpp`, 153 `.h`), 163k written lines, 148k in `.cpp`. Largest: `GSGameDBThread.cpp` (15k),
  `Inventory.cpp` (15k), `GSUserFunction.cpp` (12k). `Common` includes are not followed (`include_scope`); no CenterServer header
  is used despite `..\CenterServer` being in vcxproj search path.
- **Flags**: resolved 7,532 `#if` blocks, removed 81k lines. Converted to `X2_DEV_TOOLS`: two `_IN_HOUSE_` checks in
  `GSUserGameCommon.cpp` and logs `SERV_LOG_UNDEFINED_QUEST_TEMPLET` (as in `Common`).
- **Excluded (anti-cheat, T10)**: `HackShield/` (AhnLab, `HSAuth.cpp`), `nProtect/` (GameGuard, `GGAuth.cpp`, `NProtectFunc.cpp`),
  and XTrap (`..\..\Xtrap\Server\XTrap4Server.cpp`, `XTrapCSM.cpp`), compiled by vcxproj from outside `KncWX2Server` and not imported.
  References remain in 9 files (`GameServer.*`, `GSSimLayer.*`, `GSUser*.cpp`, `GSLogDBThread.cpp`): removed at build.
- Encoding: 274 CP949, 24 ASCII.

## Dependencies (2026-10-30)

| Area | Files/usages | Plan |
|---|---:|---|
| Boost | 52 files (`boost::str`/`wformat` 118, `shared_ptr` 35, `timer` 16, `iequals`/`replace_all` 15...) | mechanical pass ([Mechanical porting](../tools/mechanical-porting.md)); manual `iequals`/`replace_all` |
| `windows.h`/Winsock/ATL | 29 | `x2/platform/win32.h`, `net.h` |
| Intel TBB | `GameServer.cpp` (`tbbmalloc_proxy.h`) | remove (allocator; roadmap calls for `std`) |
| Anti-cheat | 9 | remove calls |
| `CTime`, `__int64`, `lua_tinker` | 64, 21, 21 | already covered by [Platform](platform.md) and KNCSDK |
| Database | 502 `DO_QUERY` | `KODBC` already ported; procedures are in `Game01` |
| Admin/cheat tools | `GSUserAdminCheat.*`, `UserCheatManager.*`, `RobotTestManager.*` | review under `X2_DEV_TOOLS` (T10) |

## Status (2026-10-31, branch `server/game-run`)

**Runs on Linux** from the directory assembled by `x2_run_game` ([Run directories](../tools/runtime.md)): connects to four Docker
SQL Server databases (FreeTDS), loads scripts from `ServerResource` and `ClientScript` generated from the client (production-data load
errors are in [Original bugs](../original/bugs.md#data-serverresource-clientscript)), authenticates to DB (UID 2, "Game"), receives
Center (9100), Login (9200), and Global (9500) lists, attempts to connect, and reaches ready state (`OnServerReadyComplete`), with Lua
prompt responding. Exits with code 0 (see [Run the servers](../running.md)). Packets to servers that do not yet exist are discarded
([Dependencies](#dependencies-on-other-servers-survey-from-2026-10-31)).

**Tested with original client** (Wine, 2026-10-31, with EPublisherServer and ChannelServer): "The server is full" no longer appears.
Client connects to port 9300, exchanges key, and sends `EGS_VERIFY_ACCOUNT_REQ`; GameServer forwards it as
`ELG_NEXON_USER_AUTHENTICATE_REQ`, which is discarded because LoginServer does not exist, and client remains at "Connecting to server".
With [LoginServer](server-login.md) (2026-10-31), client gets past this and reaches town; leaving for field requires CenterServer.

**Build** (2026-10-30, `server/game-build`): executable (target `x2_game_server`, `main` uses `KServerMain` like ChannelServer)
builds and links without warnings with GCC Debug, Clang Debug, GCC Release, Clang Release, and clang-cl (Windows test VM); all 6 tests
pass in each. Clang and clang-cl found three original bugs (below) that GCC did not report.

Build effort (GCC Debug, `-k 0`): 632 errors in 59 files after mechanical pass; 0 errors and 1,099 warnings (190 distinct) after fixes
below and `fix_warnings.py`.

**Build resources:** with 16 jobs, GCC exhausted host RAM (~12 GB free; 10k–15k-line `.cpp` files use ~1.5 GB each): use `-j 6`.

## Changes from the original

| Where | Change | Reason |
|---|---|---|
| Anti-cheat (`GameServer`, `GSUser`, `GSUserFunction`, `GSUserSession`, `GSUserSecurity`, `GSSimLayer`) | Remove HackShield (`m_kHSAuth`, `_AhnHS_*`), XTrap (`m_cXTrapCSM`, `map*.cs3`, `ReloadXTRAP_MAP`), nProtect, and handlers `EGS_HACKSHIELD_CHECK_ACK`, `EGS_REQUEST_HACKSHIELD_CHECK_NOT`, `EGS_XTRAP_ACK`. Packets remain in `Common` | T10. Production US config enabled `CF_XTRAP`; without code server behaves as with flags disabled |
| `GameServer.cpp` | Remove Intel TBB malloc proxy (`tbbmalloc_proxy.h`) and log "TBB 메모리 관리자 사용" | System allocator; roadmap planned TBB removal |
| `GameServer`, `GSScriptRefreshManager` | Remove KSMS calls (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\GameServer*`), including 38 "script reloaded" notices | External Windows-only tool, already removed from ChannelServer |
| `NexonSOAP`, `NexonSOAPManager`, `TRIOCP`, `TRIOThread` | Remove files | Dead code: no callers (original TCP relay already used `Common` `KIocp`) |
| `KSkTRUser::SendDataWithProfile` | Removed | Copy of `KSocketObject::SendData` with profiler commented out, no callers |
| `ServerDefine.h` | Force-include in every server file (`-include`/`/FI`, in `x2_server_common`) | Without precompiled header, files reaching `Enum.h` before it compiled the other `EXTEND_SERVER_GROUP_MASK` branch in same inline function |
| `DEBBilling*` (`Common/OnlyGlobal/AuthAndBilling`) | Compiled by GameServer, not `Common` library | Includes GameServer's `BillManager.h` (original compiled `Common` into each server) |
| `NexonBillingTCPPacket` | `unsigned long` → `DWORD` | 4-byte protocol fields; `unsigned long` is 8 bytes on Linux (`Read8Byte` read 16) |
| `UserLogManager`, `RobotTestManager` | Files use UTF-8 (`Utf8FileLocale`), `/` paths, `std::filesystem` | Original used `"korean"` locale (CP949) and `CreateDirectoryA` |
| `XSLXmlParser` (Nexon policy) | `W2A` → UTF-8 | XML declares `encoding="utf-8"`; `W2A` used ANSI code page |
| Always-false comparisons | Remove `u_char == -1` (guild grade) and `unsigned < 0` (skill level, active page) | Also always false in MSVC: behavior unchanged ([Original bugs](../original/bugs.md)) |
| `ON_DBE_SERVER_ON_ACK` (`GameServer.cpp`) | When DB rejects server, response calls `SetAuthenticateResult( false )` and `Init` fails; when accepted, sets `true` | As in ChannelServer ([Changes](server-channel.md#changes-from-the-original)): original called `::exit( 0 )` on that thread. Without `true`, ported `Common` `Authenticate` rejected GameServer |

### Original bugs fixed

| Where | Bug | Fix |
|---|---|---|
| `GSLogDBThread` (character statistics) | Four ED columns (`US_ED_OpenCubeSpendED`, `US_ED_PBattleFieldED`, `US_ED_MCoboExpressTicketED`, `US_ED_MItemResolve`) were read from `etcData[22]` at indices 22–25: out-of-bounds read (stack garbage in log) | Read from `edData`, where they are summed (2026-10-30, found by Clang `-Warray-bounds`) |
| `GSUserAdminCheat` (`/dhn`) | Nickname `std::wstring` passed through `CString::Format` `...`: `%s` interpreted its structure as pointer | `.c_str()` (Clang rejects: "call will abort at runtime") |
| `KGSHenirRanking::SetLastRank` | `m_uiLastRank = m_uiLastRank`: setter did nothing and Henir ranking limit always used initial value | Store parameter (`-Wself-assign-field`) |
| `GSGameDBThread`, character selection | Missing `EndFetch` after reading `dbo.P_GSkill_Expand_SEL` row: next query (`dbo.P_GUnitLastPosition_GET`) found open cursor (`24000`), so `KODBC` reconnected and retried | Add `EndFetch` (2026-10-31, found in client test during `server/login-run`) |
| `CXSLSkillTree::LoadSkillTemplet_Stat` (`Common`) | Stat table with fewer than 30 levels (guild skills 500 and 501 have 10) was read through level 30: out-of-bounds read | Missing levels become 0 (2026-10-31, libstdc++ Debug assertion). Inverted check that allows short table remains for Phase 5 ([Original bugs](../original/bugs.md)) |

Portability (no behavior change): mechanical pass (`mechanical.py`); `BOOST_MAP_CONST_FOREACH` → range-for;
`boost::iequals`/`ifind_first`/`replace_all` → `x2::platform::IEquals`/`IContains`/`ReplaceAll` (ASCII folding, like Boost in "C" locale);
move 52 declarations skipped by `goto end_proc` to function start (`tools/port/fix_goto.py`, plus 2 manual); KNCSDK `LOG_CONDITION`
became an expression; `_KCSLOCK_TYPEDEF_` made variadic; `KUserManager::MultiQueueingByCID` now exposes base overload
(previously pending in `Common`); `CString::GetBuffer`; `std::min/max` instead of macros; `_itoa`/`_i64tow` → `std::to_string`/`_i64tow_s`;
`StrStrW` → `wcsstr`; `virtual` destructors in 14 singletons; `using` declarations for hidden `RoutePacket`/`Init` overloads; remove `#pragma NOTE`.

## Pending

- **Nexon (KR) still compiles as dead code** (`NexonBillingTCP*`, `KNexonBillingTCPManager`): US uses `BF_GLOBAL_SERVICE` and
  `SiKBillManager()` (DEB). Remove in roadmap publisher cleanup step. Before that, `CI_EXPAND_SKILL_PAGE` in
  `GSUserCashShop_Global.cpp` queries Nexon catalog, always empty in US: purchase failed in original (verify when testing cash).
- Original bugs retained (magic defense for skills, guild grade, `IsPossibleLowLevel`...) and production-data load errors:
  [Original bugs](../original/bugs.md), for Phase 5.
- 105 `unsigned long` in GameServer (8 bytes on Linux): review those sent to network, DB, or fixed formats when running.

## Dependencies on other servers (survey from 2026-10-31)

GameServer does not know other servers from configuration. On DB authentication, it receives Center, Login, and Global lists
(`m_mapCenterList`/`m_mapLoginList`/`m_mapGlobalList`, `GameServer.cpp`) and `KProxyManager` (`Common`) connects to all
(`ConnectAll`), with a thread retrying every 10 seconds. **It does not wait for these connections before becoming ready**
(`SetServerReady` follows immediately). Without target server, `SendTo*Server` logs "Proxy pointer not found" and discards packet.
No response or timeout: user remains in current state.

From channel through town (`GSUserSession.cpp`, `GSUserGameCommon.cpp`, `GSUserField.cpp`, `GSUserFunction.cpp`):

| Point | Packet | Destination | If server is absent |
|---|---|---|---|
| `EGS_VERIFY_ACCOUNT_REQ` | `ELG_NEXON_USER_AUTHENTICATE_REQ` → `_ACK` | LoginServer authenticates against `Account` (`P_MUser_SEL_Auth`, `SERV_GLOBAL_AUTH` branch) | **blocks**: client never receives `EGS_VERIFY_ACCOUNT_ACK` |
| after `_ACK` | `ELG_REGISTER_USER_REQ` → `ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK` | LoginServer (user list, account block) | **blocks**: user does not enter `S_LOGINED` |
| server and character selection | none (only `Game` and `Account` databases) | — | — |
| character selected | `ELG_REGISTER_UNIT_NOT`, `EGB_REGISTER_UNIT_NOT`, `ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ` | Login, Global | warnings only; nothing waits for replies |
| enter town | `ERM_JOIN_FIELD_*` | GameServer itself (`SendToGSField`) | — |

Conclusion: **only LoginServer is needed to reach town** (45 `.cpp`, ~42k original lines, 2026-10-31). CenterServer (~87k lines:
dungeon rooms, PvP, square, parties, trade) and GlobalServer (~21k: global missions, random daily quest, CCU) are only needed after town.
LoginServer does not connect to any other server; it accepts their connections.

## Plan (one branch per step)

1. ~~**`server/game-import`**~~: this step.
2. ~~**`server/game-build`**~~ (2026-10-30): build without warnings in GCC, Clang, clang-cl, using same `main` (`KServerMain`) and
   server hooks (`g_szConfigFile`, `CreateDBThread`). Larger than ChannelServer (15× the code); may span multiple branches.
3. ~~**`server/game-run`**~~ (2026-10-31): `x2_run_game` (`runtime/GameServer`, `Game01`) and GameServer to ready state, with client
   connecting. Login stops at `EGS_VERIFY_ACCOUNT_REQ` until LoginServer exists
   ([Dependencies](#dependencies-on-other-servers-survey-from-2026-10-31)).
4. **`server/login-import`**, **`server/login-build`**, and **`server/login-run`**: LoginServer, like ChannelServer and GameServer.
   Then client goes from character selection to town. CenterServer and GlobalServer follow (dungeon, PvP, parties).
