#include "x2/servermanager/protocol.h"

#include "x2/platform/json.h"

namespace x2::servermanager {

using x2::platform::Json;

const char* ToString(ServerState state)
{
    switch (state) {
    case ServerState::Loading: return "loading";
    case ServerState::Ready: return "ready";
    case ServerState::Stopping: return "stopping";
    case ServerState::Stopped: return "stopped";
    case ServerState::Unknown: break;
    }
    return "unknown";
}

ServerState ParseServerState(std::string_view text)
{
    if (text == "loading")
        return ServerState::Loading;
    if (text == "ready")
        return ServerState::Ready;
    if (text == "stopping")
        return ServerState::Stopping;
    if (text == "stopped")
        return ServerState::Stopped;
    return ServerState::Unknown;
}

const char* ToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Error: return "error";
    case LogLevel::Warn: return "warn";
    case LogLevel::Info: return "info";
    case LogLevel::Note: return "note";
    }
    return "info";
}

std::optional<LogLevel> ParseLogLevel(std::string_view text)
{
    if (text == "error")
        return LogLevel::Error;
    if (text == "warn")
        return LogLevel::Warn;
    if (text == "info")
        return LogLevel::Info;
    if (text == "note")
        return LogLevel::Note;
    return std::nullopt;
}

ServerMessage ParseServerMessage(std::string_view line)
{
    const auto doc = x2::platform::ParseJson(line);
    if (!doc || doc->GetType() != Json::Object)
        return RawLine{std::string(line)};

    const std::string type = doc->GetString("type");
    if (type == "hello") {
        HelloMessage m;
        m.protocol = static_cast<int>(doc->GetInteger("protocol"));
        m.server = doc->GetString("server");
        m.pid = doc->GetInteger("pid");
        m.state = ParseServerState(doc->GetString("state"));
        return m;
    }
    if (type == "log") {
        LogMessage m;
        m.time = doc->GetString("time");
        m.level = ParseLogLevel(doc->GetString("level")).value_or(LogLevel::Info);
        m.stream = doc->GetString("stream");
        m.func = doc->GetString("func");
        m.file = doc->GetString("file");
        m.line = static_cast<int>(doc->GetInteger("line"));
        m.message = doc->GetString("message");
        if (const Json* fields = doc->Find("fields"))
            for (const Json& f : fields->Items())
                m.fields.push_back({f.GetString("name"), f.GetString("value")});
        if (const Json* text = doc->Find("text"))
            for (const Json& t : text->Items())
                if (t.GetType() == Json::String)
                    m.text.push_back(t.Text());
        return m;
    }
    if (type == "output")
        return OutputMessage{doc->GetString("text")};
    if (type == "status")
        return StatusMessage{ParseServerState(doc->GetString("state"))};
    if (type == "result") {
        ResultMessage m;
        if (const Json* id = doc->Find("id"))
            if (const auto value = id->AsInteger())
                m.id = *value;
        m.ok = doc->GetBool("ok");
        m.output = doc->GetString("output");
        m.error = doc->GetString("error");
        return m;
    }
    return RawLine{std::string(line)};
}

std::string LuaRequest(std::int64_t id, std::string_view lua)
{
    std::string out = "{\"id\":" + std::to_string(id) + ",\"lua\":";
    x2::platform::AppendJsonString(out, lua);
    out += '}';
    return out;
}

std::string StopRequest(std::int64_t id)
{
    return "{\"id\":" + std::to_string(id) + ",\"stop\":true}";
}

}  // namespace x2::servermanager
