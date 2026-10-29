#include "x2/platform/time.h"

#include <doctest/doctest.h>

TEST_CASE("CTime fields and Format")
{
    const CTime t(2013, 2, 19, 13, 5, 9);
    CHECK(t.GetYear() == 2013);
    CHECK(t.GetMonth() == 2);
    CHECK(t.GetDay() == 19);
    CHECK(t.GetHour() == 13);
    CHECK(t.GetMinute() == 5);
    CHECK(t.GetSecond() == 9);
    CHECK(t.GetDayOfWeek() == 3); // Tuesday, 1 = Sunday
    CHECK(t.Format(L"%Y-%m-%d %H:%M:%S") == L"2013-02-19 13:05:09");
}

TEST_CASE("CTime arithmetic with CTimeSpan")
{
    const CTime a(2013, 2, 28, 23, 0, 0);
    const CTime b = a + CTimeSpan(1, 2, 0, 0);
    CHECK(b.Format(L"%Y-%m-%d %H:%M") == L"2013-03-02 01:00");
    const CTimeSpan d = b - a;
    CHECK(d.GetDays() == 1);
    CHECK(d.GetHours() == 2);
    CHECK(d.GetTotalMinutes() == 26 * 60);
    CHECK(d.Format(L"%D %H:%M:%S") == L"1 02:00:00");
    CHECK(a < b);
}

TEST_CASE("tick counters advance")
{
    const DWORD t0 = GetTickCount();
    LARGE_INTEGER c0, c1, f;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c0);
    QueryPerformanceCounter(&c1);
    CHECK(f.QuadPart > 0);
    CHECK(c1.QuadPart >= c0.QuadPart);
    CHECK(GetTickCount() - t0 < 1000u);
}
