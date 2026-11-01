[Início](../README.md) › [Porte](README.md) › Servidor: CenterServer

# Servidor: CenterServer (`src/server/CenterServer`)

Servidor das salas: campos de batalha (`BattleFieldRoom`), dungeons (`DungeonRoom`, inclusive Henir e defesa), PvP (`PvPRoom`, partidas
e resultados), praça (`Square`), festas (`NewCnParty`, febre de festa), troca (`Trade`), loja pessoal (`PersonalShop`), centro de
treino e salão de casamento. É o lado servidor da simulação: monstros, drops e resultados das salas. Recebe as conexões dos GameServers
(porta 9100); os GameServers mandam para ele os `ERM_*` (o primeiro a aparecer é o `ERM_OPEN_BATTLE_FIELD_REQ`, ao sair da vila).
Origem: `KncWX2Server/CenterServer`, importado em 2026-10-31 com as [flags do servidor](../tools/flags.md)
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
| Boost | 31 arquivos (`timer` 28, `wformat`/`str` 54, `shared_ptr` 9, `bind` 3) | passada mecânica ([Porte mecânico](../tools/mechanical-porting.md)); `bind` à mão |
| `windows.h`/winsock/ATL | 14 | `x2/platform/win32.h`, `net.h` |
| UDP (`InteriorUdpSession`, sobre o `KTDNUDP` do `Common`) | 2 | o `KTDNUDP` já foi portado (`server/common-udp`) |
| `CTime`, `lua_tinker` | 16, 15 | já cobertos pela [Plataforma](platform.md) e pelo KNCSDK |
| Banco | 50 `DO_QUERY` | `KODBC` já portado; as procedures são as do `Game01`, `Log` e `Account` |

## Estado (2026-10-31, branch `server/center-run`)

**Roda no Linux** pela pasta montada por `x2_run_center` ([Pastas de execução](../tools/runtime.md)): conecta nos três bancos
(`Account`, `Game01`, `Statistics`), carrega os scripts (os erros de carga são os de dungeon e linemap já listados em
[Bugs do original](../original/bugs.md#dados-serverresource-clientscript)), se autentica no banco e chega ao estado pronto; o GameServer
conecta nele (porta 9100). Encerra com código 0 (ver [Rodar os servidores](../running.md)).

**Testado com o cliente original** (no Wine, 2026-10-31, com o EPublisherServer, o LoginServer, o ChannelServer e o GameServer): saída da
vila para o **campo**, entrada numa **dungeon** e troca de personagem, sem erro no CenterServer. Os pacotes para o GlobalServer (`EGB_*`:
CCU, missões, quest do dia, bloqueio) continuam descartados pelo GameServer, sem travar nada. A troca de personagem derrubava o GameServer
(Debug) no relay TCP: era o `KSerBuffer::UnCompress` de um evento sem dados, corrigido no [KNCSDK](kncsdk.md).

### Build (2026-10-31, branch `server/center-build`)

**Compila e linka sem warnings** (alvo `x2_center_server`, executável `CenterServer`, `main` com `KServerMain` como os outros
servidores) em Clang Debug, GCC Debug, Clang Release, GCC Release e clang-cl (Windows, na VM de testes), com os 6 testes passando
no Linux e no Windows.

Medidas do caminho, para dar a escala: 51 erros (Clang Debug, `-k 0`) em 13 arquivos depois da passada mecânica, 29 no GCC (quase
todos `goto end_proc`); 112 warnings distintos no Clang e 143 no GCC antes do `fix_warnings.py` e das correções à mão. Menor que o
LoginServer: o grosso do acoplamento já tinha saído no `Common`.

## Mudanças em relação ao original

| Onde | Mudança | Motivo |
|---|---|---|
| `KCnServer`, `CnRMEventProcess` | Saem as chamadas ao KSMS (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\CenterServer*`, religadas a cada 60 s no `Tick`, e o aviso de script recarregado) | Como no [ChannelServer](server-channel.md#mudanças-em-relação-ao-original) e nos outros servidores |
| `ON_DBE_SERVER_ON_ACK` (`CnServer.cpp`) | Quando o banco recusa o servidor, a resposta marca `SetAuthenticateResult( false )` e o `Init` falha; quando aceita, marca `true` | Como no [GameServer](server-game.md#mudanças-em-relação-ao-original): o original chamava `::exit( 0 )` nessa thread |
| `KBattleFieldMonsterManager::SetMonsterDie( int, UidType )` | Renomeado para `SetFieldMonsterDie` (as 4 chamadas do `KBattleFieldRoom`) | Escondia a virtual `KRoomMonsterManager::SetMonsterDie( int, UidType, bool = true )`; um `using` deixaria as chamadas de 2 argumentos ambíguas. As chamadas eram todas pelo tipo concreto: nada muda |
| `Cn*DBThread.h` | `ProcessEvent` deixa de ser `virtual inline` | Definido no `.cpp`: ODR inválido (`-Wundefined-inline` do clang-cl), como no LoginServer |

Portabilidade (sem mudança de comportamento): passada mecânica (`mechanical.py`); declarações puladas por `goto end_proc` sobem
(`fix_goto.py`, que passou a entender arrays com `= {0,}`), e duas referências `ITEM_DATA&` do `ERM_GET_ITEM_REQ` (campo e dungeon)
viram ponteiro declarado no topo; `boost::timer` → `x2::Timer` também no `Knc::KSingleObject` do KNCSDK (`KncContainer.h`, que nenhum
servidor usava até agora); o argumento padrão temporário numa referência não `const` (`KSquareUserManager::SetPersonalShopData`, extensão
do MSVC) vira uma sobrecarga; `__noop`, `intsafe.h` e as macros `min`/`max` (com o tipo do original: `size_t`, `__time64_t`, `char`)
saem; includes que o MSVC dispensava nos templates (`BaseServer.h` nos DB threads); destrutores `virtual` em 8 singletons; `using` das
sobrecargas escondidas de `Init` e `SetRoomInfo`; `[[fallthrough]]` no fechamento de festa (`S_* → I_TO_CLOSE → I_TO_INIT`, de
propósito); variáveis mortas removidas (o `GetKillNPC_LUA()` dos resultados de dungeon continua sendo chamado: ele sobe o `m_nKillNPC`
para 1); o cast descartado do `DUNGEON_TIME_RESULT` sai (o rank vem do `GetRank()` logo depois); o teste `0 <= iRank` (`size_t`, sempre
verdadeiro também no MSVC) sai; `nGSUID`, `m_CheckClass`/`m_CheckType` e `iCandidateTeam` começam em 0 (o GCC Release acusava
`-Wmaybe-uninitialized`; só eram lidos depois de receber valor); o `erase` do `KncUtil::NumberShuffle` (KNCSDK) vira `std::move` +
`pop_back`, a mesma operação na mesma ordem, porque o GCC 15 Release acusava um falso `-Wfree-nonheap-object` no `erase` expandido.

### Bugs do original corrigidos

| Onde | Bug | Correção |
|---|---|---|
| `KDropTable::AddExtraStageMonsterDropInfo_LUA` | A validação da chave era `m_nMonsterID == -1 && m_nMonsterID == 0`, sempre falsa: uma entrada sem `MonsterID` (ou com 0) entrava na tabela de drop do estágio extra | `\|\|` (2026-10-31, `-Wtautological-overlap-compare` do Clang) |

## Pendências

- O bug da checagem de "sub-stage limpa em 1 s" do `DungeonRoom` fica mantido ([Bugs do original](../original/bugs.md)).
- SMS (`CnSMSDBThread`) compila e continua: a config US não liga o `DC_SMS`. Conferir ao rodar.

## Plano (um branch por passo)

1. ~~**`server/center-import`**~~ (2026-10-31).
2. ~~**`server/center-build`**~~ (2026-10-31): compilar sem warnings em Clang, GCC e clang-cl, com o `main` do `KServerMain` (como os outros servidores).
3. ~~**`server/center-run`**~~ (2026-10-31): `runtime/CenterServer`, `x2_run_center`, e o cliente original no campo e numa dungeon. O GameServer acha o
   Center pela lista do banco (`Statistics.LServerList`) e reconecta a cada 10 s
   ([Dependências do GameServer](server-game.md#dependências-de-outros-servidores-levantamento-de-2026-10-31)).
