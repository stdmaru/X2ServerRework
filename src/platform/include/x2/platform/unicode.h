// Conversions between wide strings (UTF-16 on Windows, UTF-32 elsewhere), UTF-8 and UTF-16.
#pragma once

#include <string>
#include <string_view>

namespace x2::platform {

std::string    WideToUtf8(std::wstring_view s);
std::wstring   Utf8ToWide(std::string_view s);

// UTF-16 is the wire format of every wide string (the original was built on Windows).
std::u16string WideToUtf16(std::wstring_view s);
std::wstring   Utf16ToWide(std::u16string_view s);

// Translates a printf format written for MSVC wide functions to the C library in use:
// in MSVC `%s`/`%c` in a wide format mean wide arguments and `%S` narrow ones (the opposite of
// ISO C), and `%I64d` is `%lld`. Identity on Windows.
std::wstring   TranslateWideFormat(const wchar_t* fmt);

} // namespace x2::platform
