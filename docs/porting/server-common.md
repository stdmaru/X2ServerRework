[Home](../README.md) › [Porting](README.md) › Server: Common

# Server: Common (`src/server/Common`)

Code shared by all five servers and used in part by the client (packet definitions, enums, and `EventID_*`).
Source: `KncWX2Server/Common`, imported on 2026-10-29 with [server flags](../tools/flags.md)
([report](../../tools/import/reports/server-common/REPORT.md)). The port keeps the original folder name (`Common`) because
source files include headers as `"../Common/x.h"`.

## Import

- **Selection**: only `Common/` sources compiled by at least one server vcxproj (`Release_US`): 239 sources, plus included headers,
  for **414 files**. Of the 239 sources, 107 are used by all five servers and 89 by only one.
- **Flags**: resolved 6,627 `#if` blocks and removed 49,000 lines. Dev (`X2_DEV_TOOLS`): only logs `SERV_LOG_UNDEFINED_QUEST_TEMPLET` and `DELETE_INCORRECT_ERROR_LOG`.
- **Excluded**:

  | What | Reason |
  |---|---|
  | `ui/` (MFC) | replaced by terminal, T15 |
  | `OnlyGlobal/AuthAndBilling/<country>/` | country-specific publisher integrations, including US (K3RCombo, Payletter), replaced by DEB |
  | `OnlyGlobal/NetCafe`, `Nexon`, `SOAP` | publisher integrations |
  | `OnlyGlobal/ProcessCommuniationModule` | KSMS tool |
  | `CrashRpt`, `Mail/` | closed-source binaries (alert SMTP) |

  References to these, ~190 includes, become stubs or are removed in the manual port.
- **Retained: DEB ("Debelzak Custom Auth")**, enabled by `_DEB_AUTH` (Channel and Game in `Release_US`). **Not KOG code**: written by
  the project owner, along with a custom SOAP authentication server built from scratch. It provides authentication (`KDEBAuth`, SOAP
  client that calls `Login` on a configurable `AuthService`; gSOAP until T31) and billing (`KDEBBilling`) **used by this server**.
  In `#if` chains it replaces the US publisher. Included are `AuthAndBilling/DEB*`, gSOAP stubs in `AuthAndBilling/DEB/` (removed
  on 2026-10-31, T31), and `KncSendToPublisher.h`. Fix from 2026-10-29: an early version disabled `_DEB_AUTH`, assuming it was only a publisher integration.
- KNCSDK includes with corrected spelling (`KncSingleton.h` → `KNCSingleton.h`, `RTTI.H` → `RTTI.h`...) via `extern_dirs`.

## DEB client (T31)

`OnlyGlobal/AuthAndBilling/DEB/` contains two custom `x2::deb` components instead of gSOAP 2.7 (runtime `stdsoap2` + generated `AuthService` code):

| File | Role |
|---|---|
| `Soap.{h,cpp}` | SOAP 1.1 document/literal messages for `AuthService` (`urn:AuthService`): builds requests, responses, faults; reads the `Body` element and fields (`SoapMessage::Text/Int/UInt`; missing field becomes empty/0, as gSOAP did). [EPublisherServer](server-epublisher.md) uses the same functions on server side |
| `SoapClient.{h,cpp}` | One HTTP/1.1 `POST` per call over a new connection (`Connection: close`), blocking like gSOAP; reads until `Content-Length` or connection close; 10-second read timeout. `SOAP-ENV:Fault`, non-200 HTTP status, or network error return `nullopt` with a text reason |

Requests have the same bytes gSOAP wrote (request line, `Host`, `User-Agent: gSOAP/2.7`, `Content-Type`, `SOAPAction: ""`,
and envelope with `ns1`/`ns2` declarations), verified against a capture from the old client (test in
`EPublisherServer/tests/test_epublisher.cpp`). An `AuthService` designed for the gSOAP client still works. Address comes from Lua config,
as before (`DEBAuthManager:AddAuthServiceAddress`, `DEBBillingManager:AddWebServiceAddress`), and must be `http://host[:port][/path]`.

## Remaining dependencies (2026-10-29)

| Area | Files | Plan |
|---|---:|---|
| ~~gSOAP (`AuthAndBilling/DEB/`, `stdsoap2`)~~ | 3 | removed on 2026-10-31 (T31): custom SOAP client, see [DEB client](#deb-client-t31) |
| `windows.h`/ATL/WinInet/`stdafx.h` | ~35 | `x2/platform/win32.h`; assess whether WinInet (`KFtpWrapper`) is needed |
| Threads/events (`_beginthreadex`, `WaitForSingleObject`...) | 29 | covered by [Platform](platform.md) |
| `CTime`/`CTimeSpan`, `CString` | 46 / 10 | covered by Platform (extend `CString` if needed) |
| `__int64`, `__forceinline`, `__declspec` | 15 | Platform types/macros |
| Boost | 39 | `std` |
| Lua (`lua_tinker`, `luaL_*`) | 53 | `x2::kncsdk_lua` |
| **Sockets/IOCP** (`Socket/`) | 15 | portable networking (standalone Asio), retaining `KSession`/framing/crypto |
| ~~**ODBC** (`ODBC/`)~~ | 2 | done on 2026-10-30 (T17): [ODBC database](server-common-db.md) |
| **D3DX math** (`X2Data/XSLBattleField`, `LineMap/`, `CommonPacket.h`) | 6 (187 uses) | `x2/platform/d3dx_math.h` (ready); replace `#include <d3dx9math.h>` |
| ~~Inverted dependency~~ | 1 | resolved on 2026-10-30 (`server/channel-build`): `KStatisticsLocalLog.cpp` no longer includes `GameServer/Inventory.h` or `CenterServer/PvPRoom.h` (see Changes) |

Size: ~99,000 lines of `.cpp` excluding gSOAP stubs (removed in T31). Largest are packet files: `ServerPacket.cpp` (16k), `ClientPacket.cpp` (12k), and `CommonPacket.cpp` (7k).

## Manual port plan (one branch per step)

1. ~~**`platform/math`**~~: done on 2026-10-29 (`x2/platform/d3dx_math.h`, see [Platform](platform.md)).
2. ~~**`server/common-build`**~~: done on 2026-10-29 (see Status below). CMake target with all `Common`, except `Socket/` and `ODBC/`, building under GCC/Clang.
   Mechanical pass: platform includes, `typename`/`template`, Boost → `std`, stubs for removed integrations.
3. ~~**`server/common-packets`**~~: done on 2026-10-29 (see Tests below). Serialization tests for packets (`ClientPacket`/`CommonPacket`), basis of compatibility (T4).
4. ~~**`server/common-net`**~~: done on 2026-10-29. TCP `Socket/` on Asio (T20), see Networking below.
5. ~~**`server/common-udp`**~~: done on 2026-10-29. UDP `Socket/` (`KTDNUDP`, `UdpEcho`, `LBSUdpEcho`, `X2Packet`), see Networking below.
6. ~~**`server/common-db`**~~: done on 2026-10-30. Portable `ODBC/` (T17), tested against SQL Server with FreeTDS: [ODBC database](server-common-db.md).

Next came ChannelServer (`server/channel`).

## Status (2026-10-29, branch `server/common-build`)

Target `x2_server_common` (all `Common` except `.cpp` in `Socket/`, `ODBC/`, `KStatisticsLocalLog.cpp` (added on 2026-10-30), and `DEBBilling*`)
**builds on Linux without errors or warnings** (GCC 16 Debug/Release, Clang 23 Release). `Socket/` (TCP and UDP) has built and been tested
since `server/common-net`/`server/common-udp`, and `ODBC/` since `server/common-db` (2026-10-30, see [ODBC database](server-common-db.md)).
At the time, `Common` had **no tests** (added in `server/common-packets`) and had not been run.
With **MinGW**, `Common` and its tests build without warnings and pass under Wine since 2026-10-30 (LuaJIT via T21). This was the first
build of `Common` `_WIN32` paths (Asio over IOCP, `ConfigureUdpSocket`, `odbc32`); remaining issues are under Changes › Portability.

Progress: ~15,000 errors after import → ~4,000 (mechanical pass) → 2,291 at start of resumed work → 0. Then 81,000 warnings
(73,000 from one header) → 0. Tools used: [Mechanical porting](../tools/mechanical-porting.md).

## Networking (`Socket/`, TCP)

IOCP engine replaced with `x2::platform::net` (decision T20, see [Platform](platform.md)); API used by the rest of the server remains the same.

| Original | Now |
|---|---|
| `KIocp`: completion port + `GetQueuedCompletionStatus` | `KIocp`: an `IoService`; completion keys and `OnIOCompleted`/`OnClosedByLocal` dispatch remain |
| `KIOThread::Loop`: waits for a completion packet | runs ready handlers for up to 100 ms (`IoService::RunFor`) |
| `KSocketObject`: `SOCKET` + overlapped `WSARecv`/`WSASend` | `TcpStream` + async read/write; handler keeps object alive until operation completes |
| No-IOCP mode (`KSession::Init( false )`): events in `OVERLAPPED` | handler stores result in `KOVERLAPPED` and signals same event (under `m_csSock`); session thread calls `OnIOCompleted`. Owners call `DetachIOEvents()` before `CloseHandle`: the aborted operations of the closed socket complete later on an I/O thread and are dropped. Without it they signaled a freed event, a crash on Linux, where `HANDLE` is a pointer (found by CI on 2026-11-05, ~23% of `TCP relay framing` runs on one core) |
| `KAccepter`: `WSASocket` + blocking `accept` | `socket` + blocking `accept`; accepted socket is passed to `TcpStream` |

UDP (`server/common-udp`):

| Original | Now |
|---|---|
| `CKTDNUDP`: up to 512 pending `WSARecvFrom` and `WSASendTo`, with *completion routines* (APCs) run during alertable wait on owning thread | nonblocking socket: loop over `recvfrom` (up to 512 per call) and direct `sendto`, each followed by the same handling as the original routine on the same thread |
| `KLBSUdpEcho::Run`: `WaitForSingleObjectEx( kill, INFINITE, TRUE )` | waits for socket data (up to 10 ms) and checks stop event |
| `KUdpEcho::Loop`: blocking `recvfrom`; `ShutDown` closed socket to unblock it | waits up to 100 ms for data before `recvfrom`; `ShutDown` stops thread, then closes socket (closing does not unblock `recvfrom` on Linux) |
| `SIO_UDP_CONNRESET`, `UDP_CHECKSUM_COVERAGE` | `x2::platform::net::ConfigureUdpSocket` (Windows only) |

- `X2Lib/X2UDPPacket.*` (client datagram structs) came through importer module `server-x2lib`, under `src/server/X2Lib`
  (the same relative path as in the original). The two `u_long` fields in `KXPT_PORT_CHECK_REQ/ACK` became `std::uint32_t`: on Linux they
  would be 8 bytes and change the datagram. `static_assert` checks sizes. `X2OPTIMIZE_NPC_NONHOST_SIMULATION` (undefined for the
  original servers, true for the client) was resolved as true by hand at the client import (2026-11-05,
  [Client](client.md#code-shared-with-the-servers-t33)).
- Declarations with `__forceinline` whose bodies are in `.cpp` lost `__forceinline` (GCC rejects it).
- `UdpRelayChecker/`: files become empty after US flags; nothing to port.

Intentional behavior changes:

- `~KIOThread` no longer posts `SIGNAL_SHUTDOWN_SERVER`: in original, that stopped *another* I/O thread, whichever picked up the packet.
- `~KIocp` stops I/O threads, closes registered sockets, and runs pending completions before destroying service.
- `KIocp::GenNewCompKey`: seeded generator (original used unseeded `mt19937`). Keys only need to be unique.
- `NetCommon::GetWsaMsg`: per-thread message (original used a shared static buffer); `GetLocalIP` returns `""` instead of `NULL` on failure.
- `KSimObject`: name (`SOB_<date>_<time>_<n>`) was `SOB_(null)_(null)_...` because original passed `_wstrdate_s` return code as a string.
- `OnSendCompleted` used `OVERLAPPED::InternalHigh` for sent-byte count; now uses completion byte count (same value).

## Network core shared with the client (T34)

Since 2026-11-05 (`client/serverprotocol`) the event/actor model and the TCP layer are their own library,
**`x2::common_net`** (`CommonNet.cmake`, included from `src/CMakeLists.txt` when the servers or the client are built):
`Event`, `Performer`, `SimObject`, `ThreadManager` and `Socket/` `CircularQueue`, `IOCP`, `IOThread`, `NetCommon`,
`Session`, `SocketObject`. `x2_server_common` leaves these files out and links it; the client's
[X2ServerProtocol](client-serverprotocol.md) links it instead of its own copy. Nothing in it may include a server-only
header.

- **Event names are per side.** `KEvent::ms_szEventID`, `GetIDStr()` and `IsValidEventID()` are not in the core's
  `Event.cpp`: the servers define them in `Common/EventNames.cpp` (every event), the client in its own `EventNames.cpp`
  (system and client events only), so the server-to-server event names (`EventID_Server.h`) do not ship with the game.
- `KSession( bIsProxy, bClientSide = false )`: the client side keeps its own `KSADatabase` and uses the `*_Local` crypto
  calls, as the client's copy did (details on the client page). Servers, including their proxies to other servers, are
  unchanged.
- The disconnection reasons moved from `Statistics.h` to `DisconnectReason.h` (`KDisconnectReason`, which `KStatistics`
  derives from: `KStatistics::eSIColDR_*` still works). `Socket/` used to include `Statistics.h`, and through it every
  packet header.
- `ThreadManager.cpp` no longer includes `ODBC/Odbc.h` (unused).

## Tests (`src/server/Common/tests`, target `x2_server_common_tests`)

| Test | Coverage |
|---|---|
| `test_packets_{client,common,server}.cpp` | **All 1,964 serialized types** (those under `#ifdef X2_DEBUG` only in that build) in packets (2026-10-29): repeated `Put` → `Get` on value-initialized object: size stays constant, bytes repeat from second pass (some constructors leave members uninitialized, so first pass may contain garbage), and `Get` reads all data. Generated by `tools/port/gen_packet_tests.py` ([Mechanical porting](../tools/mechanical-porting.md)), which first verifies `PUT` and `GET` list same fields in same order and each block uses only its own macro |
| `test_login_packets.cpp` | Exact bytes for `ECH_VERIFY_ACCOUNT_REQ/ACK` and `EGS_CONNECT_REQ/ACK`, hand-built from field order and [network format](../original/protocol.md) |
| `test_session.cpp` | `KSession` loopback with real I/O threads: accept, key exchange (`E_ACCEPT_CONNECTION_NOT`), encrypted packets both ways, 24 KB packet (more than one read), disconnect. Both proxy modes: on I/O threads and on its own thread (without IOCP) |
| `test_udp.cpp` | Datagrams between two `CKTDNUDP`s (ID + struct, checking sender address and port) and `XPT_PORT_CHECK_REQ` response from `KLBSUdpEcho` thread, which must exit without traffic |
| `test_world_line_maps.cpp` | `KWorldLineMaps` on fixture scripts: village → world → line map, a line map shared by two worlds named in different letter case, a missing world script, a square taken by world id and by name, a world outside villages and rooms skipped, a line's end read from its table |
| `test_square_unit_physics.cpp` | `KSquareUnitPhysics` on a fixture `SQUARE_Unit.lua`: `INIT_PHYSIC`, state IDs from `INIT_STATE`, speeds from `SPEED_*` and `PASSIVE_SPEED_*` with multipliers, `LAND_CONNECT`, a state with no table, a name outside the list (0) |
| `test_village_sync_packets.cpp` | The village sync packets (T53) keep negative, large and boundary values (tick near 2³², speed −910, sequence 255), a batch of three units, and a batch grows by a fixed size per unit, small enough for 35 visible units at 10 Hz (2026-11-19: 8 bytes plus 33 per unit, about 11.5 KB/s) |
| `test_village_move_validator.cpp` | `KVillageMoveValidator` on a fixture line map (floor, platforms at 300 and 1000, a jump line, a moving walkway) and the real unit numbers: walking and running accepted, too fast, unknown state, a tick that does not advance, a burst, a fast clock, standing in the air, flying, hovering past the jump's fall, jumps one after another whose landings fall between updates, hovering that pretends to land, a dash jump onto the platform at 300 and not to 1000, a jump line's throw in a line state, a walkway's speed, walls (a line end with nothing below, on the ground and in the air, a gap crossed between two updates, a closed end, an `LT_WALL` line, passing above it), correction grace, `Reset`, the operator factor, `MaxHeight` |
| `test_field_unit_snapshots.cpp` | `KFieldServerClock` (offset, a slower batch moves it 1 ms, a faster one is taken, `ToLocal`, wrapping clocks) and `KFieldUnitSnapshots` (empty, a single state, interpolation, the state reached so far, older states ignored, going on 150 ms at most, staying after a stop, pruning, standing then moving, teleports, the size limit) |
| `test_field_move_sync.cpp` | `KFieldMoveSync` on a fixture floor: entering (above the floor, outside the map put on the nearest line), batches (nothing to a viewer still loading, never a unit to itself, only the latest state, the time it was accepted, nothing when nothing changed, a unit coming into view sent once, only to the viewers named, a server move), corrections (the last valid state, nothing to viewers, updates made before the correction dropped, violations counted in a window, a kick), the operator factor, shared batches (a unit entering goes to all and gets those already there, a moved unit only in the shared batch), stamps 100 ms apart however late the states arrive and never after they arrived |
| `test_odbc.cpp` | `KODBC`: `.dsn` conversion and filters without DB; `db` suite against real SQL Server when `X2_TEST_ODBC` is set. See [ODBC database](server-common-db.md#tests) |
| `server_hooks.cpp` | Not a test: defines what each server supplies to `Common` (`g_szConfigFile`, `CreateDBThread`) so tests can link without a server. In executables these come from server (e.g. `ChannelServer.cpp`) |
| `packets_generated` (ctest) | Fails if generated tests are out of date with packet `.cpp` files |

**Limit:** tests prove the port is self-consistent and matches the format read from code, not that it matches `elsword.exe` byte for byte.
That remains pending ([KNCSDK › Pending](kncsdk.md#pending)). They also cannot detect a field present on server but not client
(client flags have not been generated yet; see [Roadmap](../project/roadmap.md)).

## Changes from the original

### Intentional

| Where | Change | Reason |
|---|---|---|
| File paths (2026-10-30) | `"Statistics\\..."`, `"Config\\US\\"`, and `%Y-%m-%d\\` backup path use `/`; statistics backup goes to `StatisticsBackup/<date>/` **next to server**; `KAutoPath::AddPath` changes `\\` to `/` outside Windows | On Linux `\\` became part of the name (folders like `C:\StatisticsBackup\` inside server directory). Original wrote to `C:\StatisticsBackup\`, unavailable outside Windows; `/` works on both |
| `KStatisticsLocalLog` (`Statistics/SI_LOG_*.txt`) | Files are **UTF-8** (`x2::platform::Utf8FileLocale`), `Statistics/` path uses `/`, existence checked with `std::filesystem::exists` | Original used `std::locale("korean")` (CP949, MSVC only) and `_stat`. Old CP949 files with Korean header text are not read identically; numbers are ASCII |
| `KAutoPath`, `KFolderTraverse`, statistics | `FindFirstFile`/`GetCurrentDirectory`/`CopyFile`... → `std::filesystem`. `KApplyAllFiles` passes `directory_entry` to functor and no longer changes current directory | Portable. Script search order remains the same (current folder, then subfolders depth-first, by name) |
| `KAutoPath::GetPullPath` (2026-11-16, `server/script-load-errors`) | After the exact name fails in every searched folder, a second pass finds it **ignoring ASCII letter case** (`x2::platform::FindFileIgnoreCase`) | Windows finds names in any case and the data relies on it: `DungeonData.lua` names `Dungeon_Training_LIRE_0.lua`, `Dungeon_Besma_MINE2_Normal.lua`, `Field_Velder_Stage2_LineMap.lua`..., the client ships `..._lire_0.lua`, `..._Mine2_...`, `..._Linemap.lua`; on Linux 24 dungeon scripts and 2 line maps failed to load. Exact names are tried first so found files cost no directory scans; differs from Windows only if two folders hold names differing in case (none do) |
| `KFolderTraverse` | Original skipped first `FindFirstFile` entry (usually `.`); now no entries are skipped | `std::filesystem` does not list `.`/`..` |
| `BaseServer::LoadINIFile`, `KStringTable` | INI read through platform reader on **all** platforms, accepting UTF-16, UTF-8, and **CP949** | Production `config_*.ini` files are CP949; Windows API uses system code page, which is 949 only on Korean Windows |
| `XSLCheckSumManager` | CMarkup → TinyXml (already in `Common`); remove `XML/Markup.*`. Duplicate-checksum `MessageBox` becomes error log | CMarkup license requires permission for commercial use; server has no window |
| `BaseServer` | Remove CrashRpt (handler install and callback calling `DumpUploader.exe`, Release only) | Closed-source binary, already excluded from import |
| `BaseServer` (email) | `KMailManager` removed (2026-10-31). Until then it was a port stub that sent email to log. Only email was script parse-error report, enabled by `SetScriptParsingErrReport( true )` and `AddScriptParsingErrReportMail`. Neither function was used by any config, including `_reference`; both were removed along with `SetMailingList`. Now if a script fails, `OnServerReadyComplete` always logs an error with the list. Keep `KEMAIL_SEND_EMAIL_NOT` packet and `EMAIL_SEND_EMAIL_NOT` to avoid shifting event IDs | Delivery used closed DLLs (`MailSMTP`/`MailMIME`), and servers do not send email |
| `BaseServer::Run` | [Server console](console.md) (`KServerConsole`: T15 terminal, T30 protocol), opened by `KServerMain` before `Init` (`SetConsole`); `--no-stdin`, `--machine`, `--control=<name>`, and `--no-control` belong to console, not server | Original came from modified `lua.c`; MFC window had its own command field |
| `RegToLua` | New `consolelog( n )`: console log level, independent of file `log( n )` (T16) | Console has its own level (see [KNCSDK › Logging](kncsdk.md#logging-t16)) |
| `NetError` | Message tables (`szErrorStr*`) exist only in `NetError.cpp`; `GetErrStr*` return `const wchar_t*` | They were `static` in header, copied into every `.cpp` (73k warnings). All callers copy into `std::wstring` |
| `ActorManager::MultiQueueing*` | Default argument `std::vector<UidType>& = std::vector<UidType>()` replaced with one-argument overload | MSVC extension. **Pending:** `GameServer/UserManager.h` has same pattern |
| `ActorProxy`/`BaseServer` | Event `std::shared_ptr<void>` → `shared_ptr<remove_pointer_t<HANDLE>>` | Outside Windows, `HANDLE` is not `void*` |
| `KDEBAuth`, `KDEBBilling` (`AuthAndBilling/`) | gSOAP removed (T31): `soap_call___ns1__*` calls replaced by `x2::deb::SoapClient::Call` with same elements and fields. No `SoapInitialize`/`SoapDestroy` (no state between calls). Network/SOAP failure is now logged with address and reason, including balance (`EPUBLISHER_BILLING_BALANCE_REQ`), which previously failed silently. See [DEB client](#deb-client-t31) | gSOAP was third-party code copied from `_reference` (17k lines compiled with `-w`) |
| `DBConnectionManager` (DES connection strings) | `x2::crypto::DesCbc*` instead of Crypto++ `CBC_Mode<DES>` (T27); same bytes. Removed unused XOR `EncryptString`/`DecryptString` | Crypto++ does not compile with clang-cl |
| `Socket/` (`SocketObject`, `NetCommon`, `UdpEcho`, `KTDNUDP`), `NetLayer`, `ThreadStatisticsManager` (and EPublisherServer) | `inet_addr`/`inet_ntoa`/`gethostbyname` → `x2::platform::IpFromString`/`IpToString`/`ResolveIPv4` (`inet_pton`/`inet_ntop`/`getaddrinfo`). Accepts only dotted-quad IPv4 (`inet_addr` accepted `127.1` and hex) | Obsolete Winsock APIs (clang-cl warnings). `ThreadStatisticsManager` no longer crashes if machine name does not resolve (original dereferenced `NULL` from `gethostbyname`) |
| `LineMap` | Removed `LPDIRECT3DVERTEXBUFFER9` member | Client leftover, never used in server |
| `CXSLLineMap::LoadLineMap` (2026-11-19, `server/village-sync`, T53) | Runs a line map script in a given `lua_State` (binding `g_pLineMap` with `AddLine_LUA`, `AddStartPos`, `SetTeamStartPos`, `SetLandHeight`, `AddCameraData_LUA`) and builds the line groups; `AddLine_LUA` reads that state (else `g_pLua`); the vectors of a line, camera, line animation and rect (`START_POS`, `END_POS`, ...) are read from the table being loaded, zero when missing, as the client does; the destructor is public; `GetNumLineData` is `const` and `GetLineData` has a `const` overload | The original's `LoadLineMap` was a stub with the loading commented out, so the servers had no line maps; the original also read those vectors with `lua_tinker::get`, which reads globals, so no line could load (the code never ran). Needed to validate village movement |
| `KWorldLineMaps` (`LineMap/WorldLineMaps.*`, new, 2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, the GameServer and the CenterServer load at start the line map of every village, dungeon gate and lounge, battle field rest area, square and wedding hall, following the client's chain: `DLG_Map_Village.lua` (`WORLD_ID`) → `WorldData.lua` (`dataFileName`) → the world script's `g_pWorld:SetLineMap` → the line map script. The village list and the world scripts run in a scratch Lua state where what the server lacks (strings, meshes) does nothing; each line map runs in a clean one with `Enum.lua`. Worlds naming the same line map share it. Squares and wedding halls are found by world name (`GetByWorldName`) | See [Village movement sync](../original/village-sync.md#line-maps-on-the-servers-2026-11-19) |
| `KSquareUnitPhysics` (`LineMap/SquareUnitPhysics.*`, new, 2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, the GameServer and the CenterServer read at start how a village unit moves from the client's `SQUARE_Unit.lua`, as `CX2SquareUnit` does: `INIT_PHYSIC`, and each state of `INIT_STATE` (ID = its index) with its fastest speeds and `LAND_CONNECT`. The state functions are not run. `KClientScriptState` (`LineMap/ClientScriptState.*`) is the scratch Lua state both loaders run the client's scripts in | See [Village movement sync](../original/village-sync.md#movement-limits-2026-11-19) |
| Village sync packets (`ClientPacket.h`, `EventID_Client.h`, new, 2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, three client events at the end of the client enum (before `EGS_CLIENT_EVENT_ID_END`, so no existing ID moves): `EGS_FIELD_UNIT_STATE_NOT` (client → server: client tick in ms and a `KFieldUnitState`), `EGS_FIELD_UNIT_STATE_BATCH_NOT` (server → client, once per field tick: server tick and, per changed visible unit, UID, the server tick it was accepted and its state) and `EGS_FIELD_UNIT_CORRECTION_NOT` (server → sender: the rejected tick, the last valid state and a `REASON`). `KFieldUnitState`: position (`VECTOR3`), speeds X/Y in units per second (`int16_t`), last touched line (`int16_t`, −1 none), state ID (`INIT_STATE` index), a state change counter and the facing. Units still enter and leave the view with `EGS_CHANGE_USER_LIST_IN_SECTOR_NOT` | Replace the input replay of `EGS_FIELD_UNIT_SYNC_DATA_NOT` ([Village movement sync](../original/village-sync.md)); not understood by `x2.exe` (T53) |
| `KVillageMoveValidator` (`LineMap/VillageMoveValidator.*`, new, 2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, one per village unit: checks each `EGS_FIELD_UNIT_STATE_NOT` against the world's line map, `KSquareUnitPhysics` and the last accepted state, by the client's tick (whose clock may not lead the server's by more than 500 ms). Rejects a burst or a tick that does not advance (`R_RATE`, token bucket of 20/s, burst 10), an unknown state (`R_STATE`), moving faster than the fastest speed × 1.2 + 50 (the line's `SPEED` and a jump line's launch count; `R_SPEED`), a state with `LAND_CONNECT` away from every line (40 units) unless a jump line threw it (`R_OFF_LINE`), and being higher above the ground it left than the fastest jump allows at that time + 60 (`R_RISE`, with the takeoff anywhere between the last sample on the ground and the first in the air; a unit falling at one update and rising at the next landed and jumped again in between, from the highest line below the last update, when that line is within the fall the last update's speed allows + 60: jumping again at once lands between updates, and every such jump was rolled back until 2026-11-19). Rejects the steps the client's walls stop (`R_WALL`): no line below within 60 anywhere on the way (sampled every 60 units), past a closed (−2) end of the line it last stood on by more than 60, or across an `LT_WALL` line (`CXSLLineMap::GetWallList`, new: the servers keep walls out of the line list). On 2026-11-19, dashing along every line of every village, into its next one, raised no rejection (15,990 steps). Rejections within 1 s of a correction are not new violations. `Reset` takes a position the server sets. No I/O; `X2_DEBUG` operators get a speed factor | The user's requirement that the server stop flying and standing in the air (T53); the original trusted the client |
| `KFieldMoveSync` (`LineMap/FieldMoveSync.*`, new, 2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, the village movement of one field or room, with no I/O: a `KVillageMoveValidator` and the last accepted state per unit, and once per tick (100 ms) a `KEGS_FIELD_UNIT_STATE_BATCH_NOT` per viewer with the latest state of each changed unit it sees (the caller names who sees whom: the sectors). A unit coming into view is sent once though it did not move (`ShowTo`, `ShowAllTo`). Where everyone sees everyone (a square), `BuildSharedBatch` makes one batch for all and, per newcomer, one with the units it starts to see. A unit entering where it has no line below is put on the nearest line and corrected. A new violation gives a correction with the last valid state; `SetKickViolations` violations within a minute kick. `X2_DEBUG` operators move 6 times faster. A state is stamped with when the client made it, on the server's clock (a `KFieldServerClock` per unit, fed the client's ticks: the arrival with the least delay), never after it arrived: stamped on arrival (until 2026-11-19), the hops on the way (the GameServer's user queue, a CenterServer room's 20 ms tick) moved samples made 100 ms apart by tens of ms and the others saw the unit speed up and slow down, worst in squares | Shared by the GameServer's fields and, later, the CenterServer's rooms ([Server: GameServer](server-game.md#changes-from-the-original)) |
| Village sync server events (`EventID_Server.h`, `ServerPacket.h`, 2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, at the end of the server enum: `ERM_FIELD_UNIT_STATE_NOT` (GSUser → field, the client's packet), `ERM_FIELD_UNIT_CORRECTION_NOT` (field → GSUser: the correction, the violations in the window and whether to kick) `ERM_SQUARE_UNIT_STATE_NOT` (GSUser → CenterServer square: the client's packet and whether the user is an `X2_DEBUG` operator), `ERM_FIELD_UNIT_STATE_BATCH_NOT` (square → GSUser: a batch; the correction is the field's event) and `ETR_FIELD_UNIT_STATE_BATCH_NOT` (field → TCP relay: one viewer and its batch); `KERM_JOIN_FIELD_REQ::m_bFastMove` (an `X2_DEBUG` operator). `ServerPacket.h` includes `ClientPacket.h` for them | |
| `KGameSysVal::SetVillageMoveKickViolations` (2026-11-19, T53) | Village movement violations within a minute that kick the user (default 20, 0 never), set in `GameSysValTable.lua`; registered with or without `X2_VILLAGE_SYNC`, so the script runs either way | lua_tinker raises an error for a method a class lacks, so the script cannot test for it |
| `KFieldServerClock`, `KFieldUnitSnapshots` (`LineMap/FieldUnitSnapshots.h`, new, 2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, header only, for the client: the server's clock estimated from the batches (the offset with the least delay, following a slower clock by 1 ms per batch: following it by 1/16 of the difference, until 2026-11-19, made the time drawn jump ahead at every quick batch and slow down between them, a constant jitter) and, per remote unit, the accepted states drawn 200 ms in the past. Between two states the position is interpolated; a gap longer than 1.5 ticks is standing and then one tick of moving; farther than 300 units in one step is a jump, not a slide; past the newest state a moving unit goes on for 150 ms at most, one that stopped stays | Lives in `Common` so the tests cover it; the client includes it ([Client: X2Lib](client-x2lib.md#village-movement-sync-2026-11-19-servervillage-sync-t53)). 200 ms, not T53's ~100 ms: the server sends a unit's newest state once per tick and the client updates it at that rate, so the newest state is up to two ticks old when it arrives |
| `KNetLayer` accepter (2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, the client connections disable Nagle (`TCP_NODELAY`), as the TCP relay's (port 9301) already did | Movement updates and corrections are small and frequent; Nagle held them for up to the next acknowledgement |
| `KSession::Connect` (2026-11-19, T53) | Under `X2_VILLAGE_SYNC`, outgoing connections disable Nagle too: the client's to its servers and the servers' to each other | The client's movement updates at 10 Hz; the client's TCP relay connection already had it |
| `CXSLLineMap::LINE_TYPE` (2026-11-19, `server/village-sync`) | Explicit values 0–10 matching `Enum.lua` (`LT_MONSTER_ROAD` 7, `LT_UNIT_ROAD` 8, `LT_OTHER_ROAD` 9, `LT_CLIFF` 10) | The original's ifdefs left `LT_CLIFF` at 7 on the servers against 10 in the scripts, so the servers read line types 7–10 wrong (they never loaded lines, so it never mattered) |
| `Socket/Overlapped.h` (2026-11-05, T34) | `MAX_PACKET_SIZE` 64 KB instead of 32 KB, and `KSkSession::OnRecvCompleted` no longer disconnects with `eSIColDR_RecvBufferFull` when a length exceeds it (a `USHORT` length always fits now) | One network core with the client, which used 64 KB. Each connection has 64 KB more buffers; packets of 32–64 KB from a client are now read instead of dropping the connection |
| `~KIocp` (2026-11-05) | Joins the I/O threads directly instead of calling `KThreadManager::EndThread( 3000 )` | `EndThread` waits for `m_nTerminateCount`, which only event-queue threads increment: it always waited the full 3 s at shutdown |

### Original bugs fixed

| Where | Bug | Fix |
|---|---|---|
| `KBuffFactor` (`CommonPacket.h`) | Copy constructor left `m_uidMesGameUnit` (buff caster) and `m_bIsMesGameUnitNpc` uninitialized; assignment copied them | Copy constructor now copies both (2026-10-30, found by GameServer `-Wdeprecated-copy`) |
| `KXPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL` (`X2UDPPacket.h`) | `operator=` assigned `m_fPosX/Y/Z` to themselves: position was not copied | Copy from `kRight` |
| `GetFrom` for `KEGIANT_BILLING_CONSUME_ACK` and `KELOG_ITEM_TRADE_ERROR_NOT` | `GET` block called `PUT` (copy/paste): wrote instead of reading and left rest of packet unread | Corrected to `GET`. First belongs to unused Giant integration; second is inter-server log whose date never arrived |
| `KSwapPerformer` | Inverted checks (`_JIF( !m_pEnqueueEvent, return )` returned with non-null pointer and continued only when null; destructor dereferenced when null) | Checks corrected. Class is unused by any server |
| `CXSLPetManager::*SpecialFeedIntimacyRate_LUA` | Validation `> 1 && < 0` was never true | `\|\|`. Current scripts use 0.0201 and 0.0501, so behavior is unchanged |
| `XSLCharacterStatTable` | Compared `UNIT_TYPE` with `UC_NONE` and `UNIT_CLASS` with `UT_NONE` | Correct enums (both equal 0, so no behavior change) |
| `XSLGuildManager` | `u_char < 0` | Removed (always false) |
| `SimObject`, `SerBuffer`, etc. | Minor: `%d` with `__int64`, uninitialized values | See diffs |
| `KDEBAuth` (`EPUBLISHER_AUTHENTICATION_REQ`) | `if( response.strPurchaseToken.empty() )`: purchase token copied only when empty | `!empty()` (2026-10-31). `wstrPurchTok` now arrives populated at ChannelServer |
| DEB gSOAP client | Non-ASCII UTF-8 text was encoded twice: gSOAP 2.7 without `SOAP_C_UTFSTRING` treated each byte as Latin-1. Account `é가` reached service as `Ã©ê°\u0080` | T31 client sends UTF-8 unchanged (2026-10-31, verified against gSOAP before removal) |
| `KSession::Begin` (`Socket/Session.cpp`) | Put `KThread`'s kill event into `m_hEvents[EVENT_DISCONNECTED]` only after `KThread::Begin()` had started the thread, and the slot was never initialized: the no-IOCP session thread could `WaitForMultipleObjects` on a garbage handle. Harmless on Windows (`WAIT_FAILED`), a crash on Linux, where `HANDLE` is a pointer (half of the GCC client test runs on 2026-11-05) | The slot is set in `KThread::OnBeforeThreadStart()`, before the thread starts, and initialized to `NULL` (2026-11-05, `client/kom-mass-file`) |

### Portability (no behavior change)

- Found while compiling ChannelServer (2026-10-30): `KStatisticsLocalLog.cpp` is compiled by all servers, so added to `x2_server_common`;
  it included unused `GameServer/Inventory.h` and `CenterServer/PvPRoom.h` only for `KPvPRoom::SP_NUM`, replaced by local constant
  14 (from `CenterServer/StatPvP_def.h`, to verify when CenterServer was ported). `IMPL_ON_FUNC*` (`Event.h`) again names `anTrace_`
  (with `[[maybe_unused]]`): warning pass had commented out the name, so `FIRST_SENDER_UID` stopped compiling in servers.
  `KActorManager::ForEach` takes `T&&` (MSVC accepted temporary with `T&`). `HackingProtectionManager.h` includes `BaseServer.h`,
  used by template body. `ServerMain.{h,cpp}`: common server `main` (see [ChannelServer](server-channel.md)).
- When running ChannelServer (2026-10-30): `KBaseServer::Authenticate` returns result that each server's `DBE_SERVER_ON_ACK` stores
  with `SetAuthenticateResult` (instead of original `::exit( 0 )`, see [ChannelServer › Changes](server-channel.md#changes-from-the-original));
  `NetCommon::SetLocalIP` (Lua `SetLocalIP`) sets address returned by `GetLocalIP`. Without this call, behavior is as original.
- Windows (found in first MinGW build, 2026-10-30): build now defines `UNICODE`/`_UNICODE`, like original (otherwise `LPCTSTR` was `const char*`);
  `byte` and `u_char`/`u_short`/`u_int`/`u_long`, formerly provided by `windows.h` without `WIN32_LEAN_AND_MEAN`, come from platform `types.h`;
  gSOAP included `x2/platform/net.h` instead of `winsock.h` it included indirectly (until T31); `zlib` became a public dependency of
  `x2_server_common` (`Socket/KTDNUDP.h` includes `zlib.h`).
- Packet `long`/`unsigned long` fields (190, as of 2026-10-29) became `LONG`/`DWORD`: on Linux `long` is 8 bytes, and `Put` failed
  for values outside 32-bit range. Wire format is unchanged (same size and tag), and Windows types are unchanged.
- Added `typename`/`template` that MSVC accepted, fixed `goto` crossing initialization (declarations moved to function start), gave enums
  explicit underlying type (`enum ePerformerID : int`), replaced `min`/`max` macros with `std::`, gave `KClearString` explicit copy
  constructor, added `virtual` destructors to polymorphic classes deleted by singletons, and ordered initializer lists by declaration.
- `KFtpWrapper` (rewritten as socket-based FTP client in prior pass): send loop could fail to terminate when a send failed. Fixed.

## Pending

- ~~**Paths with `\\`**~~: resolved on 2026-10-30 (see Intentional changes).
- ~~**Case-sensitive file names**~~: resolved on 2026-11-16 (`KAutoPath::GetPullPath`, see Intentional changes).
- `XSLCashItemManager::AddNotSellCashItemProductNo`: retained `!GetServerGroupID() != iServerGroupID`; likely should be `GetServerGroupID() != iServerGroupID`.
  Decide with user (changes which items appear in store).
- `KLottery` uses fixed seed (42) and shared generator without lock (decide with user).
- `KLogReporter::Send` logs send failure but returns `eLRError_OK` (retained).
- `NetError_def_*.h` for other languages: US build uses **`NetError_def_BR.h`** (enum and text);
  others can be removed, but BR enum order goes to client and must remain.
