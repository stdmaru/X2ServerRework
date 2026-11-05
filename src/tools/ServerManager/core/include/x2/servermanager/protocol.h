#pragma once

// Control protocol v1 of the servers, client side (decisions T30, docs/porting/console.md): JSON lines from the server
// (hello, log, output, status, result) and the requests ServerManager sends (lua, stop, history).

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace x2::servermanager {

inline constexpr int kProtocolVersion = 1;

// Server life cycle as the protocol reports it (`hello.state`, `status.state`).
enum class ServerState { Unknown, Loading, Ready, Stopping, Stopped };
const char* ToString(ServerState state);
ServerState ParseServerState(std::string_view text);   // Unknown for anything else

enum class LogLevel { Error, Warn, Info, Note };
const char* ToString(LogLevel level);
std::optional<LogLevel> ParseLogLevel(std::string_view text);

struct LogField {
    std::string name;
    std::string value;
};

struct HelloMessage {
    int protocol = 0;
    std::string server;
    std::int64_t pid = 0;
    ServerState state = ServerState::Unknown;
    std::optional<std::int64_t> seq;   // last numbered line sent before the greeting (absent: no history support)
};

struct LogMessage {
    std::optional<std::int64_t> seq;   // line number, shared with output, for merging the history
    std::string time;       // YYYY-MM-DD hh:mm:ss
    LogLevel level = LogLevel::Info;
    std::string stream;     // cerr, cout2...
    std::string func;
    std::string file;
    int line = 0;
    std::string message;
    std::vector<LogField> fields;
    std::vector<std::string> text;
};

struct OutputMessage {
    std::string text;
    std::optional<std::int64_t> seq;
};

struct StatusMessage {
    ServerState state = ServerState::Unknown;
};

struct ResultMessage {
    std::optional<std::int64_t> id;     // nullopt: `null`, or an id ServerManager did not send (not a number)
    bool ok = false;
    std::string output;
    std::string error;
};

// A line that is not a protocol message (not JSON, unknown type, or missing fields): shown as it is.
struct RawLine {
    std::string text;
};

using ServerMessage = std::variant<HelloMessage, LogMessage, OutputMessage, StatusMessage, ResultMessage, RawLine>;

ServerMessage ParseServerMessage(std::string_view line);

// Requests (one line each, without the newline).
std::string LuaRequest(std::int64_t id, std::string_view lua);
std::string StopRequest(std::int64_t id);
// The server's backlog of log and output lines numbered up to `upto`, then a result.
std::string HistoryRequest(std::int64_t id, std::int64_t upto);

}  // namespace x2::servermanager
