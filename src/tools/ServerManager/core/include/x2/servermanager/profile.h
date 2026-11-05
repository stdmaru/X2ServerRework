#pragma once

// The servers ServerManager manages and how they depend on each other, read from a JSON file
// (docs/tools/servermanager.md#profile):
//   {"x2ctl": "x2ctl",
//    "servers": [{"name": "LoginServer", "directory": "LoginServer", "program": "./LoginServer",
//                 "args": ["DEV", "0"], "after": ["EPublisherServer"], "control": "LoginServer"}, ...],
//    "hosts": [{"name": "prod", "destination": "x2@10.0.0.5", "x2ctl": "/opt/x2/dist/x2ctl", "shell": "sh",
//               "controlDir": "/opt/x2/docker/control/x2", "identityFile": "~/.ssh/x2_prod", "port": 22,
//               "start": "docker compose -f /opt/x2/docker-compose.yml up -d {service}", "stop": "..."}],
//    "controlDir": "..."}
// Relative directories are relative to the profile's folder. A server with a "host" runs on that machine as its service:
// ServerManager attaches to it over SSH, and starts and stops it through that machine's commands when the profile has them.

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
    // What runs the remote command line there, and so how the x2ctl line is quoted: a POSIX shell, or cmd.exe (the
    // default shell of Windows' OpenSSH). The start and stop lines are passed as written, in that shell's syntax.
    enum class Shell { Sh, Cmd } shell = Shell::Sh;
    std::string controlDir;             // its control sockets (x2ctl --dir); empty: x2ctl's default there
    std::vector<std::string> ssh;       // program and options before the destination; empty: DefaultSshCommand()
    std::string identityFile;           // a private key on this machine (ssh -i, IdentitiesOnly); empty: ssh's own keys
    // Shell command lines run there to start and stop one of its servers, as its service manager does (docker compose,
    // systemctl); {service}, {name} and {control} become the server's. Empty: attach only, and stop through the protocol.
    std::string start;
    std::string stop;
};

// ssh -T -o BatchMode=yes -o ConnectTimeout=10 (BatchMode only without the window's askpass, AskpassBroker), and on Linux a shared connection per host (ControlMaster=auto,
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
    // Remote servers: their name in the host's service manager ({service}; empty: name), and start and stop commands
    // that replace the host's.
    std::string service;
    std::string start;
    std::string stop;

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
    // A remote server's start or stop command line with its placeholders filled in; empty when it has none.
    std::string StartCommand(const ServerSpec& server) const;
    std::string StopCommand(const ServerSpec& server) const;
    // `ssh <destination> <command line>` (the line goes to the remote shell as it is).
    x2::platform::ProcessOptions Shell(std::string_view host, const std::string& commandLine) const;
    // Duplicate or empty names, unknown or self dependencies, cycles, unknown hosts, local servers without a program or
    // with start/stop commands.
    // Empty when valid.
    std::string Validate() const;
    // Indices in an order where every server comes after what it depends on (profile order among independent ones).
    // Only for a valid profile.
    std::vector<int> StartOrder() const;
};

// `baseDir` resolves relative directories and the relative x2ctl path. nullopt and `error` when invalid.
std::optional<Profile> ParseProfile(std::string_view json, const std::string& baseDir, std::string& error);
std::optional<Profile> LoadProfile(const std::string& path, std::string& error);
// The profile as JSON that ParseProfile reads back the same: paths under `baseDir` relative to it, values that are
// the defaults left out.
std::string ProfileToJson(const Profile& profile, const std::string& baseDir);
// ProfileToJson, relative to the file's folder, written through a temporary file. False and `error` when it fails.
bool SaveProfile(const Profile& profile, const std::string& path, std::string& error);

// The six servers of the package under dist/ (docs/running.md) with the startup order of the original: the account
// service first, then the servers GameServer connects to, then GameServer, and ChannelServer (the client's entry) last.
Profile DefaultProfile(const std::string& distDir, const std::string& config = "DEV", const std::string& group = "0");

}  // namespace x2::servermanager
