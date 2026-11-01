[Home](../README.md) › [Porting](README.md) › [Server: Common](server-common.md) › Database (ODBC)

# Server database (`Common/ODBC`)

`server/common-db` step (designed on 2026-10-29, implemented on 2026-10-30). Based on decision T17 in [Decisions](../project/decisions.md)
(ODBC API, Microsoft ODBC Driver 18, FreeTDS as alternative, `.dsn` → connection strings, UTF-16 `SQLWCHAR`).
Local test database: [Development database](../tools/development-database.md).

## Status (2026-10-30)

`ODBC/` builds as part of `x2_server_common` (GCC Debug/Release, Clang, no warnings) and was **tested against a real SQL Server 2022**
(Docker, with `_database/` backups restored) using **FreeTDS 1.5**. Tests call real `Game01` procedures
(`P_GUnit_Userinfo_GET_20131212`, `P_GItem_Equipped_Item_SEL`, `P_GUnitLastPosition_GET`) via `KStoredProcedure`, as servers do.
**ODBC Driver 18** was not tested (not installed). Under MinGW, `ODBC/` builds and database-free tests pass under Wine (2026-10-30).

**The `KODBC` API and `DO_QUERY*`/`FETCH_DATA` macros are unchanged**: server code (37 files, 1,828 `DO_QUERY`, 1,118 `FETCH_DATA`,
and 860 `BeginFetch` in the original, counted 2026-10-29) uses this code without changes.

## Contents

| File | Role |
|---|---|
| `ODBC/Odbc.{h,cpp}` | `KODBC`: connection (`.dsn` or connection string), `Query` with reconnect, `Fetch` and `operator>>` by type, character filters (`IsInvalidMarkIn*`), `KDBQuerySuccessChecker` |
| `ODBC/StoredProcedure.{h,cpp}` | Fixed queries used by more than one server |
| `ODBC/DBUtil.{h,cpp}` | Procedure profiler (`KStoredProcedureProfiler`), unchanged |
| `SubThread.h` | `DeclareDBThread`, `DO_QUERY*` (builds query using `x2::wformat` and calls `KODBC::Query`), `FETCH_DATA`, unchanged |
| `DBLayer.cpp`, `DBThreadManager.cpp`, `DBConnectionManager.cpp` | `DBLayer:AddDB(type, '<file>.dsn', threads, bConnStr)` in `config_*.lua`; one `KODBC` connection per DB thread |
| `x2/platform/odbc.h` (platform) | `<sql.h>`/`<sqlext.h>` and `wchar_t` ↔ `SQLWCHAR` conversions; CMake target `x2::odbc` ([Platform](platform.md)) |

All queries are text assembled as `exec dbo.<procedure> <args>`; formats used are `%d`, `%s` (always inside `N'...'`), and `%f`.

## Configuration

Real configuration (`_reference/_original-server`) uses only `.dsn`: `[ODBC]`, `DRIVER=SQL Server` (legacy Windows driver, unavailable
on Linux), `SERVER=host,port`, `DATABASE`, `UID`, `PWD`, `WSID`, and `APP`, in UTF-16. No `config_*.lua` uses connection-string mode.

- `.dsn` remains the input, but `KODBC` no longer uses `FILEDSN=`: it reads `[ODBC]` (UTF-16, UTF-8, or CP949, via `DecodeTextFile`)
  and builds the connection string (`KODBC::ReadFileDSN` + `KODBC::BuildConnStrFromDsn`, tested without a database).
  The file's `DRIVER` is replaced with the configured driver, `WSID` is discarded, and the remaining keys keep their order (wrapped in `{}` if values contain spaces, `;`, or braces).
- Default driver: `ODBC Driver 18 for SQL Server`, with options `Encrypt=yes;TrustServerCertificate=yes` (encrypted channel,
  certificate not verified: Driver 18 requires encryption by default and rejects self-signed certificates; legacy driver did not encrypt).
- New optional calls in `config_*.lua`, **before** `AddDB`:
  - `DBLayer:SetODBCDriver('FreeTDS')`: changes driver and **clears options** (they are for Driver 18);
  - `DBLayer:SetODBCOptions('Encrypt=yes')`: options appended to the end of connection string.
- `.dsn` path remains relative to the current directory, as in original; outside Windows, `\` in configs becomes `/`.
- `SERVER=host,port` works with Driver 18 and FreeTDS (verified with FreeTDS on 2026-10-30).
- Connection-string mode (`AddDB(..., true)`, UID/PWD from command line) remains unchanged.
- Password is not logged: `.dsn` mode logs the path (`FILEDSN=<path>`), connection-string mode logs only `DATABASE`, as in original.

## Changes from the original

### Portability (no behavior change)

| Where | Original | Now |
|---|---|---|
| Text in API (`SQLExecDirectW`, `SQLDriverConnectW`, `SQL_C_WCHAR`, diagnostics) | cast `wchar_t*` to `SQLWCHAR*` | converted with `x2::platform::ToSqlWString`/`FromSqlWString` (copy on Windows; elsewhere `wchar_t` is 4 bytes and `SQLWCHAR` 2) |
| `m_szDiagMsg`, `m_sqlState` | `SQLWCHAR` arrays | `wchar_t` arrays (same names), filled by `ReadDiag()`, replacing repeated `SQLGetDiagRecW` blocks. `DO_QUERY*` macros unchanged |
| `SQLGetData` indicators | `SQLINTEGER` (4 bytes) | `SQLLEN`, 8 bytes on x64 (Windows and Linux); driver would otherwise write past the variable |
| `SQLColAttribute` with `int nLen` | result never used | removed (also wrote 8 bytes to an `int`) |
| Connection buffers | `wsprintfW` into `SQLWCHAR[255]` and `sizeof` (bytes) used as character count | full string with `SQL_NTS`; three connection functions merged into one (`AllocConnection`) |
| `THROW_INVALID_QUERY` | `std::exception("...")` (MSVC extension) | `std::runtime_error` |
| `operator>>(time_t&)` | guarded by `_USE_32BIT_TIME_T` | removed (only exists on MSVC x86) |
| `ToStringW` | `GetPrivateProfileSectionNamesW` on `.dsn` | reads `Address` from `[ODBC]` section (`.dsn` has only this section) |
| `KStoredProcedure()` | private constructor bound a reference to a temporary | `= delete` (never used) |

### Intentional

| Where | Change | Reason |
|---|---|---|
| `operator>>(std::wstring&)` and `(std::string&)` | Read whole column in chunks; `NULL` still becomes empty string | Original read at most 1,024 bytes (511 characters) and silently truncated the rest |
| `operator>>(wchar_t*)` and `(char*)` | Same limit as original (511 characters + null; 1,024 bytes) | Destination size is unknown |
| `operator>>(wchar_t&)` | Reads first character | Original requested a one-`wchar_t` buffer, which fits only the terminator: always read `L'\0'` |
| `operator>>(KSerBuffer&)` | Copies only data returned in each chunk and stops on error | Original copied `LenBin` bytes (what was *missing*, not what fit) from a 1,024-byte buffer and looped forever on `SQL_ERROR` |
| `.dsn` connection | Read file ourselves, replace driver, drop `WSID` (see Configuration) | `SQL Server` driver is unavailable outside Windows; unixODBC does not read UTF-16 `.dsn` |
| Connection failure | Log contains `SQLSTATE` and driver message | Original logged only return code |
| `ReadDiag` | When no diagnostic record exists, `m_sqlState`/`m_szDiagMsg` are empty | Original retained previous error text |
| `Query` | `SQL_NO_DATA` from `SQLExecDirectW` (command without results: procedure with `SET NOCOUNT ON` and no `SELECT`, `CREATE`) counts as success (2026-10-31, `server/login-run`) | FreeTDS returns `SQL_NO_DATA`; production's `SQL Server` driver returned success: original LoginServer logs show `dbo.P_StatCCUMinute_INS` running every minute without errors. Before this change the port logged "Query failed" and `DO_QUERY` skipped the rest (Henir ranking backup stopped at `P_Rank_SpaceTime_*_DEL`) |

### Original bugs fixed

| Where | Bug | Fix |
|---|---|---|
| `KODBC::RemoveInvalidMark` | Replaced each character with `*` while always searching from the start; because `*` is also a match, it **never terminated** when text contained a mark | Continue search after replacement. No server calls this function |
| `KStoredProcedure::Query_GetGuildMemberGrade` | `u_char == -1`, always false | Removed check (same behavior). No server calls this function |

### Retained as in original (documented)

- `USHORT` is read as `SQL_C_SSHORT` (`smallint -1` becomes 65535; values above 32,767 in an `int` column fail).
- If initial connection fails, automatic reconnect (`08S01`/`24000`) never happens because there is no SQL state to compare.

## Tests

| Test | Coverage |
|---|---|
| `platform/tests/test_odbc.cpp` (always; Linux and Wine) | `SQLWCHAR` ↔ `wchar_t`: ASCII, Korean, `♪`, ideographic space, outside BMP (surrogate pair), empty |
| `server/Common/tests/test_odbc.cpp`, no database (always) | `ReadFileDSN` with UTF-16 `.dsn` in original format (`[ODBC]`, comment, another section), `BuildConnStrFromDsn` (driver replacement, omit `WSID`, `{}` around `;`/`}`/space, options), `IsInvalidMarkIn*`, `RemoveInvalidMark` |
| Same, `db` suite (ctest `server_common_db`, only with `X2_TEST_ODBC`) | Against SQL Server: all `operator>>` types (`bit`, `smallint` into `char`/`short`/`USHORT`, `int`, `bigint`, `real`, `nvarchar` with Korean and outside BMP, 3,000 characters, `varchar`, 2,500-byte `varbinary`, `NULL`); error with `SQLSTATE` `42S02` and message; wrong password; UTF-16 `.dsn` with `DRIVER=SQL Server` and `WSID` replaced; temporary procedure via `exec` and `bigint`/`N'...'`; procedure without result (`SET NOCOUNT ON`, `INSERT` only) succeeds; `KStoredProcedure` against `Game01` |

Without `X2_TEST_ODBC`, ctest runs only database-free tests (`--test-suite-exclude=db`); running the `db` suite manually without the variable reports "skipped".

## Pending

- Test with **ODBC Driver 18** (`msodbcsql18`, AUR) and on Windows/MSVC.
- Reconnect after connection loss (`KILL` session): not automated in tests.
- Wine integration test: attempted on 2026-10-30; Wine's `odbc32` did not reach Linux FreeTDS driver (connection failed without diagnostics). No commitment to support this.
- **Out of scope for this step:** replace assembled SQL text with parameters (`SQLBindParameter`). Original builds `exec ... N'%s'` and relies on
  `IsInvalidMarkIn*` to reject quotes; changing this affects each `DO_QUERY` and needs its own decision and step. Risk noted: SQL injection
  where a `%s` does not pass through the filter.
- Connection pooling, async, databases other than MSSQL: out of scope (T7).
