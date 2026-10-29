#include "x2/platform/string.h"
#include "x2/platform/unicode.h"

#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <vector>

namespace {

std::string TranslateNarrowFormat(const char* fmt)
{
    std::string out = fmt ? fmt : "";
#if !defined(_WIN32)
    for (size_t pos; (pos = out.find("I64")) != std::string::npos;)
        out.replace(pos, 3, "ll");
#endif
    return out;
}

int VFormat(std::wstring& out, const wchar_t* fmt, va_list args)
{
    const std::wstring f = x2::platform::TranslateWideFormat(fmt);
    std::vector<wchar_t> buf(256);
    for (;;) {
        va_list copy;
        va_copy(copy, args);
        const int n = std::vswprintf(buf.data(), buf.size(), f.c_str(), copy);
        va_end(copy);
        if (n >= 0 && static_cast<size_t>(n) < buf.size()) {
            out.assign(buf.data(), static_cast<size_t>(n));
            return n;
        }
        if (buf.size() > (1u << 24)) // vswprintf does not report the needed size; give up on absurd output
            return -1;
        buf.resize(buf.size() * 4);
    }
}

int VFormat(std::string& out, const char* fmt, va_list args)
{
    const std::string f = TranslateNarrowFormat(fmt);
    va_list copy;
    va_copy(copy, args);
    const int n = std::vsnprintf(nullptr, 0, f.c_str(), copy);
    va_end(copy);
    if (n < 0)
        return n;
    out.resize(static_cast<size_t>(n) + 1);
    std::vsnprintf(out.data(), out.size(), f.c_str(), args);
    out.resize(static_cast<size_t>(n));
    return n;
}

wchar_t Upper(wchar_t c) { return static_cast<wchar_t>(std::towupper(static_cast<wint_t>(c))); }
wchar_t Lower(wchar_t c) { return static_cast<wchar_t>(std::towlower(static_cast<wint_t>(c))); }
char    Upper(char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); }
char    Lower(char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
bool    Space(wchar_t c) { return std::iswspace(static_cast<wint_t>(c)) != 0; }
bool    Space(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }

} // namespace

template <typename CharT>
CStringT<CharT>& CStringT<CharT>::MakeUpper()
{
    for (auto& c : m_str) c = Upper(c);
    return *this;
}

template <typename CharT>
CStringT<CharT>& CStringT<CharT>::MakeLower()
{
    for (auto& c : m_str) c = Lower(c);
    return *this;
}

template <typename CharT>
CStringT<CharT>& CStringT<CharT>::TrimLeft()
{
    m_str.erase(m_str.begin(), std::find_if(m_str.begin(), m_str.end(), [](CharT c) { return !Space(c); }));
    return *this;
}

template <typename CharT>
CStringT<CharT>& CStringT<CharT>::TrimRight()
{
    m_str.erase(std::find_if(m_str.rbegin(), m_str.rend(), [](CharT c) { return !Space(c); }).base(), m_str.end());
    return *this;
}

template <typename CharT>
int CStringT<CharT>::Replace(const CharT* from, const CharT* to)
{
    const string_type f = from ? from : string_type();
    const string_type t = to ? to : string_type();
    if (f.empty())
        return 0;
    int count = 0;
    for (size_t pos = 0; (pos = m_str.find(f, pos)) != string_type::npos; pos += t.size(), ++count)
        m_str.replace(pos, f.size(), t);
    return count;
}

template <typename CharT>
int CStringT<CharT>::Replace(CharT from, CharT to)
{
    int count = 0;
    for (auto& c : m_str)
        if (c == from) { c = to; ++count; }
    return count;
}

template <typename CharT>
int CStringT<CharT>::Remove(CharT ch)
{
    const auto before = m_str.size();
    std::erase(m_str, ch);
    return static_cast<int>(before - m_str.size());
}

template <typename CharT>
int CStringT<CharT>::CompareNoCase(const CharT* s) const
{
    CStringT a(*this), b(s);
    a.MakeLower();
    b.MakeLower();
    return a.m_str.compare(b.m_str);
}

template <typename CharT>
void CStringT<CharT>::FormatV(const CharT* fmt, va_list args)
{
    VFormat(m_str, fmt, args);
}

template <typename CharT>
void CStringT<CharT>::Format(const CharT* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    VFormat(m_str, fmt, args);
    va_end(args);
}

template <typename CharT>
void CStringT<CharT>::AppendFormat(const CharT* fmt, ...)
{
    string_type tail;
    va_list args;
    va_start(args, fmt);
    VFormat(tail, fmt, args);
    va_end(args);
    m_str += tail;
}

template class CStringT<wchar_t>;
template class CStringT<char>;
