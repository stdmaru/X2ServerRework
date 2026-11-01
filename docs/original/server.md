[Home](../README.md) › [Original code](README.md) › Server — Windows dependencies

# Server — Windows dependencies

Projects: `LoginServer` (199 sources in vcxproj), `CenterServer` (201), `ChannelServer` (140), `GameServer` (344), `GlobalServer` (155).
All are `Application`, **dynamic MFC** (`UseOfMfc=Dynamic`), Unicode, Win32 (x86). Many `Common/` sources are used by several executables.

Active `_reference` configuration (adjusted for the VS2026 port): `Release_US|Win32`
→ `SERV_COUNTRY_US;_SERVICE_?;_DEB_AUTH(Game/Channel);BOOST_ALLOW_DEPRECATED;BOOST_TIMER_ENABLE_DEPRECATED`.

## Platform dependencies (file counts under `KncWX2Server/`)

| Area | Files | Where | Plan |
|---|---:|---|---|
| `windows.h`/Winsock/ATL/afx | 131 | general | `platform/` layer, remove includes |
| IOCP (`CreateIoCompletionPort`, `WSARecv/Send`) | 13 | `Common/Socket/{IOCP,IOThread,Session,SocketObject,Accepter}` | standalone Asio, retaining `Session`/framing |
| Win32 threads (`_beginthreadex`, `CreateThread`) | 85 | `ThreadManager`, `DBThreadManager`, simulators | `std::thread` / `std::jthread` |
| `CRITICAL_SECTION` | 14 | `KCSLockType.h`, etc. | `std::mutex` / `std::recursive_mutex` |
| Time (`GetTickCount`, `timeGetTime`, QPC) | 116 | general | `platform::TickCount()` on `std::chrono::steady_clock` |
| ODBC | `Common/ODBC/{Odbc,StoredProcedure,DBUtil}` + `.dsn` | DB | retain ODBC; unixODBC on Linux; DSN → config |
| MFC UI | `Common/ui/{ServerApp,ServerUIDlg,DBConnectionDlg,EditLog,HookStdio}` | server window: live log (stdout/stderr redirected), input field executing Lua (`luaL_dostring`) with history, `_Loading`/`_Ready` title, close confirmation | terminal on all platforms (decision T15, completed 2026-10-31: [Server console](../porting/console.md)) |
| `CString`/`TCHAR`/`_T()` | 121 | general | `std::wstring`/`std::string` |
| `std::wstring`/`wchar_t` | 499 | general | fine in memory; **careful on wire and in DB** (see [Protocol](protocol.md)) |
| `__declspec`/`__forceinline`/`#pragma comment` | 38 | general | compiler macros / CMake |
| D3DX math | 13 | physics/position | same math library as client |
| gSOAP | 47 | `Common/SOAP`, auth | remove |
| Anti-cheat (HShield, nProtect, XTRAP) | 29 | `GameServer/{HackShield,nProtect}` | remove |
| Lua (`lua_tinker`, luabind) | 154 | loading `ServerResource/*.lua` | LuaJIT 2.1 + ported `lua_tinker` |
| Boost | 229 | general | reduce |
| `COM` (`CoInitialize`) | 2 | ODBC/SOAP? | remove/isolate |

## `Common/` subsystems (orientation)

`Socket/` networking · `ODBC/` DB · `Lua/` binder · `FSM/` + `*FSM.cpp` user state machines · `Room/` rooms/dungeons ·
`X2Data/` game tables · `Mail/` alert email via SMTP/MIME (`KMailManager`, `X2SMTPMail` over closed DLLs; in-game mail is elsewhere) ·
`Nexon/`, `OnlyGlobal/AuthAndBilling/<country>` publisher auth/billing ·
`OnlyGlobal/AuthAndBilling/DEB*` SOAP auth/billing ("Debelzak Custom Auth", `_DEB_AUTH`): **customized by the project owner** (not KOG), client for a custom SOAP server; used by this server ·
`CrashRpt/` crash reporter · `Mornitoring/`, `PacketMornitoring/` metrics · `ui/` MFC.

Core files: `BaseServer.cpp`, `Actor*.cpp`, `Event*.h`, `EventID_{Client,Server,System}.h`, `ClientPacket.h`, `CommonPacket.h`, `ServerDefine.h` (4,645 lines of flags).

## Risks

- Code duplicated across the 5 servers (same `Common/` `.cpp` files compiled multiple times with different defines) — make static libraries in CMake.
- Encoding: Korean strings in CP949 inside literals (logs, possibly DB messages).
- `long` = 4 bytes on Windows, 8 on Linux x64 — affects serialized structs and ODBC bindings.
