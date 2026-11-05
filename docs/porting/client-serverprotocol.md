[Home](../README.md) › [Porting](README.md) › Client: X2ServerProtocol

# Client: X2ServerProtocol (`src/client/X2ServerProtocol`)

The client's network library: one proxy per server (game, channel, collect: `KUserProxy`; TCP relay: `KTRUser`), each on its
own thread, behind the `CX2ServerProtocol` facade that `X2Lib` uses (`X2Main.h` includes `X2ServerProtocolLib.h`). Overview of
the client port: [Client](client.md).

## Shared with the servers (decision T34, 2026-11-05)

The original kept its own copy of the servers' event/actor model and TCP layer. The port deletes the copy and links the
servers' **`x2::common_net`** (`src/server/Common/CommonNet.cmake`: `Event`, `Performer`, `SimObject`, `ThreadManager`,
`Socket/` TCP; see [Server: Common](server-common.md#network-core-shared-with-the-client-t34)). What the copies differed in, in
the original (2026-11-05):

| Copy | Difference from the servers' | In the port |
|---|---|---|
| `Socket/Session.*` | Client-side packet security: its own `KSADatabase` per session (`ADD_COLLECT_CLIENT_INFO_PROTOCOL`) and the `*_Local` calls (`Create_Local`, `IsAuthentic_Local`, `GetPayload_Local`), which always count and check the sequence number; `OnAuthenticFailed()` on every 10th failed packet. No packet-auth limit, statistics or send-buffer log | `KSession( bIsProxy, bClientSide = true )`: the same calls, in the shared class. The servers' extra logging also runs for the client |
| `Socket/Overlapped.h` | `MAX_PACKET_SIZE` 64 KB (servers: 32 KB) | 64 KB for both (T34) |
| `Socket/IOCP.*`, `SocketObject.*`, `IOThread.*` | No `PfID`, no statistics; `rand()` completion keys; the IOCP close reason not kept | the servers' |
| `Performer.*`, `ThreadManager.*` | `ProcessEvent( KEvent* )` and no `GetPfID()`; no queue-size dump | the servers' API: `ProcessEvent( const KEventPtr& )`; `KUserProxy` is `PI_GS_PROXY` |
| `Event.*` | Only `EventID_System.h` in the enum; names set by the game (`KEvent::SetEventID`); `MAX_UID_NUM` 100 | the servers' (all event IDs). **Same bytes on the wire**. Names: `EventNames.cpp` here, system and client events only (the server-to-server names stay out of the game; `GetIDStr()` gives them all the last client name) (`KPerformerInfo`, two trace UIDs, event ID, buffer) |
| `KncSend.h`, `SimObject.*`, `KncException.h`, `KncSmartPtr.h`, `CircularQueue.*`, `NetCommon.*` | Older or smaller copies | the servers' |
| `Odbc.*` | Compiled by the original, used by nothing | deleted |

## What stays here

| File | Role |
|---|---|
| `ActorProxy.*` | `KActorProxy`: a client-side proxy `KSession` that hands each received event to the game through `SEND_MSG_FUNC` (message = server message ID, `wParam` = event ID, `lParam` = a new `KSerBuffer*`) or queues it. Not the servers' `KActorProxy` of the same name |
| `UserProxy.*` | `KUserProxy`: game, channel and collect servers; `E_CONNECTION_LOST_NOT` to the game unless the disconnection was intended; `E_CHECK_SEQUENCE_COUNT_NOT` to the game server from `OnAuthenticFailed` |
| `TRUser.*` | `KTRUser`: TCP relay. `USHORT` length + serialized `KEvent`, **no encryption**; 30 s heartbeat (one byte, `E_HEART_BEAT`) |
| `EventNames.cpp` (new) | The client's `KEvent` name table and `GetIDStr()`: system and client events only (T34) |
| `X2ServerProtocol.*` | `CX2ServerProtocol`: connects (`ConnectedTo*`), sends (`Send*Packet`, `Send*ID`) and disconnects each proxy |
| `X2ServerProtocolLib.h`, `StdAfx.h` | The header `X2Lib` includes: platform, KNCSDK, the shared core and the files above |

`X2ServerProtocolLib.h` still defines `ADD_COLLECT_CLIENT_INFO_PROTOCOL`, `X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX`
and `X2OPTIMIZE_SESSION_THREAD_SAFETY`, as the original did: `X2Lib` and `KTDXLIB` test them and get them through this header.
Inside `X2ServerProtocol` they were resolved by hand (all three on).

## Changes from the original

| Where | Change | Reason |
|---|---|---|
| `CX2ServerProtocol` | Starts one `KIOThread` (`SiKIocp()->Init( 1 )`) and stops it in the destructor, after the proxies | The proxies still wait on their own events (`Init( false )`), but the asynchronous I/O completes on an I/O thread (T20); Windows signaled the `OVERLAPPED` events itself |
| `CX2ServerProtocol` | Proxies held in `std::atomic< std::shared_ptr >` (was `boost::atomic_load/store`); the accessors of the `#else` branch (`GetUserProxy`...) are gone | The flag was on; the free `std::atomic_load` functions are deprecated in C++20 |
| `CX2ServerProtocol` | `ppEventIDStrList`/`eventIDEnd` are ignored | `EventNames.cpp` names the system and client events |
| `KActorProxy::SetHwnd` | `m_hWnd != NULL` instead of `::IsWindow()` | The handle is only a marker (the SDL window outside Windows, [Client: DXUT](client-dxut.md)); events go through the callback |
| `KSession` (client side) | The failed-packet counter is per session | The original used one `static` counter for every session, without a lock |
| Logging | The shared `KSession` logs as the servers' (`ⓢ` lines, max packet size, send-buffer-full history) | One class; filtered by the log level |

Original bugs fixed:

| Where | Bug | Fix |
|---|---|---|
| `KTRUser::Connect` | `_JIF( Connect(...), false )`: `false` is not a statement that returns, so a failed connection started the thread and reported success | `return false` |
| `KActorProxy::PostEvent`, `KTRUser::OnRecvCompleted` | The `KSerBuffer` copy leaked when no callback was set | Deleted / allocated only with a callback |
| `KTRUser` | `m_hEvents[EVENT_DISCONNECTED]` and `m_iClassID` uninitialized; destructor closed null handles | Initialized; only open handles are closed |

## Tests (`src/client/tests/test_serverprotocol.cpp`, ctest `client`)

Against server-side `KSession`s of the shared core over loopback (the test cannot link `x2_server_common`: its `KActorProxy`
has the client's name):

- Channel connection: key exchange, three encrypted packets accepted by a server session with the **sequence number check on**,
  a 40 KB packet back (more than the original servers' buffer), `E_CONNECTION_LOST_NOT` when the server closes.
- Refused game-server connection: the proxy is dropped and sends fail.
- TCP relay: the frame the client sends (`USHORT` length + `KEvent`) and one received, read by hand.
- Event names: system and client events are named, server events are not.

Byte-exact packet contents are covered by the servers' tests ([Server: Common › Tests](server-common.md#tests-srcservercommontests-target-x2_server_common_tests)).
Not tested yet: against the running ported servers (needs `X2Lib`).

## Import (2026-11-05)

- Module `client-serverprotocol` ([report](../../tools/import/reports/client-serverprotocol/REPORT.md)): **37 files**, 5.4k lines
  written; 13 `#if` groups resolved; 9 blocks became `X2_DEV_TOOLS`. Encoding: 29 CP949, 9 ASCII. 27 of them were deleted on
  2026-11-05 (T34); 10 remain.
- The original `Release_US` builds this project as `X2TOOL|Win32`, where `ServerDefine.h` turns on `_IN_HOUSE_`. The port resolves
  it with the client set, so its 9 `#ifdef _IN_HOUSE_` blocks became `X2_DEV_TOOLS`. All of them are logging.
- **Excluded**: `KncUidType.h`, an older copy of the shared `Common` header (T33, [Client](client.md#code-shared-with-the-servers-t33)).
