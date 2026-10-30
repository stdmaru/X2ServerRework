[Início](../README.md) › [Porte](README.md) › Servidor: Common

# Servidor: Common (`src/server/Common`)

Código compartilhado pelos cinco servidores, e em parte pelo cliente (pacotes, enums, `EventID_*`).
Origem: `KncWX2Server/Common`, importado em 2026-10-29 com as [flags do servidor](../ferramentas/flags.md)
([relatório](../../tools/import/reports/server-common/REPORT.md)).
Mesmo nome de pasta do original (`Common`), porque os fontes incluem `"../Common/x.h"`.

## Importação

- **Seleção**: só os fontes do `Common/` que algum vcxproj de servidor compila (`Release_US`): 239 fontes, mais os headers que eles incluem, deram **414 arquivos**.
  Dos 239 fontes, 107 são usados pelos cinco servidores e 89 por um só.
- **Flags**: 6.627 blocos `#if` resolvidos e 49 mil linhas removidas. Dev (`X2_DEV_TOOLS`): só os logs `SERV_LOG_UNDEFINED_QUEST_TEMPLET` e `DELETE_INCORRECT_ERROR_LOG`.
- **Excluídos**:

  | O que | Motivo |
  |---|---|
  | `ui/` (MFC) | substituído pelo terminal, T15 |
  | `OnlyGlobal/AuthAndBilling/<país>/` | integrações de publisher de cada país, inclusive as do US (K3RCombo, Payletter), que o DEB substitui |
  | `OnlyGlobal/NetCafe`, `Nexon`, `SOAP` | integrações de publisher |
  | `OnlyGlobal/ProcessCommuniationModule` | ferramenta KSMS |
  | `CrashRpt`, `Mail/` | binários fechados (SMTP de alerta) |

  As referências a eles, ~190 includes, viram stubs ou somem na passada manual.
- **Mantido: DEB ("Debelzak Custom Auth")**, ligado por `_DEB_AUTH` (Channel e Game no `Release_US`). **Não é código da KOG**: foi escrito pelo dono do projeto,
  junto com um **servidor SOAP próprio** de autenticação, feito do zero. É a autenticação (`KDEBAuth`, cliente gSOAP que chama `Login` num `AuthService`
  de endereço configurável) e a cobrança (`KDEBBilling`) **usadas por este servidor**. Nas cadeias de `#if`, ele substitui o publisher US.
  Entram `AuthAndBilling/DEB*`, os stubs gSOAP de `AuthAndBilling/DEB/` e o `KncSendToPublisher.h`.
  Erro corrigido em 2026-10-29: uma primeira versão desligou o `_DEB_AUTH` achando que era só integração de publisher.
- Includes para o KNCSDK com a grafia corrigida (`KncSingleton.h` → `KNCSingleton.h`, `RTTI.H` → `RTTI.h`...) via `extern_dirs`.

## Acoplamento restante (2026-10-29)

| Área | Arquivos | Plano |
|---|---:|---|
| gSOAP (`AuthAndBilling/DEB/`, `stdsoap2`) | 3 | código C portável; gerar de novo com gSOAP atual ou compilar como está, com sockets da plataforma |
| `windows.h`/ATL/WinInet/`stdafx.h` | ~35 | `x2/platform/win32.h`; WinInet (`KFtpWrapper`) avaliar se é necessário |
| Threads/eventos (`_beginthreadex`, `WaitForSingleObject`...) | 29 | já cobertos pela [Plataforma](platform.md) |
| `CTime`/`CTimeSpan`, `CString` | 46 / 10 | cobertos pela Plataforma (ampliar o `CString` se faltar algo) |
| `__int64`, `__forceinline`, `__declspec` | 15 | tipos/macros da Plataforma |
| Boost | 39 | `std` |
| Lua (`lua_tinker`, `luaL_*`) | 53 | `x2::kncsdk_lua` |
| **Sockets/IOCP** (`Socket/`) | 15 | rede portável (Asio standalone), mantendo `KSession`/framing/cripto |
| ~~**ODBC** (`ODBC/`)~~ | 2 | feito em 2026-10-30 (T17): [Banco de dados (ODBC)](server-common-db.md) |
| **Matemática D3DX** (`X2Data/XSLBattleField`, `LineMap/`, `CommonPacket.h`) | 6 (187 usos) | `x2/platform/d3dx_math.h` (pronto); trocar `#include <d3dx9math.h>` por ele |
| Dependência invertida | 1 | `KStatisticsLocalLog.cpp` inclui `GameServer/Inventory.h` e `CenterServer/PvPRoom.h` |

Tamanho: ~99 mil linhas de `.cpp` sem os stubs gSOAP. Os maiores são os pacotes: `ServerPacket.cpp` (16 mil), `ClientPacket.cpp` (12 mil) e `CommonPacket.cpp` (7 mil).

## Plano do porte manual (um branch por passo)

1. ~~**`platform/math`**~~: feito em 2026-10-29 (`x2/platform/d3dx_math.h`, ver [Plataforma](platform.md)).
2. ~~**`server/common-build`**~~: feito em 2026-10-29 (ver Estado abaixo). Alvo CMake com todo o `Common`, menos `Socket/` e `ODBC/`, compilando em GCC/Clang.
   É a passada mecânica: includes de plataforma, `typename`/`template`, Boost → `std`, stubs das integrações removidas.
3. ~~**`server/common-packets`**~~: feito em 2026-10-29 (ver Testes abaixo). Testes de serialização dos pacotes (`ClientPacket`/`CommonPacket`), que são a base da compatibilidade (T4).
4. ~~**`server/common-net`**~~: feito em 2026-10-29. TCP de `Socket/` sobre Asio (T20), ver Rede abaixo.
5. ~~**`server/common-udp`**~~: feito em 2026-10-29. UDP de `Socket/` (`KTDNUDP`, `UdpEcho`, `LBSUdpEcho`, `X2Packet`), ver Rede abaixo.
6. ~~**`server/common-db`**~~: feito em 2026-10-30. `ODBC/` portável (T17), testado contra o SQL Server com o FreeTDS: [Banco de dados (ODBC)](server-common-db.md).

Depois disso vem o ChannelServer (`server/channel`).

## Estado (2026-10-29, branch `server/common-build`)

O alvo `x2_server_common` (todo o `Common`, menos os `.cpp` de `Socket/`, `ODBC/`, `KStatisticsLocalLog.cpp` e `DEBBilling*`)
**compila no Linux sem erros e sem warnings** (GCC 16 Debug/Release, Clang 23 Release). `Socket/` (TCP e UDP) compila e tem
teste desde o `server/common-net`/`server/common-udp`, e o `ODBC/` desde o `server/common-db` (2026-10-30, ver [Banco de dados (ODBC)](server-common-db.md)). Ainda **não há testes** do `Common` (vêm no passo `server/common-packets`) e nada foi executado.
No **MinGW** o `Common` e os testes dele compilam sem warnings e passam no Wine desde 2026-10-30 (LuaJIT pela T21). Foi a primeira
compilação dos caminhos `_WIN32` do `Common` (Asio sobre IOCP, `ConfigureUdpSocket`, `odbc32`); o que faltou está em Mudanças › Portabilidade.

Caminho: ~15.000 erros após a importação → ~4.000 (passada mecânica) → 2.291 no início da retomada → 0. Depois, 81 mil warnings
(73 mil vinham de um só header) → 0. Ferramentas usadas: [Porte mecânico](../ferramentas/porte-mecanico.md).

## Rede (`Socket/`, TCP)

O motor do IOCP virou `x2::platform::net` (decisão T20, ver [Plataforma](platform.md)); a API que o resto do servidor usa ficou igual.

| Original | Agora |
|---|---|
| `KIocp`: completion port + `GetQueuedCompletionStatus` | `KIocp`: um `IoService`; as chaves de completion e o despacho `OnIOCompleted`/`OnClosedByLocal` continuam |
| `KIOThread::Loop`: espera um pacote de completion | roda os handlers prontos por até 100 ms (`IoService::RunFor`) |
| `KSocketObject`: `SOCKET` + `WSARecv`/`WSASend` overlapped | `TcpStream` + leitura/escrita assíncronas; o handler guarda o objeto vivo até a operação acabar |
| Modo sem IOCP (`KSession::Init( false )`): eventos no `OVERLAPPED` | o handler guarda o resultado no `KOVERLAPPED` e sinaliza o mesmo evento; a thread da sessão chama `OnIOCompleted` |
| `KAccepter`: `WSASocket` + `accept` bloqueante | `socket` + `accept` bloqueante; o socket aceito é entregue ao `TcpStream` |

UDP (`server/common-udp`):

| Original | Agora |
|---|---|
| `CKTDNUDP`: até 512 `WSARecvFrom` pendentes e `WSASendTo`, com *completion routines* (APC) executadas na espera alertável da thread dona | socket não bloqueante: `recvfrom` em laço (até 512 por chamada) e `sendto` direto, cada um seguido do mesmo tratamento que a rotina fazia, na mesma thread |
| `KLBSUdpEcho::Run`: `WaitForSingleObjectEx( kill, INFINITE, TRUE )` | espera o socket ter dados (até 10 ms) e confere o evento de término |
| `KUdpEcho::Loop`: `recvfrom` bloqueante; `ShutDown` fechava o socket para destravá-lo | espera dados por até 100 ms antes do `recvfrom`; `ShutDown` encerra a thread e depois fecha o socket (fechar não destrava o `recvfrom` no Linux) |
| `SIO_UDP_CONNRESET`, `UDP_CHECKSUM_COVERAGE` | `x2::platform::net::ConfigureUdpSocket` (só Windows) |

- `X2Lib/X2UDPPacket.*` (structs dos datagramas, do cliente) vieram pelo módulo `server-x2lib` do importador, em `src/server/X2Lib`
  (a mesma posição relativa do original). Os dois `u_long` de `KXPT_PORT_CHECK_REQ/ACK` viraram `std::uint32_t`: no Linux teriam 8 bytes
  e mudariam o datagrama. `static_assert` confere os tamanhos.
- Declarações com `__forceinline` cujo corpo está no `.cpp` perderam o `__forceinline` (o GCC recusa).
- `UdpRelayChecker/`: no build US os arquivos ficam vazios depois das flags; nada a portar.

Mudanças de comportamento (intencionais):

- `~KIOThread` não posta mais `SIGNAL_SHUTDOWN_SERVER`: no original isso encerrava *outra* thread de I/O, a que pegasse o pacote.
- `~KIocp` encerra as threads de I/O, fecha os sockets registrados e roda as completions pendentes antes de destruir o serviço.
- `KIocp::GenNewCompKey`: gerador semeado (o original usava um `mt19937` sem semente). As chaves só precisam ser únicas.
- `NetCommon::GetWsaMsg`: mensagem por thread (o original usava um buffer estático compartilhado); `GetLocalIP` devolve `""` em vez de `NULL` quando falha.
- `KSimObject`: o nome (`SOB_<data>_<hora>_<n>`) saía `SOB_(null)_(null)_...`, porque o original passava o código de retorno de `_wstrdate_s` como string.
- `OnSendCompleted` usava `OVERLAPPED::InternalHigh` para descontar o enviado; agora usa o número de bytes da completion (é o mesmo valor).

## Testes (`src/server/Common/tests`, alvo `x2_server_common_tests`)

| Teste | O que cobre |
|---|---|
| `test_packets_{client,common,server}.cpp` | **Todos os 1.964 tipos serializados** (os de `#if X2_DEV_TOOLS` só nesse build) dos pacotes (2026-10-29): `Put` → `Get` repetido sobre um objeto inicializado por valor: o tamanho não muda, os bytes se repetem a partir da segunda passada (alguns construtores deixam membros sem inicializar, então a primeira pode ter lixo) e o `Get` lê tudo. Gerados por `tools/port/gen_packet_tests.py` ([Porte mecânico](../ferramentas/porte-mecanico.md)), que antes confere que `PUT` e `GET` listam os mesmos campos na mesma ordem e que cada bloco só usa a própria macro |
| `test_login_packets.cpp` | Bytes exatos de `ECH_VERIFY_ACCOUNT_REQ/ACK` e `EGS_CONNECT_REQ/ACK`, montados à mão a partir da ordem dos campos e do [formato da rede](../original/protocolo.md) |
| `test_session.cpp` | `KSession` por loopback com as threads de I/O reais: accept, troca da chave (`E_ACCEPT_CONNECTION_NOT`), pacotes cifrados nos dois sentidos, um pacote de 24 KB (mais de uma leitura) e a desconexão. Nos dois modos do proxy: nas threads de I/O e com thread própria (sem IOCP) |
| `test_udp.cpp` | Datagramas entre dois `CKTDNUDP` (ID + struct, conferindo endereço e porta do remetente) e o `XPT_PORT_CHECK_REQ` respondido pela thread do `KLBSUdpEcho`, que precisa terminar sem tráfego |
| `test_odbc.cpp` | `KODBC`: conversão de `.dsn` e filtros sem banco; suíte `db` contra um SQL Server real quando `X2_TEST_ODBC` está definido. Ver [Banco de dados (ODBC)](server-common-db.md#testes) |
| `server_hooks.cpp` | Não é teste: define o que cada servidor fornece ao `Common` (`g_szConfigFile`, `CreateDBThread`) e o `KStatisticsLocalLog` (compilado com o GameServer), para os testes linkarem sem um servidor |
| `packets_generated` (ctest) | Falha se os testes gerados estiverem desatualizados em relação aos `.cpp` dos pacotes |

**Limite:** os testes provam que o porte é coerente consigo mesmo e com o formato lido no código, não que bate byte a byte com o
`elsword.exe`. Isso continua pendente ([KNCSDK › Pendências](kncsdk.md#pendências)). Também não pegam um campo que exista no servidor
e não no cliente (as flags do cliente ainda não foram geradas, ver [Roadmap](../projeto/roadmap.md)).

## Mudanças em relação ao original

### Intencionais

| Onde | Mudança | Motivo |
|---|---|---|
| `KAutoPath`, `KFolderTraverse`, estatísticas | `FindFirstFile`/`GetCurrentDirectory`/`CopyFile`... → `std::filesystem`. `KApplyAllFiles` passa `directory_entry` ao functor e não muda mais o diretório atual | Portável. A busca de scripts continua na mesma ordem (pasta atual, depois subpastas em profundidade, por nome) |
| `KFolderTraverse` | O original pulava a primeira entrada de `FindFirstFile` (normalmente `.`); aqui nenhuma entrada é pulada | `std::filesystem` não lista `.`/`..` |
| `BaseServer::LoadINIFile`, `KStringTable` | INI lido pelo leitor da plataforma em **todas** as plataformas, aceitando UTF-16, UTF-8 e **CP949** | Os `config_*.ini` de produção estão em CP949; a API do Windows usa a code page do sistema, que só é 949 num Windows coreano |
| `XSLCheckSumManager` | CMarkup → TinyXml (já presente no `Common`); `XML/Markup.*` removidos. O `MessageBox` de checksum duplicado virou log de erro | Licença do CMarkup pede permissão para uso comercial; servidor não tem janela |
| `BaseServer` | CrashRpt removido (instalação do handler e o callback que chamava `DumpUploader.exe`, só em Release) | Binário fechado, já excluído na importação |
| `BaseServer::Run` | `lua_interpreter` agora vem do KNCSDK (prompt simples em stdin) | O original vinha de um `lua.c` modificado; o terminal da T15 vai substituí-lo |
| `NetError` | As tabelas de mensagens (`szErrorStr*`) só existem em `NetError.cpp`; `GetErrStr*` devolvem `const wchar_t*` | Eram `static` no header, copiadas em todo `.cpp` (73 mil warnings). Todos os chamadores copiam para `std::wstring` |
| `ActorManager::MultiQueueing*` | Parâmetro padrão `std::vector<UidType>& = std::vector<UidType>()` virou sobrecarga de um argumento | Extensão do MSVC. **Pendente:** `GameServer/UserManager.h` tem o mesmo padrão |
| `ActorProxy`/`BaseServer` | `std::shared_ptr<void>` de evento → `shared_ptr<remove_pointer_t<HANDLE>>` | `HANDLE` não é `void*` fora do Windows |
| gSOAP (`DEB/`) | `register` removido, `SOAP_NEW` sem parênteses; `stdsoap2.cpp`/`soapC.cpp`/`soapClient.cpp` compilam sem warnings (`-w`) | Código de terceiros/gerado (gSOAP 2.7.18); sai junto com o servidor DEB |
| `LineMap` | Membro `LPDIRECT3DVERTEXBUFFER9` removido | Resto do cliente, nunca usado no servidor |

### Bugs do original corrigidos

| Onde | Bug | Correção |
|---|---|---|
| `KXPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL` (`X2UDPPacket.h`) | O `operator=` atribuía `m_fPosX/Y/Z` a eles mesmos: a posição não era copiada | Copia de `kRight` |
| `GetFrom` de `KEGIANT_BILLING_CONSUME_ACK` e `KELOG_ITEM_TRADE_ERROR_NOT` | O bloco `GET` chamava `PUT` (copiar e colar): escrevia em vez de ler, e o restante do pacote ficava sem ler | `GET`. O primeiro é da integração Giant (fora de uso); o segundo é log entre servidores, cuja data nunca chegava |
| `KSwapPerformer` | Testes invertidos (`_JIF( !m_pEnqueueEvent, return )` saía com ponteiro válido e só seguia com ele nulo; o destrutor desreferenciava quando nulo) | Testes corrigidos. A classe não é usada por nenhum servidor |
| `CXSLPetManager::*SpecialFeedIntimacyRate_LUA` | Validação `> 1 && < 0` nunca era verdadeira | `\|\|`. Os scripts atuais usam 0,0201 e 0,0501, então nada muda |
| `XSLCharacterStatTable` | Comparava `UNIT_TYPE` com `UC_NONE` e `UNIT_CLASS` com `UT_NONE` | Enum certo (os dois valem 0: sem mudança) |
| `XSLGuildManager` | `u_char < 0` | Removido (sempre falso) |
| `SimObject`, `SerBuffer` etc. | Pequenos: `%d` com `__int64`, valores não inicializados | Ver diffs |

### Portabilidade (sem mudança de comportamento)

- Windows (achado na primeira compilação MinGW, 2026-10-30): o build passa a definir `UNICODE`/`_UNICODE`, como o original (sem isso
  `LPCTSTR` era `const char*`); `byte` e `u_char`/`u_short`/`u_int`/`u_long`, que o original recebia do `windows.h` sem `WIN32_LEAN_AND_MEAN`,
  vêm do `types.h` da plataforma; o gSOAP inclui `x2/platform/net.h` no lugar do `winsock.h` que vinha junto; `zlib` passou a dependência
  pública do `x2_server_common` (`Socket/KTDNUDP.h` inclui `zlib.h`).
- Campos `long`/`unsigned long` dos pacotes (190, em 2026-10-29) viraram `LONG`/`DWORD`: no Linux `long` tem 8 bytes, e o `Put` falhava
  com valores fora de 32 bits. No fio nada muda (mesmo tamanho e mesma tag), e no Windows os tipos são os mesmos de antes.

- `typename`/`template` que o MSVC dispensava, `goto` que cruzava inicializações (declarações subidas para o início da função), `enum` declarado
  sem tipo (`enum ePerformerID : int`), `min`/`max` de macro → `std::`, `KClearString` com construtor de cópia explícito, destrutores
  `virtual` nas classes polimórficas apagadas pelos singletons, listas de inicialização na ordem de declaração.
- `KFtpWrapper` (reescrito como cliente FTP sobre sockets na passada anterior): o laço de envio podia não terminar quando um envio falhava. Corrigido.

## Pendências

- **Caminhos com `\\`** espalhados (`"Config\\US\\"`, `"Statistics\\..."`, `"C:\\StatisticsBackup\\"`): no Linux viram nomes de arquivo com barra invertida.
  Precisa de uma regra (normalizar na abertura de arquivos da plataforma, ou trocar no código).
- **Nomes de arquivo com maiúsculas/minúsculas**: o Windows não diferencia; os scripts podem ser referenciados com outra grafia. Verificar ao rodar o servidor.
- `XSLCashItemManager::AddNotSellCashItemProductNo`: `!GetServerGroupID() != iServerGroupID` mantido como está; provavelmente era
  `GetServerGroupID() != iServerGroupID`. Decidir com o usuário (muda quais itens aparecem na loja).
- `KLottery` usa semente fixa (42) e gerador compartilhado sem lock (decidir com o usuário).
- `KLogReporter::Send` registra falha de envio mas devolve `eLRError_OK` (mantido).
- E-mail real do `MailManager` (hoje só log; libcurl?).
- `NetError_def_*.h` de outros idiomas: o build US usa o **`NetError_def_BR.h`** (enum e textos);
  os outros podem sair, mas a ordem do enum do BR é a que vai para o cliente, então ele fica.
