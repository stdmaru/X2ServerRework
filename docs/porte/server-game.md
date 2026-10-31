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

O executável `GameServer` (alvo `x2_game_server`, `main` com `KServerMain` como o ChannelServer) **compila e linka sem warnings** em
GCC Debug, Clang Debug, GCC Release, Clang Release e clang-cl (Windows, na VM de testes), com os 6 testes passando em todos. Ainda não rodou: é o passo
`server/game-run`. O Clang e o clang-cl acharam três bugs do original (abaixo) que o GCC não acusava.

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

### Bugs do original corrigidos

| Onde | Bug | Correção |
|---|---|---|
| `GSLogDBThread` (estatística do personagem) | Quatro colunas de ED (`US_ED_OpenCubeSpendED`, `US_ED_PBattleFieldED`, `US_ED_MCoboExpressTicketED`, `US_ED_MItemResolve`) eram lidas do `etcData[22]` com índices 22 a 25: leitura fora do array (lixo da pilha no log) | Lidas do `edData`, onde são somadas (2026-10-30, achado pelo `-Warray-bounds` do Clang) |
| `GSUserAdminCheat` (`/dhn`) | O `std::wstring` do apelido passava pelo `...` do `CString::Format`: o `%s` lia a estrutura como ponteiro | `.c_str()` (o Clang recusa: "call will abort at runtime") |
| `KGSHenirRanking::SetLastRank` | `m_uiLastRank = m_uiLastRank`: o setter não fazia nada e o limite do ranking de Henir usava sempre o valor inicial | Grava o parâmetro (`-Wself-assign-field`) |

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

## Dependências de outros servidores (levantamento de 2026-10-31)

O GameServer não conhece os outros servidores pela configuração. Ao se autenticar no banco, recebe a lista de Center, Login e Global
(`m_mapCenterList`/`m_mapLoginList`/`m_mapGlobalList`, `GameServer.cpp`) e o `KProxyManager` (`Common`) conecta em todos
(`ConnectAll`), com uma thread que tenta de novo a cada 10 s. **Ele não espera essas conexões para ficar pronto**
(`SetServerReady` vem logo depois). Sem o servidor de destino, o `SendTo*Server` registra "Proxy 포인터를 찾을 수 없음" e descarta o pacote.
Ninguém responde e não há timeout: o usuário fica parado no estado em que estava.

Do canal até a vila (`GSUserSession.cpp`, `GSUserGameCommon.cpp`, `GSUserField.cpp`, `GSUserFunction.cpp`):

| Momento | Pacote | Destino | Sem o servidor |
|---|---|---|---|
| `EGS_VERIFY_ACCOUNT_REQ` | `ELG_NEXON_USER_AUTHENTICATE_REQ` → `_ACK` | LoginServer, que autentica no `Account` (`P_MUser_SEL_Auth`, ramo `SERV_GLOBAL_AUTH`) | **bloqueia**: o cliente nunca recebe o `EGS_VERIFY_ACCOUNT_ACK` |
| depois do `_ACK` | `ELG_REGISTER_USER_REQ` → `ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK` | LoginServer (lista de usuários, bloqueio de conta) | **bloqueia**: o usuário não passa para `S_LOGINED` |
| seleção de servidor e de personagem | nenhum (só os bancos `Game` e `Account`) | — | — |
| personagem selecionado | `ELG_REGISTER_UNIT_NOT`, `EGB_REGISTER_UNIT_NOT`, `ELG_UPDATE_USER_UNIT_BUFF_INFO_REQ` | Login, Global | só avisos, nada espera por eles |
| entrada na vila | `ERM_JOIN_FIELD_*` | o próprio GameServer (`SendToGSField`) | — |

Conclusão: **para chegar à vila só falta o LoginServer** (45 `.cpp`, ~42 mil linhas no original, 2026-10-31). O CenterServer (~87 mil
linhas: salas de dungeon, PvP, praça, festas, troca) e o GlobalServer (~21 mil: missões globais, quest aleatória do dia, CCU) só entram
depois da vila. O LoginServer não conecta em nenhum outro servidor: é ele que recebe as conexões.

## Plano (um branch por passo)

1. ~~**`server/game-import`**~~: este passo.
2. ~~**`server/game-build`**~~ (2026-10-30): compilar sem warnings em GCC, Clang e clang-cl, com o mesmo `main` (`KServerMain`) e os hooks do servidor
   (`g_szConfigFile`, `CreateDBThread`). Maior que o ChannelServer (15× o código): pode virar mais de um branch.
3. **`server/game-run`** (em andamento): `x2_run_game` (`runtime/GameServer`, `Game01`) e o GameServer até o estado pronto, com o cliente
   conectando nele. O login para no `EGS_VERIFY_ACCOUNT_REQ` até existir o LoginServer ([Dependências](#dependências-de-outros-servidores-levantamento-de-2026-10-31)).
4. **`server/login-import`**, **`server/login-build`** e **`server/login-run`**: o LoginServer, como o ChannelServer e o GameServer.
   Com ele, o cliente da seleção de personagem até a vila. O CenterServer e o GlobalServer vêm depois (dungeon, PvP, festas).
