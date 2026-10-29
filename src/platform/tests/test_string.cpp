#include "x2/platform/string.h"

#include <doctest/doctest.h>

#include <string>

TEST_CASE("CStringW basics")
{
    CStringW s(L"  Hello World  ");
    s.Trim();
    CHECK(s == L"Hello World");
    CHECK(s.GetLength() == 11);
    CHECK(s.Find(L'o') == 4);
    CHECK(s.ReverseFind(L'o') == 7);
    CHECK(s.Mid(6) == L"World");
    CHECK(s.Left(5) == L"Hello");
    CHECK(s.Right(5) == L"World");
    CHECK(CStringW(L"app.exe").SpanExcluding(L".") == L"app");
    CHECK(s.Replace(L"o", L"0") == 2);
    CHECK(s == L"Hell0 W0rld");
    CHECK(CStringW(L"ABC").CompareNoCase(L"abc") == 0);

    const std::wstring w(s); // via const wchar_t*, must not be ambiguous
    CHECK(w == L"Hell0 W0rld");
}

TEST_CASE("CStringW::Format uses MSVC semantics")
{
    CStringW s;
    s.Format(L"%s:%d:%I64d", L"name", 7, 1234567890123LL);
    CHECK(s == L"name:7:1234567890123");
    s.AppendFormat(L"/%S", "narrow");
    CHECK(s == L"name:7:1234567890123/narrow");

    CStringW big;
    big.Format(L"%s", std::wstring(2000, L'x').c_str());
    CHECK(big.GetLength() == 2000);
}

TEST_CASE("CStringA::Format")
{
    CStringA s;
    s.Format("%s-%I64u", "id", 42ULL);
    CHECK(s == "id-42");
}
