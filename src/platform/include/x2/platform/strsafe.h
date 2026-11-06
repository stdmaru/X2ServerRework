// strsafe.h's counted string functions (StringCchCopy, StringCchCat, StringCchPrintf) and lstrlen, as the client calls
// them. strsafe.h and <windows.h> on Windows; elsewhere inline. They always null-terminate a non-empty destination and
// return STRSAFE_E_INSUFFICIENT_BUFFER when the result was truncated, as strsafe.h does. The wide printf follows MSVC's
// format semantics (%s is wide, crt.h).
#pragma once

#include "x2/platform/crt.h"

#if defined(_WIN32)

#include <strsafe.h>

#else

#include <cstring>
#include <cwchar>

#define STRSAFE_E_INSUFFICIENT_BUFFER static_cast<HRESULT>(0x8007007AL)
#define STRSAFE_E_INVALID_PARAMETER   static_cast<HRESULT>(0x80070057L)

namespace x2::platform::detail {

template <typename CharT>
HRESULT StringCopy(CharT* dst, size_t cch, const CharT* src, size_t count)
{
    if (dst == nullptr || cch == 0) {
        return STRSAFE_E_INVALID_PARAMETER;
    }
    size_t i = 0;
    for (; i + 1 < cch && i < count && src != nullptr && src[i] != CharT(0); ++i) {
        dst[i] = src[i];
    }
    dst[i] = CharT(0);
    return src != nullptr && i < count && src[i] != CharT(0) ? STRSAFE_E_INSUFFICIENT_BUFFER : S_OK;
}

template <typename CharT>
HRESULT StringCat(CharT* dst, size_t cch, const CharT* src)
{
    if (dst == nullptr || cch == 0) {
        return STRSAFE_E_INVALID_PARAMETER;
    }
    size_t len = 0;
    while (len < cch && dst[len] != CharT(0)) {
        ++len;
    }
    if (len == cch) {
        return STRSAFE_E_INVALID_PARAMETER;
    }
    return StringCopy(dst + len, cch - len, src, static_cast<size_t>(-1));
}

} // namespace x2::platform::detail

inline int lstrlenW(const wchar_t* s) { return s != nullptr ? static_cast<int>(std::wcslen(s)) : 0; }
inline int lstrlenA(const char* s) { return s != nullptr ? static_cast<int>(std::strlen(s)) : 0; }
#define lstrlen lstrlenW

inline HRESULT StringCchCopyW(wchar_t* dst, size_t cch, const wchar_t* src)
{
    return x2::platform::detail::StringCopy(dst, cch, src, static_cast<size_t>(-1));
}
inline HRESULT StringCchCopyA(char* dst, size_t cch, const char* src)
{
    return x2::platform::detail::StringCopy(dst, cch, src, static_cast<size_t>(-1));
}
inline HRESULT StringCchCopyNW(wchar_t* dst, size_t cch, const wchar_t* src, size_t count)
{
    return x2::platform::detail::StringCopy(dst, cch, src, count);
}
inline HRESULT StringCchCatW(wchar_t* dst, size_t cch, const wchar_t* src)
{
    return x2::platform::detail::StringCat(dst, cch, src);
}
inline HRESULT StringCchCatA(char* dst, size_t cch, const char* src)
{
    return x2::platform::detail::StringCat(dst, cch, src);
}

inline HRESULT StringCchVPrintfW(wchar_t* dst, size_t cch, const wchar_t* fmt, va_list args)
{
    if (dst == nullptr || cch == 0) {
        return STRSAFE_E_INVALID_PARAMETER;
    }
    const int n = vswprintf_ms(dst, cch, fmt, args);
    dst[cch - 1] = L'\0';
    return n < 0 || static_cast<size_t>(n) >= cch ? STRSAFE_E_INSUFFICIENT_BUFFER : S_OK;
}
inline HRESULT StringCchPrintfW(wchar_t* dst, size_t cch, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const HRESULT hr = StringCchVPrintfW(dst, cch, fmt, args);
    va_end(args);
    return hr;
}
inline HRESULT StringCchPrintfA(char* dst, size_t cch, const char* fmt, ...)
{
    if (dst == nullptr || cch == 0) {
        return STRSAFE_E_INVALID_PARAMETER;
    }
    va_list args;
    va_start(args, fmt);
    const int n = std::vsnprintf(dst, cch, fmt, args);
    va_end(args);
    return n < 0 || static_cast<size_t>(n) >= cch ? STRSAFE_E_INSUFFICIENT_BUFFER : S_OK;
}

#define StringCchCopy   StringCchCopyW
#define StringCchCopyN  StringCchCopyNW
#define StringCchCat    StringCchCatW
#define StringCchPrintf StringCchPrintfW

#endif // _WIN32
