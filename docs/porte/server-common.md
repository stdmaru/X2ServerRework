[Início](../README.md) › [Porte](README.md) › Servidor: Common

# Servidor: Common (`src/server/Common`)

Código compartilhado pelos cinco servidores, e em parte pelo cliente (pacotes, enums, `EventID_*`).
Origem: `KncWX2Server/Common`, importado em 2026-10-29 com as [flags do servidor](../ferramentas/flags.md)
([relatório](../../tools/import/reports/server-common/REPORT.md)).
Mesmo nome de pasta do original (`Common`), porque os fontes incluem `"../Common/x.h"`.

## Importação

- **Seleção**: só os fontes do `Common/` que algum vcxproj de servidor compila (`Release_US`): 239 fontes, mais os headers que eles incluem, deram **414 arquivos**.
  Dos 239 fontes, 107 são usados pelos cinco servidores e 89 por um só.
- **Flags**: 6.627 blocos `#if` resolvidos e 49 mil linhas removidas. Dev (`X2_DEV_TOOLS`): só os logs `SERV_LOG_UNDEFINED_QUEST_TEMPLET` e `DELETE_INCORRECT_ERROR_LOG`.
- **Excluídos**:

  | O que | Motivo |
  |---|---|
  | `ui/` (MFC) | substituído pelo terminal, T15 |
  | `OnlyGlobal/AuthAndBilling/<país>/` | integrações de publisher de cada país, inclusive as do US (K3RCombo, Payletter), que o DEB substitui |
  | `OnlyGlobal/NetCafe`, `Nexon`, `SOAP` | integrações de publisher |
  | `OnlyGlobal/ProcessCommuniationModule` | ferramenta KSMS |
  | `CrashRpt`, `Mail/` | binários fechados (SMTP de alerta) |

  As referências a eles, ~190 includes, viram stubs ou somem na passada manual.
- **Mantido: DEB ("Debelzak Custom Auth")**, ligado por `_DEB_AUTH` (Channel e Game no `Release_US`). **Não é código da KOG**: foi escrito pelo dono do projeto,
  junto com um **servidor SOAP próprio** de autenticação, feito do zero. É a autenticação (`KDEBAuth`, cliente gSOAP que chama `Login` num `AuthService`
  de endereço configurável) e a cobrança (`KDEBBilling`) **usadas por este servidor**. Nas cadeias de `#if`, ele substitui o publisher US.
  Entram `AuthAndBilling/DEB*`, os stubs gSOAP de `AuthAndBilling/DEB/` e o `KncSendToPublisher.h`.
  Erro corrigido em 2026-10-29: uma primeira versão desligou o `_DEB_AUTH` achando que era só integração de publisher.
- Includes para o KNCSDK com a grafia corrigida (`KncSingleton.h` → `KNCSingleton.h`, `RTTI.H` → `RTTI.h`...) via `extern_dirs`.

## Acoplamento restante (2026-10-29)

| Área | Arquivos | Plano |
|---|---:|---|
| gSOAP (`AuthAndBilling/DEB/`, `stdsoap2`) | 3 | código C portável; gerar de novo com gSOAP atual ou compilar como está, com sockets da plataforma |
| `windows.h`/ATL/WinInet/`stdafx.h` | ~35 | `x2/platform/win32.h`; WinInet (`KFtpWrapper`) avaliar se é necessário |
| Threads/eventos (`_beginthreadex`, `WaitForSingleObject`...) | 29 | já cobertos pela [Plataforma](platform.md) |
| `CTime`/`CTimeSpan`, `CString` | 46 / 10 | cobertos pela Plataforma (ampliar o `CString` se faltar algo) |
| `__int64`, `__forceinline`, `__declspec` | 15 | tipos/macros da Plataforma |
| Boost | 39 | `std` |
| Lua (`lua_tinker`, `luaL_*`) | 53 | `x2::kncsdk_lua` |
| **Sockets/IOCP** (`Socket/`) | 15 | rede portável (Asio standalone), mantendo `KSession`/framing/cripto |
| **ODBC** (`ODBC/`) | 2 | T17: ODBC Driver 18 / FreeTDS, `SQLWCHAR` UTF-16, connection strings |
| **Matemática D3DX** (`X2Data/XSLBattleField`, `LineMap/`, `CommonPacket.h`) | 6 (187 usos) | shim de matemática compatível (`D3DXVECTOR3`...), reaproveitado pelo cliente |
| Dependência invertida | 1 | `KStatisticsLocalLog.cpp` inclui `GameServer/Inventory.h` e `CenterServer/PvPRoom.h` |

Tamanho: ~99 mil linhas de `.cpp` sem os stubs gSOAP. Os maiores são os pacotes: `ServerPacket.cpp` (16 mil), `ClientPacket.cpp` (12 mil) e `CommonPacket.cpp` (7 mil).

## Plano do porte manual (um branch por passo)

1. **`platform/math`**: shim da matemática D3DX (tipos e funções usados pelo servidor), com testes.
2. **`server/common-build`**: alvo CMake com todo o `Common`, menos `Socket/` e `ODBC/`, compilando em GCC/Clang.
   É a passada mecânica: includes de plataforma, `typename`/`template`, Boost → `std`, stubs das integrações removidas.
3. **`server/common-packets`**: testes de serialização dos pacotes (`ClientPacket`/`CommonPacket`), que são a base da compatibilidade (T4).
4. **`server/common-net`**: `Socket/` sobre Asio.
5. **`server/common-db`**: `ODBC/` portável (T17).

Depois disso vem o ChannelServer (`server/channel`).

## Pendências

- Licença do `XML/Markup.cpp` (CMarkup 8.1): o cabeçalho pede permissão para uso comercial. Avaliar a troca por tinyxml2 (o `TinyXml/` já está no módulo).
- `NetError_def_*.h` (DE/EN/ES/FR/IT/PL): textos de erro de outros idiomas; manter só o necessário para o US.
