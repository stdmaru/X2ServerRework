#if !defined(_WIN32)

#include "x2/platform/crt.h"
#include "x2/platform/unicode.h"

#include <cerrno>
#include <string>
#include <sys/stat.h>

namespace {

template <typename C>
size_t Len(const C* s)
{
    size_t n = 0;
    while (s[n]) ++n;
    return n;
}

template <typename C>
errno_t Copy(C* dst, size_t size, const C* src, size_t count, bool truncate = false)
{
    if (!dst || size == 0)
        return EINVAL;
    if (!src) {
        dst[0] = 0;
        return EINVAL;
    }
    size_t n = 0;
    while (n < count && src[n]) ++n;
    if (n >= size) {
        if (truncate) {  // _TRUNCATE: as much as fits
            for (size_t i = 0; i + 1 < size; ++i) dst[i] = src[i];
            dst[size - 1] = 0;
            return STRUNCATE;
        }
        dst[0] = 0;
        return ERANGE;
    }
    for (size_t i = 0; i < n; ++i) dst[i] = src[i];
    dst[n] = 0;
    return 0;
}

template <typename C>
errno_t Cat(C* dst, size_t size, const C* src)
{
    if (!dst || size == 0)
        return EINVAL;
    const size_t used = Len(dst);
    if (used >= size)
        return EINVAL;
    return Copy(dst + used, size - used, src, static_cast<size_t>(-1));
}

template <typename C>
errno_t ToString(long long value, C* dst, size_t size, int radix)
{
    if (!dst || size == 0 || radix < 2 || radix > 36)
        return EINVAL;
    C tmp[72];
    size_t n = 0;
    const bool neg = radix == 10 && value < 0;
    unsigned long long v = neg ? 0ull - static_cast<unsigned long long>(value) : static_cast<unsigned long long>(value);
    do {
        const int d = static_cast<int>(v % static_cast<unsigned>(radix));
        tmp[n++] = static_cast<C>(d < 10 ? '0' + d : 'a' + d - 10);
        v /= static_cast<unsigned>(radix);
    } while (v);
    if (neg) tmp[n++] = '-';
    if (n + 1 > size) {
        dst[0] = 0;
        return ERANGE;
    }
    for (size_t i = 0; i < n; ++i) dst[i] = tmp[n - 1 - i];
    dst[n] = 0;
    return 0;
}

} // namespace

errno_t wcscpy_s(wchar_t* d, size_t s, const wchar_t* src) { return Copy(d, s, src, static_cast<size_t>(-1)); }
errno_t wcsncpy_s(wchar_t* d, size_t s, const wchar_t* src, size_t c) { return Copy(d, s, src, c, c == _TRUNCATE); }
errno_t wcscat_s(wchar_t* d, size_t s, const wchar_t* src) { return Cat(d, s, src); }
errno_t strcpy_s(char* d, size_t s, const char* src) { return Copy(d, s, src, static_cast<size_t>(-1)); }
errno_t strncpy_s(char* d, size_t s, const char* src, size_t c) { return Copy(d, s, src, c, c == _TRUNCATE); }
errno_t strcat_s(char* d, size_t s, const char* src) { return Cat(d, s, src); }

int vswprintf_ms(wchar_t* dst, size_t size, const wchar_t* fmt, va_list args)
{
    const std::wstring f = x2::platform::TranslateWideFormat(fmt);
    const int n = std::vswprintf(dst, size, f.c_str(), args);
    if (n < 0 && dst && size)
        dst[0] = 0;
    return n;
}

int swprintf_s(wchar_t* dst, size_t size, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int n = vswprintf_ms(dst, size, fmt, args);
    va_end(args);
    return n;
}

int _snwprintf_s(wchar_t* dst, size_t size, size_t count, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const size_t limit = count < size ? count + 1 : size;
    const int n = vswprintf_ms(dst, limit, fmt, args);
    va_end(args);
    return n;
}

int _snprintf_s(char* dst, size_t size, size_t count, const char* fmt, ...)
{
    std::string f = fmt ? fmt : "";
    for (size_t pos; (pos = f.find("I64")) != std::string::npos;)
        f.replace(pos, 3, "ll");
    va_list args;
    va_start(args, fmt);
    const size_t limit = count < size ? count + 1 : size;
    const int n = std::vsnprintf(dst, limit, f.c_str(), args);
    va_end(args);
    return n;
}

int sprintf_s(char* dst, size_t size, const char* fmt, ...)
{
    std::string f = fmt ? fmt : "";
    for (size_t pos; (pos = f.find("I64")) != std::string::npos;)
        f.replace(pos, 3, "ll");
    va_list args;
    va_start(args, fmt);
    const int n = std::vsnprintf(dst, size, f.c_str(), args);
    va_end(args);
    return n;
}

errno_t _itow_s(int v, wchar_t* d, size_t s, int r) { return ToString(v, d, s, r); }
errno_t _i64tow_s(long long v, wchar_t* d, size_t s, int r) { return ToString(v, d, s, r); }
errno_t _itoa_s(int v, char* d, size_t s, int r) { return ToString(v, d, s, r); }

int WideCharToMultiByte(UINT, DWORD, const wchar_t* src, int cchSrc, char* dst, int cbDst, const char*, BOOL* usedDefault)
{
    if (usedDefault)
        *usedDefault = FALSE;
    if (!src)
        return 0;
    const size_t len = cchSrc < 0 ? std::wcslen(src) + 1 : static_cast<size_t>(cchSrc);
    const std::string out = x2::platform::WideToUtf8(std::wstring_view(src, len));
    if (cbDst == 0)
        return static_cast<int>(out.size());
    if (!dst || out.size() > static_cast<size_t>(cbDst))
        return 0;
    std::memcpy(dst, out.data(), out.size());
    return static_cast<int>(out.size());
}

int MultiByteToWideChar(UINT, DWORD, const char* src, int cbSrc, wchar_t* dst, int cchDst)
{
    if (!src)
        return 0;
    const size_t len = cbSrc < 0 ? std::strlen(src) + 1 : static_cast<size_t>(cbSrc);
    const std::wstring out = x2::platform::Utf8ToWide(std::string_view(src, len));
    if (cchDst == 0)
        return static_cast<int>(out.size());
    if (!dst || out.size() > static_cast<size_t>(cchDst))
        return 0;
    std::wmemcpy(dst, out.data(), out.size());
    return static_cast<int>(out.size());
}

FILE* _wfopen(const wchar_t* path, const wchar_t* mode)
{
    if (!path || !mode)
        return nullptr;
    return std::fopen(x2::platform::WideToUtf8(path).c_str(), x2::platform::WideToUtf8(mode).c_str());
}

errno_t fopen_s(FILE** file, const char* path, const char* mode)
{
    if (!file)
        return EINVAL;
    *file = (path && mode) ? std::fopen(path, mode) : nullptr;
    return *file ? 0 : (errno ? errno : EINVAL);
}

errno_t _wfopen_s(FILE** file, const wchar_t* path, const wchar_t* mode)
{
    if (!file)
        return EINVAL;
    *file = _wfopen(path, mode);
    return *file ? 0 : (errno ? errno : EINVAL);
}

errno_t localtime_s(struct tm* out, const time_t* t)
{
    if (!out || !t)
        return EINVAL;
    return ::localtime_r(t, out) ? 0 : EINVAL;
}

namespace {

errno_t FormatNow(wchar_t* dst, size_t size, const wchar_t* fmt)
{
    if (!dst || size < 9)
        return dst && size ? (dst[0] = 0, ERANGE) : EINVAL;
    const time_t now = std::time(nullptr);
    struct tm t {};
    ::localtime_r(&now, &t);
    return std::wcsftime(dst, size, fmt, &t) ? 0 : ERANGE;
}

} // namespace

errno_t _wstrdate_s(wchar_t* dst, size_t size) { return FormatNow(dst, size, L"%m/%d/%y"); }
errno_t _wstrtime_s(wchar_t* dst, size_t size) { return FormatNow(dst, size, L"%H:%M:%S"); }

int _mkdir(const char* path) { return ::mkdir(path, 0755); }
int _wmkdir(const wchar_t* path) { return ::mkdir(x2::platform::WideToUtf8(path).c_str(), 0755); }

#endif // !_WIN32
