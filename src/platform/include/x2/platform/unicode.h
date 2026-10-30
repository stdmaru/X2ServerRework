// Conversions between wide strings (UTF-16 on Windows, UTF-32 elsewhere), UTF-8 and UTF-16.
#pragma once

#include <locale>
#include <string>
#include <string_view>

namespace x2::platform {

std::string    WideToUtf8(std::wstring_view s);
std::wstring   Utf8ToWide(std::string_view s);

// UTF-16 is the wire format of every wide string (the original was built on Windows).
std::u16string WideToUtf16(std::wstring_view s);
std::wstring   Utf16ToWide(std::u16string_view s);

// Text files written by the original tools: Korean Windows code page 949 (CP949/UHC), the ANSI code page
// of the original servers. Invalid sequences become U+FFFD.
std::wstring   Cp949ToWide(std::string_view s);
bool           IsValidUtf8(std::string_view s);
// Contents of a text file whose encoding is not known: UTF-16LE or UTF-8 with BOM, then UTF-8 if it is
// valid, and CP949 otherwise (the original's config .ini files are CP949; its Lua scripts are UTF-8).
std::wstring   DecodeTextFile(std::string_view bytes);

// Translates a printf format written for MSVC wide functions to the C library in use:
// in MSVC `%s`/`%c` in a wide format mean wide arguments and `%S` narrow ones (the opposite of
// ISO C), and `%I64d` is `%lld`. Identity with the MSVC CRT.
std::wstring   TranslateWideFormat(const wchar_t* fmt);

// Locale for wide file streams (std::wfstream) that read and write UTF-8, in place of the MSVC-only
// std::locale("korean"), which wrote CP949.
std::locale    Utf8FileLocale();

} // namespace x2::platform
