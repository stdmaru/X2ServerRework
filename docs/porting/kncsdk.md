[Home](../README.md) › [Porting](README.md) › KNCSDK

# KNCSDK (`src/kncsdk`, targets `x2::kncsdk` and `x2::kncsdk_lua`)

Internal SDK on which all servers and the client depend. Parts:

| Folder | Contents |
|---|---|
| `dbg/` | logging |
| `Serializer/` | serialization (network format) |
| `KncSecurity/` | packet encryption |
| `Crypto/` | **new** (T27): DES-CBC, HMAC-MD5, MD5, SHA-256, and hex (`x2::crypto`), replacing Crypto++ |
| `Thread/` | threads and locks |
| `ObjectPool/` | object pools |
| root | `KncUtil`, macros, `lua_tinker` |

- Source: `KNCSDK_Server`, merged with files unique to `KNCSDK_Client`.
  Imported on 2026-10-29 (62 of 742 files, [report](../../tools/import/reports/kncsdk/REPORT.md)) and manually ported the same day.
- The module is **frozen** in the importer (`frozen = true`): all edits are now made directly in `src/kncsdk`.
- Original analysis: [Libraries › KNCSDK](../original/libraries.md).
- Depends on: [Platform](platform.md), zlib, LuaJIT 2.1 (`kncsdk_lua` only).

## Status (2026-10-29)

| Part | Status |
|---|---|
| Linux build (GCC 16 Debug, Clang 23 Release) | passes, no warnings |
| Windows MinGW-w64 build (GCC 16, UCRT), tests under Wine 11 | passes, no warnings; `kncsdk_tests` passes (2026-10-29) and `kncsdk_lua_tests` too, with LuaJIT 2.1 built by CMake (T21, 2026-10-30) |
| Windows build (clang-cl, VS 2026) | passes, no warnings; `kncsdk_tests` and `kncsdk_lua_tests` pass on real Windows (T26, 2026-10-30) |
| Merge with `KNCSDK_Client` | complete (`SecureBuffer`, `KCSLockType`), see below |
| Tests | 39 cases / 1,029 assertions (`kncsdk_tests`, count as of 2026-10-31) + 3 (`kncsdk_lua_tests`), all pass |
| Validation against real bytes from original client | **pending** (see Pending) |

## Changes from the original

Public interface (names, macros, signatures) retained; changes are listed here.

### Intentional

| Where | Change | Reason |
|---|---|---|
| `dbg/logfile` | Each log statement builds a **record** (`dbg::entry`) with metadata (level, function, time, file/line, `BUILD_LOG` fields) alongside the text, written under a lock at the end of the statement. The file, **UTF-8 text** (`log.log`, or `log/log_<date>.log`), receives the original text unchanged; the console has its own rendering (see [Logging](#logging-t16)) and level | Original wrote **HTML** from multiple threads without a lock (race condition); console duplicated file text (decision T16) |
| `dbg/logfile` | A statement without `dbg::endl` (`dbg::cerr << L"..." << id;`) ends its line in the file | Original left the line open, so the next log (from any thread) appended to it |
| `dbg/profiler` | Removed | Disabled in original (`TB_USE_PROFILER`), unused |
| `KncUtil` | Narrow strings are **UTF-8** on every platform | Original used Windows ANSI code page (`CP_ACP`) |
| `KncUtil::GetTimeStampByName*` | Executable modification date | Original read *link time* from PE header |
| `KncSecurity`, `KncUtil::GetMD5` | Crypto uses `x2::crypto` (`Crypto/`, T27) instead of Crypto++; same bytes (official vectors + comparison with Crypto++). Removed unused `GetRandomGenerator()` (`AutoSeededRandomPool` had no use outside commented code) | Crypto++ does not compile with clang-cl or VS 2026 STL |
| `KncSecurity` | Key/SPI generator seeded by OS, one per thread | Original used **unseeded** `boost::mt19937` (same keys every run) and shared it without a lock |
| `ObjectPool` | `BlockAllocator`/`BoostAllocator` use a custom pool (`FixedBlockPool.h`) | Original stored **32-bit** pointers in free blocks; removes Boost |
| `Import*.h` | Emptied | Only contained `#pragma comment(lib)`; CMake now links libraries |
| `lua_interpreter` | Now the [server console](console.md) (`KServerConsole`, `ServerConsole.cpp`); `lua_interpreter( L )` is the same without a name or socket | Original came from a modified Lua 5.1.4 `lua.c`, not included with LuaJIT, and MFC window (`CServerUIDlg`) had its own command field (T15) |
| `LUA_DOFILE` | Was `luaL_dofile`; now `lua_dofile` (`lua_dofile.cpp`, 2026-10-31). Same as `luaL_dofile`, except for files exceeding LuaJIT's limit of 65,536 constants in one function (`Item.lua` generated from the client, `ArcadeResultTable.lua`): split at top-level commands (identifier in column 0) and precompiled parts run in order, preserving source line numbers in messages. Splitting only works without top-level `local` and long strings (`[[`); if a part does not compile, return the original error | Original ran Lua 5.1, which accepts 2^18 constants; LuaJIT rejected both files (T21) |
| Original tests (`UnitTest/`) | Removed and replaced with doctest | They were demos (one copied Crypto++ examples) |

### Logging (T16)

Implemented on 2026-10-31 (`kncsdk/log-structured`). Macros (`dbg.hpp`) retain their names and use; only their internals changed:

| Macro | Passed to logger | File text (same as original) |
|---|---|---|
| `START_LOG( type, data )` | `logfile::Start`: level, type name, function, time, file and line; records call site (`LogStat`) | `type : Function(), YYYY-MM-DD hh:mm:ss, data` |
| `BUILD_LOG*( x )` | `dbg::field`: one field (name = expression, value) | `    x : value` |
| `END_LOG` | `dbg::location`: file and line | `    (file, line)` |
| `LOG_HEADER_STR`, `_JIF`/`LIF`, `LOG_CONDITION` | `dbg::header`: function (and time) | `Function(), date, ` / `Function(), in x` |

The first `<<` in `dbg::cout/cerr/cwarn/clog` (or `Start`) creates a temporary `dbg::record`, which writes the record when the statement
ends; if the level goes to no destination, nothing is formatted. Console output is one line per record, with fields aligned below:

```text
18:44:03 NOTE  SetLanBugOutCheckOption_Lua: 랜선렉 방지 코드: 인자 정보 수정
               m_bLanBugOutCheck            = false
               m_byteLanBugOutCheckMaxCount = 0
```

- Levels: `ERROR` (`cerr`, red), `WARN` (`cwarn`, yellow), `INFO` (`clog`, cyan), `NOTE` (`cout`, green); `*2` variants match their base level.
- Time and field names are dimmed, function is bold; no color when output is not a terminal (`IsColorTerminal`).
- Console omits file/line, separators (`lbreak`), and empty lines; standalone text (dumps, tables) is emitted as-is.
- Console level: `logfile::SetConsoleLogLevel` (Lua: `consolelog( n )`), independent from file `log( n )`; `-1` follows file level, the default.
- `logfile::SetConsoleWriter` changes console destination (used by T15 terminal); `logfile::SetRecordSink` receives each record emitted to console in structured form, and `RenderJson` writes it as JSON (T30 protocol, see [Console](console.md#protocol)).

### Server console (T15, T30)

`ServerConsole.{h,cpp}` (`KServerConsole`, in `kncsdk_lua` target): terminal and control protocol, documented separately:
[Server console](console.md).

### Original bugs fixed

| Where | Bug | Fix |
|---|---|---|
| `KSecureBuffer::GetPayload*` and similar | `_JIF( cond, false )`: the "action" `false;` does nothing, so code **continued** after failure, e.g. decrypting a packet with invalid HMAC | `_JIF( cond, return false )`, a pattern already used 1,404 times in server. Rejects only packets that were already invalid |
| `lua_tinker::dobuffer/dostring` | Inverted condition: script never ran | Fixed (server did not use these functions) |
| `lua_tinker` `tostring_s64/u64` | `%I64d` exists only in MSVC | `%lld`/`%llu` |
| `KncUtil::EraseFrontWord` | Relied on 32-bit `npos` overflow | Same result, explicit |
| `SerBuffer::Compress/UnCompress` | Passed `DWORD*` where zlib expects `uLongf*` (8 bytes on Linux) | Correctly typed variables; on-wire size remains `DWORD` |
| `SerBuffer::UnCompress` | Ignored failure to read original size and decompressed using an uninitialized value | Returns `false` (GCC Release warning) |
| `SerBuffer::UnCompress` | Events with no data are also compressed (size 0 + empty zlib stream), and decompression took `&vec[0]` from an empty vector and called `Write` with 0 bytes (`assert( len > 0 )`): undefined behavior that passed in MSVC Release | `data()` and skip `Write`: same empty buffer. Crashed GameServer (Debug) in TCP relay (`KTRUser`) while changing characters (2026-10-31, `server/center-run`); regression test in `test_serializer.cpp` |

### Portability (no behavior change)

- **Default SA keys (SPI 0)** (found with original client on 2026-10-30): original wrote `"임시로만드는키"` and
  `"해커들절대모를키"` in a CP949 source file and used the first 8 bytes. Once the importer converted the source to UTF-8, bytes changed
  and client rejected the first server packet (`IsValidICV_Local` with SPI 0, "channel server connection failed").
  They are now explicitly encoded as CP949 bytes; `test_security.cpp` checks the key and an HMAC-MD5 calculated externally.
  **General caution:** non-ASCII `char` literals sent over network or used in crypto/hash must be encoded as bytes (2026-10-30 scan: this was the only case).
- `Serializer` uses fixed on-wire sizes, matching MSVC x86. Details in [Protocol](../original/protocol.md):
  - `int`, `unsigned`, `long`, and `unsigned long` are 4 bytes; `long` outside 32-bit range **fails** instead of truncating;
  - `wchar_t` becomes 2 bytes;
  - `std::wstring` becomes UTF-16LE.
- `Serializer`: `SerializeHelper::ComputeTag/PutInto/GetFrom` gained a tag parameter `SerializeHelper::Adl`, and `KSerializer` calls them unqualified.
  Without it, GCC/Clang cannot see overloads declared after the header (the packets); old MSVC could.
  `SERIALIZE_*` macros absorb the change, so packet code is unchanged.
- `KCSLockType`: no Boost and no specializations inside the class; same macros (`KCSLOCK_BEGIN`...).
- `lua_tinker`: missing `typename`/`template`, specializations inside the class (now `if constexpr`), and `push_functor` overloads that could not be instantiated (removed).
  Lua included via `<lua.hpp>`, because LuaJIT is C; original compiled its own Lua copy as C++.
- Macros with `L#x`, `classname##::`, and `Si##Name##()` that worked only in MSVC were rewritten (`WIDENE(x)` = `L ## #x`).
- `RTTI.h`: `DynamicCast` returned `false` as `void*` (now `nullptr`) and `L#classname` became `L ## #classname`.
- `logfile.hpp` includes `<ostream>` (generic `operator<<` needed it).
- `KncUtil::NumberShuffle`: same choices as original (the `rand()`-th remaining item, in order), performed in-place using `std::rotate`; copying with `erase` triggered a GCC 15 false positive in Release (`-Wfree-nonheap-object`). Test in `test_util.cpp`.

## Pending

- **Validate protocol against real bytes**: capture packets from original `x2.exe` and server and compare with ported `Serializer`/`KSecureBuffer`.
  Expected bytes in current tests are derived from reading the code.
- `KSecureBuffer` IV is `'A' + rand() % 40` repeated (weak). Changing it affects the protocol; decide later.
- Client `lua_tinker` (`LuaCommon`) is another variant, imported apart in `src/client/LuaCommon` (2026-11-05); unifying them is
  left for later ([Client: LuaCommon](client-luacommon.md#two-lua_tinker)). The client needs nothing else from `KNCSDK_Client`
  ([Client](client.md#kncsdk_client)).
- In the original client, logging is disabled in production builds (`KNCSDK_Client/dbg/logfile.hpp`); reproduce through log level during client port.
- `KByteStream` inherits from `std::basic_string<unsigned char>`, which libc++ (Clang on macOS) no longer supports. Does not affect Linux/Windows.
- LuaJIT on MSVC: CMake recipe (T21) calls LuaJIT's `msvcbuild.bat` (T25).
