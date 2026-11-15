#include "anchors_file.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <sstream>

namespace x2::ui_editor {

namespace {

constexpr int kColumns = 120;   // the file's width, tabs counted as 4 (as the project's code)
constexpr const char* kSection = "-- Set in the UI editor (tools/ui-editor)";
constexpr const char* kHeader =
    "\xEF\xBB\xBF-- Horizontal anchors of the dialogs (T44), by dialog script:\n"
    "--   [\"DLG_x.lua\"] = { DIALOG = \"LEFT\", CONTROLS = { ControlName = \"RIGHT\" } },\n"
    "-- Anchors: CENTER (the default), LEFT, RIGHT, STRETCH. A script's own SetAnchor wins over this file.\n"
    "-- STRETCH pieces span the screen: STRETCH_ZONES = { { min, max }, ... } (layout x relative to the dialog) are the\n"
    "-- parts that grow, the plain middles between end pieces, sharing the extra width; without zones a piece is scaled\n"
    "-- whole (a plain fill).\n"
    "-- BACKDROPS = { ControlName = fade } declares full-screen pictures: black beside them where the screen is wider,\n"
    "-- their outer `fade` layout units faded into it; narrower screens crop them.\n";

std::string ScriptKey(const std::string& script)
{
    std::string key = script;
    const std::size_t slash = key.find_last_of("/\\");
    if (slash != std::string::npos) {
        key.erase(0, slash + 1);
    }
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return key;
}

bool IsNameStart(char c)
{
    return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsNameChar(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsLuaName(const std::string& name)
{
    static const char* const kKeywords[] = {"and",  "break", "do",  "else", "elseif", "end",    "false", "for",
                                            "function", "if", "in", "local", "nil",   "not",    "or",    "repeat",
                                            "return", "then", "true", "until", "while"};
    if (name.empty() || !IsNameStart(name[0]) || !std::all_of(name.begin(), name.end(), IsNameChar)) {
        return false;
    }
    return std::none_of(std::begin(kKeywords), std::end(kKeywords), [&](const char* k) { return name == k; });
}

std::string Quote(const std::string& text)
{
    std::string out = "\"";
    for (const char c : text) {
        if (c == '"' || c == '\\') {
            out += '\\';
        }
        out += c;
    }
    return out + "\"";
}

std::string Key(const std::string& name)
{
    return IsLuaName(name) ? name : "[" + Quote(name) + "]";
}

std::string Number(float value)
{
    char text[32];
    std::snprintf(text, sizeof(text), "%.6g", static_cast<double>(value));
    return text;
}

std::string Zones(const std::vector<std::pair<float, float>>& zones)
{
    std::string out = "{";
    for (std::size_t i = 0; i < zones.size(); ++i) {
        out += (i == 0 ? " { " : ", { ") + Number(zones[i].first) + ", " + Number(zones[i].second) + " }";
    }
    return out + " }";
}

std::string Backdrops(const std::vector<std::pair<std::string, float>>& backdrops)
{
    std::string out = "{";
    for (std::size_t i = 0; i < backdrops.size(); ++i) {
        out += (i == 0 ? " " : ", ") + Key(backdrops[i].first) + " = " + Number(backdrops[i].second);
    }
    return out + " }";
}

std::size_t Width(const std::string& line)
{
    std::size_t width = 0;
    for (const char c : line) {
        width += c == '\t' ? 4 : 1;
    }
    return width;
}

}  // namespace

std::string AnchorEntry::Control(const std::string& name) const
{
    for (const auto& [control, anchor] : controls) {
        if (control == name) {
            return anchor;
        }
    }
    return {};
}

void AnchorEntry::SetControl(const std::string& name, const std::string& anchor)
{
    const auto it = std::find_if(controls.begin(), controls.end(), [&](const auto& c) { return c.first == name; });
    if (anchor.empty()) {
        if (it != controls.end()) {
            controls.erase(it);
        }
    } else if (it != controls.end()) {
        it->second = anchor;
    } else {
        controls.emplace_back(name, anchor);
    }
}

float AnchorEntry::Backdrop(const std::string& name) const
{
    for (const auto& [control, fade] : backdrops) {
        if (control == name) {
            return fade;
        }
    }
    return -1.0f;
}

void AnchorEntry::SetBackdrop(const std::string& name, float fade)
{
    const auto it = std::find_if(backdrops.begin(), backdrops.end(), [&](const auto& b) { return b.first == name; });
    if (fade < 0.0f) {
        if (it != backdrops.end()) {
            backdrops.erase(it);
        }
    } else if (it != backdrops.end()) {
        it->second = fade;
    } else {
        backdrops.emplace_back(name, fade);
    }
}

// The subset of Lua the file is written in: tables with string or name keys, strings, numbers and names, comments.
class Parser {
public:
    Parser(const std::string& text, std::vector<AnchorsFile::Table>& tables) : m_text(text), m_tables(tables) {}

    std::size_t pos = 0;
    std::string problem;

    void Skip()
    {
        while (pos < m_text.size()) {
            const char c = m_text[pos];
            if (std::isspace(static_cast<unsigned char>(c)) != 0) {
                ++pos;
            } else if (m_text.compare(pos, 2, "--") == 0) {
                pos += 2;
                std::size_t level = 0;
                if (LongBracket(level)) {
                    const std::string close = "]" + std::string(level, '=') + "]";
                    const std::size_t end = m_text.find(close, pos);
                    pos = end == std::string::npos ? m_text.size() : end + close.size();
                } else {
                    const std::size_t end = m_text.find('\n', pos);
                    pos = end == std::string::npos ? m_text.size() : end + 1;
                }
            } else {
                return;
            }
        }
    }

    bool String(std::string& out)
    {
        const char quote = m_text[pos++];
        out.clear();
        while (pos < m_text.size() && m_text[pos] != quote) {
            if (m_text[pos] == '\\' && pos + 1 < m_text.size()) {
                ++pos;
            } else if (m_text[pos] == '\n') {
                return Fail("unfinished string");
            }
            out += m_text[pos++];
        }
        if (pos >= m_text.size()) {
            return Fail("unfinished string");
        }
        ++pos;
        return true;
    }

    std::string Name()
    {
        const std::size_t begin = pos;
        while (pos < m_text.size() && IsNameChar(m_text[pos])) {
            ++pos;
        }
        return m_text.substr(begin, pos - begin);
    }

    bool Value(AnchorsFile::Field& field)
    {
        field.valueBegin = pos;
        const char c = pos < m_text.size() ? m_text[pos] : '\0';
        if (c == '"' || c == '\'') {
            field.kind = AnchorsFile::Field::Kind::String;
            if (!String(field.string)) {
                return false;
            }
        } else if (c == '{') {
            field.kind = AnchorsFile::Field::Kind::Table;
            if (!Table(field.table)) {
                return false;
            }
        } else if (c == '-' || c == '.' || std::isdigit(static_cast<unsigned char>(c)) != 0) {
            char* end = nullptr;
            field.number = std::strtod(m_text.c_str() + pos, &end);
            if (end == m_text.c_str() + pos) {
                return Fail("bad number");
            }
            field.kind = AnchorsFile::Field::Kind::Number;
            pos = static_cast<std::size_t>(end - m_text.c_str());
        } else if (IsNameStart(c)) {
            field.kind = AnchorsFile::Field::Kind::Other;     // true, false, nil, a global
            Name();
        } else {
            return Fail("unexpected character");
        }
        field.valueEnd = pos;
        return true;
    }

    bool Table(std::size_t& index)
    {
        index = m_tables.size();
        m_tables.emplace_back();
        m_tables[index].open = pos++;
        for (;;) {
            Skip();
            if (pos >= m_text.size()) {
                return Fail("unfinished table");
            }
            if (m_text[pos] == '}') {
                m_tables[index].close = pos++;
                return true;
            }
            AnchorsFile::Field field;
            field.begin = pos;
            if (m_text[pos] == '[' && !LongBracketAt(pos)) {
                ++pos;
                Skip();
                if (pos >= m_text.size() || (m_text[pos] != '"' && m_text[pos] != '\'')) {
                    return Fail("only string keys are read");
                }
                if (!String(field.key)) {
                    return false;
                }
                Skip();
                if (!Expect(']')) {
                    return false;
                }
                Skip();
                if (!Expect('=')) {
                    return false;
                }
                Skip();
            } else if (IsNameStart(m_text[pos])) {
                const std::size_t start = pos;
                const std::string name = Name();
                Skip();
                if (pos < m_text.size() && m_text[pos] == '=' && m_text.compare(pos, 2, "==") != 0) {
                    field.key = name;
                    ++pos;
                    Skip();
                } else {
                    pos = start;    // a positional name
                }
            }
            if (!Value(field)) {
                return false;
            }
            const std::size_t after = pos;
            Skip();
            if (pos < m_text.size() && (m_text[pos] == ',' || m_text[pos] == ';')) {
                field.sepEnd = ++pos;
            } else {
                field.sepEnd = field.valueEnd;
                pos = after;
            }
            m_tables[index].fields.push_back(std::move(field));
        }
    }

private:
    bool LongBracketAt(std::size_t at) const
    {
        std::size_t i = at + 1;
        while (i < m_text.size() && m_text[i] == '=') {
            ++i;
        }
        return i < m_text.size() && m_text[at] == '[' && m_text[i] == '[';
    }

    bool LongBracket(std::size_t& level)
    {
        if (pos >= m_text.size() || !LongBracketAt(pos)) {
            return false;
        }
        std::size_t i = pos + 1;
        while (m_text[i] == '=') {
            ++i;
        }
        level = i - pos - 1;
        pos = i + 1;
        return true;
    }

    bool Expect(char c)
    {
        if (pos >= m_text.size() || m_text[pos] != c) {
            return Fail(std::string("expected '") + c + "'");
        }
        ++pos;
        return true;
    }

    bool Fail(const std::string& what)
    {
        if (problem.empty()) {
            const auto line = std::count(m_text.begin(), m_text.begin() + static_cast<std::ptrdiff_t>(std::min(pos, m_text.size())), '\n') + 1;
            problem = "line " + std::to_string(line) + ": " + what;
        }
        return false;
    }

    const std::string& m_text;
    std::vector<AnchorsFile::Table>& m_tables;
};

AnchorsFile AnchorsFile::Parse(std::string text)
{
    AnchorsFile file;
    file.m_text = std::move(text);
    file.Reparse();
    return file;
}

void AnchorsFile::Reparse()
{
    m_scripts.clear();
    m_tables.clear();
    m_problem.clear();
    m_listEnd = std::string::npos;

    Parser parser(m_text, m_tables);
    if (m_text.compare(0, 3, "\xEF\xBB\xBF") == 0) {
        parser.pos = 3;
    }
    std::size_t list = std::string::npos;
    while (parser.pos < m_text.size()) {
        parser.Skip();
        if (parser.pos >= m_text.size()) {
            break;
        }
        const char c = m_text[parser.pos];
        if (c == '"' || c == '\'') {
            std::string ignored;
            if (!parser.String(ignored)) {
                break;
            }
        } else if (IsNameStart(c)) {
            if (parser.Name() == "UI_ANCHORS") {
                parser.Skip();
                if (parser.pos < m_text.size() && m_text[parser.pos] == '=') {
                    ++parser.pos;
                    parser.Skip();
                    if (parser.pos < m_text.size() && m_text[parser.pos] == '{' && parser.Table(list)) {
                        break;
                    }
                    list = std::string::npos;
                    break;
                }
            }
        } else {
            ++parser.pos;
        }
    }
    m_problem = parser.problem;
    if (list == std::string::npos) {
        if (m_problem.empty() && !m_text.empty() && m_text.find("UI_ANCHORS") != std::string::npos) {
            m_problem = "UI_ANCHORS is not a table";
        }
        return;
    }
    m_listEnd = m_tables[list].close;

    const auto problem = [this](const std::string& what) {
        if (m_problem.empty()) {
            m_problem = what;
        }
    };
    for (const Field& item : m_tables[list].fields) {
        if (item.key.empty() || item.kind != Field::Kind::Table) {
            problem("an entry that is not [\"script\"] = { ... }");
            continue;
        }
        Script script;
        script.key = ScriptKey(item.key);
        script.begin = item.begin;
        script.end = item.sepEnd;
        script.table = item.table;
        for (const Field& field : m_tables[item.table].fields) {
            if (field.key == "DIALOG" && field.kind == Field::Kind::String) {
                script.entry.dialog = field.string;
            } else if (field.key == "STRETCH_ZONES" && field.kind == Field::Kind::Table) {
                for (const Field& zone : m_tables[field.table].fields) {
                    const auto& pair = m_tables[zone.kind == Field::Kind::Table ? zone.table : 0].fields;
                    if (zone.kind == Field::Kind::Table && pair.size() == 2 && pair[0].kind == Field::Kind::Number &&
                        pair[1].kind == Field::Kind::Number) {
                        script.entry.zones.emplace_back(static_cast<float>(pair[0].number),
                                                        static_cast<float>(pair[1].number));
                    } else {
                        problem("bad stretch zone in " + item.key);
                    }
                }
            } else if (field.key == "BACKDROPS" && field.kind == Field::Kind::Table) {
                for (const Field& backdrop : m_tables[field.table].fields) {
                    if (!backdrop.key.empty() && backdrop.kind == Field::Kind::Number && backdrop.number >= 0.0) {
                        script.entry.SetBackdrop(backdrop.key, static_cast<float>(backdrop.number));
                    } else {
                        problem("bad backdrop in " + item.key);
                    }
                }
            } else if (field.key == "CONTROLS" && field.kind == Field::Kind::Table) {
                for (const Field& control : m_tables[field.table].fields) {
                    if (!control.key.empty() && control.kind == Field::Kind::String) {
                        script.entry.SetControl(control.key, control.string);
                    } else {
                        problem("bad control anchor in " + item.key);
                    }
                }
            } else {
                problem("unknown field " + field.key + " in " + item.key);
            }
        }
        m_scripts.push_back(std::move(script));
    }
}

AnchorsFile AnchorsFile::Load(const std::filesystem::path& path, std::string& error)
{
    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) {
        return Parse({});
    }
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = path.string() + ": cannot be read";
        return Parse({});
    }
    std::ostringstream text;
    text << in.rdbuf();
    return Parse(text.str());
}

bool AnchorsFile::Save(const std::filesystem::path& path, std::string& error)
{
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out << m_text;
    out.close();
    if (!out) {
        error = path.string() + ": cannot be written";
        return false;
    }
    m_changed = false;
    return true;
}

std::filesystem::path AnchorsFile::Find(const std::filesystem::path& gameFolder)
{
    const std::filesystem::path data = gameFolder / "data";
    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(
             data, std::filesystem::directory_options::skip_permission_denied, ec);
         !ec && it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
        const std::string name = it->path().filename().string();
        if (it->is_directory(ec) && !name.empty() && name[0] == '.') {
            it.disable_recursion_pending();     // client_data's .kom/
        } else if (ScriptKey(name) == "uianchors.lua" && it->is_regular_file(ec)) {
            return it->path();
        }
    }
    return data / "ui" / "scripts" / "UIAnchors.lua";
}

AnchorEntry AnchorsFile::Get(const std::string& script) const
{
    const std::string key = ScriptKey(script);
    for (auto it = m_scripts.rbegin(); it != m_scripts.rend(); ++it) {    // Lua keeps the last of a repeated key
        if (it->key == key) {
            return it->entry;
        }
    }
    return {};
}

bool AnchorsFile::Has(const std::string& script) const
{
    const std::string key = ScriptKey(script);
    return std::any_of(m_scripts.begin(), m_scripts.end(), [&](const Script& s) { return s.key == key; });
}

std::string AnchorsFile::Newline() const
{
    return m_text.find("\r\n") != std::string::npos ? "\r\n" : "\n";
}

void AnchorsFile::Replace(std::size_t begin, std::size_t end, const std::string& with)
{
    m_text.replace(begin, end - begin, with);
    m_changed = true;
    Reparse();
}

void AnchorsFile::Erase(std::size_t begin, std::size_t end)
{
    m_text.erase(begin, end - begin);
    std::size_t lineBegin = m_text.rfind('\n', begin == 0 ? 0 : begin - 1);
    lineBegin = lineBegin == std::string::npos || begin == 0 ? 0 : lineBegin + 1;
    std::size_t lineEnd = m_text.find('\n', begin);
    lineEnd = lineEnd == std::string::npos ? m_text.size() : lineEnd + 1;
    const auto blank = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    if (std::all_of(m_text.begin() + static_cast<std::ptrdiff_t>(lineBegin),
                    m_text.begin() + static_cast<std::ptrdiff_t>(lineEnd), blank)) {
        m_text.erase(lineBegin, lineEnd - lineBegin);    // nothing left on the line
    } else {
        std::size_t rest = begin;
        while (rest < m_text.size() && (m_text[rest] == ' ' || m_text[rest] == '\t')) {
            ++rest;
        }
        if (rest == m_text.size() || m_text[rest] == '\r' || m_text[rest] == '\n') {
            std::size_t from = begin;     // blanks left at the end of the line
            while (from > lineBegin && (m_text[from - 1] == ' ' || m_text[from - 1] == '\t')) {
                --from;
            }
            m_text.erase(from, rest - from);
        }
    }
    m_changed = true;
    Reparse();
}

void AnchorsFile::Set(const std::string& script, const AnchorEntry& entry)
{
    if (Get(script) == entry && (Has(script) || entry.Empty())) {
        return;
    }
    const std::string key = ScriptKey(script);
    const std::string nl = Newline();
    const auto find = [&]() -> const Script* {
        for (auto it = m_scripts.rbegin(); it != m_scripts.rend(); ++it) {
            if (it->key == key) {
                return &*it;
            }
        }
        return nullptr;
    };

    if (find() == nullptr) {
        if (entry.Empty()) {
            return;
        }
        if (m_listEnd == std::string::npos) {
            if (m_text.empty()) {
                m_text = kHeader;
            } else if (m_text.back() != '\n') {
                m_text += nl;
            }
            m_text += "UI_ANCHORS =" + nl + "{" + nl + "}" + nl;
            Reparse();
            if (m_listEnd == std::string::npos) {
                return;     // the text above it could not be read
            }
        }
        // The entry, on one line when it fits; else its controls on lines of their own, as the file's long entries.
        std::vector<std::string> head;
        if (!entry.dialog.empty()) {
            head.push_back("DIALOG = " + Quote(entry.dialog));
        }
        if (!entry.zones.empty()) {
            head.push_back("STRETCH_ZONES = " + Zones(entry.zones));
        }
        if (!entry.backdrops.empty()) {
            head.push_back("BACKDROPS = " + Backdrops(entry.backdrops));
        }
        std::vector<std::string> controls;
        for (const auto& [name, anchor] : entry.controls) {
            controls.push_back(Key(name) + " = " + Quote(anchor));
        }
        const auto join = [](const std::vector<std::string>& parts) {
            std::string out;
            for (const std::string& part : parts) {
                out += (out.empty() ? "" : ", ") + part;
            }
            return out;
        };
        std::vector<std::string> fields = head;
        if (!controls.empty()) {
            fields.push_back("CONTROLS = { " + join(controls) + " }");
        }
        std::string text = "\t[" + Quote(script) + "] = { " + join(fields) + " },";
        if (Width(text) > kColumns && !controls.empty()) {
            text = "\t[" + Quote(script) + "] = { " + (head.empty() ? "" : join(head) + ", ") + "CONTROLS = {" + nl;
            std::string line = "\t\t";
            for (const std::string& control : controls) {
                if (line != "\t\t" && Width(line) + 1 + control.size() + 1 > kColumns) {
                    text += line + nl;
                    line = "\t\t";
                }
                line += (line == "\t\t" ? "" : " ") + control + ",";
            }
            text += line + nl + "\t} },";
        }

        std::size_t at = m_listEnd;
        std::size_t lineBegin = m_text.rfind('\n', at == 0 ? 0 : at - 1);
        lineBegin = lineBegin == std::string::npos ? 0 : lineBegin + 1;
        const bool ownLine = std::all_of(m_text.begin() + static_cast<std::ptrdiff_t>(lineBegin),
                                         m_text.begin() + static_cast<std::ptrdiff_t>(at),
                                         [](char c) { return c == ' ' || c == '\t'; });
        std::string insert;
        if (ownLine) {
            at = lineBegin;
        } else {
            insert = nl;
        }
        if (m_text.find(kSection) == std::string::npos) {
            const std::string section = std::string("\t") + kSection + " ";
            insert += nl + section + std::string(kColumns - Width(section), '-') + nl + nl;
        }
        insert += text + nl;
        m_text.insert(at, insert);
        m_changed = true;
        Reparse();
        return;
    }

    if (entry.Empty()) {
        const Script* s = find();
        Erase(s->begin, s->end);
        return;
    }

    // One change at a time, the text read again after each.
    const auto table = [&]() -> std::size_t { return find()->table; };
    const auto fieldIndex = [&](std::size_t t, const std::string& name) -> std::size_t {
        const auto& fields = m_tables[t].fields;
        for (std::size_t i = 0; i < fields.size(); ++i) {
            if (fields[i].key == name) {
                return i;
            }
        }
        return std::string::npos;
    };
    const auto insertField = [&](std::size_t t, const std::string& text, std::size_t before) {
        const Table& tab = m_tables[t];
        if (tab.fields.empty()) {
            Replace(tab.open + 1, tab.close, " " + text + " ");
        } else if (before < tab.fields.size()) {
            Replace(tab.fields[before].begin, tab.fields[before].begin, text + ", ");
        } else {
            const Field& last = tab.fields.back();
            if (last.sepEnd > last.valueEnd) {
                Replace(last.sepEnd, last.sepEnd, " " + text + ",");
            } else {
                Replace(last.valueEnd, last.valueEnd, ", " + text);
            }
        }
    };
    const auto eraseField = [&](std::size_t t, std::size_t i) {
        const auto& fields = m_tables[t].fields;
        const Field& field = fields[i];
        const bool nextOnItsLine = i + 1 < fields.size() &&
                                   m_text.find('\n', field.sepEnd) < fields[i + 1].begin;
        if (i + 1 < fields.size() && !nextOnItsLine) {
            Erase(field.begin, fields[i + 1].begin);
        } else if (nextOnItsLine) {
            Erase(field.begin, field.sepEnd);   // the last of its line: the next line stays as it is
        } else if (i > 0 && field.sepEnd == field.valueEnd) {
            Erase(fields[i - 1].valueEnd, field.valueEnd);
        } else {
            Erase(field.begin, field.sepEnd);
        }
    };
    const auto setField = [&](const std::string& name, const std::string& value, std::size_t before) {
        const std::size_t t = table();
        const std::size_t i = fieldIndex(t, name);
        if (i == std::string::npos) {
            if (!value.empty()) {
                insertField(t, name + " = " + value, before);
            }
        } else if (value.empty()) {
            eraseField(t, i);
        } else {
            const Field& field = m_tables[t].fields[i];
            if (m_text.compare(field.valueBegin, field.valueEnd - field.valueBegin, value) != 0) {
                Replace(field.valueBegin, field.valueEnd, value);
            }
        }
    };

    AnchorEntry old = Get(script);
    if (old.dialog != entry.dialog) {
        setField("DIALOG", entry.dialog.empty() ? std::string() : Quote(entry.dialog), 0);
    }
    if (old.zones != entry.zones) {
        const std::size_t controls = fieldIndex(table(), "CONTROLS");
        setField("STRETCH_ZONES", entry.zones.empty() ? std::string() : Zones(entry.zones), controls);
    }
    if (old.backdrops != entry.backdrops) {
        const std::size_t controls = fieldIndex(table(), "CONTROLS");
        setField("BACKDROPS", entry.backdrops.empty() ? std::string() : Backdrops(entry.backdrops), controls);
    }
    if (old.controls != entry.controls) {
        if (entry.controls.empty()) {
            setField("CONTROLS", {}, std::string::npos);
            return;
        }
        if (fieldIndex(table(), "CONTROLS") == std::string::npos) {
            setField("CONTROLS", "{}", std::string::npos);
        }
        const auto controlsTable = [&]() { return m_tables[table()].fields[fieldIndex(table(), "CONTROLS")].table; };
        for (const auto& [name, anchor] : old.controls) {
            if (entry.Control(name).empty()) {
                const std::size_t t = controlsTable();
                const std::size_t i = fieldIndex(t, name);
                if (i != std::string::npos) {
                    eraseField(t, i);
                }
            }
        }
        for (const auto& [name, anchor] : entry.controls) {
            const std::size_t t = controlsTable();
            const std::size_t i = fieldIndex(t, name);
            if (i == std::string::npos) {
                insertField(t, Key(name) + " = " + Quote(anchor), std::string::npos);
            } else if (m_tables[t].fields[i].string != anchor) {
                Replace(m_tables[t].fields[i].valueBegin, m_tables[t].fields[i].valueEnd, Quote(anchor));
            }
        }
    }
}

}  // namespace x2::ui_editor
