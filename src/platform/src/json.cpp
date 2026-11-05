#include "x2/platform/json.h"

#include <charconv>
#include <cstdio>
#include <cstdlib>

namespace x2::platform {

// Fills the private members of Json for the parser.
class JsonBuilder
{
public:
    static Json& SetType(Json& node, Json::Type type) { node.m_type = type; return node; }
    static bool& BoolOf(Json& node) { return node.m_bool; }
    static std::string& TextOf(Json& node) { return node.m_text; }
    static std::vector<Json>& ItemsOf(Json& node) { return node.m_items; }
    static std::vector<std::pair<std::string, Json>>& MembersOf(Json& node) { return node.m_members; }
};

namespace {

void AppendCodePoint(std::string& out, char32_t cp)
{
    if (cp < 0x80) {
        out += static_cast<char>(cp);
    } else if (cp < 0x800) {
        out += static_cast<char>(0xC0 | (cp >> 6));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        out += static_cast<char>(0xE0 | (cp >> 12));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        out += static_cast<char>(0xF0 | (cp >> 18));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
}

// Length of the valid UTF-8 sequence at s[i], 0 if invalid.
std::size_t ValidUtf8(std::string_view s, std::size_t i)
{
    const unsigned char c = static_cast<unsigned char>(s[i]);
    std::size_t len = c < 0x80 ? 1 : (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xE ? 3 : (c >> 3) == 0x1E ? 4 : 0;
    if (len == 0 || i + len > s.size())
        return 0;
    for (std::size_t k = 1; k < len; ++k)
        if ((static_cast<unsigned char>(s[i + k]) & 0xC0) != 0x80)
            return 0;
    return len;
}

class Parser
{
public:
    explicit Parser(std::string_view text) : m_s(text) {}

    std::optional<std::map<std::string, JsonValue>> Object()
    {
        std::map<std::string, JsonValue> result;
        Space();
        if (!Eat('{'))
            return std::nullopt;
        Space();
        if (Eat('}'))
            return End(result);
        for (;;) {
            Space();
            std::string key;
            if (!String(key))
                return std::nullopt;
            Space();
            if (!Eat(':'))
                return std::nullopt;
            Space();
            JsonValue value;
            if (!Value(value))
                return std::nullopt;
            result[key] = std::move(value);
            Space();
            if (Eat('}'))
                return End(result);
            if (!Eat(','))
                return std::nullopt;
        }
    }

    // Full document (ParseJson).
    std::optional<Json> Document();

private:
    bool Tree(Json& node, int depth);
    bool NumberText(std::string& out);

    std::optional<std::map<std::string, JsonValue>> End(std::map<std::string, JsonValue>& result)
    {
        Space();
        if (m_pos != m_s.size())
            return std::nullopt;
        return std::move(result);
    }

    void Space()
    {
        while (m_pos < m_s.size() && (m_s[m_pos] == ' ' || m_s[m_pos] == '\t' || m_s[m_pos] == '\r' || m_s[m_pos] == '\n'))
            ++m_pos;
    }

    bool Eat(char c)
    {
        if (m_pos < m_s.size() && m_s[m_pos] == c) {
            ++m_pos;
            return true;
        }
        return false;
    }

    bool Literal(std::string_view word)
    {
        if (m_s.substr(m_pos, word.size()) != word)
            return false;
        m_pos += word.size();
        return true;
    }

    bool Hex4(char32_t& value)
    {
        if (m_pos + 4 > m_s.size())
            return false;
        value = 0;
        for (int i = 0; i < 4; ++i) {
            const char c = m_s[m_pos++];
            value <<= 4;
            if (c >= '0' && c <= '9')
                value |= static_cast<char32_t>(c - '0');
            else if (c >= 'a' && c <= 'f')
                value |= static_cast<char32_t>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F')
                value |= static_cast<char32_t>(c - 'A' + 10);
            else
                return false;
        }
        return true;
    }

    bool String(std::string& out)
    {
        if (!Eat('"'))
            return false;
        while (m_pos < m_s.size()) {
            const char c = m_s[m_pos++];
            if (c == '"')
                return true;
            if (static_cast<unsigned char>(c) < 0x20)
                return false;
            if (c != '\\') {
                out += c;
                continue;
            }
            if (m_pos >= m_s.size())
                return false;
            const char e = m_s[m_pos++];
            switch (e) {
            case '"': out += '"'; break;
            case '\\': out += '\\'; break;
            case '/': out += '/'; break;
            case 'b': out += '\b'; break;
            case 'f': out += '\f'; break;
            case 'n': out += '\n'; break;
            case 'r': out += '\r'; break;
            case 't': out += '\t'; break;
            case 'u': {
                char32_t cp = 0;
                if (!Hex4(cp))
                    return false;
                if (cp >= 0xD800 && cp <= 0xDBFF) {     // surrogate pair
                    char32_t low = 0;
                    if (!Literal("\\u") || !Hex4(low) || low < 0xDC00 || low > 0xDFFF)
                        return false;
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                }
                AppendCodePoint(out, cp);
                break;
            }
            default:
                return false;
            }
        }
        return false;
    }

    bool Value(JsonValue& value)
    {
        if (m_pos >= m_s.size())
            return false;
        const char c = m_s[m_pos];
        if (c == '"') {
            value.kind = JsonValue::String;
            return String(value.text);
        }
        if (Literal("true")) {
            value = {JsonValue::Bool, "true"};
            return true;
        }
        if (Literal("false")) {
            value = {JsonValue::Bool, "false"};
            return true;
        }
        if (Literal("null")) {
            value = {JsonValue::Null, "null"};
            return true;
        }
        const std::size_t start = m_pos;
        while (m_pos < m_s.size() && (std::string_view("+-.eE").find(m_s[m_pos]) != std::string_view::npos ||
                                      (m_s[m_pos] >= '0' && m_s[m_pos] <= '9')))
            ++m_pos;
        if (m_pos == start)
            return false;   // objects and arrays are not taken
        value = {JsonValue::Number, std::string(m_s.substr(start, m_pos - start))};
        return true;
    }


    std::string_view m_s;
    std::size_t m_pos = 0;
};

std::optional<Json> Parser::Document()
{
    Json root;
    Space();
    if (!Tree(root, 0))
        return std::nullopt;
    Space();
    if (m_pos != m_s.size())
        return std::nullopt;
    return root;
}

// RFC 8259 number: -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?
bool Parser::NumberText(std::string& out)
{
    const std::size_t start = m_pos;
    auto digits = [this] {
        const std::size_t from = m_pos;
        while (m_pos < m_s.size() && m_s[m_pos] >= '0' && m_s[m_pos] <= '9')
            ++m_pos;
        return m_pos > from;
    };
    Eat('-');
    if (Eat('0')) {
        // no leading zeros
    } else if (!digits()) {
        return false;
    }
    if (Eat('.') && !digits())
        return false;
    if (Eat('e') || Eat('E')) {
        if (!Eat('+'))
            Eat('-');
        if (!digits())
            return false;
    }
    out.assign(m_s.substr(start, m_pos - start));
    return true;
}

bool Parser::Tree(Json& node, int depth)
{
    if (depth >= 64 || m_pos >= m_s.size())     // 64 levels at most
        return false;
    const char c = m_s[m_pos];
    if (c == '"') {
        JsonBuilder::SetType(node, Json::String);
        return String(JsonBuilder::TextOf(node));
    }
    if (Literal("true")) {
        JsonBuilder::SetType(node, Json::Bool);
        JsonBuilder::BoolOf(node) = true;
        return true;
    }
    if (Literal("false")) {
        JsonBuilder::SetType(node, Json::Bool);
        return true;
    }
    if (Literal("null")) {
        JsonBuilder::SetType(node, Json::Null);
        return true;
    }
    if (Eat('[')) {
        JsonBuilder::SetType(node, Json::Array);
        auto& items = JsonBuilder::ItemsOf(node);
        Space();
        if (Eat(']'))
            return true;
        for (;;) {
            Space();
            items.emplace_back();
            if (!Tree(items.back(), depth + 1))
                return false;
            Space();
            if (Eat(']'))
                return true;
            if (!Eat(','))
                return false;
        }
    }
    if (Eat('{')) {
        JsonBuilder::SetType(node, Json::Object);
        auto& members = JsonBuilder::MembersOf(node);
        Space();
        if (Eat('}'))
            return true;
        for (;;) {
            Space();
            std::string key;
            if (!String(key))
                return false;
            Space();
            if (!Eat(':'))
                return false;
            Space();
            members.emplace_back(std::move(key), Json());
            if (!Tree(members.back().second, depth + 1))
                return false;
            Space();
            if (Eat('}'))
                return true;
            if (!Eat(','))
                return false;
        }
    }
    JsonBuilder::SetType(node, Json::Number);
    return NumberText(JsonBuilder::TextOf(node));
}

} // namespace

bool Json::AsBool(bool fallback) const
{
    return m_type == Bool ? m_bool : fallback;
}

double Json::AsNumber(double fallback) const
{
    if (m_type != Number)
        return fallback;
    return std::strtod(m_text.c_str(), nullptr);
}

std::optional<long long> Json::AsInteger() const
{
    if (m_type != Number || m_text.find_first_of(".eE") != std::string::npos)
        return std::nullopt;
    long long value = 0;
    const auto [end, ec] = std::from_chars(m_text.data(), m_text.data() + m_text.size(), value);
    if (ec != std::errc() || end != m_text.data() + m_text.size())
        return std::nullopt;
    return value;
}

const Json* Json::Find(std::string_view key) const
{
    for (auto it = m_members.rbegin(); it != m_members.rend(); ++it)
        if (it->first == key)
            return &it->second;
    return nullptr;
}

std::string Json::GetString(std::string_view key, std::string_view fallback) const
{
    const Json* value = Find(key);
    return std::string(value && value->m_type == String ? std::string_view(value->m_text) : fallback);
}

long long Json::GetInteger(std::string_view key, long long fallback) const
{
    const Json* value = Find(key);
    const auto integer = value ? value->AsInteger() : std::nullopt;
    return integer ? *integer : fallback;
}

bool Json::GetBool(std::string_view key, bool fallback) const
{
    const Json* value = Find(key);
    return value ? value->AsBool(fallback) : fallback;
}

std::optional<Json> ParseJson(std::string_view text)
{
    return Parser(text).Document();
}

void AppendJsonString(std::string& out, std::string_view utf8)
{
    out += '"';
    for (std::size_t i = 0; i < utf8.size();) {
        const unsigned char c = static_cast<unsigned char>(utf8[i]);
        if (c >= 0x80) {
            const std::size_t len = ValidUtf8(utf8, i);
            if (len == 0) {
                out += "\xEF\xBF\xBD";
                ++i;
            } else {
                out.append(utf8.substr(i, len));
                i += len;
            }
            continue;
        }
        switch (c) {
        case '"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if (c < 0x20 || c == 0x7F) {
                char buffer[8];
                std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                out += buffer;
            } else {
                out += static_cast<char>(c);
            }
            break;
        }
        ++i;
    }
    out += '"';
}

std::string JsonString(std::string_view utf8)
{
    std::string out;
    AppendJsonString(out, utf8);
    return out;
}

std::optional<std::map<std::string, JsonValue>> ParseJsonObject(std::string_view text)
{
    return Parser(text).Object();
}

} // namespace x2::platform
