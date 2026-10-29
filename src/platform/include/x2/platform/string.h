// Minimal portable replacement for ATL's CStringW/CStringA (the original is a Unicode build,
// so CString is CStringW). Only the subset the original code uses; grow it as modules are ported.
#pragma once

#include <cstdarg>
#include <string>
#include <string_view>

template <typename CharT>
class CStringT
{
public:
    using string_type = std::basic_string<CharT>;

    CStringT() = default;
    CStringT(const CharT* s) : m_str(s ? s : string_type()) {}
    CStringT(const CharT* s, int len) : m_str(s, static_cast<size_t>(len)) {}
    CStringT(CharT c, int count = 1) : m_str(static_cast<size_t>(count), c) {}
    CStringT(const string_type& s) : m_str(s) {}
    CStringT(string_type&& s) : m_str(std::move(s)) {}

    // only the ATL conversion; a second implicit one would make std::wstring(cstr) ambiguous
    operator const CharT*() const { return m_str.c_str(); }
    const CharT* GetString() const { return m_str.c_str(); }
    const string_type& str() const { return m_str; }

    int  GetLength() const { return static_cast<int>(m_str.size()); }
    bool IsEmpty() const { return m_str.empty(); }
    void Empty() { m_str.clear(); }
    CharT GetAt(int i) const { return m_str[static_cast<size_t>(i)]; }
    void SetAt(int i, CharT c) { m_str[static_cast<size_t>(i)] = c; }
    CharT operator[](int i) const { return GetAt(i); }

    CStringT Mid(int first) const { return Mid(first, GetLength() - first); }
    CStringT Mid(int first, int count) const
    {
        if (first < 0) first = 0;
        if (first >= GetLength() || count <= 0) return {};
        return CStringT(m_str.substr(static_cast<size_t>(first), static_cast<size_t>(count)));
    }
    CStringT Left(int count) const { return Mid(0, count); }
    CStringT Right(int count) const
    {
        if (count >= GetLength()) return *this;
        return count <= 0 ? CStringT() : Mid(GetLength() - count, count);
    }
    CStringT SpanExcluding(const CharT* set) const { return Left(static_cast<int>(m_str.find_first_of(set) == string_type::npos ? m_str.size() : m_str.find_first_of(set))); }
    CStringT SpanIncluding(const CharT* set) const { return Left(static_cast<int>(m_str.find_first_not_of(set) == string_type::npos ? m_str.size() : m_str.find_first_not_of(set))); }

    int Find(CharT c, int start = 0) const { return ToIndex(m_str.find(c, static_cast<size_t>(start))); }
    int Find(const CharT* s, int start = 0) const { return ToIndex(m_str.find(s, static_cast<size_t>(start))); }
    int ReverseFind(CharT c) const { return ToIndex(m_str.rfind(c)); }

    CStringT& MakeUpper();
    CStringT& MakeLower();
    CStringT& Trim() { return TrimRight().TrimLeft(); }
    CStringT& TrimLeft();
    CStringT& TrimRight();
    int Replace(const CharT* from, const CharT* to);
    int Replace(CharT from, CharT to);
    int Remove(CharT c);
    int Compare(const CharT* s) const { return m_str.compare(s ? s : string_type()); }
    int CompareNoCase(const CharT* s) const;

    // MSVC printf semantics (see x2::platform::TranslateWideFormat)
    void Format(const CharT* fmt, ...);
    void AppendFormat(const CharT* fmt, ...);
    void FormatV(const CharT* fmt, va_list args);

    CStringT& operator=(const CharT* s) { m_str = s ? s : string_type(); return *this; }
    CStringT& operator=(const string_type& s) { m_str = s; return *this; }
    CStringT& operator+=(const CStringT& s) { m_str += s.m_str; return *this; }
    CStringT& operator+=(const CharT* s) { if (s) m_str += s; return *this; }
    CStringT& operator+=(CharT c) { m_str += c; return *this; }

    friend CStringT operator+(CStringT a, const CStringT& b) { a += b; return a; }
    friend CStringT operator+(CStringT a, const CharT* b) { a += b; return a; }
    friend CStringT operator+(const CharT* a, const CStringT& b) { CStringT r(a); r += b; return r; }
    friend bool operator==(const CStringT& a, const CStringT& b) { return a.m_str == b.m_str; }
    friend bool operator==(const CStringT& a, const CharT* b) { return a.Compare(b) == 0; }
    friend auto operator<=>(const CStringT& a, const CStringT& b) { return a.m_str <=> b.m_str; }

private:
    static int ToIndex(size_t pos) { return pos == string_type::npos ? -1 : static_cast<int>(pos); }

    string_type m_str;
};

using CStringW = CStringT<wchar_t>;
using CStringA = CStringT<char>;
using CString  = CStringW;

extern template class CStringT<wchar_t>;
extern template class CStringT<char>;
