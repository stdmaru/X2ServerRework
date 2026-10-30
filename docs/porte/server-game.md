[Início](../README.md) › [Porte](README.md) › Servidor: GameServer

# Servidor: GameServer (`src/server/GameServer`)

Servidor de jogo: personagens, vila, campos, dungeons, PvP, itens, quests, guildas, cash (pelo [EPublisherServer](server-epublisher.md)).
Origem: `KncWX2Server/GameServer`, importado em 2026-10-30 com as [flags do servidor](../ferramentas/flags.md)
([relatório](../../tools/import/reports/server-game/REPORT.md)). Usa o [Common](server-common.md), já portado. Mesma posição
relativa do original (`src/server/GameServer` ao lado de `src/server/Common`).

## Importação (2026-10-30)

- **Seleção**: os arquivos próprios do `GameServer/` que o `GameServer.vcxproj` compila (`Release_US`) e os headers deles:
  **298 arquivos** (145 `.cpp`, 153 `.h`), 163 mil linhas escritas, 148 mil de `.cpp`. Os maiores: `GSGameDBThread.cpp` (15 mil),
  `Inventory.cpp` (15 mil), `GSUserFunction.cpp` (12 mil). Os includes para o `Common` não são seguidos (`include_scope`); nenhum
  header do CenterServer é usado, apesar de `..\CenterServer` estar no caminho de busca do vcxproj.
- **Flags**: 7.532 blocos `#if` resolvidos, 81 mil linhas removidas. Viraram `X2_DEV_TOOLS`: dois `_IN_HOUSE_` em
  `GSUserGameCommon.cpp` e os logs `SERV_LOG_UNDEFINED_QUEST_TEMPLET` (como no `Common`).
- **Excluídos (anti-cheat, T10)**: `HackShield/` (AhnLab, `HSAuth.cpp`), `nProtect/` (GameGuard, `GGAuth.cpp`, `NProtectFunc.cpp`) e o
  XTrap (`..\..\Xtrap\Server\XTrap4Server.cpp`, `XTrapCSM.cpp`), que o vcxproj compila de fora do `KncWX2Server` e o importador não traz.
  Restam referências em 9 arquivos (`GameServer.*`, `GSSimLayer.*`, `GSUser*.cpp`, `GSLogDBThread.cpp`): saem no build.
- Encodings: 274 CP949, 24 ASCII.

## Acoplamento (2026-10-30)

| Área | Arquivos/usos | Plano |
|---|---:|---|
| Boost | 52 arquivos (`boost::str`/`wformat` 118, `shared_ptr` 35, `timer` 16, `iequals`/`replace_all` 15...) | passada mecânica ([Porte mecânico](../ferramentas/porte-mecanico.md)); `iequals`/`replace_all` à mão |
| `windows.h`/winsock/ATL | 29 | `x2/platform/win32.h`, `net.h` |
| Intel TBB | `GameServer.cpp` (`tbbmalloc_proxy.h`) | sai (alocador; o roadmap prevê `std`) |
| Anti-cheat | 9 | remover as chamadas |
| `CTime`, `__int64`, `lua_tinker` | 64, 21, 21 | já cobertos pela [Plataforma](platform.md) e pelo KNCSDK |
| Banco | 502 `DO_QUERY` | `KODBC` já portado; as procedures são as do `Game01` |
| Ferramentas de admin/cheat | `GSUserAdminCheat.*`, `UserCheatManager.*`, `RobotTestManager.*` | revisar sob `X2_DEV_TOOLS` (T10) |

## Plano (um branch por passo)

1. ~~**`server/game-import`**~~: este passo.
2. **`server/game-build`**: compilar sem warnings em GCC, Clang e MinGW, com o mesmo `main` (`KServerMain`) e os hooks do servidor
   (`g_szConfigFile`, `CreateDBThread`). Maior que o ChannelServer (15× o código): pode virar mais de um branch.
3. **`server/game-run`**: `x2_run_game` (`runtime/GameServer`, `Game01`), e o cliente passando do "servidor cheio" para a seleção de
   personagem e a vila. **Atenção:** o GameServer fala com o LoginServer (`SendToLoginServer`), o CenterServer (`ECN_*`) e o GlobalServer
   (`SendToGlobalServer`); levantar o que é preciso deles para a vila antes desse passo (provavelmente portar o LoginServer e o CenterServer).
