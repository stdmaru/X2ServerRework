#include "KncUtil.h"
#include "KNCSingleton.h"
#include "ToString.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <sstream>

TEST_CASE("GetMD5")
{
    CHECK(KncUtil::GetMD5("abc") == "900150983cd24fb0d6963f7d28e17f72");
    CHECK(KncUtil::GetMD5("") == "d41d8cd98f00b204e9800998ecf8427e");
}

TEST_CASE("narrow strings are UTF-8")
{
    CHECK(KncUtil::toNarrowString(L"한글") == "\xED\x95\x9C\xEA\xB8\x80");
    CHECK(KncUtil::toWideString("\xED\x95\x9C") == L"한");
    CHECK(KncUtil::toWideString("abcdef", 3) == L"abc");
    CHECK(KncUtil::toWideString(nullptr).empty());
    CHECK(KncUtil::toNarrowString(L"x", 0).empty());
}

TEST_CASE("hstol")
{
    char a[] = "0x1F";
    char b[] = "ff";
    char c[] = "0";
    CHECK(KncUtil::hstol(a) == 31);
    CHECK(KncUtil::hstol(b) == 255);
    CHECK(KncUtil::hstol(c) == 0);
}

TEST_CASE("ConvertStringToCTime")
{
    CTime t;
    REQUIRE(KncUtil::ConvertStringToCTime(L"2008-01-22 11:16:00", t));
    CHECK(t.Format(L"%Y-%m-%d %H:%M:%S") == L"2008-01-22 11:16:00");
    CHECK_FALSE(KncUtil::ConvertStringToCTime(L"1900-01-01 00:00:00", t));
    CHECK_FALSE(KncUtil::ConvertStringToCTime(L"2008-13-22 11:16:00", t));
    CHECK_FALSE(KncUtil::ConvertStringToCTime(L"short", t));
}

TEST_CASE("EraseFrontWord keeps the original quirks")
{
    CHECK(KncUtil::EraseFrontWord(L"ch;name", L';') == L"name");
    CHECK(KncUtil::EraseFrontWord(L"noseparator", L';') == L"noseparator");
    CHECK(KncUtil::EraseFrontWord(L";lead", L';') == L";lead");
}

TEST_CASE("shuffles are permutations")
{
    auto v = KncUtil::GetRandomNumberList(20);
    std::sort(v.begin(), v.end());
    for (int i = 0; i < 20; ++i)
        CHECK(v[i] == i);
}

TEST_CASE("GetAppName / GetTimeStampByNameA")
{
    std::wstring name;
    KncUtil::GetAppName(name);
    CHECK(name == L"kncsdk_tests");
    std::string ts;
    CHECK(KncUtil::GetTimeStampByNameA(ts));
    CHECK(ts.size() == 19);
}

namespace {

class KSingletonSample
{
    DeclareSingleton( KSingletonSample );
protected:
    KSingletonSample() = default;
public:
    int value = 42;
};
ImplementSingleton( KSingletonSample );
DefSingletonInline( KSingletonSample );

struct KToStringSample
{
    int m_iValue = 7;
    DeclToStringW;
};
ImplToStringW( KToStringSample )
{
    return stm_ << TOSTRINGW( m_iValue );
}

} // namespace

TEST_CASE("singleton and ToString macros")
{
    CHECK(SiKSingletonSample()->value == 42);
    KSingletonSample::ReleaseInstance();

    std::wostringstream os;
    KToStringSample().ToString(os);
    CHECK(os.str() == L"    m_iValue : 7\n");
}
