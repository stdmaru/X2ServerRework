[Home](../README.md) › [Porting](README.md) › Server: LoginServer

# Server: LoginServer (`src/server/LoginServer`)

Central account server: authenticates users entering GameServer (the `Account` database), maintains connected-user list
and account blocking, and stores data shared across the server group: guilds (skills, announcements, and board), rankings (including
Henir and local), personal-shop agency, buffs, wedding hall, random-item events, and CCU. Accepts connections from GameServers;
does not connect to any other server. Source: `KncWX2Server/LoginServer`, imported on 2026-10-31 with
[server flags](../tools/flags.md) ([report](../../tools/import/reports/server-login/REPORT.md)). Depends on the ported
[Common](server-common.md) library. The port preserves the original directory layout (`src/server/LoginServer` beside
`src/server/Common`).

## Import (2026-10-31)

- **Selection**: files belonging to `LoginServer/` compiled by `LoginServer.vcxproj` (`Release_US`) and their headers:
  **86 files** (44 `.cpp`, 42 `.h`), 31k written lines, 27k in `.cpp`. Largest: `LoginUser.cpp` (4.6k),
  `LoginGameDBThread.cpp` (2.5k), `Guild.cpp` (2.1k). `Main.cpp` in folder is not in project. `Common` includes are not followed (`include_scope`).
- **Flags**: resolved 1,110 `#if` blocks, removed 11k lines. No block became `X2_DEV_TOOLS`.
- **Nothing excluded during import**, as with GameServer: publisher/legacy code is removed at build time, with changes recorded here.
- Encoding: 76 CP949, 10 ASCII.

## Dependencies (2026-10-31)

| Area | Files/usages | Plan |
|---|---:|---|
| Nexon (KR): `NexonAuth*` (PC-bang/TCP authentication), `NexonBillingAuth*`, `AuthCheck` (`authcheck.lib`, proprietary binary: `AuthCheck_SetPassportLevel`/`SetLocale` in `OnServerReadyComplete`) | 12 files; `SiKNexonAuthManager` in `UserList` and `LoginSimLayer` | Publisher (T10): remove or stub. US authenticates through `ELG_NEXON_USER_AUTHENTICATE_REQ` against `Account` (`SERV_GLOBAL_AUTH` branch), not Nexon |
| Country files (`OnlyGlobal/LoginUser_CN`, `_TH`, `UserList_JP`, `_TH`, `_TW`) | 5 | after flags they contain only includes: remove |
| SMS / Mail / NXWeb (`LoginSMSDBThread`, `LoginNXWebDBThread`, `Common/Mail`, `SMSPhoneNumberManager`) | 2 + `Common` | legacy: verify whether US build uses them; `LoginNXWebDBThread` is already empty |
| KSMS (`ProcessCommunicationManager`, *named pipes*), `DLLManager` | `LoginServer.cpp`, `LoginSimLayer.cpp` | remove KSMS as in ChannelServer and GameServer; verify `DLLManager` (DLL loading) |
| Boost | 28 files (`wformat`/`str` 120, `shared_ptr` 45, `timer` 13, `random` 10, `bind` 2) | mechanical pass ([Mechanical porting](../tools/mechanical-porting.md)); handle `random`/`bind` manually |
| `windows.h`/Winsock/ATL | 11 | `x2/platform/win32.h`, `net.h` |
| `CTime`, `lua_tinker` | 25, 10 | already covered by [Platform](platform.md) and KNCSDK |
| Database | 119 `DO_QUERY` | `KODBC` already ported; procedures are in `Account`, `Game01`, and `Log` |

## Status (2026-10-31, branch `server/login-run`)

**Runs on Linux** from the directory assembled by `x2_run_login` ([Server package](../tools/runtime.md)): connects to four databases,
loads scripts, authenticates to DB (UID 58, "Login"), and reaches ready state; GameServer connects to it
(`ELG_VERIFY_SERVER_CONNECT`). Exits with code 0 at end of stdin.

**Tested with original client** (Wine, 2026-10-31, with EPublisherServer, ChannelServer, and GameServer): login, character list,
entering channel, and **reaching town**. When leaving for a field, client stops: `ERM_OPEN_BATTLE_FIELD_REQ` goes to CenterServer,
which did not yet exist. Packets for GlobalServer (`EGB_*`: CCU, missions, daily quest) are also discarded without hanging.

### Build (2026-10-31, branch `server/login-build`)

**Builds and links without warnings** (target `x2_login_server`, executable `LoginServer`, `main` uses `KServerMain` like ChannelServer
and GameServer) with Clang Debug, GCC Debug, Clang Release, GCC Release, and clang-cl (Windows test VM); all 6 tests pass
on Linux and Windows.

Build effort (Clang Debug, `-k 0`): 103 errors in 15 files after mechanical pass, 51 after removing Nexon; 39 GCC warnings
(11 non-virtual destructors). Much smaller than GameServer: most platform coupling was already removed in `Common`.

## Changes from the original

| Where | Change | Reason |
|---|---|---|
| Nexon (KR): `NexonAuth*`, `NexonBillingAuth*`, `AuthCheck` | Removed files. Also remove `AF_NEXON_KOREA` branches from `LoginSimLayer` (`NXAuthManager` thread, `Tick`, `ShutDown`, `RegToLua`) and `KUserList` (`NexonAuthLogin`/`Logout` on user registration and logout), PC-bang session sync and timeout (`CheckAuthSynchronize`, `CheckUnauthorizedSession`, `LoginTimeOut`, `UpdateSessionNoByUserID`), `ENX_AUTH_*` handlers in `KLoginServer`, `ELG_NEXON_USER_LOGIN_TIME_OUT_NOT` in `KLoginUser`, and unused `AuthenticateUser`. `PC_NX_AUTH` route only logs the error original already logged outside Korea | T10. In US (`AF_GLOBAL_SERVICE`) this was dead code: all depended on `AF_NEXON_KOREA`, and GameServer sends timeout only after Nexon PC-bang login. `AuthCheck` is a source-less proprietary binary ([Proprietary binaries](../original/proprietary-binaries.md)). Keep `ELG_NEXON_USER_AUTHENTICATE_REQ`, used in US (Nexon name, auth is against `Account`) |
| `KLoginServer::OnServerReadyComplete` | Remove `AuthCheck_SetPassportLevel`/`SetLocale`. Lua `Server:SetPassportLevel` remains accepted but has no effect | Same as above |
| `ELG_PRESENT_CASH_ITEM_REQ` (`LoginUser.cpp`) | Always responds `ERR_BUY_CASH_ITEM_21` (wrong password) | Password was checked with `AuthCheck` hash. No US GameServer sends this request |
| `OnlyGlobal/` (`LoginUser_CN`, `_TH`, `UserList_JP`, `_TH`, `_TW`) | Remove files | Other countries; after flags they contained only includes |
| `KLoginServer` | Remove KSMS calls (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\LoginServer*`, reconnected every 60 s in `Tick`, and script-reloaded notice) | As in [ChannelServer](server-channel.md#changes-from-the-original) and [GameServer](server-game.md#changes-from-the-original) |
| `KRankingManager` (`RefreshTimeBackup.ini`) | Use `std::filesystem::current_path()` instead of `GetCurrentDirectoryW` + `\\`; write with new platform `WritePrivateProfileStringW` (`WriteIniString`), which writes UTF-8 | Linux lacks API; file contains only ASCII numbers |
| `*DBThread.h` | `ProcessEvent` is no longer `virtual inline` | Defined in `.cpp`: ODR-invalid (`-Wundefined-inline` from clang-cl), as in ChannelServer |
| `ON_DBE_SERVER_ON_ACK` (`LoginServer.cpp`) | When DB rejects server, response calls `SetAuthenticateResult( false )` and `Init` fails; when accepted, sets `true` | As in [GameServer](server-game.md#changes-from-the-original): original called `::exit( 0 )` on that thread; without `true`, ported `Common` `Authenticate` rejected server |

Portability (no behavior change): mechanical pass (`mechanical.py`); `boost::variate_generator` → lambda over
`std::uniform_int_distribution` (OTP, as in ChannelServer); declarations skipped by `goto end_proc` moved upward (`fix_goto.py`, plus
4 manual); includes required by templates but omitted by MSVC (`BaseServer.h` in DB threads, `ActorManager.h` in `LoginServer.h`);
`virtual` destructors in 9 singletons; `using KThreadManager::Init` in `KLoginScriptRefreshManager`; removed dead variables and no-effect
expressions; `memset` on `wchar_t` buffer with `sizeof` (only zeroed half, harmless because reading terminates string); removed guild
announcement `unsigned < 0` check (also always false in MSVC).

### Original bugs fixed

| Where | Bug | Fix |
|---|---|---|
| `KGuildSkillTree::Init` | Guild skill level above maximum was corrected in an unused local variable: skill was inserted at DB level, only an error was logged | Insert skill at capped level (2026-10-31, GCC `-Wunused-but-set-variable`) |
| `KUserList` (`UserList.h`) | `#pragma pack(1)` covered entire class, misaligning `m_csConcurrentUser`; `pthread_mutex_lock` raised `SIGBUS` on ARM64 while registering user | Removed packing from class (it is not serialized on network); test verifies its alignment preserves `KncCriticalSection` alignment |

## Pending

- SMS (`LoginSMSDBThread`, `SMSPhoneNumberManager`) and `DLLManager` still build and remain: US config does not enable `DC_SMS`, and
  `DLLManager` only reads module list from DB. Verify when running.

## Plan (one branch per step)

1. ~~**`server/login-import`**~~ (2026-10-31).
2. ~~**`server/login-build`**~~ (2026-10-31): build without warnings with Clang, GCC, clang-cl; `main` uses `KServerMain` (as ChannelServer and GameServer).
3. ~~**`server/login-run`**~~ (2026-10-31): `runtime/LoginServer` (`config_lg_DEV.lua.in`), `x2_run_login`, and client login through GameServer:
   `ELG_NEXON_USER_AUTHENTICATE_REQ` (`P_MUser_SEL_Auth` in `Account`) and `ELG_REGISTER_USER_REQ` →
   `ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK`; then character selection and town
   ([GameServer dependencies](server-game.md#dependencies-on-other-servers-survey-from-2026-10-31)).
