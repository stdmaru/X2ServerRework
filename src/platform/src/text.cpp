#include "x2/platform/unicode.h"

#if defined(_WIN32)
#include "x2/platform/types.h" // <windows.h>
#else
#include <cerrno>
#include <iconv.h>
#endif

namespace x2::platform {

bool IsValidUtf8(std::string_view s)
{
    for (size_t i = 0; i < s.size();) {
        const auto b = static_cast<unsigned char>(s[i]);
        const int extra = b < 0x80 ? 0 : (b >> 5) == 0x6 ? 1 : (b >> 4) == 0xE ? 2 : (b >> 3) == 0x1E ? 3 : -1;
        if (extra < 0 || i + extra >= s.size())
            return false;
        for (int k = 1; k <= extra; ++k) {
            if ((static_cast<unsigned char>(s[i + k]) & 0xC0) != 0x80)
                return false;
        }
        i += static_cast<size_t>(extra) + 1;
    }
    return true;
}

std::wstring Cp949ToWide(std::string_view s)
{
    if (s.empty())
        return {};
#if defined(_WIN32)
    const int n = ::MultiByteToWideChar(949, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    std::wstring out(static_cast<size_t>(n), L'\0');
    ::MultiByteToWideChar(949, 0, s.data(), static_cast<int>(s.size()), out.data(), n);
    return out;
#else
    iconv_t cd = ::iconv_open("WCHAR_T", "CP949");
    if (cd == reinterpret_cast<iconv_t>(-1))
        return Utf8ToWide(s); // no CP949 converter installed: keep the ASCII part readable
    std::wstring out;
    char* in = const_cast<char*>(s.data());
    size_t inLeft = s.size();
    wchar_t buf[256];
    while (inLeft > 0) {
        char* outPtr = reinterpret_cast<char*>(buf);
        size_t outLeft = sizeof(buf);
        const size_t r = ::iconv(cd, &in, &inLeft, &outPtr, &outLeft);
        out.append(buf, (sizeof(buf) - outLeft) / sizeof(wchar_t));
        if (r == static_cast<size_t>(-1) && errno != E2BIG) { // invalid or truncated sequence: skip one byte
            out.push_back(L'\xFFFD');
            ++in;
            --inLeft;
        }
    }
    ::iconv_close(cd);
    return out;
#endif
}

std::wstring DecodeTextFile(std::string_view bytes)
{
    if (bytes.size() >= 2 && static_cast<unsigned char>(bytes[0]) == 0xFF && static_cast<unsigned char>(bytes[1]) == 0xFE) {
        std::u16string u16((bytes.size() - 2) / 2, u'\0');
        for (size_t i = 0; i < u16.size(); ++i)
            u16[i] = static_cast<char16_t>(static_cast<unsigned char>(bytes[2 + i * 2]) |
                                           (static_cast<unsigned char>(bytes[3 + i * 2]) << 8));
        return Utf16ToWide(u16);
    }
    if (bytes.size() >= 3 && bytes.compare(0, 3, "\xEF\xBB\xBF") == 0)
        return Utf8ToWide(bytes.substr(3));
    if (IsValidUtf8(bytes))
        return Utf8ToWide(bytes);
    return Cp949ToWide(bytes);
}

} // namespace x2::platform
