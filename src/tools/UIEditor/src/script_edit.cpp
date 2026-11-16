#include "script_edit.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
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

void AddField(const std::string& t, Range table, const std::string& field, std::vector<Edit>& edits);

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
    AddField(t, table, std::string(add) + " = D3DXVECTOR2(" + FormatNumber(dx) + ", " + FormatNumber(dy) + ")", edits);
    return true;
}

// Adds `field` (name = value) to the table at `table` ('{' to '}'), after the '{', on a line of its own when the
// table spans lines.
void AddField(const std::string& t, Range table, const std::string& field, std::vector<Edit>& edits)
{
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
}

// The fields of the table at `table`: name and the value's range.
struct Field {
    std::string name;
    Range value;
};
std::vector<Field> TableFields(const std::string& t, const std::vector<char>& code, Range table)
{
    std::vector<Field> fields;
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
        fields.push_back({name, {i + 1, item.end}});
    }
    return fields;
}

std::string Trimmed(const std::string& t, Range r)
{
    while (r.begin < r.end && IsSpace(t[r.begin])) {
        ++r.begin;
    }
    while (r.end > r.begin && IsSpace(t[r.end - 1])) {
        --r.end;
    }
    return t.substr(r.begin, r.end - r.begin);
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
    case CallKind::AddPicture:
        return name == "AddPicture";
    case CallKind::AddControl:
        return name == "AddControl";
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
    case CallKind::AddPicture:
        return "AddPicture";
    case CallKind::AddControl:
        return "AddControl";
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
    if (kind == CallKind::Texture || kind == CallKind::AddPicture || kind == CallKind::AddControl) {
        error = std::string(KindName(kind)) + where + " places nothing";
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

bool ResizeCall(std::string& text, int line, float left, float top, float right, float bottom, std::string& error)
{
    const std::vector<char> code = CodeMask(text);
    std::size_t open = 0, close = 0;
    if (!FindCall(text, code, line, CallKind::Point, open, close, error)) {
        return false;
    }
    const std::string where = " on line " + std::to_string(line);
    Range table{open, close};
    if (text[open] == '(') {
        const std::size_t brace = SkipSpaces(text, open + 1, close);
        const std::size_t braceClose = brace < close && text[brace] == '{' ? FindClose(text, code, brace) : npos;
        if (braceClose == npos || braceClose > close) {
            error = "the point call" + where + " does not take a table";
            return false;
        }
        table = {brace, braceClose};
    }
    const std::vector<Field> fields = TableFields(text, code, table);
    const auto find = [&](const char* name) -> const Field* {
        for (const Field& field : fields) {
            if (field.name == name) {
                return &field;
            }
        }
        return nullptr;
    };
    // The size comes from the texture (the default) plus ADD_SIZE_X/Y, or from the corners given.
    bool textureSize = true;
    if (const Field* use = find("USE_TEXTURE_SIZE"); use != nullptr) {
        const std::string value = Trimmed(text, use->value);
        if (value == "FALSE" || value == "false" || value == "0") {
            textureSize = false;
        } else if (value != "TRUE" && value != "true" && value != "1") {
            error = "USE_TEXTURE_SIZE" + where + " is " + value + ", not true or false";
            return false;
        }
    }
    std::vector<Edit> edits;
    const auto shiftVector = [&](const char* name, float dx, float dy, bool addWhenMissing) {
        const Field* field = find(name);
        if (field == nullptr) {
            if (addWhenMissing && (dx != 0.0f || dy != 0.0f)) {     // LEFT_TOP defaults to 0, 0
                AddField(text, table, std::string(name) + " = D3DXVECTOR2(" + FormatNumber(dx) + ", " + FormatNumber(dy) + ")",
                         edits);
            }
            return true;
        }
        Range x, y;
        if (!VectorArgs(text, code, field->value, x, y)) {
            error = std::string(name) + where + " is not a D3DXVECTOR2(x, y)";
            return false;
        }
        edits.push_back({x.begin, x.end, ShiftExpression(text.substr(x.begin, x.end - x.begin), dx)});
        edits.push_back({y.begin, y.end, ShiftExpression(text.substr(y.begin, y.end - y.begin), dy)});
        return true;
    };
    const auto shiftNumber = [&](const char* name, float delta) {
        if (delta == 0.0f) {
            return;
        }
        if (const Field* field = find(name); field != nullptr) {
            edits.push_back({field->value.begin, field->value.end,
                             ShiftExpression(text.substr(field->value.begin, field->value.end - field->value.begin), delta)});
        } else {
            AddField(text, table, std::string(name) + " = " + FormatNumber(delta), edits);
        }
    };
    if (textureSize) {
        if (!shiftVector("LEFT_TOP", left, top, true)) {
            return false;
        }
        shiftNumber("ADD_SIZE_X", right - left);
        shiftNumber("ADD_SIZE_Y", bottom - top);
    } else {
        if (find("RIGHT_BOTTOM") == nullptr) {
            error = "the point table" + where + " sets its size without a RIGHT_BOTTOM";
            return false;
        }
        if (!shiftVector("LEFT_TOP", left, top, true) || !shiftVector("RIGHT_TOP", right, top, false) ||
            !shiftVector("LEFT_BOTTOM", left, bottom, false) || !shiftVector("RIGHT_BOTTOM", right, bottom, false)) {
            return false;
        }
    }
    std::stable_sort(edits.begin(), edits.end(), [](const Edit& a, const Edit& b) { return a.begin > b.begin; });
    for (const Edit& edit : edits) {
        text.replace(edit.begin, edit.end - edit.begin, edit.text);
    }
    return true;
}

// Structural edits ----------------------------------------------------------------------------------------------------

namespace {

constexpr const char* kBom = "\xEF\xBB\xBF";

// Where the line holding `at` starts (past the BOM on the first line).
std::size_t LineBegin(const std::string& t, std::size_t at)
{
    const std::size_t nl = at == 0 ? npos : t.rfind('\n', at - 1);
    if (nl != npos) {
        return nl + 1;
    }
    return t.compare(0, 3, kBom) == 0 && at >= 3 ? 3 : 0;
}

// Where the line after the one holding `at` starts (the text's end when it is the last).
std::size_t NextLine(const std::string& t, std::size_t at)
{
    const std::size_t nl = t.find('\n', at);
    return nl == npos ? t.size() : nl + 1;
}

int LineOf(const std::string& t, std::size_t at)
{
    return 1 + static_cast<int>(std::count(t.begin(), t.begin() + static_cast<std::ptrdiff_t>(std::min(at, t.size())), '\n'));
}

std::string Indent(const std::string& t, std::size_t lineBegin)
{
    std::size_t i = lineBegin;
    while (i < t.size() && (t[i] == ' ' || t[i] == '\t')) {
        ++i;
    }
    return t.substr(lineBegin, i - lineBegin);
}

const char* Newline(const std::string& t)
{
    return t.find("\r\n") != npos ? "\r\n" : "\n";
}

bool IsName(const std::string& s)
{
    return !s.empty() && IsIdentStart(s[0]) && std::all_of(s.begin(), s.end(), IsIdent);
}

// Whether the identifier `name` is at `i` as a whole word, not a field (a.name) or a method (a:name).
bool WordAt(const std::string& t, const std::vector<char>& code, std::size_t i, const std::string& name)
{
    if (!code[i] || t.compare(i, name.size(), name) != 0) {
        return false;
    }
    if (i > 0 && (IsIdent(t[i - 1]) || t[i - 1] == '.' || t[i - 1] == ':')) {
        return false;
    }
    const std::size_t end = i + name.size();
    return end >= t.size() || !IsIdent(t[end]);
}

// A statement about a variable: `V:Method(...)` or `V:Method{...}`, or the assignment `[local] V = ...`.
struct Statement {
    std::size_t begin = 0, end = 0;     // [begin, end): from `local` or the name to past the closing bracket
    bool assignment = false;
    std::string method;                 // the method called (for an assignment, the one on its right: CreatePicture)
    std::string receiver;               // for an assignment, the object the method is called on (g_pUIDialog)
    Range args;                         // inside the call's brackets ('(' or '{' kept out)
};

// Whether a statement may start at `i`: only spaces before it on its line, or a ';'.
bool StatementStart(const std::string& t, std::size_t i)
{
    std::size_t j = i;
    while (j > 0 && (t[j - 1] == ' ' || t[j - 1] == '\t')) {
        --j;
    }
    return j == 0 || t[j - 1] == '\n' || t[j - 1] == ';' || (j == 3 && t.compare(0, 3, kBom) == 0);
}

// A call `name(` or `name{` at `i` (after spaces): the method's name, its brackets. False when not.
bool CallAt(const std::string& t, const std::vector<char>& code, std::size_t i, std::string& method, std::size_t& open,
            std::size_t& close)
{
    i = SkipSpaces(t, i, t.size());
    const std::size_t nameBegin = i;
    while (i < t.size() && IsIdent(t[i])) {
        ++i;
    }
    if (i == nameBegin || !IsIdentStart(t[nameBegin])) {
        return false;
    }
    method = t.substr(nameBegin, i - nameBegin);
    i = SkipSpaces(t, i, t.size());
    if (i >= t.size() || (t[i] != '(' && t[i] != '{') || !code[i]) {
        return false;
    }
    open = i;
    close = FindClose(t, code, open);
    return close != npos;
}

// The statements about `name` in [from, to), in order.
std::vector<Statement> StatementsOf(const std::string& t, const std::vector<char>& code, const std::string& name,
                                    std::size_t from, std::size_t to)
{
    std::vector<Statement> out;
    for (std::size_t i = from; i < to && i < t.size(); ++i) {
        if (!WordAt(t, code, i, name)) {
            continue;
        }
        Statement st;
        std::size_t j = SkipSpaces(t, i + name.size(), t.size());
        if (j < t.size() && t[j] == ':' && StatementStart(t, i)) {
            std::size_t open = 0, close = 0;
            if (CallAt(t, code, j + 1, st.method, open, close)) {
                st.begin = i;
                st.end = close + 1;
                st.args = {open + 1, close};
                out.push_back(st);
                i = close;
            }
            continue;
        }
        if (j < t.size() && t[j] == '=' && (j + 1 >= t.size() || t[j + 1] != '=')) {
            std::size_t begin = i;
            std::size_t k = i;
            while (k > 0 && (t[k - 1] == ' ' || t[k - 1] == '\t')) {
                --k;
            }
            if (k >= 5 && t.compare(k - 5, 5, "local") == 0 && (k == 5 || !IsIdent(t[k - 6])) && StatementStart(t, k - 5)) {
                begin = k - 5;
            } else if (!StatementStart(t, i)) {
                continue;
            }
            st.begin = begin;
            st.assignment = true;
            // The right side: recv:Method(...) for a creation, else up to the end of the line.
            std::size_t r = SkipSpaces(t, j + 1, t.size());
            const std::size_t recvBegin = r;
            while (r < t.size() && IsIdent(t[r])) {
                ++r;
            }
            std::size_t open = 0, close = 0;
            if (r > recvBegin && r < t.size() && t[r] == ':' && CallAt(t, code, r + 1, st.method, open, close)) {
                st.receiver = t.substr(recvBegin, r - recvBegin);
                st.end = close + 1;
                st.args = {open + 1, close};
            } else {
                st.end = t.find('\n', j);
                st.end = st.end == npos ? t.size() : st.end;
                while (st.end > j && (t[st.end - 1] == '\r' || IsSpace(t[st.end - 1]))) {
                    --st.end;
                }
            }
            out.push_back(st);
            i = st.end - 1;
        }
    }
    return out;
}

// Where an object's lines are: from its creation (the last assignment to `name` before `at`; `at`'s line when none) to
// its next assignment (the text's end when none). `creation` is set when found.
Range ObjectRange(const std::string& t, const std::vector<char>& code, const std::string& name, std::size_t at,
                  std::optional<Statement>* creation = nullptr)
{
    Range range{LineBegin(t, at), t.size()};
    for (const Statement& st : StatementsOf(t, code, name, 0, t.size())) {
        if (!st.assignment) {
            continue;
        }
        if (st.begin <= at) {
            range.begin = st.begin;
            if (creation != nullptr) {
                *creation = st;
            }
        } else {
            range.end = st.begin;
            break;
        }
    }
    return range;
}

// Whether `name` appears as a word in [from, to) outside the ranges given.
bool UsedOutside(const std::string& t, const std::vector<char>& code, const std::string& name, Range in,
                 const std::vector<Range>& except)
{
    for (std::size_t i = in.begin; i < in.end && i < t.size(); ++i) {
        if (!WordAt(t, code, i, name)) {
            continue;
        }
        if (std::none_of(except.begin(), except.end(), [&](const Range& r) { return i >= r.begin && i < r.end; })) {
            return true;
        }
    }
    return false;
}

// A call found on a line: the object it is called on, its one argument (a plain name) and where the statement is.
struct CallOnLine {
    std::string receiver, argument;
    Statement statement;
};

bool FindAddCall(const std::string& t, const std::vector<char>& code, int line, CallKind kind, CallOnLine& out,
                 std::string& error)
{
    std::size_t open = 0, close = 0;
    if (!FindCall(t, code, line, kind, open, close, error)) {
        return false;
    }
    const std::string where = " on line " + std::to_string(line);
    // The receiver: the name before the ':' before the method's name.
    std::size_t colon = open;
    while (colon > 0 && t[colon - 1] != ':') {
        --colon;
    }
    std::size_t recvBegin = colon > 0 ? colon - 1 : 0;
    while (recvBegin > 0 && IsIdent(t[recvBegin - 1])) {
        --recvBegin;
    }
    out.receiver = colon > 0 ? t.substr(recvBegin, colon - 1 - recvBegin) : std::string();
    out.argument = Trimmed(t, {open + 1, close});
    if (!IsName(out.receiver) || (recvBegin > 0 && (t[recvBegin - 1] == '.' || t[recvBegin - 1] == ':'))) {
        error = std::string(KindName(kind)) + where + " is not called on a plain name";
        return false;
    }
    if (t[open] != '(' || !IsName(out.argument)) {
        error = std::string(KindName(kind)) + where + " does not take a plain name";
        return false;
    }
    if (!StatementStart(t, recvBegin)) {
        error = std::string(KindName(kind)) + where + " is not a statement of its own";
        return false;
    }
    out.statement.begin = recvBegin;
    out.statement.end = close + 1;
    out.statement.method = KindName(kind);
    out.statement.args = {open + 1, close};
    return true;
}

// A global name the text does not use yet: base, then base_2, base_3, ...
std::string NewGlobal(const std::string& t, std::string base)
{
    for (char& c : base) {
        if (!IsIdent(c)) {
            c = '_';
        }
    }
    const std::vector<char> code = CodeMask(t);
    const auto used = [&](const std::string& name) {
        for (std::size_t i = 0; i + name.size() <= t.size(); ++i) {
            if (WordAt(t, code, i, name)) {
                return true;
            }
        }
        return false;
    };
    std::string name = base;
    for (int n = 2; used(name); ++n) {
        name = base + "_" + std::to_string(n);
    }
    return name;
}

bool CheckControlName(const std::string& name, std::string& error)
{
    if (name.empty() || name.find_first_of("\"'\\\r\n") != npos) {
        error = "a control's name is not empty and has no quotes";
        return false;
    }
    return true;
}

bool CheckTexture(const std::string& texture, const std::string& key, std::string& error)
{
    if (texture.empty() || texture.find_first_of("\"'\\\r\n") != npos || key.find_first_of("\"'\\\r\n") != npos) {
        error = "a texture and a key are plain names";
        return false;
    }
    return true;
}

// Inserts the lines (each without its newline) at `at`, a line's start, indented as that line.
void InsertLines(std::string& t, std::size_t at, const std::vector<std::string>& lines, const std::string& indent)
{
    const std::string nl = Newline(t);
    std::string block;
    for (const std::string& line : lines) {
        block += indent + line + nl;
    }
    if (at == t.size() && !t.empty() && t.back() != '\n') {
        block = nl + block;
    }
    t.insert(at, block);
}

// Removes the statements (sorted or not), and the lines they leave empty.
void RemoveStatements(std::string& t, std::vector<Range> ranges)
{
    std::sort(ranges.begin(), ranges.end(), [](const Range& a, const Range& b) { return a.begin > b.begin; });
    for (Range r : ranges) {
        // A ';' right after it goes with it.
        std::size_t end = r.end;
        std::size_t k = end;
        while (k < t.size() && (t[k] == ' ' || t[k] == '\t')) {
            ++k;
        }
        if (k < t.size() && t[k] == ';') {
            end = k + 1;
        }
        const std::size_t lineBegin = LineBegin(t, r.begin);
        std::size_t lineEnd = t.find('\n', end);
        lineEnd = lineEnd == npos ? t.size() : lineEnd;
        const auto blank = [&](std::size_t from, std::size_t to) {
            for (std::size_t i = from; i < to; ++i) {
                if (t[i] != ' ' && t[i] != '\t' && t[i] != '\r') {
                    return false;
                }
            }
            return true;
        };
        if (blank(lineBegin, r.begin) && blank(end, lineEnd)) {
            t.erase(lineBegin, std::min(lineEnd + 1, t.size()) - lineBegin);
        } else {
            t.erase(r.begin, end - r.begin);
        }
    }
}

// The text of a statement with its object's name replaced by another (only where it starts: the arguments are kept).
std::string Renamed(const std::string& t, const Statement& st, const std::string& from, const std::string& to)
{
    std::string s = t.substr(st.begin, st.end - st.begin);
    const std::size_t at = s.find(from);
    if (at != npos) {
        s.replace(at, from.size(), to);
    }
    return s;
}

// The text split into lines (without newlines), for InsertLines.
void AppendLines(std::vector<std::string>& lines, const std::string& text)
{
    std::size_t from = 0;
    while (true) {
        const std::size_t nl = text.find('\n', from);
        std::string line = text.substr(from, nl == npos ? npos : nl - from);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
        if (nl == npos) {
            break;
        }
        from = nl + 1;
    }
}

// A picture's lines to copy: the statements of `name` in its range but the ones that put it somewhere.
struct ObjectLines {
    std::optional<Statement> creation;
    Range range;
    std::vector<Statement> statements;
};
ObjectLines LinesOf(const std::string& t, const std::vector<char>& code, const std::string& name, std::size_t at)
{
    ObjectLines lines;
    lines.range = ObjectRange(t, code, name, at, &lines.creation);
    for (const Statement& st : StatementsOf(t, code, name, lines.range.begin, lines.range.end)) {
        if (!st.assignment) {
            lines.statements.push_back(st);
        }
    }
    return lines;
}

// The line the first of the lines InsertLines puts at `at` gets.
int FirstInsertedLine(const std::string& t, std::size_t at)
{
    return LineOf(t, at) + (at == t.size() && !t.empty() && t.back() != '\n' ? 1 : 0);
}

// Moves the point tables and texts copied to the lines from `first` by (dx, dy): `calls` holds each one's line, from
// `first`, and kind.
bool ShiftCopies(std::string& t, int first, const std::vector<std::pair<int, CallKind>>& calls, float dx, float dy,
                 std::string& error)
{
    if (dx == 0.0f && dy == 0.0f) {
        return true;
    }
    for (const auto& [offset, kind] : calls) {
        if (!MoveCall(t, first + offset, kind, dx, dy, error)) {
            return false;
        }
    }
    return true;
}

// The kind of the call of a copied statement that places something, or false.
bool PlacingKind(const std::string& method, CallKind& kind)
{
    if (KindMatches(CallKind::Point, method)) {
        kind = CallKind::Point;
        return true;
    }
    if (KindMatches(CallKind::String, method)) {
        kind = CallKind::String;
        return true;
    }
    return false;
}

// Adds a copied statement's lines to `lines`, noting where a placing call starts.
void CopyStatement(std::vector<std::string>& lines, std::vector<std::pair<int, CallKind>>& placing, const std::string& text,
                   const std::string& method)
{
    CallKind kind;
    if (PlacingKind(method, kind)) {
        placing.emplace_back(static_cast<int>(lines.size()), kind);
    }
    AppendLines(lines, text);
}

}  // namespace

bool PictureToNewStatic(std::string& text, int addPicture, int addControl, const std::string& name, bool before,
                        std::string& error)
{
    if (!CheckControlName(name, error)) {
        return false;
    }
    const std::vector<char> code = CodeMask(text);
    CallOnLine picture, control;
    if (!FindAddCall(text, code, addPicture, CallKind::AddPicture, picture, error) ||
        !FindAddCall(text, code, addControl, CallKind::AddControl, control, error)) {
        return false;
    }
    if (picture.receiver != control.argument) {
        error = "line " + std::to_string(addPicture) + " adds the picture to " + picture.receiver + ", line " +
                std::to_string(addControl) + " adds " + control.argument;
        return false;
    }
    const std::string var = NewGlobal(text, "g_pStatic_" + name);
    const std::string& dialog = control.receiver;
    const std::vector<std::string> create = {var + " = " + dialog + ":CreateStatic()", var + ":SetName(\"" + name + "\")"};
    const std::string add = dialog + ":AddControl(" + var + ")";

    std::string out = text;
    // From the end of the text backwards, so earlier offsets hold: the picture's AddPicture names the new static, the
    // new static is added before or after the old one, and it is created before both its uses.
    out.replace(picture.statement.begin, picture.receiver.size(), var);
    const std::size_t controlLine = LineBegin(text, control.statement.begin);
    const std::size_t addAt = before ? controlLine : NextLine(text, control.statement.end);
    const std::size_t pictureLine = LineBegin(text, picture.statement.begin);
    const std::size_t createAt = std::min(pictureLine, controlLine);
    struct Insert {
        std::size_t at;
        std::vector<std::string> lines;
        std::string indent;
    };
    std::vector<Insert> inserts;
    if (createAt == addAt) {
        std::vector<std::string> lines = create;
        lines.push_back(add);
        inserts.push_back({createAt, lines, Indent(text, controlLine)});
    } else {
        inserts.push_back({createAt, create, Indent(text, createAt)});
        inserts.push_back({addAt, {add}, Indent(text, controlLine)});
    }
    std::sort(inserts.begin(), inserts.end(), [](const Insert& a, const Insert& b) { return a.at > b.at; });
    for (const Insert& insert : inserts) {
        // The picture's statement is on a line at or after createAt: its replacement above did not move earlier text.
        InsertLines(out, insert.at + (insert.at > picture.statement.begin ? var.size() - picture.receiver.size() : 0),
                    insert.lines, insert.indent);
    }
    text = std::move(out);
    return true;
}

bool AddStatic(std::string& text, const std::string& name, const std::string& texture, const std::string& key, float x,
               float y, std::string& error)
{
    if (!CheckControlName(name, error) || !CheckTexture(texture, key, error)) {
        return false;
    }
    const std::string var = NewGlobal(text, "g_pStatic_" + name);
    const std::string pic = NewGlobal(text, "g_pPicture_" + name);
    const std::string tex = key.empty() ? pic + ":SetTex2(\"" + texture + "\")"
                                        : pic + ":SetTex(\"" + texture + "\", \"" + key + "\")";
    InsertLines(text, text.size(),
                {var + " = g_pUIDialog:CreateStatic()", var + ":SetName(\"" + name + "\")", "g_pUIDialog:AddControl(" + var + ")",
                 pic + " = g_pUIDialog:CreatePicture()", var + ":AddPicture(" + pic + ")", tex,
                 pic + ":SetPoint({ USE_TEXTURE_SIZE = true, LEFT_TOP = D3DXVECTOR2(" + FormatNumber(x) + ", " +
                     FormatNumber(y) + ") })"},
                "");
    return true;
}

bool AddPictureAfter(std::string& text, int line, CallKind kind, const std::string& texture, const std::string& key,
                     float x, float y, std::string& error)
{
    if (!CheckTexture(texture, key, error)) {
        return false;
    }
    if (kind != CallKind::AddPicture && kind != CallKind::AddControl) {
        error = "a picture is added after an AddPicture or an AddControl";
        return false;
    }
    const std::vector<char> code = CodeMask(text);
    CallOnLine call;
    if (!FindAddCall(text, code, line, kind, call, error)) {
        return false;
    }
    const std::string& target = kind == CallKind::AddPicture ? call.receiver : call.argument;
    const std::string dialog = kind == CallKind::AddControl ? call.receiver : "g_pUIDialog";
    const std::string pic = NewGlobal(text, "g_pPicture_" + target);
    const std::string tex = key.empty() ? pic + ":SetTex2(\"" + texture + "\")"
                                        : pic + ":SetTex(\"" + texture + "\", \"" + key + "\")";
    // After the last line of what that call adds (the picture's SetTex and SetPoint after its AddPicture).
    std::size_t last = call.statement.end;
    const Range targetRange = ObjectRange(text, code, target, call.statement.begin);
    for (const Statement& st : LinesOf(text, code, call.argument, call.statement.begin).statements) {
        if (st.end <= targetRange.end) {
            last = std::max(last, st.end);
        }
    }
    const std::size_t at = NextLine(text, last);
    InsertLines(text, at,
                {pic + " = " + dialog + ":CreatePicture()", target + ":AddPicture(" + pic + ")", tex,
                 pic + ":SetPoint({ USE_TEXTURE_SIZE = true, LEFT_TOP = D3DXVECTOR2(" + FormatNumber(x) + ", " +
                     FormatNumber(y) + ") })"},
                Indent(text, LineBegin(text, call.statement.begin)));
    return true;
}

bool DuplicatePicture(std::string& text, int addPicture, float dx, float dy, std::string& error)
{
    const std::vector<char> code = CodeMask(text);
    CallOnLine call;
    if (!FindAddCall(text, code, addPicture, CallKind::AddPicture, call, error)) {
        return false;
    }
    const ObjectLines picture = LinesOf(text, code, call.argument, call.statement.begin);
    std::size_t last = call.statement.end;
    for (const Statement& st : picture.statements) {
        last = std::max(last, st.end);
    }
    const std::size_t at = NextLine(text, last);
    // The static's name must still name it there.
    const Range staticRange = ObjectRange(text, code, call.receiver, call.statement.begin);
    if (at > staticRange.end) {
        error = call.receiver + " names another object before the picture's last line";
        return false;
    }
    const std::string copy = NewGlobal(text, call.argument + "_Copy");
    const std::string dialog = picture.creation && !picture.creation->receiver.empty() ? picture.creation->receiver : "g_pUIDialog";
    std::vector<std::string> lines = {copy + " = " + dialog + ":CreatePicture()", call.receiver + ":AddPicture(" + copy + ")"};
    std::vector<std::pair<int, CallKind>> placing;
    for (const Statement& st : picture.statements) {
        CopyStatement(lines, placing, Renamed(text, st, call.argument, copy), st.method);
    }
    std::string out = text;
    InsertLines(out, at, lines, Indent(text, LineBegin(text, call.statement.begin)));
    if (!ShiftCopies(out, FirstInsertedLine(text, at), placing, dx, dy, error)) {
        return false;
    }
    text = std::move(out);
    return true;
}

bool DuplicateStatic(std::string& text, int addControl, const std::string& name, float dx, float dy, std::string& error)
{
    if (!CheckControlName(name, error)) {
        return false;
    }
    const std::vector<char> code = CodeMask(text);
    CallOnLine call;
    if (!FindAddCall(text, code, addControl, CallKind::AddControl, call, error)) {
        return false;
    }
    const std::string& dialog = call.receiver;
    const ObjectLines control = LinesOf(text, code, call.argument, call.statement.begin);
    const std::string var = NewGlobal(text, "g_pStatic_" + name);
    std::vector<std::string> lines = {var + " = " + dialog + ":CreateStatic()"};
    std::vector<std::pair<int, CallKind>> placing;
    std::size_t last = call.statement.end;
    std::string scratch = text;     // NewGlobal must see the names given so far
    bool named = false;
    for (const Statement& st : control.statements) {
        last = std::max(last, st.end);
        if (st.method == "SetName") {
            lines.push_back(var + ":SetName(\"" + name + "\")");
            named = true;
            continue;
        }
        if (st.method != "AddPicture") {
            CopyStatement(lines, placing, Renamed(text, st, call.argument, var), st.method);
            continue;
        }
        const std::string pictureName = Trimmed(text, st.args);
        if (!IsName(pictureName)) {
            error = "an AddPicture on line " + std::to_string(LineOf(text, st.begin)) + " does not take a plain name";
            return false;
        }
        const ObjectLines picture = LinesOf(text, code, pictureName, st.begin);
        const std::string copy = NewGlobal(scratch, pictureName + "_Copy");
        scratch += "\n" + copy;
        const std::string from =
            picture.creation && !picture.creation->receiver.empty() ? picture.creation->receiver : dialog;
        lines.push_back(copy + " = " + from + ":CreatePicture()");
        lines.push_back(var + ":AddPicture(" + copy + ")");
        for (const Statement& ps : picture.statements) {
            last = std::max(last, ps.end);
            CopyStatement(lines, placing, Renamed(text, ps, pictureName, copy), ps.method);
        }
    }
    if (!named) {
        lines.insert(lines.begin() + 1, var + ":SetName(\"" + name + "\")");
        for (auto& p : placing) {
            ++p.first;
        }
    }
    lines.push_back(dialog + ":AddControl(" + var + ")");
    const std::size_t at = NextLine(text, last);
    if (at > control.range.end) {
        error = call.argument + " names another object before the static's last line";
        return false;
    }
    std::string out = text;
    InsertLines(out, at, lines, Indent(text, LineBegin(text, call.statement.begin)));
    if (!ShiftCopies(out, FirstInsertedLine(text, at), placing, dx, dy, error)) {
        return false;
    }
    text = std::move(out);
    return true;
}

namespace {

// The ranges that remove an object put somewhere by `put`: its creation and lines when nothing else uses its name in
// its range, else nothing. Each picture the object adds is removed so too.
void RemovalOf(const std::string& t, const std::vector<char>& code, const std::string& name, const Statement& put,
               std::vector<Range>& ranges)
{
    const ObjectLines object = LinesOf(t, code, name, put.begin);
    std::vector<Range> mine = {{put.begin, put.end}};
    if (object.creation) {
        mine.push_back({object.creation->begin, object.creation->end});
    }
    for (const Statement& st : object.statements) {
        mine.push_back({st.begin, st.end});
    }
    std::vector<Range> pictures;
    for (const Statement& st : object.statements) {
        if (st.method == "AddPicture" && IsName(Trimmed(t, st.args))) {
            RemovalOf(t, code, Trimmed(t, st.args), st, pictures);
        }
    }
    if (UsedOutside(t, code, name, object.range, mine)) {
        ranges.push_back({put.begin, put.end});     // only what puts it there: it stays, not drawn
        return;
    }
    ranges.insert(ranges.end(), mine.begin(), mine.end());
    ranges.insert(ranges.end(), pictures.begin(), pictures.end());
}

// The ranges without duplicates and without those inside others.
std::vector<Range> Distinct(std::vector<Range> ranges)
{
    std::sort(ranges.begin(), ranges.end(), [](const Range& a, const Range& b) {
        return a.begin != b.begin ? a.begin < b.begin : a.end > b.end;
    });
    std::vector<Range> out;
    for (const Range& r : ranges) {
        if (!out.empty() && r.begin < out.back().end) {
            continue;
        }
        out.push_back(r);
    }
    return out;
}

}  // namespace

bool RemovePicture(std::string& text, int addPicture, std::string& error)
{
    const std::vector<char> code = CodeMask(text);
    CallOnLine call;
    if (!FindAddCall(text, code, addPicture, CallKind::AddPicture, call, error)) {
        return false;
    }
    std::vector<Range> ranges;
    RemovalOf(text, code, call.argument, call.statement, ranges);
    RemoveStatements(text, Distinct(ranges));
    return true;
}

bool RemoveControl(std::string& text, int addControl, std::string& error)
{
    const std::vector<char> code = CodeMask(text);
    CallOnLine call;
    if (!FindAddCall(text, code, addControl, CallKind::AddControl, call, error)) {
        return false;
    }
    std::vector<Range> ranges;
    RemovalOf(text, code, call.argument, call.statement, ranges);
    RemoveStatements(text, Distinct(ranges));
    return true;
}

}  // namespace x2::ui_editor
