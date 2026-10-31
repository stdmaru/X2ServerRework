[Início](../README.md) › [Porte](README.md) › Servidor: CenterServer

# Servidor: CenterServer (`src/server/CenterServer`)

Servidor das salas: campos de batalha (`BattleFieldRoom`), dungeons (`DungeonRoom`, inclusive Henir e defesa), PvP (`PvPRoom`, partidas
e resultados), praça (`Square`), festas (`NewCnParty`, febre de festa), troca (`Trade`), loja pessoal (`PersonalShop`), centro de
treino e salão de casamento. É o lado servidor da simulação: monstros, drops e resultados das salas. Recebe as conexões dos GameServers
(porta 9100); os GameServers mandam para ele os `ERM_*` (o primeiro a aparecer é o `ERM_OPEN_BATTLE_FIELD_REQ`, ao sair da vila).
Origem: `KncWX2Server/CenterServer`, importado em 2026-10-31 com as [flags do servidor](../ferramentas/flags.md)
([relatório](../../tools/import/reports/server-center/REPORT.md)). Usa o [Common](server-common.md), já portado. Mesma posição relativa
do original (`src/server/CenterServer` ao lado de `src/server/Common`).

## Importação (2026-10-31)

- **Seleção**: os arquivos próprios do `CenterServer/` que o `CenterServer.vcxproj` compila (`Release_US`) e os headers deles:
  **123 arquivos** (59 `.cpp`, 64 `.h`), 65 mil linhas escritas, 57 mil de `.cpp`. Os maiores: `RoomUserManager.cpp` (7,2 mil),
  `DungeonRoom.cpp` (6,6 mil), `BattleFieldRoom.cpp` (4,9 mil). O `Main.cpp` da pasta não entra no projeto. Os includes para o `Common`
  não são seguidos (`include_scope`).
- **Flags**: 2.389 blocos `#if` resolvidos, 20 mil linhas removidas. Nenhum bloco virou `X2_DEV_TOOLS`.
- **Nada excluído na importação**, como no GameServer e no LoginServer: o que é de publisher ou legado sai no build, com a mudança
  registrada aqui. Ficam de fora só os `.dsn` (os da execução vêm de `runtime/`, T22).
- Encodings: 113 CP949, 10 ASCII.

## Acoplamento (2026-10-31)

| Área | Arquivos/usos | Plano |
|---|---:|---|
| KSMS (`ProcessCommunicationManager`, *named pipes*) | `CnServer.*`, `CnRMEventProcess.cpp` | sai, como nos outros servidores |
| SMS (`CnSMSDBThread`, `DC_SMS`) e o `SMSPhoneNumberManager` | 2 + `CnSimLayer`, `CnServer`, `CnPartyFSM`, `CnHackingCheckManager`, `CnScriptRefreshThread` | compila e fica, como no LoginServer (a config US não liga o `DC_SMS`); conferir ao rodar |
| Anti-cheat, TBB, publisher | nenhum (o TBB só aparece comentado no `CnServer.cpp`) | — |
| Boost | 31 arquivos (`timer` 28, `wformat`/`str` 54, `shared_ptr` 9, `bind` 3) | passada mecânica ([Porte mecânico](../ferramentas/porte-mecanico.md)); `bind` à mão |
| `windows.h`/winsock/ATL | 14 | `x2/platform/win32.h`, `net.h` |
| UDP (`InteriorUdpSession`, sobre o `KTDNUDP` do `Common`) | 2 | o `KTDNUDP` já foi portado (`server/common-udp`) |
| `CTime`, `lua_tinker` | 16, 15 | já cobertos pela [Plataforma](platform.md) e pelo KNCSDK |
| Banco | 50 `DO_QUERY` | `KODBC` já portado; as procedures são as do `Game01`, `Log` e `Account` |

## Estado (2026-10-31, branch `server/center-import`)

Importado, ainda sem build.

## Plano (um branch por passo)

1. ~~**`server/center-import`**~~ (2026-10-31): este passo.
2. **`server/center-build`**: compilar sem warnings em Clang, GCC e clang-cl, com o `main` do `KServerMain` (como os outros servidores).
3. **`server/center-run`**: `runtime/CenterServer`, `x2_run_center`, e o cliente original no campo e numa dungeon. O GameServer acha o
   Center pela lista do banco (`Statistics.LServerList`) e reconecta a cada 10 s
   ([Dependências do GameServer](server-game.md#dependências-de-outros-servidores-levantamento-de-2026-10-31)).
