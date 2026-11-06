// MSVC CRT extensions used by the original code. Native on Windows, provided elsewhere.
// Wide printf variants follow MSVC semantics (%s = wide string), see TranslateWideFormat.
#pragma once

#include "x2/platform/types.h"

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
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

// <stdlib.h>'s min/max macros
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a, b) (((a) < (b)) ? (a) : (b))

// count argument of strncpy_s/wcsncpy_s: copy what fits, return STRUNCATE
#define _TRUNCATE (static_cast<size_t>(-1))
#ifndef STRUNCATE
#define STRUNCATE 80
#endif

#define ZeroMemory(dst, size) std::memset((dst), 0, (size))
#define CopyMemory(dst, src, size) std::memcpy((dst), (src), (size))

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
template <size_t N> errno_t wcsncpy_s(wchar_t (&dst)[N], const wchar_t* src, size_t count) { return wcsncpy_s(dst, N, src, count); }
template <size_t N> errno_t strncpy_s(char (&dst)[N], const char* src, size_t count) { return strncpy_s(dst, N, src, count); }

int vswprintf_ms(wchar_t* dst, size_t size, const wchar_t* fmt, va_list args); // MSVC format semantics
int swprintf_s(wchar_t* dst, size_t size, const wchar_t* fmt, ...);
int _snwprintf_s(wchar_t* dst, size_t size, size_t count, const wchar_t* fmt, ...);
int sprintf_s(char* dst, size_t size, const char* fmt, ...);
int _snprintf_s(char* dst, size_t size, size_t count, const char* fmt, ...);  // count: _TRUNCATE or a limit
template <size_t N, typename... A> int swprintf_s(wchar_t (&dst)[N], const wchar_t* fmt, A... a) { return swprintf_s(dst, N, fmt, a...); }
template <size_t N, typename... A> int sprintf_s(char (&dst)[N], const char* fmt, A... a) { return sprintf_s(dst, N, fmt, a...); }
template <size_t N, typename... A> int _snwprintf_s(wchar_t (&dst)[N], size_t count, const wchar_t* fmt, A... a) { return _snwprintf_s(dst, N, count, fmt, a...); }

errno_t fopen_s(FILE** file, const char* path, const char* mode);
errno_t _wfopen_s(FILE** file, const wchar_t* path, const wchar_t* mode);
errno_t localtime_s(struct tm* out, const time_t* t); // MSVC argument order (C11's localtime_s is reversed)
// "MM/DD/YY" and "HH:MM:SS" of the local time; the buffer must hold 9 characters
errno_t _wstrdate_s(wchar_t* dst, size_t size);
errno_t _wstrtime_s(wchar_t* dst, size_t size);
template <size_t N> errno_t _wstrdate_s(wchar_t (&dst)[N]) { return _wstrdate_s(dst, N); }
template <size_t N> errno_t _wstrtime_s(wchar_t (&dst)[N]) { return _wstrtime_s(dst, N); }

errno_t _itow_s(int value, wchar_t* dst, size_t size, int radix);
errno_t _i64tow_s(long long value, wchar_t* dst, size_t size, int radix);
errno_t _itoa_s(int value, char* dst, size_t size, int radix);
template <size_t N> errno_t _itow_s(int v, wchar_t (&dst)[N], int radix) { return _itow_s(v, dst, N, radix); }
template <size_t N> errno_t _itoa_s(int v, char (&dst)[N], int radix) { return _itoa_s(v, dst, N, radix); }

int _mkdir(const char* path);
int _wmkdir(const wchar_t* path);

#define _countof(a) (sizeof(a) / sizeof((a)[0]))

// Code pages: narrow strings are UTF-8 on every platform, so CP_ACP behaves as CP_UTF8 (see KncUtil.h).
#define CP_ACP  0
#define CP_UTF8 65001
#define MB_PRECOMPOSED 0x00000001   // accepted and ignored
// Win32 semantics: cchSrc/cbSrc == -1 means NUL-terminated (the NUL is converted and counted);
// a zero destination size returns the required size.
int WideCharToMultiByte(UINT codePage, DWORD flags, const wchar_t* src, int cchSrc, char* dst, int cbDst,
                        const char* defaultChar, BOOL* usedDefault);
int MultiByteToWideChar(UINT codePage, DWORD flags, const char* src, int cbSrc, wchar_t* dst, int cchDst);

FILE* _wfopen(const wchar_t* path, const wchar_t* mode);

// <tchar.h> (Unicode build: TCHAR is wchar_t)
typedef wchar_t _TCHAR;
#define _tcslen   wcslen
#define _tcscpy   wcscpy
#define _tcsncpy  wcsncpy
#define _tcscat   wcscat
#define _tcscmp   wcscmp
#define _tcsncmp  wcsncmp
#define _tcsicmp  _wcsicmp
#define _tcschr   wcschr
#define _tcsrchr  wcsrchr
#define _tcsstr   wcsstr
#define _tcstok   wcstok
#define _tcstol   wcstol
#define _tcstoul  wcstoul
#define _tcstod   wcstod
#define _ttoi     _wtoi
#define _ttol     _wtol
#define _tfopen   _wfopen
#define _stprintf_s swprintf_s
#define _tcscpy_s wcscpy_s
#define _tcscat_s wcscat_s

#endif // !_WIN32

