[Home](../README.md) › [Porting](README.md) › Server: EPublisherServer

# Server: EPublisherServer (`src/server/EPublisherServer`)

Authentication and cash service used by ChannelServer and GameServer through the DEB client (`_DEB_AUTH`: `KDEBAuth`,
`KDEBBilling` in `Common/OnlyGlobal/AuthAndBilling`). Original code calls this role *EPublisher* (`EPUBLISHER_*` packets).
Decision T23 in [Decisions](../project/decisions.md).

**Reimplementation**, on 2026-10-30, of the project owner's `AuthServer` (`_reference/AuthServer`: C++, gSOAP 2.8, SOCI, OpenSSL,
custom Lua 5.1). It is considered part of original code: remains until Phase 5, when functionality should move into ChannelServer.
**Rule:** EPublisherServer uses `Common` (`KODBC`, `NetError`, logging, `lua_tinker`, DEB SOAP messages); **nothing in `Common` or other servers depends on it.**

## Status (2026-10-30)

Builds without warnings (GCC, Clang, MinGW); tests pass on Linux and under Wine. Runs against Docker database: login `test`/`test`
returns account 20004. Starts alongside ChannelServer (`run/` directory, see [Run directories](../tools/runtime.md)).

## Protocol

SOAP 1.1 document/literal over HTTP/1.1, exactly what servers' gSOAP 2.7 client wrote and read: `POST` with
`Content-Length`, elements qualified by `urn:AuthService` namespace (prefix `ns2`). Since T31 (2026-10-31), servers use a custom
client writing the same bytes ([Common › DEB client](server-common.md#deb-client-t31)); messages (`x2::deb`,
`Common/OnlyGlobal/AuthAndBilling/DEB/Soap.{h,cpp}`) are shared by both sides.

| Operation | Request | Response |
|---|---|---|
| `LoginRequest` | `strUsername`, `strPassword`, `strIPAddress` | `LoginResponse`: `iResult`, `strID` (= UID as text), `iPublisherSN` (= UID), `strPurchaseToken`, `iChannelingCode` (0) |
| `GetUserCashRequest` | `uiPublisherUID` | `GetUserCashResponse`: `iOK`, `ulBalance` |
| `PurchaseProductRequest` | `uiPublisherUID`, `iChannelingCode`, `iUnitUID`, `strUserIP`, `strNickName`, `strTotalOrderID`, `strTotalList` (`<count>_<id>_<price>\|...`) | `PurchaseProductResponse`: `iOK`, `iTotalCash` |

Results use `Common` `NetError` codes (same ones ChannelServer forwards to client): login `NET_OK`, `ERR_VERIFY_20`
(account or password), `ERR_VERIFY_12` (maintenance), `ERR_VERIFY_21` (DB error); cash `ERR_GASH_02`/`ERR_GASH_00`; purchase
`ERR_PAYLETTER_BILLING_08` (balance), `_09` (account), `_30` (list or error).

`Account` database: `DEBAccounts` (`UID`, `AccountID`, `Password` = lowercase hexadecimal SHA-256 of password + salt, `PurchaseToken`)
and `DEBUserCash` (`UserUID`, `Cash`). Schema is unchanged.

## Files

| File | Role |
|---|---|
| `Http.{h,cpp}` | One HTTP request at a time, keep-alive when requested; 10-second read timeout, like original |
| `PublisherService.{h,cpp}` | Three operations over `IAccountStore`, no SOAP or DB (testable) |
| `OdbcAccountStore.{h,cpp}` | `IAccountStore` on `KODBC`, with server `.dsn` and driver |
| `EPublisherServer.{h,cpp}`, `main.cpp` | Read request and build response using `Common` SOAP messages (`x2::deb`); Lua config, port, one thread per connection (up to 100), same terminal as other servers (T15), `--no-stdin` for service |
| `tests/test_epublisher.cpp` | Hash, SOAP, operations with fake store, **captured request from gSOAP client** before its removal (T31), and **server client** (`x2::deb::SoapClient`) against service |

## Usage

`EPublisherServer DEV` reads `config_ep_DEV.lua` (template in `runtime/EPublisherServer/`):

```lua
EPublisher:SetBindAddress( "0.0.0.0" )
EPublisher:SetPort( 8000 )
EPublisher:SetMaintenance( false )
EPublisher:SetODBCDriver( "..." )   -- like DBLayer:SetODBCDriver
EPublisher:SetODBCOptions( "..." )
EPublisher:SetDSN( "Account.dsn" )
log( 0 )
```

## Differences from original AuthServer

| Where | Original | Now | Reason |
|---|---|---|---|
| Dependencies | gSOAP (server), SOCI, OpenSSL, Lua 5.1 and custom lua_tinker | `KODBC`, `x2::crypto` (SHA-256, T27), project's LuaJIT and `lua_tinker`; custom HTTP/SOAP | No new dependencies (user request) |
| Name/config | `AuthServer`, `AuthConfig.lua` with `Auth:SetPort/SetLogLevel/SetMaintenance`, text `mssql.dsn` | `EPublisherServer <tag>`, `config_ep_<tag>.lua` with `EPublisher:...`, `log()`, `.dsn` like other servers | Same model as other servers |
| `NetError` | own copy of `NetError.h` | `Common` `NetError` | Codes are used by ChannelServer and client |
| Purchase | reads balance and writes new balance in two queries; ignores negative price; malformed list throws | one `UPDATE ... OUTPUT ... WHERE Cash >= price`; malformed list or negative price is rejected (`_30`) | Two simultaneous purchases cannot spend the same balance |
| `iTotalCash` | never populated (0) | balance after purchase (or current balance if insufficient funds) | Informational; `KDEBBilling` does not read field |
| Login DB error | unhandled exception on request thread | `ERR_VERIFY_21` | |
| Shutdown | infinite loop; `exit` if port fails | like other servers (T15 terminal: confirmation, stdin EOF, or signal); code 1 if port or DB fails | |

Observed in DEB client (`Common`, unchanged here): `KDEBAuth` copies response `strPurchaseToken` only when it is **empty**
(inverted check), so token never reaches server. Recorded for when GameServer uses cash.

## Pending

- Full login with `elsword.exe` (`server/channel-run` step).
- Cash and purchase with GameServer (`KDEBBilling` is not yet compiled).
