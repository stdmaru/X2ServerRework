// ODBC API (decision T17). Replaces `#include <sql.h>` / `<sqlext.h>` in ported code.
// Windows: the native headers. Elsewhere: unixODBC, whose sqltypes.h typedefs its own Win32 names with
// different types (ULONG is 64-bit, TCHAR is char, WCHAR is unsigned short...). Those are renamed while
// the headers are read, so ours stay in charge; the SQL* types are untouched.
#pragma once

#include "x2/platform/types.h"

#if defined(_WIN32)

#include <sql.h>
#include <sqlext.h>

#else

#define BOOL      x2_odbc_BOOL
#define HWND      x2_odbc_HWND
#define HINSTANCE x2_odbc_HINSTANCE
#define CHAR      x2_odbc_CHAR
#define TCHAR     x2_odbc_TCHAR
#define WCHAR     x2_odbc_WCHAR
#define BYTE      x2_odbc_BYTE
#define WORD      x2_odbc_WORD
#define DWORD     x2_odbc_DWORD
#define UCHAR     x2_odbc_UCHAR
#define USHORT    x2_odbc_USHORT
#define UINT      x2_odbc_UINT
#define ULONG     x2_odbc_ULONG
#define LPSTR     x2_odbc_LPSTR
#define LPCSTR    x2_odbc_LPCSTR
#define LPWSTR    x2_odbc_LPWSTR
#define LPCWSTR   x2_odbc_LPCWSTR
#define LPTSTR    x2_odbc_LPTSTR
#define LPDWORD   x2_odbc_LPDWORD

#include <sql.h>
#include <sqlext.h>

#undef BOOL
#undef HWND
#undef HINSTANCE
#undef CHAR
#undef TCHAR
#undef WCHAR
#undef BYTE
#undef WORD
#undef DWORD
#undef UCHAR
#undef USHORT
#undef UINT
#undef ULONG
#undef LPSTR
#undef LPCSTR
#undef LPWSTR
#undef LPCWSTR
#undef LPTSTR
#undef LPDWORD

#endif // _WIN32
