[Início](../README.md) › [Porte](README.md) › Servidor: ChannelServer

# Servidor: ChannelServer (`src/server/ChannelServer`)

Servidor de canal: lista de servidores, login da conta (pelo DEB, `_DEB_AUTH`) e passagem do usuário para o GameServer.
Origem: `KncWX2Server/ChannelServer`, importado em 2026-10-30 com as [flags do servidor](../ferramentas/flags.md)
([relatório](../../tools/import/reports/server-channel/REPORT.md)). Usa o [Common](server-common.md), já portado.
Mesma posição relativa do original (`src/server/ChannelServer` ao lado de `src/server/Common`), porque os fontes incluem `"../Common/x.h"`.

## Importação (2026-10-30)

- **Seleção**: os arquivos próprios do `ChannelServer/` que o `ChannelServer.vcxproj` compila (`Release_US`), mais os headers deles:
  **36 arquivos** (17 `.cpp`), 6.071 linhas escritas (4.913 de `.cpp`). Os includes para o `Common` não são seguidos (`include_scope`):
  o relatório os lista, e todos existem em `src/server/Common`.
- **Fora da seleção**: os `OnlyGlobal/ChannelUserAuth_<país>.cpp` (BR, CN, EU, ID, IN, JP, PH, TH, TW, US), que o vcxproj não compila;
  só o `OnlyGlobal/ChannelUserAuth.cpp` entra. Configs (`config_ch_*.lua`, `.ini`), `.dsn`, `.rc`, ícone e DLLs também não são fontes.
- **Flags**: 370 blocos `#if` resolvidos, 2.898 linhas removidas. Nenhum bloco virou `X2_DEV_TOOLS`.
- Encodings: 29 CP949, 3 UTF-8, 4 ASCII. Includes com a grafia corrigida: `odbc/` → `ODBC/`, `socket/` → `Socket/`, `\` → `/`.

## Para o passo de build

- `ChannelServer.cpp` e `ChannelSimLayer.cpp` incluem `Common/OnlyGlobal/ProcessCommuniationModule/ProcessCommunicationManager.h`,
  excluído do `Common` (ferramenta KSMS, comunicação por *named pipe* `\\.\pipe\ChannelServerRead`/`Write`). Só é usado quando
  `GameSysVal:GetProcessCommunication()` está ligado: as chamadas saem.
- Boost (`boost::format`, `boost::timer`, `boost::random`, `boost::bind`), `windows.h`/`WinSock2.h`: passada mecânica ([Porte mecânico](../ferramentas/porte-mecanico.md)).
- `main` sem MFC e os hooks que o `Common` espera de cada servidor (`g_szConfigFile`, `CreateDBThread`; hoje stubs em
  `src/server/Common/tests/server_hooks.cpp`).
