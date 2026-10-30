[Início](../README.md) › [Porte](README.md) › Servidor: EPublisherServer

# Servidor: EPublisherServer (`src/server/EPublisherServer`)

Serviço de autenticação e cash que o ChannelServer e o GameServer chamam pelo cliente DEB (`_DEB_AUTH`: `KDEBAuth`,
`KDEBBilling` em `Common/OnlyGlobal/AuthAndBilling`). O código original chama esse papel de *EPublisher* (pacotes
`EPUBLISHER_*`). Decisão T23 em [Decisões](../projeto/decisoes.md).

**Reimplementação**, em 2026-10-30, do `AuthServer` do dono do projeto (`_reference/AuthServer`: C++, gSOAP 2.8, SOCI, OpenSSL, Lua 5.1 próprio).
Ele é considerado parte do código original: fica até a Fase 5, quando a função deve ir para o ChannelServer.
**Regra:** o EPublisherServer usa o `Common` (`KODBC`, `NetError`, log, `lua_tinker`); **nada do `Common` ou dos outros servidores usa ele.**

## Estado (2026-10-30)

Compila sem warnings (GCC, Clang, MinGW), testes passando no Linux e no Wine. Roda contra o banco do Docker: o login `test`/`test`
devolve a conta 20004. Sobe junto com o ChannelServer (pasta `run/`, ver [Pastas de execução](../ferramentas/runtime.md)).

## Protocolo

SOAP 1.1 document/literal sobre HTTP/1.1, exatamente o que o cliente gSOAP 2.7 dos servidores escreve e lê: `POST` com
`Content-Length`, elementos qualificados pelo namespace `urn:AuthService` (prefixo `ns2`).

| Operação | Pedido | Resposta |
|---|---|---|
| `LoginRequest` | `strUsername`, `strPassword`, `strIPAddress` | `LoginResponse`: `iResult`, `strID` (= UID em texto), `iPublisherSN` (= UID), `strPurchaseToken`, `iChannelingCode` (0) |
| `GetUserCashRequest` | `uiPublisherUID` | `GetUserCashResponse`: `iOK`, `ulBalance` |
| `PurchaseProductRequest` | `uiPublisherUID`, `iChannelingCode`, `iUnitUID`, `strUserIP`, `strNickName`, `strTotalOrderID`, `strTotalList` (`<nº>_<id>_<preço>\|...`) | `PurchaseProductResponse`: `iOK`, `iTotalCash` |

Os resultados são os `NetError` do `Common` (os mesmos que o ChannelServer repassa ao cliente): login `NET_OK`, `ERR_VERIFY_20`
(conta ou senha), `ERR_VERIFY_12` (manutenção), `ERR_VERIFY_21` (erro de banco); cash `ERR_GASH_02`/`ERR_GASH_00`; compra
`ERR_PAYLETTER_BILLING_08` (saldo), `_09` (conta), `_30` (lista ou erro).

Banco `Account`: `DEBAccounts` (`UID`, `AccountID`, `Password` = sha256 hexadecimal minúsculo de senha + sal, `PurchaseToken`) e
`DEBUserCash` (`UserUID`, `Cash`). A estrutura não muda.

## Arquivos

| Arquivo | Papel |
|---|---|
| `Soap.{h,cpp}` | lê o envelope (operação e campos, entidades XML) e monta respostas e falhas |
| `Http.{h,cpp}` | um pedido HTTP por vez, com keep-alive quando pedido; tempo limite de 10 s por leitura, como o original |
| `PublisherService.{h,cpp}` | as três operações sobre `IAccountStore`, sem SOAP nem banco (testável) |
| `OdbcAccountStore.{h,cpp}` | `IAccountStore` sobre o `KODBC`, com o `.dsn` e o driver dos servidores |
| `EPublisherServer.{h,cpp}`, `main.cpp` | config Lua, porta, uma thread por conexão (até 100), prompt Lua no stdin como os outros servidores |
| `tests/test_epublisher.cpp` | hash, SOAP, operações com um armazenamento falso e o **cliente gSOAP dos servidores** (`soap_call___ns1__Login`...) contra o serviço |

## Uso

`EPublisherServer DEV` lê `config_ep_DEV.lua` (template em `runtime/EPublisherServer/`):

```lua
EPublisher:SetBindAddress( "0.0.0.0" )
EPublisher:SetPort( 8000 )
EPublisher:SetMaintenance( false )
EPublisher:SetODBCDriver( "..." )   -- como DBLayer:SetODBCDriver
EPublisher:SetODBCOptions( "..." )
EPublisher:SetDSN( "Account.dsn" )
log( 0 )
```

## Diferenças em relação ao AuthServer original

| Onde | Original | Agora | Motivo |
|---|---|---|---|
| Dependências | gSOAP (servidor), SOCI, OpenSSL, Lua 5.1 e lua_tinker próprios | `KODBC`, Crypto++, LuaJIT e `lua_tinker` do projeto; HTTP/SOAP próprios | Sem dependência nova (pedido do usuário) |
| Nome, config | `AuthServer`, `AuthConfig.lua` com `Auth:SetPort/SetLogLevel/SetMaintenance`, `mssql.dsn` em texto | `EPublisherServer <tag>`, `config_ep_<tag>.lua` com `EPublisher:...`, `log()` e `.dsn` como os servidores | Mesmo esquema dos outros servidores |
| `NetError` | cópia própria do `NetError.h` | o `NetError` do `Common` | Os códigos são os que o ChannelServer e o cliente usam |
| Compra | lê o saldo e grava o novo em duas consultas; preço negativo é ignorado; lista malformada lança exceção | um `UPDATE ... OUTPUT ... WHERE Cash >= preço`; lista malformada ou preço negativo é recusado (`_30`) | Duas compras simultâneas não gastam o mesmo saldo |
| `iTotalCash` | nunca preenchido (0) | saldo depois da compra (ou o atual, se faltou saldo) | Informativo; o `KDEBBilling` não lê o campo |
| Erro de banco no login | exceção não tratada na thread do pedido | `ERR_VERIFY_21` | |
| Encerramento | laço sem fim; `exit` se a porta falha | fim do stdin encerra (como os servidores); código 1 se a porta ou o banco falham | |

Observado no cliente DEB (`Common`, não muda aqui): `KDEBAuth` só copia o `strPurchaseToken` da resposta quando ele está **vazio**
(teste invertido), então o token nunca chega ao servidor. Anotado para quando o GameServer usar o cash.

## Pendências

- Login completo com o `elsword.exe` (passo `server/channel-run`).
- Cash e compra com o GameServer (o `KDEBBilling` ainda não é compilado).
