[Início](../README.md) › [Porte](README.md) › Servidor: LoginServer

# Servidor: LoginServer (`src/server/LoginServer`)

Servidor central de contas: autentica o usuário que entra no GameServer (banco `Account`), mantém a lista de usuários conectados
e o bloqueio de conta, e guarda o que é do grupo de servidores inteiro: guildas (com skills, anúncios e quadro), rankings (incluindo
Henir e o local), agência de loja pessoal, buffs, salão de casamento, eventos de item aleatório e o CCU. Recebe as conexões dos
GameServers; não conecta em nenhum outro servidor. Origem: `KncWX2Server/LoginServer`, importado em 2026-10-31 com as
[flags do servidor](../ferramentas/flags.md) ([relatório](../../tools/import/reports/server-login/REPORT.md)). Usa o
[Common](server-common.md), já portado. Mesma posição relativa do original (`src/server/LoginServer` ao lado de `src/server/Common`).

## Importação (2026-10-31)

- **Seleção**: os arquivos próprios do `LoginServer/` que o `LoginServer.vcxproj` compila (`Release_US`) e os headers deles:
  **86 arquivos** (44 `.cpp`, 42 `.h`), 31 mil linhas escritas, 27 mil de `.cpp`. Os maiores: `LoginUser.cpp` (4,6 mil),
  `LoginGameDBThread.cpp` (2,5 mil), `Guild.cpp` (2,1 mil). O `Main.cpp` da pasta não entra no projeto. Os includes para o `Common`
  não são seguidos (`include_scope`).
- **Flags**: 1.110 blocos `#if` resolvidos, 11 mil linhas removidas. Nenhum bloco virou `X2_DEV_TOOLS`.
- **Nada excluído na importação**, como no GameServer: o que é de publisher ou legado sai no build, com a mudança registrada aqui.
- Encodings: 76 CP949, 10 ASCII.

## Acoplamento (2026-10-31)

| Área | Arquivos/usos | Plano |
|---|---:|---|
| Nexon (KR): `NexonAuth*` (PC-bang/autenticação por TCP), `NexonBillingAuth*`, `AuthCheck` (`authcheck.lib`, binário proprietário: `AuthCheck_SetPassportLevel`/`SetLocale` no `OnServerReadyComplete`) | 12 arquivos; `SiKNexonAuthManager` no `UserList` e no `LoginSimLayer` | publisher (T10): sai ou vira stub. O US autentica pelo `ELG_NEXON_USER_AUTHENTICATE_REQ` no `Account` (ramo `SERV_GLOBAL_AUTH`), não pela Nexon |
| Arquivos de país (`OnlyGlobal/LoginUser_CN`, `_TH`, `UserList_JP`, `_TH`, `_TW`) | 5 | depois das flags só têm includes: saem |
| SMS / Mail / NXWeb (`LoginSMSDBThread`, `LoginNXWebDBThread`, `Common/Mail`, `SMSPhoneNumberManager`) | 2 + `Common` | legado: conferir no build se o US chega a usar; o `LoginNXWebDBThread` já ficou vazio |
| KSMS (`ProcessCommunicationManager`, *named pipes*), `DLLManager` | `LoginServer.cpp`, `LoginSimLayer.cpp` | KSMS sai como no ChannelServer e no GameServer; o `DLLManager` (carga de DLL) a conferir |
| Boost | 28 arquivos (`wformat`/`str` 120, `shared_ptr` 45, `timer` 13, `random` 10, `bind` 2) | passada mecânica ([Porte mecânico](../ferramentas/porte-mecanico.md)); `random`/`bind` à mão |
| `windows.h`/winsock/ATL | 11 | `x2/platform/win32.h`, `net.h` |
| `CTime`, `lua_tinker` | 25, 10 | já cobertos pela [Plataforma](platform.md) e pelo KNCSDK |
| Banco | 119 `DO_QUERY` | `KODBC` já portado; as procedures são as do `Account`, `Game01` e `Log` |

## Plano (um branch por passo)

1. ~~**`server/login-import`**~~ (2026-10-31): este passo.
2. **`server/login-build`**: compilar sem warnings em Clang, GCC e clang-cl, com o `main` do `KServerMain` (como o ChannelServer e o GameServer).
3. **`server/login-run`**: `runtime/LoginServer` (`config_lg_DEV.lua.in`), `x2_run_login`, e o login do cliente pelo GameServer:
   `ELG_NEXON_USER_AUTHENTICATE_REQ` (`P_MUser_SEL_Auth` no `Account`) e `ELG_REGISTER_USER_REQ` →
   `ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK`; depois, a seleção de personagem e a vila
   ([Dependências do GameServer](server-game.md#dependências-de-outros-servidores-levantamento-de-2026-10-31)).
