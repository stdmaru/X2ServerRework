// Tick counters (GetTickCount/timeGetTime) and a portable replacement for ATL's CTime/CTimeSpan,
// which the server uses in ~200 files. Same API subset and semantics (local time, seconds precision).
#pragma once

#include "x2/platform/string.h"
#include "x2/platform/types.h"

#include <ctime>

#if !defined(_WIN32)

DWORD     GetTickCount();
ULONGLONG GetTickCount64();
DWORD     timeGetTime();

union LARGE_INTEGER
{
    struct { DWORD LowPart; LONG HighPart; } u;
    struct { DWORD LowPart; LONG HighPart; };
    LONGLONG QuadPart;
};
BOOL QueryPerformanceCounter(LARGE_INTEGER* counter);
BOOL QueryPerformanceFrequency(LARGE_INTEGER* frequency);

struct SYSTEMTIME
{
    WORD wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds;
};
void GetLocalTime(SYSTEMTIME* st);

#else
#include <mmsystem.h> // timeGetTime (winmm is linked by x2::platform)
#endif

class CTimeSpan
{
public:
    CTimeSpan() = default;
    CTimeSpan(std::int64_t seconds) : m_span(seconds) {}
    CTimeSpan(LONG days, int hours, int minutes, int seconds)
        : m_span(((static_cast<std::int64_t>(days) * 24 + hours) * 60 + minutes) * 60 + seconds) {}

    std::int64_t GetTimeSpan() const { return m_span; }
    std::int64_t GetDays() const { return m_span / (24 * 3600); }
    std::int64_t GetTotalHours() const { return m_span / 3600; }
    std::int64_t GetTotalMinutes() const { return m_span / 60; }
    std::int64_t GetTotalSeconds() const { return m_span; }
    LONG GetHours() const { return static_cast<LONG>(GetTotalHours() - GetDays() * 24); }
    LONG GetMinutes() const { return static_cast<LONG>(GetTotalMinutes() - GetTotalHours() * 60); }
    LONG GetSeconds() const { return static_cast<LONG>(GetTotalSeconds() - GetTotalMinutes() * 60); }

    CTimeSpan operator+(CTimeSpan o) const { return CTimeSpan(m_span + o.m_span); }
    CTimeSpan operator-(CTimeSpan o) const { return CTimeSpan(m_span - o.m_span); }
    CTimeSpan& operator+=(CTimeSpan o) { m_span += o.m_span; return *this; }
    CTimeSpan& operator-=(CTimeSpan o) { m_span -= o.m_span; return *this; }
    auto operator<=>(const CTimeSpan&) const = default;

    // %D days, %H hours, %M minutes, %S seconds (ATL CTimeSpan::Format codes)
    CStringW Format(const wchar_t* fmt) const;

private:
    std::int64_t m_span = 0;
};

class CTime
{
public:
    CTime() = default;
    CTime(std::int64_t t) : m_time(t) {}
    // Local time; nDST < 0 lets the C library decide (as ATL does).
    CTime(int year, int month, int day, int hour, int minute, int second, int nDST = -1);
    CTime(const SYSTEMTIME& st, int nDST = -1)
        : CTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, nDST) {}

    static CTime GetCurrentTime();

    std::int64_t GetTime() const { return m_time; }
    bool GetLocalTm(std::tm* out) const;
    bool GetGmtTm(std::tm* out) const;
    bool GetAsSystemTime(SYSTEMTIME& st) const;

    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetHour() const;
    int GetMinute() const;
    int GetSecond() const;
    int GetDayOfWeek() const; // 1 = Sunday (ATL)

    // strftime codes, local time
    CStringW Format(const wchar_t* fmt) const;
    CStringW FormatGmt(const wchar_t* fmt) const;

    CTime operator+(CTimeSpan s) const { return CTime(m_time + s.GetTimeSpan()); }
    CTime operator-(CTimeSpan s) const { return CTime(m_time - s.GetTimeSpan()); }
    CTimeSpan operator-(CTime o) const { return CTimeSpan(m_time - o.m_time); }
    CTime& operator+=(CTimeSpan s) { m_time += s.GetTimeSpan(); return *this; }
    CTime& operator-=(CTimeSpan s) { m_time -= s.GetTimeSpan(); return *this; }
    auto operator<=>(const CTime&) const = default;

private:
    std::int64_t m_time = 0;
};
