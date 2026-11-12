[Home](../README.md) › [Porting](README.md) › Platform

# Platform (`src/platform`, target `x2::platform`)

The only part of the code that talks directly to the operating system (per [AGENTS.md](../../AGENTS.md)).
New code, with no equivalent in the original. Created on 2026-10-29 (decision T11).

## Idea

The original uses Win32/ATL APIs extensively. Counts in the server alone:

| API | Usage |
|---|---|
| `CTime` | ~1,700 occurrences |
| `DWORD` | ~900 |
| Events/`WaitForSingleObject` | ~50 files |
| `CString` | ~300 |

Rewriting all of this now would be impractical and risky. The layer therefore provides **replacements with the same names and semantics**:
- **On Windows**, it uses real `windows.h`, and this is the only place where it is included.
- **On Linux**, it emulates the used subset.

Modernization (use `std::` directly) is deferred to Phase 5.

Ported code includes `x2/platform/win32.h` instead of `<windows.h>`, `<atltime.h>`, and `<tchar.h>`.

## Contents

| Header | Provides | Linux | Windows |
|---|---|---|---|
| `types.h` | `DWORD`, `BYTE`, `LONG`, `INT64`, `TCHAR`, `_T()`, `MAX_PATH`... with Windows sizes (`DWORD`/`LONG` = 32 bits) | typedefs | `windows.h` |
| `sync.h` | `CRITICAL_SECTION` (recursive), events, `WaitForSingle/MultipleObjects`, `_beginthreadex`, `Sleep`, `GetCurrentThreadId` | emulation (one global mutex/condition variable) | native |
| `time.h` | `GetTickCount`, `timeGetTime`, `QueryPerformanceCounter`, `GetLocalTime`, **`CTime`/`CTimeSpan`** | `steady_clock` / custom implementation | native + custom `CTime` |
| `string.h` | `CStringW`/`CStringA`/`CString` (subset) | custom implementation | same (without ATL) |
| `crt.h` | `_wtoi`, `swprintf_s`, `wcscpy_s`, `_wcsicmp`, `_itow_s`, `_mkdir`, `fopen_s`, `localtime_s` (MSVC argument order), `strncpy_s`/`wcsncpy_s` with `_TRUNCATE` (returns `STRUNCATE`), `_wstrdate_s`/`_wstrtime_s`, `ZeroMemory`, `_tcs*`, `WideCharToMultiByte`... and MSVC array overloads | custom implementation | native |
| `ini.h` | `x2::platform::ReadIniString`/`ReadIniInt` (`GetPrivateProfileString` rules), `WriteIniString` (`WritePrivateProfileString`, writes UTF-8; 2026-10-31, for LoginServer's `RefreshTimeBackup.ini`), `GetPrivateProfileStringW`/`IntW`, `WritePrivateProfileStringW` | custom implementation | also custom via `ReadIni*` (see Pitfalls) |
| `net.h` | Winsock names (`SOCKET`, `closesocket`, `SOCKADDR_IN`, the `WSAE*` codes...); `OVERLAPPED`/`WSABUF` as types only; `IpToString`/`IpFromString`/`ResolveIPv4` instead of `inet_ntoa`/`inet_addr`/`gethostbyname`; `SocketReadable` (poll without waiting) and `SendToScattered` (`WSASendTo` of several buffers, for the client's UDP; 2026-11-06) | BSD sockets; `sendmsg` (at most 16 buffers) | Winsock 2; `WSASendTo` |
| `net_io.h` | Server async networking (T20): `IoService` (`RunFor`, `Stop`, `Drain`), `TcpStream` (`Connect`, `Assign` accepted socket, `AsyncReadSome`/`AsyncWriteSome`, `Close` aborting pending operations), `SocketErrorMessage` | Asio (epoll) | Asio (IOCP) |
| `odbc.h` | `<sql.h>`/`<sqlext.h>` without conflict with platform Win32 types; `SqlWString`, `ToSqlWString`/`FromSqlWString`/`SqlWPtr` (`wchar_t` ↔ `SQLWCHAR`). Target `x2::odbc` (header-only; links ODBC) | unixODBC, with its typedefs renamed; `SQLWCHAR` is UTF-16 | native; `odbc32` |
| `format.h`, `timer.h`, `noncopyable.h` | `x2::wformat`/`x2::str` (faithful to Boost.Format), `x2::Timer`, `x2::noncopyable` | — | — |
| `unicode.h` | wide ↔ UTF-8 ↔ UTF-16; `TranslateWideFormat`; `Cp949ToWide`, `IsValidUtf8`, `DecodeTextFile`; `Utf8FileLocale` (UTF-8 locale for `std::wfstream`, instead of `std::locale("korean")`) | iconv (CP949) | `MultiByteToWideChar(949)` |
| `debug.h` | runtime `__WFILE__`, `__WFUNCTION__`; `OutputDebugStringW`/`A` (2026-11-06) | to stderr | native |
| `rect.h` (from `win32.h`) | `winuser.h`'s rectangle helpers: `SetRect`, `SetRectEmpty`, `CopyRect`, `IsRectEmpty`, `PtInRect`, `OffsetRect`, `InflateRect`, `IntersectRect`, `UnionRect`, `EqualRect` with Win32's results (2026-11-06, for the client's UI) | inline | native |
| `strsafe.h` (from `win32.h`) | `StringCchCopy`/`Cat`/`Printf` (wide and narrow) and `lstrlen`, always null-terminated, `STRSAFE_E_INSUFFICIENT_BUFFER` on truncation; the wide printf with MSVC's `%s` (2026-11-06) | inline | `strsafe.h` |
| `file.h` | `ReadOnlyFile`: open, size, thread-safe `ReadAt` (positional reads; 2026-11-05, for [`.kom` archives](kom.md)); `FindFileIgnoreCase(directory, name)`: the file itself, else the directory entry equal to it ignoring ASCII case, else an empty path (2026-11-12, for the client's loose files) | `open` + `pread` | `CreateFileW` (`FILE_SHARE_READ`, so the file cannot be replaced while open) + `ReadFile` with an `OVERLAPPED` offset |
| `terminal.h` | `Terminal` (server terminal, T15: raw input, prompt redrawn after output, stdout/stderr piped, title, stop signals), `WaitForStop` (service mode), `KeyDecoder` (UTF-8 + VT sequences → keys), `LineEditor` (editing, history, East Asian character widths, horizontal scrolling). See [Server console](console.md); `machine` and `SetOutputHandler` for machine mode | `termios`, `poll`, pipe with `dup2` | `ReadConsoleW` with VT input, `_pipe` + `_dup2` + `SetStdHandle`, `WriteConsoleW`, `SetConsoleCtrlHandler` |
| `control.h` | Server control socket (T30): `ControlServer` (listen, queue and writer thread per client, greeting, broadcast), `ControlClient` (`x2ctl`), `ControlSocketDir`/`Path`, `ListControlSockets`. See [Server console](console.md) | `AF_UNIX` | `AF_UNIX` (`afunix.h`, Windows 10 1803+) |
| `json.h` | `AppendJsonString`/`JsonString` (escaping; invalid UTF-8 becomes U+FFFD), `ParseJsonObject` (object without nested objects or arrays: control protocol commands), `ParseJson`/`Json` (full RFC 8259 document, at most 64 levels: [ServerManager](../tools/servermanager.md) reads log lines) | — | — |
| `process.h` | `Process` (child with stdin/stdout/stderr pipes, working directory, `Terminate`/`Kill`/`WaitFor`, exit code; POSIX: own process group, SIGPIPE ignored in the parent), `QuoteWindowsArgument`, `QuoteCmdArgument`, `QuoteShellArgument`, `SetEnvironment` (inherited by later children), `CurrentProcessId`/`ParentProcessId` (ServerManager, T32); `SingleInstance`, a named lock one process holds at a time (the client's `ElswordX2` mutex, 2026-11-12) | `posix_spawnp` with `addchdir_np`, `pipe2(O_CLOEXEC)`, `waitpid`; 128 + signal when killed by a signal; `setenv`/`unsetenv`; `getpid`/`getppid`; `SingleInstance`: `flock` on `$XDG_RUNTIME_DIR/<name>.lock` | `CreateProcessW` with only the pipe handles inherited (`PROC_THREAD_ATTRIBUTE_HANDLE_LIST`), `CREATE_NO_WINDOW`; `Terminate` closes stdin; `_wputenv_s`; the parent from a Toolhelp32 snapshot; `SingleInstance`: `CreateMutexW` |
| `console.h` | `InitConsole` (at start of each server `main`), `IsColorTerminal` (used by logging), `Utf8Arguments` (`main` arguments as UTF-8; on Windows, from UTF-16 command line) | no-op; `isatty` | UTF-8 console input/output (`SetConsoleCP`/`SetConsoleOutputCP`, restored on exit) and ANSI escapes enabled (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`); color only if console supports it |
| `window_messages.h` (from `win32.h`) | `WM_*` (including IME), `VK_*`, `WA_*`/`SIZE_*`/`SC_*`/`MK_*`/`XBUTTON*`/`GCS_*` with their Windows values; `WPARAM`/`LPARAM`/`LRESULT`, `POINT`/`RECT`, `LOWORD`/`HIWORD`/`MAKELPARAM`/`GET_X_LPARAM`/`GET_WHEEL_DELTA_WPARAM`... (2026-11-05, for the client's `MsgProc`; see [Client: DXUT](client-dxut.md)) | definitions | `windows.h` |
| `ime.h` | IMM32 types and constants for the client's IME edit box: `HIMC`, `HKL`, `CANDIDATELIST`, `INPUTCONTEXT`, `IMN_*`, `NI_*`, `CPS_*`, `IME_CMODE_*`, `ATTR_*`, `LANG_*` (2026-11-05; the functions are emulated in the client, see [Client: IME](client-ime.md)) | definitions | `imm.h`, `immdev.h` (types only) |
| `bimap.h` | `x2::bimap<L, R>`: the part of Boost.Bimap the client uses (`insert` of a pair when neither key exists, read-only `left`/`right` maps, `clear`, `size`; 2026-11-07) | two `std::map`s | same |
| `xml.h` | `ParseXml` → `XmlElement` tree (name, attributes, children, text; `Attribute`, `Child`): BOM, prolog, comments, processing instructions, CDATA, the five entities and character references; `DOCTYPE` is skipped. Replaces libxml for `CX2GameOption` (decided with the user, 2026-11-07) | custom implementation | same |
| `d3dx_math.h` | D3DX 9 math for the servers and the client: `D3DXVECTOR2/3/4`, `D3DXMATRIX`, `D3DXCOLOR`, `D3DXQUATERNION`, `D3DXPLANE` (D3DX's full member sets), `D3DXKEY_VECTOR3/QUATERNION`, `D3DX_PI`, `D3DXToRadian/Degree`, `D3DX_ALIGN16`; `D3DXVec2/3/4*`, `D3DXMatrix*`, `D3DXQuaternion*`, `D3DXPlane*`, `D3DXComputeBoundingSphere` (list in [D3DX math](#d3dx-math)); and the `d3d9types.h` pieces it needs: `D3DCOLORVALUE`, `D3DVIEWPORT9`, `D3DCOLOR_ARGB/RGBA/XRGB/COLORVALUE`, `D3D_OK`/`D3DERR_INVALIDCALL` | custom implementation | same (without DXSDK) |

## Pitfalls handled by the layer

- **`%s` in wide format strings**: in MSVC, `%s` in `swprintf`/`CString::Format` means a *wide* string; in glibc it means a *narrow* string (crash).
  `TranslateWideFormat` converts `%s`→`%ls`, `%c`→`%lc`, `%S`→`%s`, and `%I64d`→`%lld`; it is used by `CString::Format` and `*printf_s`.
  Only MSVC CRT (used by clang-cl) does not need translation.
- **`L ## __FUNCTION__`** exists only in MSVC. `__WFUNCTION__` is now a runtime `const wchar_t*`, which can be passed and concatenated via `<<`,
  but not pasted into another literal. The server uses it only as an argument.
- **`GetCurrentTime`** is a `windows.h` macro: `types.h` undefines it so it does not break `CTime::GetCurrentTime()`.
- `GetTickCount` wraps every ~49.7 days, as on Windows (subtraction-based unsigned comparisons still work).
- **`&D3DXVECTOR3(0, 0, 0)`**: the original passes addresses of temporaries to D3DX functions, an MSVC extension. D3DX types have an
  `operator&` for temporaries only (qualified with `&&`), returning the same result; `&` on variables remains native.
- D3DX math: same conventions and formulas (row vector `v·M`, left-handed, `D3DXMatrixInverse` returns `NULL` for singular matrices),
  checked against Wine's implementation. See [D3DX math](#d3dx-math).
- **Original text files**: production `config_*.ini` files are **CP949**, and `.lua` scripts are UTF-8 with BOM (5,829 of 5,832, as of 2026-10-29).
  `DecodeTextFile` detects UTF-16/UTF-8 and falls back to CP949. INI is read with `ReadIniString` on Windows too, because the native API uses
  the system code page (under Wine, or on non-Korean Windows, version strings would be corrupted; Wine tests showed this).
- **unixODBC** defines its own `ULONG` (64 bits), `TCHAR` (`char`), `WCHAR` (`unsigned short`)... `odbc.h` renames these typedefs during inclusion.
  Its `SQLWCHAR` is 2 bytes (UTF-16), while `wchar_t` is 4: text always goes through `odbc.h` conversions, never casts.
- **Networking**: Asio is included only by `src/platform/src/net_io.cpp`. `TcpStream::Close` cancels before `shutdown`, because on Windows
  `shutdown` finishes a pending read as end-of-connection while on Linux it is cancellation; this makes both report `Aborted`. Code destroying
  `IoService` with pending operations must call `Drain()` first (`~KIocp` does this), or handlers release sockets during destruction.
- **Windows**: `types.h` includes `windows.h` with `WIN32_LEAN_AND_MEAN`, excluding `rpcndr.h` and `winsock.h`; names the original got from them
  (`byte`, `u_char`, `u_short`, `u_int`, `u_long`) are declared with the same types. Windows builds define `UNICODE`/`_UNICODE` (`x2::options`).
- **Windows stdout redirection** (`terminal.cpp`): `_dup2(pipe, 1)` closes the handle previously associated with fd 1, which is the same one
  returned earlier by `GetStdHandle(STD_OUTPUT_HANDLE)`. The terminal writes through a duplicate (`_dup(1)`); with the old handle, nothing
  appeared — neither logs nor prompt (found in the VM on 2026-10-31).
- `IsWindow` always returns false and `HWND_DESKTOP` is null outside Windows (server has no window).

## Known limitations

- `_beginthreadex` does not support `CREATE_SUSPENDED`.
- `CString` covers only the subset used so far (it will grow as the server port continues).
- Windows console (2026-10-31): before `InitConsole`, Korean log text was corrupted because the console decoded UTF-8 using the OEM code page.
  With it, output is correct, but conhost with Consolas lacks Hangul glyphs (Windows Terminal uses font *fallback*) and reading non-ASCII UTF-8
  from conhost stdin is unreliable: therefore the terminal (`terminal.h`) reads with `ReadConsoleW`.
- Windows: builds without warnings and passes tests with **clang-cl** (Visual Studio 2026, MSVC 14.51) on real Windows (T26, 2026-10-30).
  From 2026-10-29 through 2026-10-30 it was checked with MinGW-w64 + Wine (T18). Microsoft's STL debug mode found undefined behavior in
  `x2::format` (`assign` with a reference to its own vector); fixed.

## D3DX math

A custom header, not a shim over GLM: same names, layouts (`static_assert`ed) and formulas as D3DX 9, taken from Wine's
`d3dx9_36/math.c` and `d3dx9math.inl` (read on 2026-11-05). No x86 intrinsics. Only what the code calls is implemented; the
types have all of D3DX's constructors and operators, so client code that compiled against D3DX compiles here.

Survey of the client (`X2Lib`, `KTDXLIB`, `X2`; 2026-11-05), calls per name:

| Group | Used by the client |
|---|---|
| Types | `D3DXVECTOR3` (6,798), `D3DXVECTOR2` (5,173), `D3DXCOLOR` (2,208), `D3DXMATRIX` (273), `D3DXVECTOR4` (48), `D3DXKEY_VECTOR3` (48), `D3DXKEY_QUATERNION` (30), `D3DXQUATERNION` (22), `D3DXPLANE` (6); `D3DCOLOR_ARGB` (40), `D3DCOLOR_RGBA` (16), `D3DCOLOR_COLORVALUE` (11) |
| Vectors | `Vec3Normalize` (186), `Vec3Dot` (110), `Vec3Length` (76), `Vec3Transform` (40), `Vec3LengthSq` (34), `Vec3TransformCoord` (22), `Vec3Cross` (20), `Vec3Project` (15), `Vec3CatmullRom` (9), `Vec2Length` (7), `Vec3TransformNormal` (4), `Vec4Normalize` (3), `Vec2Normalize`, `Vec2Dot`, `Vec3Add/Subtract/Scale/Minimize/Maximize`, `Vec2Transform` |
| Matrices | `MatrixIdentity` (54), `Multiply` (24), `Translation` (19), `RotationYawPitchRoll` (19), `RotationAxis` (18), `Scaling` (14), `LookAtLH` (14), `Inverse` (12), `RotationY` (4), `PerspectiveFovLH` (3), `OrthoLH`, `Transformation2D` |
| Quaternions, planes | `PlaneDotCoord` (32), `PlaneIntersectLine` (8), `PlaneNormalize` (6), `PlaneFromPointNormal` (4), `QuaternionNormalize` (4), `QuaternionSlerp`, `QuaternionMultiply`, `QuaternionConjugate` (2 each) |
| Mesh math | `D3DXComputeBoundingSphere` (3) |

Added for the client on 2026-11-05 (`client/d3dx-math`): `D3DXQUATERNION`, `D3DXPLANE`, the animation keys, the
`D3DXCOLOR` members the servers did not need (`D3DCOLORVALUE` conversions, `/`, unary `-`), and the functions of the last four
rows that were missing; plus helpers they are built on (`D3DXMatrixRotationQuaternion`, `D3DXMatrixTransformation`,
`D3DXQuaternionLength/Dot/Identity`, `D3DXVec4Length/Dot`, `D3DXPlaneDot/DotNormal`). The rest of the client's `D3DX*` names
are not math and belong to [Phase 4](../project/roadmap.md#phase-4--client-graphics): `D3DXFrame*`, `D3DXCreate*`,
`D3DXLoadMesh*`, `D3DXIntersect`, `D3DXWeldVertices`, `D3DXComputeNormals`, `D3DXGetFVFVertexSize`, `D3DXSaveSurfaceToFile`,
effects, animation controllers and their constants.

Behavior worth knowing (all as in D3DX):

- `D3DXVec2/3Normalize` and `D3DXPlaneNormalize` return zero for a zero input; `D3DXVec4Normalize` and
  `D3DXQuaternionNormalize` divide unchecked (NaN).
- `D3DXQuaternionMultiply(out, q1, q2)` is the rotation `q1` followed by `q2` (Hamilton product `q2·q1`), so it matches
  `M(q1) * M(q2)`. `D3DXQuaternionSlerp` takes the shortest path and interpolates linearly within 0.001.
- `D3DXPlaneIntersectLine` intersects the infinite line, not the segment, and returns `NULL` only when parallel.
- `D3DXMatrixTransformation2D` treats a zero angle as "no rotation" and uses z scaling 1.
- `D3DXCOLOR(DWORD)` multiplies by `1/255` (until 2026-11-05 it divided by 255, which can differ in the last bit);
  `D3DCOLOR_COLORVALUE` truncates (`0.5` → `0x7F`) while `D3DXCOLOR`'s `DWORD` conversion rounds.

## Tests

`src/platform/tests` (doctest): Unicode, terminal (key decoder, line editor), JSON (flat and full), processes (with `x2_process_helper` as the child), control socket, MSVC formats, `CString`,
`CTime`/`CTimeSpan`, events, threads, CRT, `bimap`, XML, and D3DX math
(expected values derived manually from D3DX conventions; every function added for the client has a case, 2026-11-05).
