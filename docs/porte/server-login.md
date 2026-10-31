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

## Estado (2026-10-31, branch `server/login-build`)

**Compila e linka sem warnings** (alvo `x2_login_server`, executável `LoginServer`, `main` com `KServerMain` como o ChannelServer e o
GameServer) em Clang Debug, GCC Debug, Clang Release, GCC Release e clang-cl (Windows, na VM de testes), com os 6 testes passando no
Linux e no Windows. Ainda não roda: falta a pasta de execução (`server/login-run`).

Medidas do caminho (Clang Debug, `-k 0`), para dar a escala: 103 erros em 15 arquivos depois da passada mecânica, 51 depois de tirar a
Nexon; 39 warnings no GCC (11 destrutores não virtuais). Bem menor que o GameServer: o grosso do acoplamento já tinha saído no `Common`.

## Mudanças em relação ao original

| Onde | Mudança | Motivo |
|---|---|---|
| Nexon (KR): `NexonAuth*`, `NexonBillingAuth*`, `AuthCheck` | Arquivos removidos. Saem também os ramos `AF_NEXON_KOREA` do `LoginSimLayer` (thread do `NXAuthManager`, `Tick`, `ShutDown`, `RegToLua`) e do `KUserList` (`NexonAuthLogin`/`Logout` no registro e na saída do usuário), a sincronização e o timeout de sessão de PC-bang (`CheckAuthSynchronize`, `CheckUnauthorizedSession`, `LoginTimeOut`, `UpdateSessionNoByUserID`), os handlers `ENX_AUTH_*` do `KLoginServer` e o `ELG_NEXON_USER_LOGIN_TIME_OUT_NOT` do `KLoginUser`, e o `AuthenticateUser` (sem chamador). A rota `PC_NX_AUTH` só registra o erro que o original já registrava fora da Coreia | T10. No US (`AF_GLOBAL_SERVICE`) era código morto: tudo dependia de `AF_NEXON_KOREA`, e o GameServer só manda o timeout depois de um login de PC-bang da Nexon. O `AuthCheck` é um binário proprietário sem fonte ([Binários proprietários](../original/binarios-proprietarios.md)). O pacote `ELG_NEXON_USER_AUTHENTICATE_REQ`, que o US usa, fica (o nome é da Nexon, a autenticação é no `Account`) |
| `KLoginServer::OnServerReadyComplete` | Saem `AuthCheck_SetPassportLevel`/`SetLocale`. O `Server:SetPassportLevel` do Lua continua aceito, sem efeito | Idem |
| `ELG_PRESENT_CASH_ITEM_REQ` (`LoginUser.cpp`) | Responde sempre `ERR_BUY_CASH_ITEM_21` (senha errada) | A senha era conferida com o hash do `AuthCheck`. Nenhum GameServer US manda esse pedido |
| `OnlyGlobal/` (`LoginUser_CN`, `_TH`, `UserList_JP`, `_TH`, `_TW`) | Arquivos removidos | Outros países: depois das flags só tinham includes |
| `KLoginServer` | Saem as chamadas ao KSMS (`ProcessCommunicationManager`, *named pipes* `\\.\pipe\LoginServer*`, religadas a cada 60 s no `Tick`, e o aviso de script recarregado) | Como no [ChannelServer](server-channel.md#mudanças-em-relação-ao-original) e no [GameServer](server-game.md#mudanças-em-relação-ao-original) |
| `KRankingManager` (`RefreshTimeBackup.ini`) | Caminho com `std::filesystem::current_path()` no lugar de `GetCurrentDirectoryW` + `\\`; a gravação usa o `WritePrivateProfileStringW` novo da [Plataforma](platform.md) (`WriteIniString`), que grava UTF-8 | O Linux não tem a API; o arquivo só tem números ASCII |
| `*DBThread.h` | `ProcessEvent` deixa de ser `virtual inline` | Definido no `.cpp`: ODR inválido (`-Wundefined-inline` do clang-cl), como no ChannelServer |

Portabilidade (sem mudança de comportamento): passada mecânica (`mechanical.py`); `boost::variate_generator` → lambda sobre o
`std::uniform_int_distribution` (OTP, como no ChannelServer); declarações puladas por `goto end_proc` sobem (`fix_goto.py`, mais 4 à mão);
includes que o MSVC dispensava nos templates (`BaseServer.h` nos DB threads, `ActorManager.h` no `LoginServer.h`); destrutores `virtual`
em 9 singletons; `using KThreadManager::Init` no `KLoginScriptRefreshManager`; variáveis mortas e expressões sem efeito removidas;
`memset` do buffer de `wchar_t` com `sizeof` (zerava só metade, sem efeito porque a leitura termina a string); o teste `unsigned < 0`
do tipo de anúncio de guilda sai (sempre falso também no MSVC).

## Pendências

- Bugs do original mantidos (nível de skill de guilda acima do máximo): [Bugs do original](../original/bugs.md), para a Fase 5.
- SMS (`LoginSMSDBThread`, `SMSPhoneNumberManager`) e o `DLLManager` compilam e continuam: a config US não liga o `DC_SMS` e o
  `DLLManager` só lê a lista de módulos do banco. Conferir ao rodar.

## Plano (um branch por passo)

1. ~~**`server/login-import`**~~ (2026-10-31).
2. ~~**`server/login-build`**~~ (2026-10-31): compilar sem warnings em Clang, GCC e clang-cl, com o `main` do `KServerMain` (como o ChannelServer e o GameServer).
3. **`server/login-run`**: `runtime/LoginServer` (`config_lg_DEV.lua.in`), `x2_run_login`, e o login do cliente pelo GameServer:
   `ELG_NEXON_USER_AUTHENTICATE_REQ` (`P_MUser_SEL_Auth` no `Account`) e `ELG_REGISTER_USER_REQ` →
   `ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK`; depois, a seleção de personagem e a vila
   ([Dependências do GameServer](server-game.md#dependências-de-outros-servidores-levantamento-de-2026-10-31)).
