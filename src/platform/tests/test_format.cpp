#include "x2/platform/format.h"
#include "x2/platform/timer.h"

#include <doctest/doctest.h>

#include <string>

TEST_CASE("printf-like directives, as Boost.Format")
{
    CHECK(x2::str(x2::wformat(L"%d|%s|%c") % 42 % std::wstring(L"abc") % L'z') == L"42|abc|z");
    CHECK(x2::str(x2::wformat(L"%2.2f ~ %2.2f (%2.2f%%)") % 1.5 % 2.0 % 50.0) == L"1.50 ~ 2.00 (50.00%)");
    CHECK(x2::str(x2::format("%05d|%-4d|%4d|%x|%X|%+d") % 42 % 7 % 7 % 255 % 255 % 3) == "00042|7   |   7|ff|FF|+3");
    CHECK(x2::str(x2::format("%d") % "text") == "text"); // type-safe: never reinterprets the argument
    CHECK(x2::str(x2::format("%I64d") % 9000000000LL) == "9000000000");
    CHECK(x2::str(x2::format("%|5d|") % 1) == "    1");
}

TEST_CASE("positional directives")
{
    CHECK(x2::str(x2::wformat(L"%1%_%2%_%1%") % 7 % L"b") == L"7_b_7");
    CHECK(x2::str(x2::format("%2% %1%") % "world" % "hello") == "hello world");
}

TEST_CASE("query building like DBLayer")
{
    // DBLayer: boost::str(boost::wformat( query L" " argtype ) % arg)
    const std::wstring q = x2::str(x2::wformat(L"exec dbo.mup_get_user" L" " L"%d, N'%s'") % 1234 % L"name");
    CHECK(q == L"exec dbo.mup_get_user 1234, N'name'");
}

TEST_CASE("stream output and lenient argument count")
{
    std::wostringstream os;
    os << (x2::wformat(L"[%s]") % L"section");
    CHECK(os.str() == L"[section]");
    CHECK(x2::str(x2::format("%d-%d") % 1) == "1-");
    CHECK(x2::str(x2::format("%d") % 1 % 2) == "1");
}

TEST_CASE("Timer")
{
    x2::Timer t;
    CHECK(t.elapsed() >= 0.0);
    t.restart();
    CHECK(t.elapsed() < 1.0);
}
