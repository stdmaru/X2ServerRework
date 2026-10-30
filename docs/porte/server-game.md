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

## Estado (2026-10-30, branch `server/game-build`)

O executável `GameServer` (alvo `x2_game_server`, `main` com `KServerMain` como o ChannelServer) **compila e linka** em GCC Debug.
Os warnings estão sendo zerados; depois vêm Clang, GCC Release e o clang-cl na VM Windows. Ainda não rodou: é o passo `server/game-run`.

Medidas do caminho (GCC Debug, `-k 0`), para dar a escala: 632 erros em 59 arquivos depois da passada mecânica; 0 erros e 1.099
warnings (190 distintos) depois das correções abaixo e do `fix_warnings.py`.

**Build:** com 16 jobs o GCC estourou a RAM do host (~12 GB livres; os `.cpp` de 10 a 15 mil linhas usam ~1,5 GB cada): `-j 6`.

## Mudanças em relação ao original

| Onde | Mudança | Motivo |
|---|---|---|
| Anti-cheat (`GameServer`, `GSUser`, `GSUserFunction`, `GSUserSession`, `GSUserSecurity`, `GSSimLayer`) | Saem HackShield (`m_kHSAuth`, `_AhnHS_*`), XTrap (`m_cXTrapCSM`, mapas `map*.cs3`, `ReloadXTRAP_MAP`) e nProtect, e os handlers `EGS_HACKSHIELD_CHECK_ACK`, `EGS_REQUEST_HACKSHIELD_CHECK_NOT` e `EGS_XTRAP_ACK`. Os pacotes continuam no `Common` | T10. O config US de produção ligava o `CF_XTRAP`; sem o código o servidor age como com as flags desligadas |
| `GameServer.cpp` | Sai o proxy de malloc do Intel TBB (`tbbmalloc_proxy.h`) e o log "TBB 메모리 관리자 사용" | Alocador do sistema; o roadmap previa tirar o TBB |
| `GameServer`, `GSScriptRefreshManager` | Saem as chamadas ao KSMS (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\GameServer*`), inclusive os 38 avisos de "script recarregado" | Ferramenta externa só Windows, já tirada do ChannelServer |
| `NexonSOAP`, `NexonSOAPManager`, `TRIOCP`, `TRIOThread` | Arquivos removidos | Código morto: sem chamador (o relay TCP do próprio original já usava o `KIocp` do Common) |
| `KSkTRUser::SendDataWithProfile` | Removido | Cópia do `KSocketObject::SendData` com o profiler comentado, sem chamador |
| `ServerDefine.h` | Incluído à força em todo arquivo do servidor (`-include`/`/FI`, no `x2_server_common`) | Sem um cabeçalho pré-compilado, arquivos que chegavam ao `Enum.h` antes dele compilavam o outro ramo do `EXTEND_SERVER_GROUP_MASK` na mesma função inline |
| `DEBBilling*` (`Common/OnlyGlobal/AuthAndBilling`) | Compilados pelo GameServer, não pela biblioteca do `Common` | Incluem o `BillManager.h` do GameServer (o original compilava o `Common` dentro de cada servidor) |
| `NexonBillingTCPPacket` | `unsigned long` → `DWORD` | Campos de 4 bytes do protocolo; `unsigned long` tem 8 bytes no Linux (o `Read8Byte` lia 16) |
| `UserLogManager`, `RobotTestManager` | Arquivos em UTF-8 (`Utf8FileLocale`), caminhos com `/`, `std::filesystem` | O original usava o locale `"korean"` (CP949) e `CreateDirectoryA` |
| `XSLXmlParser` (política da Nexon) | `W2A` → UTF-8 | O XML declara `encoding="utf-8"`; o `W2A` usava a code page ANSI |
| Comparações sempre falsas | `u_char == -1` (grau de guilda) e `unsigned < 0` (nível de skill, página ativa) saem | Sempre falsas também no MSVC: o comportamento fica o mesmo (ver Pendências) |

Portabilidade (sem mudança de comportamento): passada mecânica (`mechanical.py`); `BOOST_MAP_CONST_FOREACH` → range-for;
`boost::iequals`/`ifind_first`/`replace_all` → `x2::platform::IEquals`/`IContains`/`ReplaceAll` (dobra ASCII, como o Boost no locale "C");
52 declarações puladas por `goto end_proc` sobem para o topo da função (`tools/port/fix_goto.py`, mais 2 à mão); `LOG_CONDITION` (KNCSDK)
virou uma expressão; `_KCSLOCK_TYPEDEF_` variádico; `KUserManager::MultiQueueingByCID` com a sobrecarga da base (o `Common` deixara isso
pendente); `CString::GetBuffer`; `std::min/max` no lugar das macros; `_itoa`/`_i64tow` → `std::to_string`/`_i64tow_s`; `StrStrW` → `wcsstr`;
destrutores `virtual` em 14 singletons; `using` das sobrecargas escondidas de `RoutePacket`/`Init`; `#pragma NOTE` removidos.

## Pendências

- **Nexon (KR) ainda compila como código morto** (`NexonBillingTCP*`, `KNexonBillingTCPManager`): o US usa `BF_GLOBAL_SERVICE` e o
  `SiKBillManager()` (DEB). Sai no passo do roadmap que remove os publishers. Antes, o `CI_EXPAND_SKILL_PAGE` do
  `GSUserCashShop_Global.cpp` consulta o catálogo da Nexon, que no US está sempre vazio: no original a compra falhava (conferir ao rodar o cash).
- Grau de guilda: o original testava `== -1` num `u_char` (sempre falso). Conferir no `server/game-run` o que a procedure devolve quando o
  personagem não tem guilda.
- `KPartyListManager::IsPossibleLowLevel`: o `OUT bool isPartyNULL` é recebido por valor (quem chama nunca vê o valor); mantido como no original.
- 105 `unsigned long` no GameServer (8 bytes no Linux): revisar os que vão para rede, banco ou formato fixo ao rodar.

## Plano (um branch por passo)

1. ~~**`server/game-import`**~~: este passo.
2. **`server/game-build`** (em andamento): compilar sem warnings em GCC, Clang e clang-cl, com o mesmo `main` (`KServerMain`) e os hooks do servidor
   (`g_szConfigFile`, `CreateDBThread`). Maior que o ChannelServer (15× o código): pode virar mais de um branch.
3. **`server/game-run`**: `x2_run_game` (`runtime/GameServer`, `Game01`), e o cliente passando do "servidor cheio" para a seleção de
   personagem e a vila. **Atenção:** o GameServer fala com o LoginServer (`SendToLoginServer`), o CenterServer (`ECN_*`) e o GlobalServer
   (`SendToGlobalServer`); levantar o que é preciso deles para a vila antes desse passo (provavelmente portar o LoginServer e o CenterServer).
