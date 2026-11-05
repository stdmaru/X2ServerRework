#pragma once

// The servers ServerManager manages and how they depend on each other, read from a JSON file
// (docs/tools/servermanager.md#profile):
//   {"x2ctl": "x2ctl",
//    "servers": [{"name": "LoginServer", "directory": "LoginServer", "program": "./LoginServer",
//                 "args": ["DEV", "0"], "after": ["EPublisherServer"], "control": "LoginServer"}, ...]}
// Relative directories are relative to the profile's folder.

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace x2::servermanager {

struct ServerSpec {
    std::string name;                   // unique within the profile
    std::string directory;              // working directory (absolute once loaded)
    std::string program;                // ./LoginServer (relative to directory), or a path
    std::vector<std::string> args;      // before the console options ServerManager adds (--machine, --control=)
    std::vector<std::string> after;     // servers that must be ready before this one starts
    std::string control;                // control socket name; empty: the program's file name (the server's default)

    // The control socket name the server listens on.
    std::string ControlName() const;
};

struct Profile {
    std::string x2ctl = "x2ctl";        // for attaching to servers already running (path, or a name in PATH)
    std::vector<ServerSpec> servers;

    int Find(std::string_view name) const;   // index, or -1
    // Duplicate or empty names, unknown or self dependencies, cycles. Empty when valid.
    std::string Validate() const;
    // Indices in an order where every server comes after what it depends on (profile order among independent ones).
    // Only for a valid profile.
    std::vector<int> StartOrder() const;
};

// `baseDir` resolves relative directories and the relative x2ctl path. nullopt and `error` when invalid.
std::optional<Profile> ParseProfile(std::string_view json, const std::string& baseDir, std::string& error);
std::optional<Profile> LoadProfile(const std::string& path, std::string& error);

// The six servers of the package under dist/ (docs/running.md) with the startup order of the original: the account
// service first, then the servers GameServer connects to, then GameServer, and ChannelServer (the client's entry) last.
Profile DefaultProfile(const std::string& distDir, const std::string& config = "DEV", const std::string& group = "0");

}  // namespace x2::servermanager
