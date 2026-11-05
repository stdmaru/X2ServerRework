// Minimal JSON for the servers' control protocol (decision T30): string escaping, flat objects (the servers read
// commands) and full documents (ServerManager reads log lines, which carry arrays).
#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace x2::platform {

// Appends `utf8` as a JSON string, quotes included. Invalid UTF-8 bytes become U+FFFD.
void AppendJsonString(std::string& out, std::string_view utf8);
std::string JsonString(std::string_view utf8);

// One value of a flat object: strings come back unescaped; numbers, true/false/null as their text.
struct JsonValue
{
    enum Kind { String, Number, Bool, Null } kind = Null;
    std::string text;
};

// Parses an object whose values are not objects or arrays ({"id":1,"lua":"print(1)"}). nullopt if it is not one.
std::optional<std::map<std::string, JsonValue>> ParseJsonObject(std::string_view text);

// A parsed JSON document. Numbers keep their text (Text()) and are also available as Number()/Integer().
class Json
{
public:
    enum Type { Null, Bool, Number, String, Array, Object };

    Type GetType() const { return m_type; }
    bool IsNull() const { return m_type == Null; }

    // Value of this node, or `fallback` when it has another type.
    bool AsBool(bool fallback = false) const;
    double AsNumber(double fallback = 0) const;
    std::optional<long long> AsInteger() const;     // a Number without fraction or exponent that fits
    const std::string& Text() const { return m_text; }  // String: the text; Number: as written; else empty

    const std::vector<Json>& Items() const { return m_items; }                          // Array
    const std::vector<std::pair<std::string, Json>>& Members() const { return m_members; }  // Object, in order

    // Object member (the last one, for repeated keys), or nullptr.
    const Json* Find(std::string_view key) const;
    // Shortcuts on Find: the member's value, or `fallback` when absent or of another type.
    std::string GetString(std::string_view key, std::string_view fallback = {}) const;
    long long GetInteger(std::string_view key, long long fallback = 0) const;
    bool GetBool(std::string_view key, bool fallback = false) const;

private:
    friend class JsonBuilder;
    Type m_type = Null;
    bool m_bool = false;
    std::string m_text;
    std::vector<Json> m_items;
    std::vector<std::pair<std::string, Json>> m_members;
};

// Parses one JSON value (RFC 8259) taking the whole text. nullopt on a syntax error or more than 64 levels of nesting.
std::optional<Json> ParseJson(std::string_view text);

} // namespace x2::platform
