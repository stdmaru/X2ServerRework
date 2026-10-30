#include "x2/platform/odbc.h"

#include <doctest/doctest.h>

using namespace x2::platform;

TEST_CASE("SQLWCHAR round trip: ASCII, Hangul, symbols, non-BMP and empty")
{
    for (const std::wstring wide : {L"SELECT 1", L"엘소드", L"♪ 　", L"\U0001F600x", L""}) {
        SqlWString sql = ToSqlWString(wide);
        CHECK(FromSqlWString(sql) == wide);
        CHECK(FromSqlWString(SqlWPtr(sql)) == wide);          // zero-terminated
    }
}

TEST_CASE("SQLWCHAR is UTF-16 on every platform")
{
    const SqlWString sql = ToSqlWString(L"a\U0001F600");
    REQUIRE(sql.size() == 3);                                   // a surrogate pair
    CHECK(sql[1] == 0xD83D);
    CHECK(sql[2] == 0xDE00);
    CHECK(sizeof(SQLWCHAR) == 2);
}
