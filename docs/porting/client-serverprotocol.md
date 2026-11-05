[Home](../README.md) › [Porting](README.md) › Client: X2ServerProtocol

# Client: X2ServerProtocol (`src/client/X2ServerProtocol`)

The client's network library: TCP sessions over its own copy of the IOCP `Socket/` (`KSession`, `KSocketObject`, `KIOCP`,
`KIOThread`), the event/actor model (`Event`, `Performer`, `SimObject`, `ActorProxy`, `UserProxy`), the TCP relay user
(`TRUser`) and the `X2ServerProtocol` facade that `X2Lib` uses. Overview of the client port: [Client](client.md).

## Import (2026-11-05)

- Module `client-serverprotocol` ([report](../../tools/import/reports/client-serverprotocol/REPORT.md)): **37 files**, 5.4k lines
  written; 13 `#if` groups resolved; 9 blocks became `X2_DEV_TOOLS`. Encoding: 29 CP949, 9 ASCII.
- The original `Release_US` builds this project as `X2TOOL|Win32`, where `ServerDefine.h` turns on `_IN_HOUSE_`. The port resolves
  it with the client set, so its 9 `#ifdef _IN_HOUSE_` blocks became `X2_DEV_TOOLS`. All of them are logging (`START_LOG`,
  `_JIF`/`LIF` versus a plain `if`), so service builds lose only those log lines.
- **Excluded**: `KncUidType.h`, an older copy of the shared `Common` header (T33, [Client](client.md#code-shared-with-the-servers-t33)).
- It includes `KncWX2Server/Common/EventID_System.h` and `SystemPacket.h` (shared, in `src/server/Common`) and many KNCSDK headers
  (`Serializer/`, `KncSecurity/`, `Thread/`, `dbg/`), all resolved in `src/kncsdk`.
- `Odbc.*` is compiled by the original project but nothing uses it (its include in `ThreadManager.cpp` is commented out).

## Status

Imported, not compiled yet. Like the server's `Socket/` (T20), the IOCP code has to move to `x2::platform` networking; the two
`Socket/` copies are to be compared then (roadmap Phase 0 rules).
