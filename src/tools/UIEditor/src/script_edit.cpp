#include "script_edit.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace x2::ui_editor {

namespace {

constexpr std::size_t npos = std::string::npos;

bool IsIdentStart(char c)
{
    return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsIdent(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

// 1 where the text is Lua code, 0 inside strings and comments.
std::vector<char> CodeMask(const std::string& t)
{
    const std::size_t n = t.size();
    std::vector<char> code(n, 1);
    // The level of a long bracket opening at `at` ([[, [=[, ...), or -1.
    const auto longLevel = [&](std::size_t at) {
        std::size_t j = at + 1;
        int level = 0;
        while (j < n && t[j] == '=') {
            ++level;
            ++j;
        }
        return j < n && t[j] == '[' ? level : -1;
    };
    const auto pastLongClose = [&](std::size_t from, int level) {
        const std::string close = "]" + std::string(static_cast<std::size_t>(level), '=') + "]";
        const std::size_t end = t.find(close, from);
        return end == npos ? n : end + close.size();
    };
    std::size_t i = 0;
    while (i < n) {
        std::size_t end = npos;
        const char c = t[i];
        if (c == '-' && i + 1 < n && t[i + 1] == '-') {
            const int level = i + 2 < n && t[i + 2] == '[' ? longLevel(i + 2) : -1;
            if (level >= 0) {
                end = pastLongClose(i + 4 + static_cast<std::size_t>(level), level);
            } else {
                end = t.find('\n', i);
                end = end == npos ? n : end;
            }
        } else if (c == '"' || c == '\'') {
            std::size_t j = i + 1;
            while (j < n && t[j] != c && t[j] != '\n') {
                j += t[j] == '\\' ? 2 : 1;
            }
            end = std::min(j + 1, n);
        } else if (c == '[') {
            if (const int level = longLevel(i); level >= 0) {
                end = pastLongClose(i + 2 + static_cast<std::size_t>(level), level);
            }
        }
        if (end == npos) {
            ++i;
            continue;
        }
        std::fill(code.begin() + static_cast<std::ptrdiff_t>(i), code.begin() + static_cast<std::ptrdiff_t>(end), 0);
        i = end;
    }
    return code;
}

struct Range {
    std::size_t begin = 0, end = 0;     // [begin, end)
};

// The bracket closing the one at `open`, or npos.
std::size_t FindClose(const std::string& t, const std::vector<char>& code, std::size_t open)
{
    int depth = 0;
    for (std::size_t i = open; i < t.size(); ++i) {
        if (!code[i]) {
            continue;
        }
        const char c = t[i];
        if (c == '(' || c == '[' || c == '{') {
            ++depth;
        } else if (c == ')' || c == ']' || c == '}') {
            if (--depth == 0) {
                return i;
            }
        }
    }
    return npos;
}

// The parts of [begin, end) between top-level separators.
std::vector<Range> Split(const std::string& t, const std::vector<char>& code, Range range, const char* separators)
{
    std::vector<Range> parts;
    int depth = 0;
    std::size_t start = range.begin;
    for (std::size_t i = range.begin; i < range.end; ++i) {
        if (!code[i]) {
            continue;
        }
        const char c = t[i];
        if (c == '(' || c == '[' || c == '{') {
            ++depth;
        } else if (c == ')' || c == ']' || c == '}') {
            --depth;
        } else if (depth == 0 && std::strchr(separators, c) != nullptr) {
            parts.push_back({start, i});
            start = i + 1;
        }
    }
    parts.push_back({start, range.end});
    return parts;
}

std::size_t SkipSpaces(const std::string& t, std::size_t i, std::size_t end)
{
    while (i < end && IsSpace(t[i])) {
        ++i;
    }
    return i;
}

struct Edit {
    std::size_t begin = 0, end = 0;
    std::string text;
};

// The arguments of a D3DXVECTOR2(x, y) at [begin, end), or false.
bool VectorArgs(const std::string& t, const std::vector<char>& code, Range value, Range& x, Range& y)
{
    std::size_t i = SkipSpaces(t, value.begin, value.end);
    static const std::string kName = "D3DXVECTOR2";
    if (t.compare(i, kName.size(), kName) != 0) {
        return false;
    }
    i = SkipSpaces(t, i + kName.size(), value.end);
    if (i >= value.end || t[i] != '(') {
        return false;
    }
    const std::size_t close = FindClose(t, code, i);
    if (close == npos || close >= value.end) {
        return false;
    }
    const std::vector<Range> args = Split(t, code, {i + 1, close}, ",");
    if (args.size() != 2) {
        return false;
    }
    x = args[0];
    y = args[1];
    return true;
}

// Shifts the D3DXVECTOR2 fields named in `keys` of the table at `table` ('{' to '}'); adds `add` = (dx, dy) when none
// of them is there.
bool MoveTable(const std::string& t, const std::vector<char>& code, Range table, std::initializer_list<const char*> keys,
               const char* add, float dx, float dy, std::vector<Edit>& edits, std::string& error)
{
    bool found = false;
    for (const Range& item : Split(t, code, {table.begin + 1, table.end}, ",;")) {
        std::size_t i = item.begin;
        while (i < item.end && (IsSpace(t[i]) || !code[i])) {    // comments before the field
            ++i;
        }
        const std::size_t nameBegin = i;
        while (i < item.end && IsIdent(t[i])) {
            ++i;
        }
        const std::string name = t.substr(nameBegin, i - nameBegin);
        i = SkipSpaces(t, i, item.end);
        if (name.empty() || i >= item.end || t[i] != '=' || (i + 1 < item.end && t[i + 1] == '=')) {
            continue;
        }
        if (std::none_of(keys.begin(), keys.end(), [&](const char* key) { return name == key; })) {
            continue;
        }
        Range x, y;
        if (!VectorArgs(t, code, {i + 1, item.end}, x, y)) {
            error = name + " is not a D3DXVECTOR2(x, y)";
            return false;
        }
        edits.push_back({x.begin, x.end, ShiftExpression(t.substr(x.begin, x.end - x.begin), dx)});
        edits.push_back({y.begin, y.end, ShiftExpression(t.substr(y.begin, y.end - y.begin), dy)});
        found = true;
    }
    if (found) {
        return true;
    }
    // None given: add one after the '{', on a line of its own when the table spans lines.
    const std::string field = std::string(add) + " = D3DXVECTOR2(" + FormatNumber(dx) + ", " + FormatNumber(dy) + ")";
    std::size_t after = table.begin + 1;
    while (after < table.end && (t[after] == ' ' || t[after] == '\t')) {
        ++after;
    }
    if (after < table.end && (t[after] == '\n' || t[after] == '\r')) {
        const std::size_t next = t.find('\n', table.begin) + 1;
        std::size_t indentEnd = next;
        while (indentEnd < table.end && (t[indentEnd] == ' ' || t[indentEnd] == '\t')) {
            ++indentEnd;
        }
        edits.push_back({next, next, t.substr(next, indentEnd - next) + field + ",\n"});
    } else if (after >= table.end) {
        edits.push_back({table.begin + 1, table.end, " " + field + " "});
    } else {
        edits.push_back({after, after, field + ", "});
    }
    return true;
}

bool KindMatches(CallKind kind, const std::string& full)
{
    // Some bindings keep their C++ name (g_pNavi:SetBGPoint_LUA).
    const std::string name = full.size() > 4 && full.compare(full.size() - 4, 4, "_LUA") == 0 ? full.substr(0, full.size() - 4) : full;
    switch (kind) {
    case CallKind::DialogPos:
        return name == "SetPos";
    case CallKind::Point:
        return name == "SetRect" || (name.size() > 5 && name.compare(name.size() - 5, 5, "Point") == 0);
    case CallKind::String:
        return name == "AddString";
    case CallKind::Texture:
        return name == "SetTex";
    }
    return false;
}

const char* KindName(CallKind kind)
{
    switch (kind) {
    case CallKind::DialogPos:
        return "SetPos";
    case CallKind::Point:
        return "point table (Set...Point)";
    case CallKind::String:
        return "AddString";
    case CallKind::Texture:
        return "SetTex";
    }
    return "?";
}

}  // namespace

std::string FormatNumber(float value)
{
    const double v = static_cast<double>(value);
    char buffer[32];
    if (std::fabs(v - std::round(v)) < 0.0005) {
        std::snprintf(buffer, sizeof(buffer), "%ld", std::lround(v));
        return buffer;
    }
    std::snprintf(buffer, sizeof(buffer), "%.3f", v);
    std::string text = buffer;
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    return text;
}

std::string ShiftExpression(const std::string& e, float delta)
{
    if (delta == 0.0f) {
        return e;
    }
    // Additive terms at the top level: [begin, end) of each, and the binary + or - before it (npos for the first).
    struct Term {
        std::size_t op = npos, begin = 0, end = 0;
    };
    std::vector<Term> terms(1);
    int depth = 0;
    bool expectOperand = true;
    std::size_t i = 0;
    while (i < e.size()) {
        const char c = e[i];
        if (IsSpace(c)) {
            ++i;
            continue;
        }
        if (depth == 0 && (c == '+' || c == '-') && !expectOperand) {
            terms.back().end = i;
            terms.push_back({i, i + 1, 0});
            expectOperand = true;
            ++i;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)) != 0 ||
            (c == '.' && i + 1 < e.size() && std::isdigit(static_cast<unsigned char>(e[i + 1])) != 0)) {
            while (i < e.size() && (std::isalnum(static_cast<unsigned char>(e[i])) != 0 || e[i] == '.')) {
                const bool exponent = (e[i] == 'e' || e[i] == 'E') && i + 1 < e.size() && (e[i + 1] == '+' || e[i + 1] == '-');
                i += exponent ? 2 : 1;
            }
            expectOperand = false;
            continue;
        }
        if (IsIdentStart(c)) {
            while (i < e.size() && IsIdent(e[i])) {
                ++i;
            }
            expectOperand = false;
            continue;
        }
        if (c == '(' || c == '[' || c == '{') {
            ++depth;
            expectOperand = true;
        } else if (c == ')' || c == ']' || c == '}') {
            --depth;
            expectOperand = false;
        } else {
            expectOperand = true;   // another operator
        }
        ++i;
    }
    terms.back().end = e.size();

    // Trimmed bounds of a term (after its operator).
    const auto trimmed = [&](const Term& term) {
        Range r{term.begin, term.end};
        while (r.begin < r.end && IsSpace(e[r.begin])) {
            ++r.begin;
        }
        while (r.end > r.begin && IsSpace(e[r.end - 1])) {
            --r.end;
        }
        return r;
    };
    const auto number = [&](Range r, double& value) {
        if (r.begin >= r.end) {
            return false;
        }
        const std::string text = e.substr(r.begin, r.end - r.begin);
        std::string compact;
        for (const char c : text) {
            if (!IsSpace(c)) {
                compact += c;
            }
        }
        char* stop = nullptr;
        value = std::strtod(compact.c_str(), &stop);
        return stop != compact.c_str() && *stop == '\0' && compact.find_first_of("xX") == npos;
    };

    for (std::size_t k = 0; k < terms.size(); ++k) {
        const Range r = trimmed(terms[k]);
        double value = 0.0;
        if (!number(r, value)) {
            continue;
        }
        if (terms[k].op != npos && e[terms[k].op] == '-') {
            value = -value;
        }
        const double moved = value + static_cast<double>(delta);
        const bool zero = std::fabs(moved) < 0.0005;
        std::string out = e;
        if (k == 0) {
            if (zero && terms.size() > 1) {
                // 0 + X -> X, 0 - X -> -X
                const Term& next = terms[1];
                const std::size_t operand = trimmed(next).begin;
                out.replace(r.begin, operand - r.begin, e[next.op] == '-' ? "-" : "");
            } else {
                out.replace(r.begin, r.end - r.begin, FormatNumber(static_cast<float>(moved)));
            }
        } else if (zero) {
            std::size_t from = terms[k].op;
            while (from > 0 && IsSpace(e[from - 1])) {
                --from;
            }
            out.erase(from, r.end - from);
        } else {
            const std::string spacing = e.substr(terms[k].op + 1, r.begin - terms[k].op - 1);
            out.replace(terms[k].op, r.end - terms[k].op,
                        std::string(moved < 0 ? "-" : "+") + spacing + FormatNumber(static_cast<float>(std::fabs(moved))));
        }
        return out;
    }
    // No number: append the delta.
    const Range all = trimmed({npos, 0, e.size()});
    std::string out = e;
    out.insert(all.end, std::string(delta < 0 ? " - " : " + ") + FormatNumber(std::fabs(delta)));
    return out;
}

namespace {

// The one call of `kind` on `line`: its '(' or '{' and the bracket closing it. False with `error` otherwise.
bool FindCall(const std::string& text, const std::vector<char>& code, int line, CallKind kind, std::size_t& open,
              std::size_t& close, std::string& error)
{
    std::size_t lineBegin = 0;
    for (int l = 1; l < line; ++l) {
        lineBegin = text.find('\n', lineBegin);
        if (lineBegin == npos) {
            error = "the script has no line " + std::to_string(line);
            return false;
        }
        ++lineBegin;
    }
    std::size_t lineEnd = text.find('\n', lineBegin);
    lineEnd = lineEnd == npos ? text.size() : lineEnd;

    // The calls obj:Name( / obj:Name{ on the line that match the kind.
    std::vector<std::size_t> opens;
    for (std::size_t i = lineBegin; i < lineEnd; ++i) {
        if (!code[i] || text[i] != ':' || i + 1 >= lineEnd || !IsIdentStart(text[i + 1])) {
            continue;
        }
        std::size_t j = i + 1;
        while (j < lineEnd && IsIdent(text[j])) {
            ++j;
        }
        const std::string name = text.substr(i + 1, j - i - 1);
        j = SkipSpaces(text, j, text.size());
        if (j < text.size() && (text[j] == '(' || text[j] == '{') && KindMatches(kind, name)) {
            opens.push_back(j);
        }
    }
    const std::string where = " on line " + std::to_string(line);
    if (opens.empty()) {
        error = std::string("no ") + KindName(kind) + " call" + where;
        return false;
    }
    if (opens.size() > 1) {
        error = std::string("more than one ") + KindName(kind) + " call" + where;
        return false;
    }
    open = opens.front();
    close = FindClose(text, code, open);
    if (close == npos) {
        error = "the call" + where + " is not closed";
        return false;
    }
    return true;
}

// The text between the quotes of a string literal at [begin, end) (spaces around it allowed), or false.
bool StringLiteral(const std::string& t, Range range, Range& inside)
{
    std::size_t b = SkipSpaces(t, range.begin, range.end), e = range.end;
    while (e > b && IsSpace(t[e - 1])) {
        --e;
    }
    if (e - b < 2 || (t[b] != '"' && t[b] != '\'') || t[e - 1] != t[b]) {
        return false;
    }
    for (std::size_t i = b + 1; i + 1 < e; ++i) {
        if (t[i] == t[b] || t[i] == '\\') {
            return false;
        }
    }
    inside = {b + 1, e - 1};
    return true;
}

}  // namespace

bool MoveCall(std::string& text, int line, CallKind kind, float dx, float dy, std::string& error)
{
    const std::vector<char> code = CodeMask(text);
    std::size_t open = 0, close = 0;
    if (!FindCall(text, code, line, kind, open, close, error)) {
        return false;
    }
    const std::string where = " on line " + std::to_string(line);
    if (kind == CallKind::Texture) {
        error = "SetTex" + where + " places nothing";
        return false;
    }

    std::vector<Edit> edits;
    if (kind == CallKind::DialogPos) {
        const std::vector<Range> args = Split(text, code, {open + 1, close}, ",");
        if (text[open] != '(' || args.size() != 2) {
            error = "SetPos" + where + " does not take x, y";
            return false;
        }
        edits.push_back({args[0].begin, args[0].end, ShiftExpression(text.substr(args[0].begin, args[0].end - args[0].begin), dx)});
        edits.push_back({args[1].begin, args[1].end, ShiftExpression(text.substr(args[1].begin, args[1].end - args[1].begin), dy)});
    } else {
        Range table{open, close};
        if (text[open] == '(') {
            const std::size_t brace = SkipSpaces(text, open + 1, close);
            const std::size_t braceClose = brace < close && text[brace] == '{' ? FindClose(text, code, brace) : npos;
            if (braceClose == npos || braceClose > close) {
                error = std::string("the ") + KindName(kind) + " call" + where + " does not take a table";
                return false;
            }
            table = {brace, braceClose};
        }
        const bool ok = kind == CallKind::Point
                            ? MoveTable(text, code, table, {"LEFT_TOP", "RIGHT_TOP", "LEFT_BOTTOM", "RIGHT_BOTTOM"},
                                        "LEFT_TOP", dx, dy, edits, error)
                            : MoveTable(text, code, table, {"POS"}, "POS", dx, dy, edits, error);
        if (!ok) {
            error += where;
            return false;
        }
    }
    std::sort(edits.begin(), edits.end(), [](const Edit& a, const Edit& b) { return a.begin > b.begin; });
    for (const Edit& edit : edits) {
        text.replace(edit.begin, edit.end - edit.begin, edit.text);
    }
    return true;
}

bool SetTexCall(std::string& text, int line, const std::string& texture, const std::string& key, std::string& error)
{
    const std::vector<char> code = CodeMask(text);
    std::size_t open = 0, close = 0;
    if (!FindCall(text, code, line, CallKind::Texture, open, close, error)) {
        return false;
    }
    const std::string where = " on line " + std::to_string(line);
    const std::vector<Range> args = Split(text, code, {open + 1, close}, ",");
    Range file, name;
    if (text[open] != '(' || args.size() != 2 || !StringLiteral(text, args[0], file) || !StringLiteral(text, args[1], name)) {
        error = "SetTex" + where + " does not take two quoted names";
        return false;
    }
    for (const std::string* value : {&texture, &key}) {
        if (value->empty() || value->find_first_of("\"'\\\n") != npos) {
            error = "a texture and a key are plain names";
            return false;
        }
    }
    text.replace(name.begin, name.end - name.begin, key);
    text.replace(file.begin, file.end - file.begin, texture);
    return true;
}

}  // namespace x2::ui_editor
