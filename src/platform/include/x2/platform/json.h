// Minimal JSON for the servers' control protocol (decision T30): string escaping and flat objects.
#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>

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

} // namespace x2::platform
