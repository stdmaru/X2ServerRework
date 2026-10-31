[Início](../README.md) › [Porte](README.md) › Plataforma

# Plataforma (`src/platform`, alvo `x2::platform`)

Única parte do código que fala direto com o sistema operacional (regra do [AGENTS.md](../../AGENTS.md)).
Código novo, sem equivalente no original. Criada em 2026-10-29 (decisão T11).

## Ideia

O original usa APIs Win32/ATL em massa. Contagem só no servidor:

| API | Uso |
|---|---|
| `CTime` | ~1.700 vezes |
| `DWORD` | ~900 |
| Eventos/`WaitForSingleObject` | ~50 arquivos |
| `CString` | ~300 |

Reescrever tudo isso agora seria inviável e arriscado. Então a camada oferece **substitutos com os mesmos nomes e semântica**:
- **no Windows**, usa o `windows.h` real, e este é o único lugar onde ele é incluído;
- **no Linux**, emula o subconjunto usado.

A modernização (trocar por `std::` direto) fica para a Fase 5.

Código portado inclui `x2/platform/win32.h` no lugar de `<windows.h>`, `<atltime.h>` e `<tchar.h>`.

## Conteúdo

| Header | O que oferece | Linux | Windows |
|---|---|---|---|
| `types.h` | `DWORD`, `BYTE`, `LONG`, `INT64`, `TCHAR`, `_T()`, `MAX_PATH`... com os tamanhos do Windows (`DWORD`/`LONG` = 32 bits) | typedefs | `windows.h` |
| `sync.h` | `CRITICAL_SECTION` (recursiva), eventos, `WaitForSingle/MultipleObjects`, `_beginthreadex`, `Sleep`, `GetCurrentThreadId` | emulação (um mutex/condvar global) | nativo |
| `time.h` | `GetTickCount`, `timeGetTime`, `QueryPerformanceCounter`, `GetLocalTime`, **`CTime`/`CTimeSpan`** | `steady_clock` / implementação própria | nativo + `CTime` próprio |
| `string.h` | `CStringW`/`CStringA`/`CString` (subconjunto) | implementação própria | idem (sem ATL) |
| `crt.h` | `_wtoi`, `swprintf_s`, `wcscpy_s`, `_wcsicmp`, `_itow_s`, `_mkdir`, `fopen_s`, `localtime_s` (ordem do MSVC), `_wstrdate_s`/`_wstrtime_s`, `ZeroMemory`, `_tcs*`, `WideCharToMultiByte`... e as sobrecargas com array do MSVC | implementação própria | nativo |
| `ini.h` | `x2::platform::ReadIniString`/`ReadIniInt` (regras do `GetPrivateProfileString`), `WriteIniString` (do `WritePrivateProfileString`, grava UTF-8; 2026-10-31, para o `RefreshTimeBackup.ini` do LoginServer), `GetPrivateProfileStringW`/`IntW`, `WritePrivateProfileStringW` | implementação própria | **também a própria** via `ReadIni*` (ver Armadilhas) |
| `net.h` | Nomes do Winsock (`SOCKET`, `closesocket`, `SOCKADDR_IN`...); `OVERLAPPED`/`WSABUF` só como tipos; `IpToString`/`IpFromString`/`ResolveIPv4` no lugar de `inet_ntoa`/`inet_addr`/`gethostbyname` | BSD sockets | Winsock 2 |
| `net_io.h` | Rede assíncrona do servidor (T20): `IoService` (`RunFor`, `Stop`, `Drain`), `TcpStream` (`Connect`, `Assign` de socket aceito, `AsyncReadSome`/`AsyncWriteSome`, `Close` que aborta o pendente), `SocketErrorMessage` | Asio (epoll) | Asio (IOCP) |
| `odbc.h` | `<sql.h>`/`<sqlext.h>` sem conflito com os tipos Win32 da camada; `SqlWString`, `ToSqlWString`/`FromSqlWString`/`SqlWPtr` (`wchar_t` ↔ `SQLWCHAR`). Alvo `x2::odbc` (só header; liga o ODBC) | unixODBC, com os typedefs dele renomeados; `SQLWCHAR` é UTF-16 | nativo; `odbc32` |
| `format.h`, `timer.h`, `noncopyable.h` | `x2::wformat`/`x2::str` (fiel ao Boost.Format), `x2::Timer`, `x2::noncopyable` | — | — |
| `unicode.h` | wide ↔ UTF-8 ↔ UTF-16; `TranslateWideFormat`; `Cp949ToWide`, `IsValidUtf8`, `DecodeTextFile`; `Utf8FileLocale` (locale UTF-8 para `std::wfstream`, no lugar de `std::locale("korean")`) | iconv (CP949) | `MultiByteToWideChar(949)` |
| `debug.h` | `__WFILE__`, `__WFUNCTION__` em tempo de execução | — | — |
| `console.h` | `InitConsole` (no início do `main` de cada servidor), `IsColorTerminal` (usado pelo log) | nada a fazer; `isatty` | console em UTF-8 na entrada e na saída (`SetConsoleCP`/`SetConsoleOutputCP`, restaurados ao sair) e escapes ANSI ligados (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`); cor só se o console tiver esse modo |
| `d3dx_math.h` | Matemática do D3DX 9: `D3DXVECTOR2/3/4`, `D3DXMATRIX`, `D3DXCOLOR`, `D3DX_PI`, `D3DXToDegree`, `D3DXVec3*` (normalize, cross, dot, transform...), `D3DXMatrix*` (identity, multiply, inverse, rotações, translação, escala, `LookAtLH`) | implementação própria | idem (sem DXSDK) |

## Armadilhas que a camada resolve

- **`%s` em formato largo**: no MSVC, `%s` em `swprintf`/`CString::Format` significa string *larga*; na glibc significa *estreita* (crash).
  `TranslateWideFormat` converte `%s`→`%ls`, `%c`→`%lc`, `%S`→`%s` e `%I64d`→`%lld`, e é usada por `CString::Format` e pelos `*printf_s`.
  Só o CRT do MSVC (o do clang-cl) dispensa a tradução.
- **`L ## __FUNCTION__`** só existe no MSVC. `__WFUNCTION__` agora é um `const wchar_t*` em tempo de execução, que pode ser passado e concatenado via `<<`,
  mas não colado a outro literal. O servidor só usa como argumento.
- **`GetCurrentTime`** é macro no `windows.h`: o `types.h` faz `#undef` para não quebrar `CTime::GetCurrentTime()`.
- `GetTickCount` dá a volta a cada ~49,7 dias, como no Windows (comparações com subtração sem sinal continuam certas).
- **`&D3DXVECTOR3(0, 0, 0)`**: o original passa o endereço de temporários para funções D3DX, uma extensão do MSVC. Os tipos D3DX têm um
  `operator&` só para temporários (qualificado com `&&`), que dá o mesmo resultado; `&` em variáveis continua sendo o nativo.
- Matemática D3DX: mesmas convenções e fórmulas (vetor-linha `v·M`, mão esquerda, vetor nulo normaliza para nulo, `D3DXMatrixInverse` devolve `NULL` em matriz singular),
  conferidas com a implementação do Wine. Só o subconjunto usado pelo servidor; o cliente vai pedir mais (quatérnios, planos, `D3DXVec3Project`...).

- **Arquivos de texto do original**: os `config_*.ini` de produção estão em **CP949**, e os scripts `.lua` em UTF-8 com BOM (5.829 de 5.832, em 2026-10-29).
  `DecodeTextFile` detecta UTF-16/UTF-8 e cai para CP949. O INI é lido por `ReadIniString` também no Windows, porque a API nativa usa a
  code page do sistema (no Wine, ou num Windows não coreano, as strings de versão viriam corrompidas; os testes no Wine mostraram isso).
- **unixODBC** define seus próprios `ULONG` (64 bits), `TCHAR` (`char`), `WCHAR` (`unsigned short`)...: o `odbc.h` renomeia esses typedefs durante o include.
  O `SQLWCHAR` dele tem 2 bytes (UTF-16) e o `wchar_t` 4: texto passa pelas conversões do `odbc.h`, nunca por cast.
- **Rede**: o Asio só entra em `src/platform/src/net_io.cpp`. `TcpStream::Close` cancela antes do `shutdown`, porque no Windows o `shutdown`
  termina a leitura pendente como fim de conexão e no Linux como cancelamento; assim os dois dão `Aborted`. Quem destrói o `IoService` com
  operações pendentes precisa chamar `Drain()` antes (o `~KIocp` faz isso), senão os handlers soltam sockets durante a destruição.
- **Windows**: o `types.h` inclui o `windows.h` com `WIN32_LEAN_AND_MEAN`, que deixa de fora o `rpcndr.h` e o `winsock.h`; os nomes que o original
  recebia deles (`byte`, `u_char`, `u_short`, `u_int`, `u_long`) são declarados com os mesmos tipos. O build Windows define `UNICODE`/`_UNICODE` (`x2::options`).
- `IsWindow` sempre falso e `HWND_DESKTOP` nulo fora do Windows (servidor sem janela).

## Limitações conhecidas

- `_beginthreadex` não suporta `CREATE_SUSPENDED`.
- `CString` cobre só o subconjunto usado até agora (vai crescer conforme o servidor for portado).
- Console do Windows (2026-10-31): até o `InitConsole`, o texto coreano do log saía corrompido, porque o console decodificava o UTF-8
  pela code page OEM. Com ele a saída fica certa, mas o conhost com Consolas não tem glifos de Hangul (o Windows Terminal resolve por
  *fallback* de fonte) e a leitura de UTF-8 não ASCII pelo stdin no conhost não é confiável: o terminal da T15 deve ler com `ReadConsoleW`.
- Windows: compila sem warnings e passa nos testes com **clang-cl** (Visual Studio 2026, MSVC 14.51) num Windows real (T26, 2026-10-30).
  De 2026-10-29 a 2026-10-30 foi verificado com MinGW-w64 + Wine (T18). O debug da STL da Microsoft achou um comportamento indefinido no
  `x2::format` (`assign` com referência para o próprio vetor), corrigido.

## Testes

`src/platform/tests` (doctest): unicode, formatos MSVC, `CString`, `CTime`/`CTimeSpan`, eventos, threads, CRT e matemática D3DX
(valores esperados derivados à mão das convenções do D3DX).
