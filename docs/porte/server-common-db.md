[Início](../README.md) › [Porte](README.md) › [Servidor: Common](server-common.md) › Banco de dados (ODBC)

# Servidor: banco de dados (`Common/ODBC`)

Desenho do passo `server/common-db` (2026-10-29), ainda **não implementado**. Base: decisão T17 em [Decisões](../projeto/decisoes.md)
(API ODBC, Microsoft ODBC Driver 18, FreeTDS como alternativa, `.dsn` → connection strings, `SQLWCHAR` UTF-16).
Quando o passo terminar, esta página passa a descrever o que foi feito e o que mudou em relação ao original.

## O que existe

| Arquivo | Linhas | Papel |
|---|---:|---|
| `ODBC/Odbc.{h,cpp}` | 194 + 1.363 | `KODBC`: conexão (`.dsn` ou connection string), `Query` com reconexão, `Fetch` e `operator>>` por tipo, filtros de caracteres (`IsInvalidMarkIn*`), `KDBQuerySuccessChecker` |
| `ODBC/StoredProcedure.{h,cpp}` | 35 + 317 | consultas fixas usadas por mais de um servidor |
| `ODBC/DBUtil.{h,cpp}` | 79 + 108 | profiler das procedures (`KStoredProcedureProfiler`) |
| `SubThread.h` | — | `DeclareDBThread`, `DO_QUERY*` (monta a query com `x2::wformat` e chama `KODBC::Query`), `FETCH_DATA` |
| `DBLayer.cpp`, `DBThreadManager.cpp`, `DBConnectionManager.cpp` | — | `DBLayer:AddDB(tipo, '<arquivo>.dsn', threads, bConnStr)` nos `config_*.lua`; uma conexão `KODBC` por thread de DB |

Uso nos servidores originais (`KncWX2Server`, 2026-10-29): 37 arquivos, 1.828 `DO_QUERY`, 1.118 `FETCH_DATA`, 860 `BeginFetch`.
Todas as consultas são `exec dbo.<procedure> <args>` montadas como texto; os formatos usados são `%d` (6.562), `%s` (942, sempre
dentro de `N'...'`) e `%f` (154). **A API do `KODBC` e as macros não mudam**: os servidores portados continuam iguais.

Configuração real (`_reference/_original-server`): cada servidor tem os seus `.dsn` (`Account_US_SERVICE.dsn`, `Game01_US_SERVICE.dsn`,
`log_US_SERVICE.dsn`, `Billing_US_SERVICE.dsn`...), todos com `[ODBC]`, `DRIVER=SQL Server` (driver legado do Windows, que não existe
no Linux), `SERVER=host,porta`, `DATABASE`, `UID`, `PWD`, `WSID` e `APP`. Nenhum `config_*.lua` usa o modo connection string.

## Problemas encontrados

1. **`wchar_t` ≠ `SQLWCHAR` fora do Windows.** No unixODBC `SQLWCHAR` tem 2 bytes (UTF-16) e `wchar_t` tem 4. Hoje o código faz cast:
   `SQLExecDirectW((SQLWCHAR*)wstr.c_str())`, `SQLGetData(SQL_C_WCHAR)` direto num `wchar_t[]`, `m_szDiagMsg`/`m_sqlState` lidos
   como `wchar_t*` e comparados com `wcscmp`. No Linux isso manda e lê lixo. No Windows (MSVC e MinGW) os dois tipos são iguais.
2. **Indicadores de 32 bits.** `SQLINTEGER LenBin` é passado onde a API pede `SQLLEN*`, que tem 8 bytes em x64 (Windows e Linux), e
   `int nLen` é passado ao `SQLColAttribute`. O original era 32 bits; em x64 nem compila, e com cast o driver escreveria além da variável.
3. **Textos truncados em silêncio.** `operator>>(wchar_t*)` lê no máximo 1.024 bytes (512 unidades UTF-16) e o `std::wstring` passa por
   ele. Valores maiores voltam com `SQL_SUCCESS_WITH_INFO` (01004) e o resto se perde. O mesmo vale para `char*`/`std::string`.
4. **Buffers da conexão.** `wsprintfW` num `SQLWCHAR[255]` e `sizeof(InCon)` (bytes) passado como tamanho em caracteres.
5. **Conexão por `FILEDSN=`.** Monta `FILEDSN=<diretório atual>\<arquivo>` e entrega ao driver manager; depende do `DRIVER=SQL Server`.
   O `ToStringW` relê o `.dsn` com `GetPrivateProfileSectionNamesW`.
6. **Coisas só do MSVC:** `std::exception("...")` no `THROW_INVALID_QUERY`, `wchar_t* err = L"..."`, `#pragma warning`,
   `GetCurrentDirectoryW`, `CTime::Format(_T(...))`, `operator>>(time_t&)` sob `_USE_32BIT_TIME_T` (código morto em x64).

## Desenho

### 1. Plataforma: `x2/platform/odbc.h` e o alvo `x2::odbc`

- O header já existe (renomeia os typedefs Win32 do unixODBC). Ganha conversões, em `src/odbc.cpp`:
  - `std::u16string`/buffer `SQLWCHAR` a partir de `std::wstring` (UTF-32 → UTF-16 com pares substitutos);
  - `std::wstring` a partir de `const SQLWCHAR*` + tamanho (ou terminado em zero).
  - Quando `sizeof(SQLWCHAR) == sizeof(wchar_t)` (Windows), as duas viram cópia simples (`if constexpr`).
- Alvo CMake `x2::odbc` em `cmake/X2Dependencies.cmake`: `find_package(ODBC)` (unixODBC no Linux; `odbc32` no MinGW e no MSVC).
  O `x2_platform` não passa a depender de ODBC; só quem usa `odbc.h` linka `x2::odbc`.
- Teste `test_odbc` (sem banco): ida e volta de ASCII, coreano, `♪`, caractere fora do BMP (par substituto) e string vazia.

### 2. `KODBC` portável (API igual)

- `Query`: converte `m_strLastQuery` para `SQLWCHAR` e chama `SQLExecDirectW(..., SQL_NTS)`. A reconexão em `08S01`/`24000` fica.
- Diagnóstico: `m_szDiagMsg` e `m_sqlState` passam a ser `std::wstring` (mesmo nome; as macros `DO_QUERY*` e os servidores usam
  esses membros só para log). Um helper `ReadDiag()` substitui os seis blocos repetidos de `SQLGetDiagRecW`.
- `operator>>`:
  - todos usam `SQLLEN` como indicador; `NULL` continua deixando a variável como estava (comportamento do original);
  - numéricos ficam como estão (`SQL_C_SLONG`, `SQL_C_SBIGINT`, `SQL_C_FLOAT`...);
  - `std::wstring` lê `SQL_C_WCHAR` em pedaços até o fim (laço do `SQLGetData` com 01004) e converte; sem limite de 512;
  - `wchar_t*` e `char*` (destino de tamanho desconhecido) mantêm o limite do original (1.024 bytes; para `wchar_t*`, 511 caracteres + zero);
  - `wchar_t&` lê um `SQLWCHAR`; `std::string` lê `SQL_C_CHAR` em pedaços;
  - `KSerBuffer` já lê em pedaços; só troca o indicador;
  - `time_t` (`_USE_32BIT_TIME_T`) sai.
- `THROW_INVALID_QUERY` → `std::runtime_error`. `GetCurrentDirectoryW` → `std::filesystem`. Comentários coreanos tocados viram inglês.
- `KDBQuerySuccessChecker`, `IsInvalidMarkIn*` e `RemoveInvalidMark` não mudam (só compilam).

### 3. Conexão: `.dsn` → connection string

O `.dsn` continua aceito como entrada (é o que a configuração existente usa), mas o `KODBC` não usa mais `FILEDSN=`:

- `DriverConnectFileDSN` lê a seção `[ODBC]` com o leitor INI da plataforma (`x2::platform::ReadIniString`, o mesmo do `config_*.ini`)
  e monta a connection string: `DRIVER={<driver>};SERVER=...;DATABASE=...;UID=...;PWD=...;APP=...`. `WSID` é descartado.
- O `DRIVER=SQL Server` do arquivo é trocado pelo driver configurado. Padrão: `ODBC Driver 18 for SQL Server`. Para trocar (ex.: FreeTDS),
  uma chamada nova e opcional no `config_*.lua`: `DBLayer:SetODBCDriver('FreeTDS')`, antes dos `AddDB`. Chaves extras opcionais vão por
  `DBLayer:SetODBCOptions('Encrypt=yes;TrustServerCertificate=yes')`, concatenadas ao fim.
- **Criptografia:** o Driver 18 liga `Encrypt=yes` por padrão e recusa certificado autoassinado; o driver legado `SQL Server` não
  criptografava. Para a configuração antiga funcionar sem mexer no servidor SQL, a conversão de `.dsn` usa por padrão
  `Encrypt=yes;TrustServerCertificate=yes` (canal criptografado, certificado não verificado). Quem tiver certificado válido troca por
  `SetODBCOptions`. **Confirmar com o usuário** (ver Perguntas).
- `SERVER=host,porta` funciona no Driver 18; no FreeTDS, verificar na implementação se aceita a vírgula ou se é preciso `Port=`.
- O modo connection string (`AddDB(..., true)`, com UID/PWD vindos da linha de comando) continua igual, só com a conversão do item 2.
- Senha nunca vai para o log: `ToStringW` e as mensagens de erro mostram o arquivo, o servidor e o banco, não a string completa
  (como o original já fazia no modo connection string ao cortar tudo menos o `DATABASE`).
- A montagem da string fica numa função pura (`BuildConnStrFromDsn(map de chaves, driver, opções)`), testável sem banco.

### 4. Build e testes

- O glob de `src/server/Common/CMakeLists.txt` deixa de excluir `ODBC/`; `x2_server_common` linka `x2::odbc`.
- `tests/server_hooks.cpp` perde os stubs de `KODBC`.
- **Testes sem banco** (sempre no `ctest`): conversões (plataforma), `BuildConnStrFromDsn` com um `.dsn` de exemplo nos testes
  (inclui a troca do driver, `WSID` descartado, valores com `;` ou `{}`), `IsInvalidMarkIn*`.
- **Teste de integração** (opcional): só roda com a variável `X2_TEST_ODBC` contendo uma connection string; sem ela, o caso registra
  "skipped" e passa. Cobre: conectar; `SELECT` de `nvarchar` com coreano, fora do BMP e com mais de 512 caracteres; `bit`, `tinyint`,
  `smallint`, `int`, `bigint`, `real`, `NULL` (variável intacta), `varbinary` grande; uma procedure temporária com `exec`, como os
  `DO_QUERY`; erro de sintaxe preenchendo `m_sqlState`/`m_szDiagMsg`; reconexão depois de `KILL` da sessão (se der para automatizar).
  Ambiente local sugerido: SQL Server 2022 em Docker (`mcr.microsoft.com/mssql/server:2022-latest`) com o Driver 18 (AUR `msodbcsql18`)
  e/ou FreeTDS (`pacman -S freetds`). Instalar pacotes e baixar a imagem precisa do OK do usuário.
- MinGW: compila e roda os testes sem banco no Wine. O de integração no Wine (o `odbc32` do Wine repassa ao unixODBC do sistema) fica
  como experimento, sem compromisso.

### Fora deste passo

- Trocar o texto montado por parâmetros (`SQLBindParameter`). O original monta `exec ... N'%s'` e depende de `IsInvalidMarkIn*` para
  barrar aspas; trocar isso muda cada um dos 1.828 `DO_QUERY` e merece decisão e passo próprios. Fica anotado como risco (injeção de
  SQL onde um `%s` não passa pelo filtro).
- Pool de conexões, async, outro banco que não o MSSQL.

## Perguntas para o usuário (antes ou durante a implementação)

1. Pode instalar `msodbcsql18` (AUR) e/ou `freetds`, e subir o SQL Server em Docker para o teste de integração?
2. O padrão `Encrypt=yes;TrustServerCertificate=yes` na conversão de `.dsn` está bom, ou prefere `Encrypt=no` (igual ao driver legado)
   ou exigir certificado válido?

## Ordem dos commits (no branch `server/common-db`)

1. `x2::odbc` + conversões em `x2/platform/odbc.h` + `test_odbc`.
2. `KODBC` compilando e portável (itens 2 e 6), `ODBC/` entrando no alvo; `server_hooks.cpp` sem os stubs.
3. Conexão por `.dsn` convertido + `SetODBCDriver`/`SetODBCOptions` + testes sem banco.
4. Teste de integração opcional.
5. Docs: esta página vira o registro do que mudou; [Servidor: Common](server-common.md) (Estado, Mudanças, Pendências),
   [Roadmap](../projeto/roadmap.md), [Plataforma](platform.md). Matriz completa (GCC Debug/Release, Clang, MinGW), links, integrar.
