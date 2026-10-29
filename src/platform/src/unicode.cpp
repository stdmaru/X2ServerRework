#include "x2/platform/unicode.h"

#include <cwchar>

namespace x2::platform {

namespace {

constexpr char32_t kReplacement = 0xFFFD;

// Decodes the wide string into code points (handles surrogate pairs where wchar_t is 16-bit).
template <typename F>
void ForEachCodePoint(std::wstring_view s, F&& f)
{
    for (size_t i = 0; i < s.size(); ++i) {
        char32_t c = static_cast<char32_t>(s[i]);
        if constexpr (sizeof(wchar_t) == 2) {
            c &= 0xFFFF;
            if (c >= 0xD800 && c <= 0xDBFF && i + 1 < s.size()) {
                const char32_t lo = static_cast<char32_t>(s[i + 1]) & 0xFFFF;
                if (lo >= 0xDC00 && lo <= 0xDFFF) {
                    c = 0x10000 + ((c - 0xD800) << 10) + (lo - 0xDC00);
                    ++i;
                }
            }
        }
        f(c);
    }
}

void AppendWide(std::wstring& out, char32_t c)
{
    if constexpr (sizeof(wchar_t) == 2) {
        if (c >= 0x10000) {
            c -= 0x10000;
            out.push_back(static_cast<wchar_t>(0xD800 + (c >> 10)));
            out.push_back(static_cast<wchar_t>(0xDC00 + (c & 0x3FF)));
            return;
        }
    }
    out.push_back(static_cast<wchar_t>(c));
}

} // namespace

std::string WideToUtf8(std::wstring_view s)
{
    std::string out;
    out.reserve(s.size());
    ForEachCodePoint(s, [&](char32_t c) {
        if ((c >= 0xD800 && c <= 0xDFFF) || c > 0x10FFFF)
            c = kReplacement;
        if (c < 0x80) {
            out.push_back(static_cast<char>(c));
        } else if (c < 0x800) {
            out.push_back(static_cast<char>(0xC0 | (c >> 6)));
            out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else if (c < 0x10000) {
            out.push_back(static_cast<char>(0xE0 | (c >> 12)));
            out.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xF0 | (c >> 18)));
            out.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
    });
    return out;
}

std::wstring Utf8ToWide(std::string_view s)
{
    std::wstring out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size();) {
        const auto b = static_cast<unsigned char>(s[i]);
        int extra = b < 0x80 ? 0 : (b >> 5) == 0x6 ? 1 : (b >> 4) == 0xE ? 2 : (b >> 3) == 0x1E ? 3 : -1;
        if (extra < 0 || i + extra >= s.size()) { // invalid lead byte or truncated sequence
            AppendWide(out, kReplacement);
            ++i;
            continue;
        }
        char32_t c = extra == 0 ? b : b & (0x3F >> extra);
        bool ok = true;
        for (int k = 1; k <= extra; ++k) {
            const auto cb = static_cast<unsigned char>(s[i + k]);
            if ((cb & 0xC0) != 0x80) {
                ok = false;
                break;
            }
            c = (c << 6) | (cb & 0x3F);
        }
        if (!ok) {
            AppendWide(out, kReplacement);
            ++i;
            continue;
        }
        AppendWide(out, c);
        i += extra + 1;
    }
    return out;
}

std::u16string WideToUtf16(std::wstring_view s)
{
    if constexpr (sizeof(wchar_t) == 2) {
        return std::u16string(reinterpret_cast<const char16_t*>(s.data()), s.size());
    } else {
        std::u16string out;
        out.reserve(s.size());
        ForEachCodePoint(s, [&](char32_t c) {
            if (c >= 0x10000 && c <= 0x10FFFF) {
                c -= 0x10000;
                out.push_back(static_cast<char16_t>(0xD800 + (c >> 10)));
                out.push_back(static_cast<char16_t>(0xDC00 + (c & 0x3FF)));
            } else {
                out.push_back(static_cast<char16_t>(c > 0x10FFFF ? kReplacement : c));
            }
        });
        return out;
    }
}

std::wstring Utf16ToWide(std::u16string_view s)
{
    if constexpr (sizeof(wchar_t) == 2) {
        return std::wstring(reinterpret_cast<const wchar_t*>(s.data()), s.size());
    } else {
        std::wstring out;
        out.reserve(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            char32_t c = s[i];
            if (c >= 0xD800 && c <= 0xDBFF && i + 1 < s.size() && s[i + 1] >= 0xDC00 && s[i + 1] <= 0xDFFF) {
                c = 0x10000 + ((c - 0xD800) << 10) + (s[i + 1] - 0xDC00);
                ++i;
            } else if (c >= 0xD800 && c <= 0xDFFF) {
                c = kReplacement;
            }
            out.push_back(static_cast<wchar_t>(c));
        }
        return out;
    }
}

std::wstring TranslateWideFormat(const wchar_t* fmt)
{
    // MSVC's CRT keeps the MSVC meaning. MinGW's libstdc++ turns on __USE_MINGW_ANSI_STDIO, which swaps
    // the printf family for MinGW's own ISO C implementation: that one needs the translation like glibc.
#if defined(_WIN32) && !(defined(__USE_MINGW_ANSI_STDIO) && __USE_MINGW_ANSI_STDIO)
    return fmt ? std::wstring(fmt) : std::wstring();
#else
    std::wstring out;
    if (!fmt)
        return out;
    for (const wchar_t* p = fmt; *p; ++p) {
        out.push_back(*p);
        if (*p != L'%')
            continue;
        ++p;
        if (*p == L'%') {
            out.push_back(*p);
            continue;
        }
        // flags, width, precision
        while (*p && wcschr(L"-+ #0123456789.*", *p))
            out.push_back(*p++);
        bool explicitLength = false;
        if (p[0] == L'I' && p[1] == L'6' && p[2] == L'4') { // %I64d -> %lld
            out += L"ll";
            p += 3;
            explicitLength = true;
        } else if (p[0] == L'I' && p[1] == L'3' && p[2] == L'2') {
            p += 3;
        } else {
            while (*p && wcschr(L"hlLqjzt", *p)) {
                if (*p == L'h' || *p == L'l')
                    explicitLength = true;
                out.push_back(*p++);
            }
        }
        if (!*p)
            break;
        if ((*p == L's' || *p == L'c') && !explicitLength) {
            out.push_back(L'l'); // MSVC: %s in a wide format is a wide string
            out.push_back(*p);
        } else if (*p == L'S' || *p == L'C') {
            out.push_back(static_cast<wchar_t>(*p - L'A' + L'a')); // MSVC: %S is a narrow string
        } else {
            out.push_back(*p);
        }
    }
    return out;
#endif
}

} // namespace x2::platform
