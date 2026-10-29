// Small, faithful replacement for boost::format / boost::wformat as the original uses them, including to
// build database queries (DBLayer): `x2::str( x2::wformat( L"%d %s" ) % a % b )`.
//
// Directives, as in Boost.Format:
//   %N%                      positional argument N (1-based)
//   %[flags][width][.prec]T  printf-like; T in d i u x X o e E f F g G s S c C a A p
//   %|...|                   printf-like inside bars (Boost extension)
//   %%                       a literal '%'
// Arguments are written through a stream, so the type letter only selects formatting (hex, fixed...), it never
// reinterprets the argument (type-safe, like Boost). Missing arguments produce nothing; extra ones are ignored
// (Boost throws in both cases).
#pragma once

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace x2 {

template <typename CharT>
class basic_format
{
public:
    using string_type = std::basic_string<CharT>;

    explicit basic_format(const CharT* fmt) { Parse(fmt ? string_type(fmt) : string_type()); }
    explicit basic_format(const string_type& fmt) { Parse(fmt); }

    template <typename T>
    basic_format& operator%(const T& value)
    {
        const size_t n = m_next++;
        if (n < m_argSpecs.size()) {
            std::basic_ostringstream<CharT> os;
            const Item& spec = m_items[m_argSpecs[n]];
            Apply(os, spec);
            os << value;
            m_args.resize(m_argSpecs.size());
            m_args[n] = Pad(os.str(), spec);
        }
        return *this;
    }

    string_type str() const
    {
        string_type out;
        size_t sequential = 0;
        for (const Item& it : m_items) {
            if (it.kind == Item::Literal) {
                out += it.text;
                continue;
            }
            const size_t idx = it.position > 0 ? static_cast<size_t>(it.position - 1) : sequential++;
            if (idx < m_args.size())
                out += m_args[idx];
        }
        return out;
    }

    friend std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const basic_format& f)
    {
        return os << f.str();
    }

private:
    struct Item
    {
        enum Kind { Literal, Directive } kind = Literal;
        string_type text;     // literal text
        int position = 0;     // %N%: N, printf-like: 0 (sequential)
        bool left = false, plus = false, space = false, zero = false, alt = false;
        int width = -1, precision = -1;
        CharT type = 0;
    };

    void Parse(const string_type& f)
    {
        string_type lit;
        auto flush = [&] {
            if (!lit.empty()) {
                Item it;
                it.text = lit;
                m_items.push_back(it);
                lit.clear();
            }
        };
        for (size_t i = 0; i < f.size(); ++i) {
            if (f[i] != CharT('%')) {
                lit += f[i];
                continue;
            }
            if (i + 1 < f.size() && f[i + 1] == CharT('%')) {
                lit += CharT('%');
                ++i;
                continue;
            }
            Item it;
            it.kind = Item::Directive;
            size_t j = i + 1;
            const bool bars = j < f.size() && f[j] == CharT('|');
            if (bars)
                ++j;
            // %N%
            size_t k = j;
            int num = 0;
            while (k < f.size() && f[k] >= CharT('0') && f[k] <= CharT('9'))
                num = num * 10 + (f[k++] - CharT('0'));
            if (!bars && k > j && k < f.size() && f[k] == CharT('%')) {
                flush();
                it.position = num;
                m_argSpecs.push_back(m_items.size());
                m_items.push_back(it);
                i = k;
                continue;
            }
            // printf-like
            for (; j < f.size(); ++j) {
                const CharT c = f[j];
                if (c == CharT('-')) it.left = true;
                else if (c == CharT('+')) it.plus = true;
                else if (c == CharT(' ')) it.space = true;
                else if (c == CharT('0')) it.zero = true;
                else if (c == CharT('#')) it.alt = true;
                else break;
            }
            if (j < f.size() && f[j] >= CharT('1') && f[j] <= CharT('9')) {
                it.width = 0;
                while (j < f.size() && f[j] >= CharT('0') && f[j] <= CharT('9'))
                    it.width = it.width * 10 + (f[j++] - CharT('0'));
            }
            if (j < f.size() && f[j] == CharT('.')) {
                ++j;
                it.precision = 0;
                while (j < f.size() && f[j] >= CharT('0') && f[j] <= CharT('9'))
                    it.precision = it.precision * 10 + (f[j++] - CharT('0'));
            }
            while (j < f.size() && (f[j] == CharT('h') || f[j] == CharT('l') || f[j] == CharT('L') ||
                                    f[j] == CharT('I') || f[j] == CharT('6') || f[j] == CharT('4') ||
                                    f[j] == CharT('q') || f[j] == CharT('j') || f[j] == CharT('z') || f[j] == CharT('t')))
                ++j;
            if (j >= f.size()) { // malformed: keep as text
                lit += f.substr(i);
                break;
            }
            it.type = f[j];
            if (bars) {
                while (j < f.size() && f[j] != CharT('|'))
                    ++j;
            }
            flush();
            m_argSpecs.push_back(m_items.size());
            m_items.push_back(it);
            i = j;
        }
        flush();
        // Boost: directives are either all positional (%N%, possibly repeated) or all sequential.
        // Positional ones carry no formatting, so argument N only needs any of its items.
        size_t maxPos = 0;
        for (size_t idx : m_argSpecs)
            maxPos = std::max<size_t>(maxPos, static_cast<size_t>(m_items[idx].position));
        if (maxPos > 0)
            m_argSpecs.assign(maxPos, m_argSpecs.front());
    }

    static void Apply(std::basic_ostream<CharT>& os, const Item& it)
    {
        if (it.kind != Item::Directive || it.position > 0)
            return;
        switch (it.type) {
        case 'x': os << std::hex; break;
        case 'X': os << std::hex << std::uppercase; break;
        case 'o': os << std::oct; break;
        case 'e': os << std::scientific; break;
        case 'E': os << std::scientific << std::uppercase; break;
        case 'f': case 'F': os << std::fixed; break;
        case 'G': os << std::uppercase; break;
        default: break;
        }
        if (it.alt)
            os << std::showbase << std::showpoint;
        if (it.plus)
            os << std::showpos;
        if (it.precision >= 0)
            os << std::setprecision(it.precision);
    }

    static string_type Pad(string_type s, const Item& it)
    {
        if (it.position > 0 || it.width < 0 || static_cast<int>(s.size()) >= it.width)
            return s;
        const size_t pad = static_cast<size_t>(it.width) - s.size();
        if (it.left)
            return s + string_type(pad, CharT(' '));
        if (it.zero && it.type != 's' && it.type != 'S' && it.type != 'c' && it.type != 'C') {
            size_t signPos = (!s.empty() && (s[0] == CharT('-') || s[0] == CharT('+'))) ? 1 : 0;
            if (s.size() > signPos + 1 && s[signPos] == CharT('0') && (s[signPos + 1] == CharT('x') || s[signPos + 1] == CharT('X')))
                signPos += 2;
            return s.substr(0, signPos) + string_type(pad, CharT('0')) + s.substr(signPos);
        }
        return string_type(pad, CharT(' ')) + s;
    }

    std::vector<Item> m_items;
    std::vector<size_t> m_argSpecs;     // argument n -> index of its item
    std::vector<string_type> m_args;
    size_t m_next = 0;
};

using format = basic_format<char>;
using wformat = basic_format<wchar_t>;

template <typename CharT>
std::basic_string<CharT> str(const basic_format<CharT>& f)
{
    return f.str();
}

} // namespace x2
