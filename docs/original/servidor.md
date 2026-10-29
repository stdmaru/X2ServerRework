[Início](../README.md) › [Código original](README.md) › Servidor — acoplamento com Windows

# Servidor — acoplamento com Windows

Projetos: `LoginServer` (199 fontes no vcxproj), `CenterServer` (201), `ChannelServer` (140), `GameServer` (344), `GlobalServer` (155).
Todos são `Application`, **MFC dinâmico** (`UseOfMfc=Dynamic`), Unicode, Win32 (x86). Muitos fontes de `Common/` entram em vários executáveis.

Config ativa no `_reference` (a que foi ajustada no porte VS2026): `Release_US|Win32`
→ `SERV_COUNTRY_US;_SERVICE_?;_DEB_AUTH(Game/Channel);BOOST_ALLOW_DEPRECATED;BOOST_TIMER_ENABLE_DEPRECATED`.

## Pontos de acoplamento (contagem de arquivos em `KncWX2Server/`)

| Área | Arquivos | Onde | Plano |
|---|---:|---|---|
| `windows.h`/winsock/ATL/afx | 131 | geral | camada `platform/`, remover includes |
| IOCP (`CreateIoCompletionPort`, `WSARecv/Send`) | 13 | `Common/Socket/{IOCP,IOThread,Session,SocketObject,Accepter}` | Asio (standalone) mantendo `Session`/framing |
| Threads Win32 (`_beginthreadex`, `CreateThread`) | 85 | `ThreadManager`, `DBThreadManager`, simuladores | `std::thread` / `std::jthread` |
| `CRITICAL_SECTION` | 14 | `KCSLockType.h` etc. | `std::mutex` / `std::recursive_mutex` |
| Tempo (`GetTickCount`, `timeGetTime`, QPC) | 116 | geral | `platform::TickCount()` sobre `std::chrono::steady_clock` |
| ODBC | `Common/ODBC/{Odbc,StoredProcedure,DBUtil}` + `.dsn` | DB | mantém ODBC; unixODBC no Linux; DSN → config |
| MFC UI | `Common/ui/{ServerApp,ServerUIDlg,DBConnectionDlg,EditLog,HookStdio}` | janela de servidor | **headless**: console + log + comandos |
| `CString`/`TCHAR`/`_T()` | 121 | geral | `std::wstring`/`std::string` |
| `std::wstring`/`wchar_t` | 499 | geral | ok em memória; **cuidado no fio e no DB** (ver [protocolo.md](protocolo.md)) |
| `__declspec`/`__forceinline`/`#pragma comment` | 38 | geral | macros de compilador / CMake |
| D3DX matemática | 13 | física/posição | mesma lib de matemática do cliente |
| gSOAP | 47 | `Common/SOAP`, auth | remover |
| Anti-cheat (HShield, nProtect, XTRAP) | 29 | `GameServer/{HackShield,nProtect}` | remover |
| Lua (`lua_tinker`, luabind) | 154 | carregamento de `ServerResource/*.lua` | LuaJIT 2.1 + `lua_tinker` portado |
| Boost | 229 | geral | reduzir |
| `COM` (`CoInitialize`) | 2 | ODBC/SOAP? | remover/isolar |

## Subsistemas de `Common/` (para orientação)

`Socket/` rede · `ODBC/` DB · `Lua/` binder · `FSM/` + `*FSM.cpp` máquinas de estado de usuário · `Room/` salas/dungeons ·
`X2Data/` tabelas de jogo · `Mail/` correio in-game (não confundir com MailSMTP) · `Nexon/`, `OnlyGlobal/AuthAndBilling/<país>` auth/billing de publisher ·
`CrashRpt/` crash reporter · `Mornitoring/`, `PacketMornitoring/` métricas · `ui/` MFC.

Arquivos centrais: `BaseServer.cpp`, `Actor*.cpp`, `Event*.h`, `EventID_{Client,Server,System}.h`, `ClientPacket.h`, `CommonPacket.h`, `ServerDefine.h` (4.645 linhas de flags).

## Riscos

- Código duplicado entre os 5 servidores (mesmos `.cpp` de `Common/` compilados várias vezes com defines diferentes) — virar libs estáticas em CMake.
- Encoding: strings coreanas em CP949 dentro de literais (logs, possivelmente mensagens ao DB).
- `long` = 4 bytes no Windows, 8 no Linux x64 — afeta structs serializados e binding ODBC.
