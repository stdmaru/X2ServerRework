// MSVC CRT extensions used by the original code. Native on Windows, provided elsewhere.
// Wide printf variants follow MSVC semantics (%s = wide string), see TranslateWideFormat.
#pragma once

#include "x2/platform/types.h"

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

#if !defined(_WIN32)

#include <strings.h>

inline int       _wtoi(const wchar_t* s) { return static_cast<int>(std::wcstol(s, nullptr, 10)); }
inline long      _wtol(const wchar_t* s) { return std::wcstol(s, nullptr, 10); }
inline long long _wtoi64(const wchar_t* s) { return std::wcstoll(s, nullptr, 10); }
inline double    _wtof(const wchar_t* s) { return std::wcstod(s, nullptr); }
inline long long _atoi64(const char* s) { return std::strtoll(s, nullptr, 10); }

inline int _stricmp(const char* a, const char* b) { return ::strcasecmp(a, b); }
inline int _strnicmp(const char* a, const char* b, size_t n) { return ::strncasecmp(a, b, n); }
inline int _wcsicmp(const wchar_t* a, const wchar_t* b) { return ::wcscasecmp(a, b); }
inline int _wcsnicmp(const wchar_t* a, const wchar_t* b, size_t n) { return ::wcsncasecmp(a, b, n); }
#define stricmp  _stricmp
#define strnicmp _strnicmp
#define wcsicmp  _wcsicmp

typedef int errno_t;

errno_t wcscpy_s(wchar_t* dst, size_t size, const wchar_t* src);
errno_t wcsncpy_s(wchar_t* dst, size_t size, const wchar_t* src, size_t count);
errno_t wcscat_s(wchar_t* dst, size_t size, const wchar_t* src);
errno_t strcpy_s(char* dst, size_t size, const char* src);
errno_t strncpy_s(char* dst, size_t size, const char* src, size_t count);
errno_t strcat_s(char* dst, size_t size, const char* src);

template <size_t N> errno_t wcscpy_s(wchar_t (&dst)[N], const wchar_t* src) { return wcscpy_s(dst, N, src); }
template <size_t N> errno_t wcscat_s(wchar_t (&dst)[N], const wchar_t* src) { return wcscat_s(dst, N, src); }
template <size_t N> errno_t strcpy_s(char (&dst)[N], const char* src) { return strcpy_s(dst, N, src); }
template <size_t N> errno_t strcat_s(char (&dst)[N], const char* src) { return strcat_s(dst, N, src); }

int vswprintf_ms(wchar_t* dst, size_t size, const wchar_t* fmt, va_list args); // MSVC format semantics
int swprintf_s(wchar_t* dst, size_t size, const wchar_t* fmt, ...);
int _snwprintf_s(wchar_t* dst, size_t size, size_t count, const wchar_t* fmt, ...);
int sprintf_s(char* dst, size_t size, const char* fmt, ...);
template <size_t N, typename... A> int swprintf_s(wchar_t (&dst)[N], const wchar_t* fmt, A... a) { return swprintf_s(dst, N, fmt, a...); }
template <size_t N, typename... A> int sprintf_s(char (&dst)[N], const char* fmt, A... a) { return sprintf_s(dst, N, fmt, a...); }

errno_t _itow_s(int value, wchar_t* dst, size_t size, int radix);
errno_t _i64tow_s(long long value, wchar_t* dst, size_t size, int radix);
errno_t _itoa_s(int value, char* dst, size_t size, int radix);
template <size_t N> errno_t _itow_s(int v, wchar_t (&dst)[N], int radix) { return _itow_s(v, dst, N, radix); }
template <size_t N> errno_t _itoa_s(int v, char (&dst)[N], int radix) { return _itoa_s(v, dst, N, radix); }

int _mkdir(const char* path);
int _wmkdir(const wchar_t* path);

#define _countof(a) (sizeof(a) / sizeof((a)[0]))

#endif // !_WIN32
