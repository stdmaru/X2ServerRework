[Início](../README.md) › [Porte](README.md) › Servidor: GlobalServer

# Servidor: GlobalServer (`src/server/GlobalServer`)

Servidor do que vale para todos os grupos de servidores ao mesmo tempo: missões globais (`GlobalMissionManager`) e de mundo
(`WorldMissionManager`, a defesa do cristal, `P_LDefense_Crystal_*`), eventos de encantamento aleatório e de limite de troca
(`GlobalEventDBThread`), a contagem de usuários (`GlobalServerCCUManager`), a lista de bloqueio (`BlockListManager`, `P_MPenalty_GET`),
a montagem automática de grupos (`AutoParty*`), o *matchmaking* de PvP (`Match*`), o chefe de campo (`FieldBossManager`) e os números
de série dos servidores (`ServerSNManager`). Recebe as conexões dos GameServers (porta 9500, `Statistics.LServerList`); os GameServers
mandam para ele os `EGB_*`, que hoje descartam por falta do servidor
([Dependências do GameServer](server-game.md#dependências-de-outros-servidores-levantamento-de-2026-10-31)).
Origem: `KncWX2Server/GlobalServer`, importado em 2026-10-31 com as [flags do servidor](../ferramentas/flags.md)
([relatório](../../tools/import/reports/server-global/REPORT.md)). Usa o [Common](server-common.md), já portado. Mesma posição relativa
do original (`src/server/GlobalServer` ao lado de `src/server/Common`).

## Importação (2026-10-31)

- **Seleção**: os arquivos próprios do `GlobalServer/` que o `GlobalServer.vcxproj` compila (`Release_US`) e os headers deles:
  **77 arquivos** (34 `.cpp`, 43 `.h`), 16,8 mil linhas escritas, 13,7 mil de `.cpp`. Os maiores: `MatchMakingManager.cpp` (1,3 mil),
  `GlobalUser.cpp` (1,1 mil), `AutoPartyMakingProcess.cpp` (1 mil). O `PvpMatchScriptManager.cpp` da pasta não entra no projeto.
  Os includes para o `Common` não são seguidos (`include_scope`); todos existem no `Common` portado, menos o
  `OnlyGlobal/ProcessCommuniationModule/ProcessCommunicationManager.h` (o KSMS, que saiu dos outros servidores).
- **Flags**: 506 blocos `#if` resolvidos, 4,2 mil linhas removidas (1,2 mil só no `WorldMissionManager.cpp`). Nenhum bloco virou `X2_DEV_TOOLS`.
- **Nada excluído na importação**, como nos outros servidores: o que é de publisher ou legado sai no build, com a mudança
  registrada aqui. Ficam de fora só os `.dsn` (os da execução vêm de `runtime/`, T22).
- Encodings: 62 CP949, 15 ASCII.

## Acoplamento (2026-10-31)

| Área | Arquivos/usos | Plano |
|---|---:|---|
| KSMS (`ProcessCommunicationManager`, *named pipes*) | `GlobalServer.*` (23 usos) | sai, como nos outros servidores |
| SMS (`GlobalSMSDBThread`, `DC_SMS`, `SMS.dbo.SMS_Send`) | 2 + `GlobalServer`, `GlobalSimLayer`, `GlobalScriptRefreshThread` | compila e fica, como no LoginServer e no CenterServer (a config US não liga o `DC_SMS`) |
| Anti-cheat, TBB, Mail, publisher | nenhum no código (as DLLs do TBB e do Mail estão só na pasta de produção) | — |
| Boost | `timer` 11 arquivos (25 usos), `shared_ptr` 4 (19), `wformat`/`str` 2 (7), `bind` 1 (2) | passada mecânica ([Porte mecânico](../ferramentas/porte-mecanico.md)); `bind` à mão |
| `windows.h`/winsock/ATL/`intsafe.h` | 11 | `x2/platform/win32.h`, `net.h` |
| `CTime`, `lua_tinker` | 13 arquivos (132 usos), 10 (50) | já cobertos pela [Plataforma](platform.md) e pelo KNCSDK |
| Banco | 27 `DO_QUERY` em 4 arquivos | `KODBC` já portado; a config US usa só `DC_ACCOUNT` e `DC_LOG` (`Account_US_SERVICE.dsn`, `log_US_SERVICE.dsn`) |

## Estado (2026-10-31, branch `server/global-import`)

Importado, ainda não compila: falta o `CMakeLists.txt`, que vem no `server/global-build`.

## Plano

1. **`server/global-import`** (2026-10-31): `tools/import/modules/server-global.toml` e esta página.
2. **`server/global-build`**: compilar sem warnings em Clang, GCC (Debug e Release) e clang-cl, com o `main` do `KServerMain`
   (como os outros servidores) e o ícone do original; o KSMS sai.
3. **`server/global-run`**: `runtime/GlobalServer` (do `config_gb_US_SERVICE.lua` da pasta de produção, `.dsn` do `Account` e do
   `Statistics`/log), a pasta montada no build e os `EGB_*` do GameServer respondidos. Os `.lua` da pasta de produção já estão
   resolvidos pela T28: o código só carrega o `WorldMissionConfig.lua` (no `ServerResource`), além do `Enum.lua`, do
   `DungeonEnum.lua` e do `PvpMatchData.lua`; o `GlobalMissionConfig.lua` e o `MissEliosConfig.lua` da pasta de produção não são
   lidos por nada (conferido no código importado, 2026-10-31).
