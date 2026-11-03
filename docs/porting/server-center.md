[Home](../README.md) › [Porting](README.md) › Server: CenterServer

# Server: CenterServer (`src/server/CenterServer`)

Room server: battlefields (`BattleFieldRoom`), dungeons (`DungeonRoom`, including Henir and defense), PvP (`PvPRoom`, matches
and results), square (`Square`), parties (`NewCnParty`, party fever), trade (`Trade`), personal shop (`PersonalShop`), training center,
and wedding hall. It handles server-side simulation for monsters, drops, and room results. It accepts GameServer connections on port
9100; GameServers send it `ERM_*` messages (the first is `ERM_OPEN_BATTLE_FIELD_REQ`, sent when leaving town).
Source: `KncWX2Server/CenterServer`, imported on 2026-10-31 with [server flags](../tools/flags.md)
([report](../../tools/import/reports/server-center/REPORT.md)). Depends on the ported [Common](server-common.md) library.
The port preserves the original directory layout (`src/server/CenterServer` beside `src/server/Common`).

## Import (2026-10-31)

- **Selection**: files belonging to `CenterServer/` compiled by `CenterServer.vcxproj` (`Release_US`) and their headers:
  **123 files** (59 `.cpp`, 64 `.h`), 65k written lines, 57k in `.cpp`. Largest: `RoomUserManager.cpp` (7.2k),
  `DungeonRoom.cpp` (6.6k), `BattleFieldRoom.cpp` (4.9k). Folder's `Main.cpp` is not in project. `Common` includes are not followed (`include_scope`).
- **Flags**: resolved 2,389 `#if` blocks, removed 20k lines. No block became `X2_DEV_TOOLS`.
- **Nothing excluded during import**, as with GameServer and LoginServer: publisher/legacy code is removed at build, with changes
  documented here. Only `.dsn` files are omitted (runtime versions come from `runtime/`, T22).
- Encoding: 113 CP949, 10 ASCII.

## Dependencies (2026-10-31)

| Area | Files/usages | Plan |
|---|---:|---|
| KSMS (`ProcessCommunicationManager`, *named pipes*) | `CnServer.*`, `CnRMEventProcess.cpp` | remove, as in other servers |
| SMS (`CnSMSDBThread`, `DC_SMS`) and `SMSPhoneNumberManager` | 2 + `CnSimLayer`, `CnServer`, `CnPartyFSM`, `CnHackingCheckManager`, `CnScriptRefreshThread` | builds and remains, as in LoginServer (US config does not enable `DC_SMS`); verify when running |
| Anti-cheat, TBB, publisher | none (TBB appears only in comment in `CnServer.cpp`) | — |
| Boost | 31 files (`timer` 28, `wformat`/`str` 54, `shared_ptr` 9, `bind` 3) | mechanical pass ([Mechanical porting](../tools/mechanical-porting.md)); manual `bind` |
| `windows.h`/Winsock/ATL | 14 | `x2/platform/win32.h`, `net.h` |
| UDP (`InteriorUdpSession`, on top of `Common` `KTDNUDP`) | 2 | `KTDNUDP` already ported (`server/common-udp`) |
| `CTime`, `lua_tinker` | 16, 15 | already covered by [Platform](platform.md) and KNCSDK |
| Database | 50 `DO_QUERY` | `KODBC` already ported; procedures are in `Game01`, `Log`, and `Account` |

## Status (2026-10-31, branch `server/center-run`)

**Runs on Linux** from the directory assembled by `x2_run_center` ([Server package](../tools/runtime.md)): connects to three databases
(`Account`, `Game01`, `Statistics`), loads scripts (dungeon and line-map load errors already listed under
[Original bugs](../original/bugs.md#data-serverresource-clientscript)), authenticates to DB, and reaches ready state; GameServer
connects to it (port 9100). Exits with code 0 (see [Run the servers](../running.md)).

**Tested with original client** (Wine, 2026-10-31, with EPublisherServer, LoginServer, ChannelServer, and GameServer): leaving
town for a **field**, entering a **dungeon**, and switching characters all work without CenterServer errors. Packets to GlobalServer
(`EGB_*`: CCU, missions, daily quest, blocklist) continue to be discarded by GameServer without hanging. Character switching had
crashed GameServer (Debug) in TCP relay: caused by `KSerBuffer::UnCompress` for an event with no data, fixed in [KNCSDK](kncsdk.md).

### Build (2026-10-31, branch `server/center-build`)

**Builds and links without warnings** (target `x2_center_server`, executable `CenterServer`, `main` uses `KServerMain` like other
servers) with Clang Debug, GCC Debug, Clang Release, GCC Release, and clang-cl (Windows test VM); all 6 tests pass
on Linux and Windows.

Build effort: 51 errors (Clang Debug, `-k 0`) in 13 files after mechanical pass, 29 under GCC (almost all `goto end_proc`);
112 distinct Clang warnings and 143 GCC warnings before `fix_warnings.py` and manual fixes. Smaller than LoginServer: most
platform coupling had already been removed in `Common`.

## Changes from the original

| Where | Change | Reason |
|---|---|---|
| `KCnServer`, `CnRMEventProcess` | Remove KSMS calls (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\CenterServer*`, reconnected every 60 s in `Tick`, and script-reloaded notice) | As in [ChannelServer](server-channel.md#changes-from-the-original) and other servers |
| `ON_DBE_SERVER_ON_ACK` (`CnServer.cpp`) | When DB rejects server, response calls `SetAuthenticateResult( false )` and `Init` fails; when accepted, sets `true` | As in [GameServer](server-game.md#changes-from-the-original): original called `::exit( 0 )` on that thread |
| `KBattleFieldMonsterManager::SetMonsterDie( int, UidType )` | Renamed to `SetFieldMonsterDie` (four `KBattleFieldRoom` calls) | Hid virtual `KRoomMonsterManager::SetMonsterDie( int, UidType, bool = true )`; `using` would make two-argument calls ambiguous. All calls used concrete type: no behavior change |
| `Cn*DBThread.h` | `ProcessEvent` is no longer `virtual inline` | Defined in `.cpp`: ODR-invalid (`-Wundefined-inline` from clang-cl), as in LoginServer |

Portability (no behavior change): mechanical pass (`mechanical.py`); declarations skipped by `goto end_proc` moved upward
(`fix_goto.py`, updated to understand arrays initialized with `= {0,}`), and two `ITEM_DATA&` references in `ERM_GET_ITEM_REQ`
(field and dungeon) became pointers declared at function start; `boost::timer` → `x2::Timer`, including KNCSDK `Knc::KSingleObject`
(`KncContainer.h`, unused by servers until then); temporary default argument to non-`const` reference
(`KSquareUserManager::SetPersonalShopData`, MSVC extension) became an overload; remove `__noop`, `intsafe.h`, and `min`/`max` macros
(preserving original types: `size_t`, `__time64_t`, `char`); add `BaseServer.h` to DB-thread headers where templates need it;
`virtual` destructors in 8 singletons; `using` declarations for hidden `Init` and `SetRoomInfo` overloads; `[[fallthrough]]` in party close
(`S_* → I_TO_CLOSE → I_TO_INIT`, intentional); remove dead variables (still call `GetKillNPC_LUA()` for dungeon results: it sets
`m_nKillNPC` to 1); remove discarded `DUNGEON_TIME_RESULT` cast (rank comes from following `GetRank()`); remove `0 <= iRank` test
(`size_t`, also always true in MSVC); initialize `nGSUID`, `m_CheckClass`/`m_CheckType`, and `iCandidateTeam` to 0 (GCC Release
reported `-Wmaybe-uninitialized`; they were read only after assignment); KNCSDK `KncUtil::NumberShuffle` `erase` becomes `std::move` +
`pop_back`, same operation in same order, because GCC 15 Release reported false `-Wfree-nonheap-object` in expanded `erase`.

### Original bugs fixed

| Where | Bug | Fix |
|---|---|---|
| `KDropTable::AddExtraStageMonsterDropInfo_LUA` | Key validation was `m_nMonsterID == -1 && m_nMonsterID == 0`, always false: entry with no `MonsterID` (or 0) entered extra-stage drop table | `\|\|` (2026-10-31, Clang `-Wtautological-overlap-compare`) |

## Pending

- Retain `DungeonRoom` "sub-stage cleared in 1 s" check bug ([Original bugs](../original/bugs.md)).
- `CnSMSDBThread` builds and remains: US config does not enable `DC_SMS`. Verify when running.

## Plan (one branch per step)

1. ~~**`server/center-import`**~~ (2026-10-31).
2. ~~**`server/center-build`**~~ (2026-10-31): build without warnings with Clang, GCC, clang-cl; `main` uses `KServerMain` (as other servers).
3. ~~**`server/center-run`**~~ (2026-10-31): `runtime/CenterServer`, `x2_run_center`, and original client in field and dungeon. GameServer
   finds Center from DB list (`Statistics.LServerList`) and retries every 10 seconds
   ([GameServer dependencies](server-game.md#dependencies-on-other-servers-survey-from-2026-10-31)).
