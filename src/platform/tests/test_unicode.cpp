#include "x2/platform/unicode.h"

#include <doctest/doctest.h>

using namespace x2::platform;

TEST_CASE("UTF-8 round trip, including Hangul and non-BMP")
{
    const std::wstring wide = L"abc 한글 \U0001F600";
    const std::string utf8 = WideToUtf8(wide);
    CHECK(utf8 == "abc \xED\x95\x9C\xEA\xB8\x80 \xF0\x9F\x98\x80");
    CHECK(Utf8ToWide(utf8) == wide);
}

TEST_CASE("invalid UTF-8 becomes U+FFFD")
{
    CHECK(Utf8ToWide("a\xFF" "b") == L"a�b");
    CHECK(Utf8ToWide("\xE3\x81") == L"��"); // truncated sequence
}

TEST_CASE("UTF-16 is the wire format")
{
    const std::wstring wide = L"한\U0001F600";
    const std::u16string u16 = WideToUtf16(wide);
    REQUIRE(u16.size() == 3); // one BMP unit + surrogate pair
    CHECK(u16[0] == 0xD55C);
    CHECK(u16[1] == 0xD83D);
    CHECK(u16[2] == 0xDE00);
    CHECK(Utf16ToWide(u16) == wide);
}

#if !defined(_WIN32) || (defined(__USE_MINGW_ANSI_STDIO) && __USE_MINGW_ANSI_STDIO)
TEST_CASE("MSVC wide format semantics are translated")
{
    CHECK(TranslateWideFormat(L"%s=%d") == L"%ls=%d");
    CHECK(TranslateWideFormat(L"%S") == L"%s");
    CHECK(TranslateWideFormat(L"%-10s|%c") == L"%-10ls|%lc");
    CHECK(TranslateWideFormat(L"%ls %hs") == L"%ls %hs");
    CHECK(TranslateWideFormat(L"%I64d %I64u") == L"%lld %llu");
    CHECK(TranslateWideFormat(L"100%%") == L"100%%");
}
#endif
