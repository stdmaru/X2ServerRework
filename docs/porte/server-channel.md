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

## Estado (2026-10-30, branch `server/channel-run`)

Roda no Linux pela pasta montada por `x2_run_channel` ([Pastas de execução](../ferramentas/runtime.md)): conecta no SQL Server do Docker
(FreeTDS), carrega os scripts (`Enum.lua`, `DungeonEnum.lua`, `GameSysValTable.lua`, `WaitQueueData.lua`, `LogConfig.lua`...),
se autentica no banco (UID 59, "Channel"), lê a lista de canais e chega ao estado pronto (`OnServerReadyComplete`), com o prompt Lua
respondendo. Encerra com código 0 (ver [Rodar os servidores](../running.md)).

**Testado com o cliente original** (`x2_DEBUG.exe` e `x2.exe`, no Wine, 2026-10-30), com o [EPublisherServer](server-epublisher.md):
o login é aceito e o cliente recebe as listas de grupos de servidores e de canais. Em seguida ele conecta no GameServer
(`LChannelList`: `127.0.0.1:9300`), que ainda não existe, e mostra "O servidor está cheio. Tente de novo.", o esperado. O primeiro teste
falhou por causa das chaves padrão do `KncSecurity` ([KNCSDK](kncsdk.md#portabilidade-sem-mudança-de-comportamento)).

## Build (2026-10-30, branch `server/channel-build`)

O executável `ChannelServer` (alvo `x2_channel_server`) **compila e linka sem warnings** em GCC Debug, Clang e MinGW (build do zero).
Sem argumentos ele sobe, recusa a falta de argumentos como o original (`Init` falha), encerra todas as camadas e sai com código 1,
no Linux e no Wine. Ainda não rodou com configuração, banco e cliente: é o passo `server/channel-run`.

## Executável e linha de comando

- `main.cpp` chama `KServerMain` (`Common/ServerMain.{h,cpp}`), que substitui a janela MFC do original (`Common/ui`: `CServerApp`/`CServerUIDlg`)
  com a mesma sequência: `CreateKNC()`, `SetCmdArgs`, `Init`, `Run`, `ShutDown`, `ReleaseKObj`. O campo de entrada Lua da janela é o
  prompt do `lua_interpreter` (`KBaseServer::Run( true )`). Desde 2026-10-31 é o terminal da T15 ([KNCSDK › Terminal](kncsdk.md#terminal-t15)).
- Argumentos como no original: `ChannelServer <tag> <grupo de servidores> [<id do banco> <senha do banco>]`; a tag escolhe
  `config_ch_<tag>.lua` (por exemplo `US_SERVICE`).
- O código de saída é 1 quando o `Init` falha (a janela original ficava aberta até ser fechada).
- `g_szConfigFile` e `CreateDBThread`, que o `Common` espera de cada servidor, vêm do `ChannelServer.cpp` (como no original);
  os testes do `Common` continuam com os stubs de `server_hooks.cpp`.

## Mudanças em relação ao original

| Onde | Mudança | Motivo |
|---|---|---|
| `KChannelServer`, `KChannelSimLayer` | Saem as chamadas ao `ProcessCommunicationManager` (KSMS: *named pipes* `\\.\pipe\ChannelServerRead`/`Write`, religadas a cada 60 s no `Tick`, e as mensagens de modo de manutenção) | Ferramenta de gerência da KOG, excluída do `Common`. Só funcionava com `GameSysVal:SetProcessCommunication(true)` |
| `ON_DBE_SERVER_ON_ACK` | Quando o banco recusa o servidor, a resposta marca `SetAuthenticateResult( false )` e libera o `Authenticate`; o `Init` falha e o servidor encerra pelo `ShutDown` (código 1) | O original chamava `::exit( 0 )` nessa thread, com as outras rodando: os destrutores estáticos liberavam o pool de eventos em uso (falha de segmentação achada com ASan em 2026-10-30) |
| `config_ch_DEV.lua` | `SetLocalIP( ... )` fixa o endereço com que o servidor se identifica | O original usava o primeiro endereço do nome do host, que no Linux pode ser qualquer interface (aqui, a VPN) |
| `-popup` | Recusado com mensagem; os dados do banco vão como argumentos | Era um diálogo MFC pedindo id e senha |
| `ChannelAccountDBThread` | `boost::variate_generator` (OTP) → lambda sobre a mesma distribuição; `_itow` → `_itow_s` | Mesmo resultado |
| `ChannelLogDBThread`, `ChannelAccountDBThread`, `ChannelSMSDBThread` | `ProcessEvent` deixa de ser `virtual inline` (é definido no `.cpp`) | `inline` sem definição no header é ODR inválido; o clang-cl acusa (`-Wundefined-inline`). O GameServer tem o mesmo padrão |

Portabilidade (sem mudança de comportamento): passada mecânica (Boost → `std`/`x2::`, includes de plataforma), `goto end_proc`
que cruzava uma declaração em `ON_DBE_SERVER_OFF_NOT` (declaração subiu), `BaseServer.h` incluído nos headers cujos templates usam
`KBaseServer::GetKObj()`, destrutores `virtual` em `KCCUToolManager`, `KMachineBlockManager` e `KWaitQueueManager` (apagados pelos
singletons por ponteiro da base), `using` das sobrecargas escondidas de `RoutePacket`/`Init`. As mudanças no `Common` estão em
[Servidor: Common › Mudanças](server-common.md#mudanças-em-relação-ao-original).

## Pendências

- O `cerr` "INVALID_SOCKET" do `KAccepter` no encerramento vem de fechar o socket que espera no `accept`: também acontece no original (mantido).
