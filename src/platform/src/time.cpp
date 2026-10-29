#include "x2/platform/time.h"

#include <chrono>
#include <cwchar>

#if !defined(_WIN32)

namespace {

std::chrono::steady_clock::duration SinceStart()
{
    static const auto start = std::chrono::steady_clock::now();
    return std::chrono::steady_clock::now() - start;
}

} // namespace

ULONGLONG GetTickCount64()
{
    return static_cast<ULONGLONG>(std::chrono::duration_cast<std::chrono::milliseconds>(SinceStart()).count());
}

// Wraps around every ~49.7 days, like on Windows; callers compare with unsigned subtraction.
DWORD GetTickCount()
{
    return static_cast<DWORD>(GetTickCount64());
}

DWORD timeGetTime()
{
    return GetTickCount();
}

BOOL QueryPerformanceCounter(LARGE_INTEGER* counter)
{
    if (!counter)
        return FALSE;
    counter->QuadPart = std::chrono::duration_cast<std::chrono::nanoseconds>(SinceStart()).count();
    return TRUE;
}

BOOL QueryPerformanceFrequency(LARGE_INTEGER* frequency)
{
    if (!frequency)
        return FALSE;
    frequency->QuadPart = 1'000'000'000;
    return TRUE;
}

void GetLocalTime(SYSTEMTIME* st)
{
    if (!st)
        return;
    const auto now = std::chrono::system_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    CTime(std::chrono::system_clock::to_time_t(now)).GetAsSystemTime(*st);
    st->wMilliseconds = static_cast<WORD>(ms);
}

#endif // !_WIN32

namespace {

bool ToLocal(std::int64_t t, std::tm* out)
{
    const std::time_t tt = static_cast<std::time_t>(t);
#if defined(_WIN32)
    return localtime_s(out, &tt) == 0;
#else
    return localtime_r(&tt, out) != nullptr;
#endif
}

bool ToGmt(std::int64_t t, std::tm* out)
{
    const std::time_t tt = static_cast<std::time_t>(t);
#if defined(_WIN32)
    return gmtime_s(out, &tt) == 0;
#else
    return gmtime_r(&tt, out) != nullptr;
#endif
}

CStringW FormatTm(const std::tm& tm, const wchar_t* fmt)
{
    wchar_t buf[512];
    const size_t n = std::wcsftime(buf, sizeof(buf) / sizeof(buf[0]), fmt ? fmt : L"", &tm);
    return CStringW(buf, static_cast<int>(n));
}

int Field(std::int64_t t, int std::tm::*field)
{
    std::tm tm{};
    return ToLocal(t, &tm) ? tm.*field : 0;
}

} // namespace

CTime::CTime(int year, int month, int day, int hour, int minute, int second, int nDST)
{
    std::tm tm{};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = nDST;
    m_time = static_cast<std::int64_t>(std::mktime(&tm));
}

CTime CTime::GetCurrentTime()
{
    return CTime(static_cast<std::int64_t>(std::time(nullptr)));
}

bool CTime::GetLocalTm(std::tm* out) const { return out && ToLocal(m_time, out); }
bool CTime::GetGmtTm(std::tm* out) const { return out && ToGmt(m_time, out); }

bool CTime::GetAsSystemTime(SYSTEMTIME& st) const
{
    std::tm tm{};
    if (!ToLocal(m_time, &tm))
        return false;
    st.wYear = static_cast<WORD>(tm.tm_year + 1900);
    st.wMonth = static_cast<WORD>(tm.tm_mon + 1);
    st.wDayOfWeek = static_cast<WORD>(tm.tm_wday);
    st.wDay = static_cast<WORD>(tm.tm_mday);
    st.wHour = static_cast<WORD>(tm.tm_hour);
    st.wMinute = static_cast<WORD>(tm.tm_min);
    st.wSecond = static_cast<WORD>(tm.tm_sec);
    st.wMilliseconds = 0;
    return true;
}

int CTime::GetYear() const { return Field(m_time, &std::tm::tm_year) + 1900; }
int CTime::GetMonth() const { return Field(m_time, &std::tm::tm_mon) + 1; }
int CTime::GetDay() const { return Field(m_time, &std::tm::tm_mday); }
int CTime::GetHour() const { return Field(m_time, &std::tm::tm_hour); }
int CTime::GetMinute() const { return Field(m_time, &std::tm::tm_min); }
int CTime::GetSecond() const { return Field(m_time, &std::tm::tm_sec); }
int CTime::GetDayOfWeek() const { return Field(m_time, &std::tm::tm_wday) + 1; }

CStringW CTime::Format(const wchar_t* fmt) const
{
    std::tm tm{};
    return ToLocal(m_time, &tm) ? FormatTm(tm, fmt) : CStringW();
}

CStringW CTime::FormatGmt(const wchar_t* fmt) const
{
    std::tm tm{};
    return ToGmt(m_time, &tm) ? FormatTm(tm, fmt) : CStringW();
}

CStringW CTimeSpan::Format(const wchar_t* fmt) const
{
    CStringW out;
    for (const wchar_t* p = fmt; p && *p; ++p) {
        if (*p != L'%' || !p[1]) {
            out += *p;
            continue;
        }
        CStringW part;
        switch (*++p) {
        case L'D': part.Format(L"%lld", static_cast<long long>(GetDays())); break;
        case L'H': part.Format(L"%02d", GetHours()); break;
        case L'M': part.Format(L"%02d", GetMinutes()); break;
        case L'S': part.Format(L"%02d", GetSeconds()); break;
        case L'%': part = L"%"; break;
        default: part += L'%'; part += *p; break;
        }
        out += part;
    }
    return out;
}
