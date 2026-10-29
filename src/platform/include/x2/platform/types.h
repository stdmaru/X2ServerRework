// Win32 base types used all over the original code (DWORD, BYTE, TCHAR...).
//
// On Windows they come from <windows.h> (this layer is the only place that includes it).
// Elsewhere they are defined with the sizes they have on Windows, which is also what goes on the wire:
// DWORD/LONG/ULONG are 32-bit even though `long` is 64-bit on Linux x64.
#pragma once

#include <cfloat>
#include <climits>
#include <cstddef>
#include <cstdint>

#if defined(_WIN32)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
// <windows.h> defines GetCurrentTime() as a macro, which breaks CTime::GetCurrentTime().
#undef GetCurrentTime

#else

typedef std::uint8_t   BYTE;
typedef std::uint8_t   UCHAR;
typedef std::uint16_t  WORD;
typedef std::uint16_t  USHORT;
typedef std::uint32_t  DWORD;
typedef std::uint32_t  UINT;
typedef std::uint32_t  ULONG;
typedef std::int32_t   LONG;
typedef std::int32_t   INT;
typedef std::int32_t   BOOL;
typedef long long          INT64, LONGLONG;
typedef unsigned long long UINT64, ULONGLONG, DWORD64;
typedef float          FLOAT;
typedef char           CHAR;
typedef wchar_t        WCHAR;
typedef std::intptr_t  INT_PTR, LONG_PTR;
typedef std::uintptr_t UINT_PTR, ULONG_PTR, DWORD_PTR;

typedef void*          LPVOID;
typedef const void*    LPCVOID;
typedef char*          LPSTR;
typedef const char*    LPCSTR;
typedef wchar_t*       LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef DWORD*         LPDWORD;
typedef BYTE*          LPBYTE;

#define __int64 long long
#define __int32 int
#define __int16 short
#define __int8  char

// basetsd.h fixed-width names
typedef std::int8_t    INT8;
typedef std::int16_t   INT16;
typedef std::int32_t   INT32, LONG32;
typedef std::uint8_t   UINT8;
typedef std::uint16_t  UINT16;
typedef std::uint32_t  UINT32, ULONG32, DWORD32;
typedef long long      __time64_t;
typedef std::int32_t   __time32_t;
typedef unsigned char  byte; // rpcndr.h (also typedef'd by KncSecurity)

typedef DWORD*         PDWORD;
typedef BYTE*          PBYTE;
typedef WORD*          PWORD, *LPWORD;
typedef LONG*          PLONG, *LPLONG;
typedef UINT*          PUINT;
typedef BOOL*          PBOOL, *LPBOOL;

// COM result codes
typedef LONG HRESULT;
#define S_OK          static_cast<HRESULT>(0x00000000L)
#define S_FALSE       static_cast<HRESULT>(0x00000001L)
#define E_FAIL        static_cast<HRESULT>(0x80004005L)
#define E_INVALIDARG  static_cast<HRESULT>(0x80070057L)
#define E_OUTOFMEMORY static_cast<HRESULT>(0x8007000EL)
#define E_NOTIMPL     static_cast<HRESULT>(0x80004001L)
#define SUCCEEDED(hr) (static_cast<HRESULT>(hr) >= 0)
#define FAILED(hr)    (static_cast<HRESULT>(hr) < 0)

// window handles appear in signatures of server code (no windows exist here)
struct HWND__;
typedef HWND__* HWND;
struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE, *HMODULE;
#define HWND_DESKTOP (static_cast<HWND>(nullptr))
inline BOOL IsWindow(HWND) { return 0; } // no windows exist here

#define __forceinline inline __attribute__((always_inline))

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#define MAX_PATH  260
#define _MAX_PATH 260
#define _MAX_FNAME 256
#define INFINITE  0xFFFFFFFFu

#define WINAPI
#define CALLBACK
#define __stdcall
#define __cdecl

#endif // _WIN32

// The original is always a Unicode build: TCHAR is wchar_t.
#if !defined(_WIN32)
typedef wchar_t        TCHAR;
typedef wchar_t*       LPTSTR;
typedef const wchar_t* LPCTSTR;
#endif

#ifndef _T
#define X2_WIDEN_(x) L##x
#define _T(x) X2_WIDEN_(x)
#endif

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

static_assert(sizeof(DWORD) == 4 && sizeof(WORD) == 2 && sizeof(BYTE) == 1, "Win32 type sizes");
static_assert(sizeof(LONG) == 4 && sizeof(ULONG) == 4 && sizeof(UINT) == 4, "Win32 type sizes");
static_assert(sizeof(INT64) == 8 && sizeof(UINT64) == 8, "Win32 type sizes");
