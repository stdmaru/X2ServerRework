#include "x2/platform/crt.h"

#include <doctest/doctest.h>

#include <string>

TEST_CASE("MSVC CRT subset")
{
    wchar_t buf[32];
    CHECK(swprintf_s(buf, L"%s-%d", L"ab", 5) == 4);
    CHECK(std::wstring(buf) == L"ab-5");

    CHECK(wcscpy_s(buf, L"hello") == 0);
    CHECK(wcscat_s(buf, L" world") == 0);
    CHECK(std::wstring(buf) == L"hello world");

    wchar_t tiny[4];
    CHECK(wcscpy_s(tiny, L"too long") != 0);
    CHECK(tiny[0] == 0);

    CHECK(_wtoi(L"  -42") == -42);
    CHECK(_wtoi64(L"9000000000") == 9000000000LL);
    CHECK(_wcsicmp(L"AbC", L"abc") == 0);
    CHECK(_stricmp("AbC", "abd") < 0);

    CHECK(_itow_s(-255, buf, 10) == 0);
    CHECK(std::wstring(buf) == L"-255");
    CHECK(_itow_s(255, buf, 16) == 0);
    CHECK(std::wstring(buf) == L"ff");
}
