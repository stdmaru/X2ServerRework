// ODBC API (decision T17). Replaces `#include <sql.h>` / `<sqlext.h>` in ported code; link x2::odbc.
// Windows: the native headers. Elsewhere: unixODBC, whose sqltypes.h typedefs its own Win32 names with
// different types (ULONG is 64-bit, TCHAR is char, WCHAR is unsigned short...). Those are renamed while
// the headers are read, so ours stay in charge; the SQL* types are untouched.
#pragma once

#include "x2/platform/types.h"
#include "x2/platform/unicode.h"

#include <string>
#include <string_view>

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

namespace x2::platform {

// SQLWCHAR is wchar_t on Windows, and UTF-16 (unsigned short) in unixODBC, where wchar_t is UTF-32:
// wide text crosses the API through these, never through a cast of a wchar_t pointer.
#if defined(_WIN32)
using SqlWString = std::wstring;
#else
using SqlWString = std::u16string;
#endif
static_assert(sizeof(SqlWString::value_type) == sizeof(SQLWCHAR));

inline SqlWString ToSqlWString(std::wstring_view s)
{
#if defined(_WIN32)
    return SqlWString(s);
#else
    return WideToUtf16(s);
#endif
}

inline std::wstring FromSqlWString(const SQLWCHAR* s, size_t len)
{
    const auto* p = reinterpret_cast<const SqlWString::value_type*>(s);
#if defined(_WIN32)
    return std::wstring(p, len);
#else
    return Utf16ToWide(std::u16string_view(p, len));
#endif
}

// zero-terminated
inline std::wstring FromSqlWString(const SQLWCHAR* s)
{
    size_t len = 0;
    while (s[len] != 0)
        ++len;
    return FromSqlWString(s, len);
}

inline std::wstring FromSqlWString(const SqlWString& s)
{
    return FromSqlWString(reinterpret_cast<const SQLWCHAR*>(s.data()), s.size());
}

inline SQLWCHAR* SqlWPtr(SqlWString& s) { return reinterpret_cast<SQLWCHAR*>(s.data()); }

} // namespace x2::platform
