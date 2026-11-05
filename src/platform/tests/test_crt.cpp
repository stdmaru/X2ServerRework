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

#if !defined(_WIN32)
    // Our implementation reports the overflow. The real CRT calls the invalid parameter handler instead,
    // which by default ends the process (as in the original), so this only checks ours.
    wchar_t tiny[4];
    CHECK(wcscpy_s(tiny, L"too long") != 0);
    CHECK(tiny[0] == 0);
#endif

    CHECK(_wtoi(L"  -42") == -42);
    CHECK(_wtoi64(L"9000000000") == 9000000000LL);
    CHECK(_wcsicmp(L"AbC", L"abc") == 0);
    CHECK(_stricmp("AbC", "abd") < 0);

    CHECK(_itow_s(-255, buf, 10) == 0);
    CHECK(std::wstring(buf) == L"-255");
    CHECK(_itow_s(255, buf, 16) == 0);
    CHECK(std::wstring(buf) == L"ff");
}

TEST_CASE("MSVC CRT: array overloads, dates and fopen_s")
{
    wchar_t buf[16];
    CHECK(_snwprintf_s(buf, 16, L"%2.1f|%s", 2.5, L"x") > 0);
    CHECK(std::wstring(buf) == L"2.5|x");
    CHECK(wcsncpy_s(buf, L"abcdef", 3) == 0);
    CHECK(std::wstring(buf) == L"abc");

    // _TRUNCATE copies what fits; any other count that does not fit fails (ours empties the buffer)
    char small[4];
    CHECK(strncpy_s(small, sizeof(small), "abcdef", _TRUNCATE) == STRUNCATE);
    CHECK(std::string(small) == "abc");
    CHECK(strncpy_s(small, sizeof(small), "ab", _TRUNCATE) == 0);
    CHECK(std::string(small) == "ab");
#if !defined(_WIN32)  // the real CRT calls the invalid parameter handler instead (see above)
    CHECK(strncpy_s(small, sizeof(small), "abcdef", 5) != 0);
    CHECK(small[0] == '\0');
#endif
    CHECK(wcsncpy_s(buf, 3, L"abcdef", _TRUNCATE) == STRUNCATE);
    CHECK(std::wstring(buf) == L"ab");

    wchar_t date[9], time[9];
    CHECK(_wstrdate_s(date) == 0);
    CHECK(_wstrtime_s(time) == 0);
    CHECK(std::wstring(date).size() == 8);
    CHECK(date[2] == L'/');
    CHECK(time[2] == L':');

    time_t t0 = 0;
    struct tm t {};
    CHECK(localtime_s(&t, &t0) == 0);

    FILE* f = reinterpret_cast<FILE*>(1);
    CHECK(fopen_s(&f, "this/does/not/exist", "rb") != 0);
    CHECK(f == nullptr);
}
