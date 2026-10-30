[Início](../README.md) › [Porte](README.md) › [Servidor: Common](server-common.md) › Banco de dados (ODBC)

# Servidor: banco de dados (`Common/ODBC`)

Passo `server/common-db` (desenho em 2026-10-29, implementado em 2026-10-30). Base: decisão T17 em [Decisões](../projeto/decisoes.md)
(API ODBC, Microsoft ODBC Driver 18, FreeTDS como alternativa, `.dsn` → connection strings, `SQLWCHAR` UTF-16).
O banco de testes local está em [Banco de desenvolvimento](../ferramentas/banco-dev.md).

## Estado (2026-10-30)

`ODBC/` compila no `x2_server_common` (GCC Debug/Release, Clang, sem warnings) e foi **testado contra um SQL Server 2022 real**
(Docker, com os backups de `_database/` restaurados) usando o **FreeTDS 1.5**. Os testes chamam procedures reais do `Game01`
(`P_GUnit_Userinfo_GET_20131212`, `P_GItem_Equipped_Item_SEL`, `P_GUnitLastPosition_GET`) pelo `KStoredProcedure`, como os servidores fazem.
O **ODBC Driver 18** não foi testado (não está instalado). No MinGW o `ODBC/` compila e os testes sem banco passam no Wine (2026-10-30).

**A API do `KODBC` e as macros `DO_QUERY*`/`FETCH_DATA` não mudaram**: os servidores (37 arquivos, 1.828 `DO_QUERY`, 1.118 `FETCH_DATA` e
860 `BeginFetch` no original, contagem de 2026-10-29) entram sem alterações nesse código.

## O que existe

| Arquivo | Papel |
|---|---|
| `ODBC/Odbc.{h,cpp}` | `KODBC`: conexão (`.dsn` ou connection string), `Query` com reconexão, `Fetch` e `operator>>` por tipo, filtros de caracteres (`IsInvalidMarkIn*`), `KDBQuerySuccessChecker` |
| `ODBC/StoredProcedure.{h,cpp}` | consultas fixas usadas por mais de um servidor |
| `ODBC/DBUtil.{h,cpp}` | profiler das procedures (`KStoredProcedureProfiler`), sem mudança |
| `SubThread.h` | `DeclareDBThread`, `DO_QUERY*` (monta a query com `x2::wformat` e chama `KODBC::Query`), `FETCH_DATA`, sem mudança |
| `DBLayer.cpp`, `DBThreadManager.cpp`, `DBConnectionManager.cpp` | `DBLayer:AddDB(tipo, '<arquivo>.dsn', threads, bConnStr)` nos `config_*.lua`; uma conexão `KODBC` por thread de DB |
| `x2/platform/odbc.h` (plataforma) | `<sql.h>`/`<sqlext.h>` e as conversões `wchar_t` ↔ `SQLWCHAR`; alvo CMake `x2::odbc` ([Plataforma](platform.md)) |

Todas as consultas são `exec dbo.<procedure> <args>` montadas como texto; os formatos usados são `%d`, `%s` (sempre dentro de `N'...'`) e `%f`.

## Configuração

A configuração real (`_reference/_original-server`) usa só `.dsn`: `[ODBC]`, `DRIVER=SQL Server` (driver legado do Windows, que não existe
no Linux), `SERVER=host,porta`, `DATABASE`, `UID`, `PWD`, `WSID` e `APP`, em UTF-16. Nenhum `config_*.lua` usa o modo connection string.

- O `.dsn` continua sendo a entrada, mas o `KODBC` não usa mais `FILEDSN=`: lê a seção `[ODBC]` (UTF-16, UTF-8 ou CP949, por
  `DecodeTextFile`) e monta a connection string (`KODBC::ReadFileDSN` + `KODBC::BuildConnStrFromDsn`, testadas sem banco).
  O `DRIVER` do arquivo é trocado pelo configurado, `WSID` é descartado, o resto das chaves passa na mesma ordem (com `{}` quando o valor tem espaço, `;` ou chaves).
- Driver padrão: `ODBC Driver 18 for SQL Server`, com as opções `Encrypt=yes;TrustServerCertificate=yes` (canal criptografado,
  certificado não verificado: o Driver 18 exige criptografia por padrão e recusa certificado autoassinado; o driver legado não criptografava).
- Chamadas novas e opcionais no `config_*.lua`, **antes** dos `AddDB`:
  - `DBLayer:SetODBCDriver('FreeTDS')`: troca o driver e **zera as opções** (elas são do Driver 18);
  - `DBLayer:SetODBCOptions('Encrypt=yes')`: chaves acrescentadas ao fim da string.
- O caminho do `.dsn` continua relativo ao diretório atual, como no original; fora do Windows o `\` dos configs vira `/`.
- `SERVER=host,porta` funciona no Driver 18 e no FreeTDS (conferido no FreeTDS em 2026-10-30).
- O modo connection string (`AddDB(..., true)`, com UID/PWD vindos da linha de comando) continua igual.
- A senha não vai para o log: no modo `.dsn` aparece o caminho (`FILEDSN=<caminho>`), no modo connection string só o `DATABASE`, como no original.

## Mudanças em relação ao original

### Portabilidade (sem mudança de comportamento)

| Onde | Original | Agora |
|---|---|---|
| Texto na API (`SQLExecDirectW`, `SQLDriverConnectW`, `SQL_C_WCHAR`, diagnóstico) | cast de `wchar_t*` para `SQLWCHAR*` | conversão por `x2::platform::ToSqlWString`/`FromSqlWString` (no Windows é cópia; fora dele `wchar_t` tem 4 bytes e `SQLWCHAR` tem 2) |
| `m_szDiagMsg`, `m_sqlState` | arrays de `SQLWCHAR` | arrays de `wchar_t` (mesmo nome), preenchidos por `ReadDiag()`, que substitui os blocos repetidos de `SQLGetDiagRecW`. As macros `DO_QUERY*` não mudam |
| Indicadores do `SQLGetData` | `SQLINTEGER` (4 bytes) | `SQLLEN`, que tem 8 bytes em x64 (Windows e Linux); o driver escreveria além da variável |
| `SQLColAttribute` com `int nLen` | resultado nunca usado | removido (também escrevia 8 bytes num `int`) |
| Buffers da conexão | `wsprintfW` num `SQLWCHAR[255]` e `sizeof` (bytes) como tamanho em caracteres | a string inteira com `SQL_NTS`; as três funções de conexão viraram uma (`AllocConnection`) |
| `THROW_INVALID_QUERY` | `std::exception("...")` (extensão do MSVC) | `std::runtime_error` |
| `operator>>(time_t&)` | sob `_USE_32BIT_TIME_T` | removido (só existe no MSVC x86) |
| `ToStringW` | `GetPrivateProfileSectionNamesW` no `.dsn` | lê `Address` da seção `[ODBC]` (o `.dsn` só tem essa seção) |
| `KStoredProcedure()` | construtor privado ligando uma referência a um temporário | `= delete` (nunca foi usado) |

### Intencionais

| Onde | Mudança | Motivo |
|---|---|---|
| `operator>>(std::wstring&)` e `(std::string&)` | Leem a coluna inteira, em pedaços; `NULL` continua virando string vazia | O original lia no máximo 1.024 bytes (511 caracteres) e cortava o resto em silêncio |
| `operator>>(wchar_t*)` e `(char*)` | Mesmo limite do original (511 caracteres + zero; 1.024 bytes) | O tamanho do destino é desconhecido |
| `operator>>(wchar_t&)` | Lê o primeiro caractere | O original pedia um buffer de um `wchar_t`, que só cabe o terminador: lia sempre `L'\0'` |
| `operator>>(KSerBuffer&)` | Copia só o que veio em cada pedaço e para em erro | O original copiava `LenBin` bytes (o que *faltava*, não o que coube) de um buffer de 1.024 e repetia para sempre em `SQL_ERROR` |
| Conexão por `.dsn` | Arquivo lido por nós, driver trocado, `WSID` fora (ver Configuração) | O driver `SQL Server` não existe fora do Windows; o unixODBC não lê `.dsn` em UTF-16 |
| Falha de conexão | O log traz o `SQLSTATE` e a mensagem do driver | O original só logava o código de retorno |
| `ReadDiag` | Quando não há registro de diagnóstico, `m_sqlState`/`m_szDiagMsg` ficam vazios | O original deixava o texto do erro anterior |

### Bugs do original corrigidos

| Onde | Bug | Correção |
|---|---|---|
| `KODBC::RemoveInvalidMark` | Troca cada marca por `*` buscando sempre do início; como `*` também é marca, **nunca terminava** quando o texto tinha alguma marca | A busca continua depois da troca. Nenhum servidor chama a função |
| `KStoredProcedure::Query_GetGuildMemberGrade` | `u_char == -1`, sempre falso | Teste removido (comportamento igual). Nenhum servidor chama a função |

### Mantido como no original (anotado)

- `Query` devolve `false` quando o comando não produz resultado (`SQL_NO_DATA`), por exemplo um `CREATE`: os servidores só chamam procedures que fazem `SELECT`.
- `USHORT` é lido como `SQL_C_SSHORT` (`smallint -1` vira 65535; valores acima de 32.767 numa coluna `int` falham).
- Se a primeira conexão falha, a reconexão automática (`08S01`/`24000`) nunca acontece, porque não há estado SQL para comparar.

## Testes

| Teste | O que cobre |
|---|---|
| `platform/tests/test_odbc.cpp` (sempre; Linux e Wine) | `SQLWCHAR` ↔ `wchar_t`: ASCII, coreano, `♪`, espaço ideográfico, fora do BMP (par substituto), vazio |
| `server/Common/tests/test_odbc.cpp`, sem banco (sempre) | `ReadFileDSN` com um `.dsn` UTF-16 no formato do original (seção `[ODBC]`, comentário, outra seção), `BuildConnStrFromDsn` (troca do driver, `WSID` fora, `{}` em `;`/`}`/espaço, opções), `IsInvalidMarkIn*`, `RemoveInvalidMark` |
| idem, suíte `db` (ctest `server_common_db`, só com `X2_TEST_ODBC`) | Contra o SQL Server: todos os tipos do `operator>>` (`bit`, `smallint` em `char`/`short`/`USHORT`, `int`, `bigint`, `real`, `nvarchar` com coreano e fora do BMP, 3.000 caracteres, `varchar`, `varbinary` de 2.500 bytes, `NULL`); erro com `SQLSTATE` `42S02` e mensagem; senha errada; `.dsn` UTF-16 com `DRIVER=SQL Server` e `WSID` trocados; procedure temporária com `exec` e `bigint`/`N'...'`; `KStoredProcedure` sobre o `Game01` |

Sem `X2_TEST_ODBC`, o ctest roda só os testes sem banco (`--test-suite-exclude=db`); a suíte `db` rodada à mão sem a variável passa com a mensagem "skipped".

## Pendências

- Testar com o **ODBC Driver 18** (`msodbcsql18`, AUR) e no Windows/MSVC.
- Reconexão depois de queda (`KILL` da sessão): não automatizada nos testes.
- Teste de integração no Wine: tentado em 2026-10-30, o `odbc32` do Wine não chegou ao driver FreeTDS do Linux (a conexão falha sem diagnóstico). Sem compromisso.
- **Fora deste passo:** trocar o texto montado por parâmetros (`SQLBindParameter`). O original monta `exec ... N'%s'` e depende de
  `IsInvalidMarkIn*` para barrar aspas; mudar isso afeta cada `DO_QUERY` e pede decisão e passo próprios. Risco anotado: injeção de SQL
  onde um `%s` não passa pelo filtro.
- Pool de conexões, async, outro banco que não o MSSQL: fora do escopo (T7).
