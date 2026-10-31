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

## Estado (2026-10-31, branch `server/global-run`)

**Roda no Linux** pela pasta que o build monta (`x2_run_global`, [Pastas de execução](../ferramentas/runtime.md)): conecta nos dois
bancos (`Account` e `Statistics`), carrega os scripts (`Enum`, `DungeonEnum`, `WorldMissionConfig`, `PvpMatchData`...; os erros de carga
são os de dungeon e linemap já listados), se autentica no banco, chega ao `OnServerReadyComplete` e aceita a conexão do GameServer
(porta 9500). Encerra com código 0 (ver [Rodar os servidores](../running.md)). Três grupos de procedures que ele chama não existem nos backups (missões globais,
encantamento por tempo, troca com limite): essas funções sobem sem dados ([Bugs do original](../original/bugs.md#dados-serverresource-clientscript)).

**Testado com o cliente original** (no Wine, 2026-10-31, com os outros cinco servidores): login, vila, campo e dungeon como antes, sem erro
no GlobalServer; o GameServer passa a entregar os `EGB_*` (nenhum descarte depois que os servidores sobem) e recebe do Global o
`EGB_UPDATE_AUTO_PARTY_BONUS_INFO_NOT`. Com ele, **todos os servidores do original estão portados**.

### Build (2026-10-31, branch `server/global-build`)

**Compila e linka sem warnings** (alvo `x2_global_server`, executável `GlobalServer`, `main` com `KServerMain` como os outros
servidores, ícone do original) em Clang Debug, GCC Debug, Clang Release, GCC Release e clang-cl (Windows, na VM de testes), com os
6 testes passando no Linux e no Windows. Sem argumentos ele sobe, recusa a falta de configuração (`Init` falha), encerra todas as
camadas e sai com código 1, como os outros.

Medidas do caminho, para dar a escala: 50 erros no Clang Debug (`-k 0`) depois da passada mecânica, quase todos de dois templates do
`GlobalServer.h`, e 7 no GCC; ~40 warnings distintos no Linux e 17 no clang-cl. O menor dos servidores: o acoplamento já tinha saído
no `Common`.

## Mudanças em relação ao original

| Onde | Mudança | Motivo |
|---|---|---|
| `KGlobalServer` | Saem as chamadas ao KSMS (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\GlobalServer*`, religadas a cada 60 s no `Tick`, e o aviso de script recarregado) | Como no [CenterServer](server-center.md#mudanças-em-relação-ao-original) e nos outros servidores |
| `ON_DBE_SERVER_ON_ACK` (`GlobalServer.cpp`) | Quando o banco recusa o servidor, a resposta marca `SetAuthenticateResult( false )` e o `Init` falha; quando aceita, marca `true` | Como no [GameServer](server-game.md#mudanças-em-relação-ao-original): o original chamava `::exit( 0 )` nessa thread |
| `KGlobalAccountDBThread` | Saem os templates `SendToNXGMToolTcpManager`/`SendToNXGMToolTcpServer` | Ferramenta de GM da Nexon (publisher); nunca eram chamados, e os singletons que usavam não existem no build US |
| `KBlockListManager` (bloqueio por IP) | `inet_addr` → `x2::platform::IpFromString` (`inet_pton`) | `inet_addr` é obsoleto no Winsock. Só endereços com quatro números: as formas curtas que o `inet_addr` aceitava (`127.1`, hexadecimal) deixam de casar com um bloqueio |
| `Global*DBThread.h` | `ProcessEvent` deixa de ser `virtual inline` | Definido no `.cpp`: ODR inválido (`-Wundefined-inline` do clang-cl), como no CenterServer e no LoginServer |

Portabilidade (sem mudança de comportamento): passada mecânica (`mechanical.py`); a declaração pulada por `goto end_proc` no
`GlobalGameDBThread` sobe (`fix_goto.py`); includes que o MSVC dispensava nos templates (`ActorManager.h` no `GlobalServer.h`,
`BaseServer.h` nos DB threads); `intsafe.h` → `<climits>` (`BYTE_MAX` → `UCHAR_MAX`, o mesmo 255); `max` → `std::max`; destrutores
`virtual` nos singletons `KGlobalMissionManager`, `KWorldMissionManager` e `KGlobalServerCCUManager`; `using KThreadManager::Init` no
`KGlobalScriptRefreshManager`; `default: break;` nos `switch` incompletos; laços que copiavam `shared_ptr` e estruturas por valor passam a
referência `const`; comparações de `enum` com `double` (os prazos de troca limitada) com `static_cast`; `unsigned < 0` (sempre falso
também no MSVC) sai do `KMatchUserManager`; variáveis e instruções mortas saem (`bChanged` do `KBlockData::UpdateAndChanged`, que
devolve pelo `goto`; `tCurrentTime` do `CheckExchangeLimit`; membros só citados, sem atribuição, no `KBlockListManager` e no
`KMatch::GetRoomInfoForPvpGame`, que ficam com os valores padrão dos construtores, como no original).

### Bugs do original corrigidos

| Onde | Bug | Correção |
|---|---|---|
| `KWorldMissionManager::ToStringW` | Usava `BUILD_LOG` (manipuladores do logger, `dbg::tab`/`dbg::endl`) num `std::wostream` comum: o MSVC convertia os ponteiros de função em endereços, e o *dump* saía com números no lugar de tabulações e quebras de linha | `TOSTRINGW`, como o resto da função (2026-10-31, `-Wmicrosoft-cast` do clang-cl) |

## Plano

1. ~~**`server/global-import`**~~ (2026-10-31): `tools/import/modules/server-global.toml` e esta página.
2. ~~**`server/global-build`**~~ (2026-10-31): compilar sem warnings em Clang, GCC (Debug e Release) e clang-cl, com o `main` do `KServerMain`
   (como os outros servidores) e o ícone do original; o KSMS sai.
3. ~~**`server/global-run`**~~ (2026-10-31): `runtime/GlobalServer` (do `config_gb_US_SERVICE.lua` da pasta de produção, `.dsn` do `Account` e do
   `Statistics`/log), a pasta montada no build e os `EGB_*` do GameServer respondidos. Os `.lua` da pasta de produção já estão
   resolvidos pela T28: o código só carrega o `WorldMissionConfig.lua` (no `ServerResource`), além do `Enum.lua`, do
   `DungeonEnum.lua` e do `PvpMatchData.lua`; o `GlobalMissionConfig.lua` e o `MissEliosConfig.lua` da pasta de produção não são
   lidos por nada (conferido no código importado, 2026-10-31).
