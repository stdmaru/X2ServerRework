#pragma once

// The servers ServerManager manages and how they depend on each other, read from a JSON file
// (docs/tools/servermanager.md#profile):
//   {"x2ctl": "x2ctl",
//    "servers": [{"name": "LoginServer", "directory": "LoginServer", "program": "./LoginServer",
//                 "args": ["DEV", "0"], "after": ["EPublisherServer"], "control": "LoginServer"}, ...],
//    "hosts": [{"name": "prod", "destination": "x2@10.0.0.5", "x2ctl": "/opt/x2/dist/x2ctl",
//               "controlDir": "/opt/x2/docker/control/x2"}],
//    "controlDir": "..."}
// Relative directories are relative to the profile's folder. A server with a "host" runs on that machine: ServerManager
// attaches to it over SSH and never starts it.

#include "x2/platform/process.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace x2::servermanager {

// A machine reached over SSH (decision T30): `<ssh...> <destination> '<x2ctl> attach <name>'`.
struct HostSpec {
    std::string name;                   // unique within the profile; what servers refer to
    std::string destination;            // what ssh connects to: host, user@host, or a Host of ~/.ssh/config
    int port = 0;                       // ssh -p; 0: ssh's own (22, or the Port of ~/.ssh/config)
    std::string x2ctl = "x2ctl";        // on that machine (a path, or a name in its PATH)
    std::string controlDir;             // its control sockets (x2ctl --dir); empty: x2ctl's default there
    std::vector<std::string> ssh;       // program and options before the destination; empty: DefaultSshCommand()
};

// ssh -T -o BatchMode=yes -o ConnectTimeout=10, and on Linux a shared connection per host (ControlMaster=auto,
// ControlPersist=60, the socket in the private ControlSocketDir()). Windows' OpenSSH has no ControlMaster.
std::vector<std::string> DefaultSshCommand();

struct ServerSpec {
    std::string name;                   // unique within the profile
    std::string host;                   // a HostSpec name; empty: this machine
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
    // Where this machine's servers have their control sockets, when not the default (servers in containers with a
    // bind-mounted socket folder). Only where ServerManager looks: the servers it starts use their own default.
    std::string controlDir;
    std::vector<ServerSpec> servers;
    std::vector<HostSpec> hosts;

    int Find(std::string_view name) const;   // index, or -1
    const HostSpec* FindHost(std::string_view name) const;
    // `x2ctl <args>` here (empty host) or on a host through ssh, as a child process.
    x2::platform::ProcessOptions X2ctl(std::string_view host, const std::vector<std::string>& args) const;
    // Duplicate or empty names, unknown or self dependencies, cycles, unknown hosts, local servers without a program.
    // Empty when valid.
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
