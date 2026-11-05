#include "x2/servermanager/profile.h"

#include "x2/platform/json.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <filesystem>

using namespace x2::servermanager;

namespace {

// Same path once normalized: on Windows the profile's paths come back with backslashes.
bool SamePath(const std::string& a, const std::string& b)
{
    return std::filesystem::path(a).lexically_normal() == std::filesystem::path(b).lexically_normal();
}

#if defined(_WIN32)
constexpr const char* kAbsolute = "C:/abs/B";
#else
constexpr const char* kAbsolute = "/abs/B";
#endif

}  // namespace

TEST_CASE("profile: parsing and paths")
{
    std::string error;
    const std::string json = std::string(R"({
        "x2ctl": "./x2ctl",
        "servers": [
            {"name": "A", "directory": "A", "program": "./A", "args": ["DEV"]},
            {"name": "B", "directory": ")") + kAbsolute + R"(", "program": "./B.exe", "args": ["DEV", "0"],
             "after": ["A"], "control": "Bee"}
        ]})";
    const auto profile = ParseProfile(json, "/dist", error);
    REQUIRE_MESSAGE(profile, error);
    CHECK(SamePath(profile->x2ctl, "/dist/x2ctl"));
    REQUIRE(profile->servers.size() == 2);
    CHECK(SamePath(profile->servers[0].directory, "/dist/A"));
    CHECK(profile->servers[0].ControlName() == "A");
    CHECK(SamePath(profile->servers[1].directory, kAbsolute));
    CHECK(profile->servers[1].args == std::vector<std::string>{"DEV", "0"});
    CHECK(profile->servers[1].after == std::vector<std::string>{"A"});
    CHECK(profile->servers[1].ControlName() == "Bee");
    ServerSpec plain;
    plain.program = "./GameServer.exe";
    CHECK(plain.ControlName() == "GameServer");

    const auto bare = ParseProfile(R"({"servers":[]})", "/dist", error);
    REQUIRE(bare);
    CHECK(bare->x2ctl == "x2ctl");      // a bare name stays a PATH lookup
}

TEST_CASE("profile: invalid profiles")
{
    std::string error;
    for (const char* bad : {
             "[]",
             R"({"servers": 1})",
             R"({"servers": [{"name": "A"}]})",                                             // no program
             R"({"servers": [{"name": "A", "program": "a"}, {"name": "A", "program": "b"}]})",
             R"({"servers": [{"name": "A", "program": "a", "after": ["Z"]}]})",
             R"({"servers": [{"name": "A", "program": "a", "after": ["A"]}]})",
             R"({"servers": [{"name": "A", "program": "a", "after": "B"}]})",
             R"({"servers": [{"name": "A", "program": "a", "after": ["B"]}, {"name": "B", "program": "b", "after": ["C"]},
                             {"name": "C", "program": "c", "after": ["A"]}]})",
             R"({"servers": [{"name": "A", "host": "nowhere", "control": "A"}]})",                  // unknown host
             R"({"hosts": [{"name": "h"}], "servers": []})",                                       // no destination
             R"({"hosts": [{"name": "h", "destination": "-oProxyCommand=x"}], "servers": []})",
             R"({"hosts": [{"name": "h", "destination": "a"}, {"name": "h", "destination": "b"}], "servers": []})",
             R"({"hosts": [{"name": "h", "destination": "a", "ssh": "ssh"}], "servers": []})",
             R"({"hosts": [{"name": "h", "destination": "a", "port": 0}], "servers": []})",
             R"({"hosts": [{"name": "h", "destination": "a", "port": 70000}], "servers": []})",
             R"({"hosts": [{"name": "h", "destination": "a", "port": "22"}], "servers": []})",
             R"({"hosts": [{"name": "h", "destination": "a"}], "servers": [{"name": "A", "host": "h"}]})",  // no socket name
             R"({"servers": [{"name": "A", "program": "a", "start": "x"}]})",              // commands are for remote servers
             R"({"servers": [{"name": "A", "program": "a", "service": "a"}]})",
         }) {
        error.clear();
        CHECK_MESSAGE(!ParseProfile(bad, "", error), bad);
        CHECK_MESSAGE(!error.empty(), bad);
    }
}

TEST_CASE("profile: start order and the default package")
{
    const Profile profile = DefaultProfile("/dist");
    CHECK(profile.Validate().empty());
    std::vector<std::string> names;
    for (const int i : profile.StartOrder())
        names.push_back(profile.servers[static_cast<std::size_t>(i)].name);
    CHECK(names == std::vector<std::string>{"EPublisherServer", "LoginServer", "CenterServer", "GlobalServer", "GameServer",
                                            "ChannelServer"});
    CHECK(profile.servers[0].args == std::vector<std::string>{"DEV"});
    CHECK(profile.servers[4].args == std::vector<std::string>{"DEV", "0"});
    CHECK(SamePath(profile.servers[4].directory, "/dist/GameServer"));
    CHECK(SamePath(profile.x2ctl, "/dist/x2ctl"));

    // listed out of order: dependencies still come first
    std::string error;
    const auto shuffled = ParseProfile(R"({"servers": [{"name": "C", "program": "c", "after": ["B"]},
        {"name": "A", "program": "a"}, {"name": "B", "program": "b", "after": ["A"]}]})", "", error);
    REQUIRE(shuffled);
    CHECK(shuffled->StartOrder() == std::vector<int>{1, 2, 0});
}

TEST_CASE("profile: hosts and the x2ctl command")
{
    std::string error;
    const auto profile = ParseProfile(R"({
        "x2ctl": "x2ctl",
        "hosts": [{"name": "prod", "destination": "x2@10.0.0.5", "x2ctl": "/opt/x2 dist/x2ctl"},
                  {"name": "lab", "destination": "lab", "ssh": ["ssh", "-p", "2222"]}],
        "servers": [{"name": "Login", "program": "./LoginServer"},
                    {"name": "Game", "host": "prod", "control": "GameServer", "after": ["Login"]},
                    {"name": "Channel", "host": "lab", "program": "./ChannelServer", "directory": "/srv/x2"}]})",
                                      "/base", error);
    REQUIRE_MESSAGE(profile, error);
    REQUIRE(profile->hosts.size() == 2);
    CHECK(profile->hosts[1].x2ctl == "x2ctl");
    CHECK(profile->servers[1].host == "prod");
    CHECK(profile->servers[2].directory == "/srv/x2");    // not resolved: it is the other machine's
    CHECK(profile->servers[2].ControlName() == "ChannelServer");

    const auto local = profile->X2ctl("", {"attach", "LoginServer"});
    CHECK(local.program == "x2ctl");
    CHECK(local.args == std::vector<std::string>{"attach", "LoginServer"});

    const auto prod = profile->X2ctl("prod", {"attach", "GameServer"});
    CHECK(prod.program == "ssh");
    REQUIRE(prod.args.size() >= 3);
    CHECK(std::find(prod.args.begin(), prod.args.end(), "BatchMode=yes") != prod.args.end());
    CHECK(prod.args[prod.args.size() - 2] == "x2@10.0.0.5");
    CHECK(prod.args.back() == "'/opt/x2 dist/x2ctl' attach GameServer");   // one line for the remote shell

    const auto lab = profile->X2ctl("lab", {"list"});
    CHECK(lab.program == "ssh");
    CHECK(lab.args == std::vector<std::string>{"-p", "2222", "lab", "x2ctl list"});
}

TEST_CASE("profile: control socket folders")
{
    std::string error;
    const auto profile = ParseProfile(R"({
        "controlDir": "control/x2",
        "hosts": [{"name": "vps", "destination": "vps", "ssh": ["ssh"], "controlDir": "/srv/x2 docker/control/x2"}],
        "servers": [{"name": "Game", "host": "vps", "control": "GameServer"}]})",
                                      kAbsolute, error);
    REQUIRE_MESSAGE(profile, error);
    CHECK(SamePath(profile->controlDir, std::string(kAbsolute) + "/control/x2"));
    const auto local = profile->X2ctl("", {"list"});
    REQUIRE(local.args.size() == 2);
    CHECK(SamePath(local.args[0].substr(6), profile->controlDir));
    CHECK(local.args[0].rfind("--dir=", 0) == 0);
    CHECK(local.args[1] == "list");
    const auto remote = profile->X2ctl("vps", {"attach", "GameServer"});
    CHECK(remote.args == std::vector<std::string>{"vps", "x2ctl '--dir=/srv/x2 docker/control/x2' attach GameServer"});
}

TEST_CASE("profile: a Windows host's cmd.exe")
{
    std::string error;
    const auto profile = ParseProfile(R"({
        "hosts": [{"name": "win", "destination": "Administrator@win", "ssh": ["ssh"], "shell": "cmd",
                   "x2ctl": "C:\\x2 dist\\x2ctl.exe", "controlDir": "C:\\x2\\control 100%",
                   "start": "schtasks /run /tn \"x2 {service}\""}],
        "servers": [{"name": "Game", "host": "win", "control": "GameServer"}]})",
                                      "", error);
    REQUIRE_MESSAGE(profile, error);
    CHECK(profile->hosts[0].shell == HostSpec::Shell::Cmd);
    // the program in plain quotes; the arguments with carets before cmd's metacharacters, quotes included
    CHECK(profile->X2ctl("win", {"attach", "GameServer"}).args
          == std::vector<std::string>{"Administrator@win",
                                      R"("C:\x2 dist\x2ctl.exe" ^"--dir=C:\x2\control 100^%^" attach GameServer)"});
    // the start line is passed as written
    CHECK(profile->StartCommand(profile->servers[0]) == R"(schtasks /run /tn "x2 Game")");

    CHECK_FALSE(ParseProfile(R"({"hosts": [{"name": "h", "destination": "h", "shell": "bash"}], "servers": []})", "",
                             error));
    CHECK(error.find("shell") != std::string::npos);
}

TEST_CASE("profile: an ssh port")
{
    std::string error;
    const auto profile = ParseProfile(R"({
        "hosts": [{"name": "vps", "destination": "x2@vps", "port": 2222},
                  {"name": "lab", "destination": "lab", "ssh": ["ssh"], "port": 22022}],
        "servers": [{"name": "Game", "host": "vps", "control": "GameServer"}]})",
                                      "", error);
    REQUIRE_MESSAGE(profile, error);
    CHECK(profile->hosts[0].port == 2222);
    const auto vps = profile->X2ctl("vps", {"list"});
    REQUIRE(vps.args.size() >= 4);
    // after the default options (BatchMode, ControlMaster...), before the destination
    CHECK(std::find(vps.args.begin(), vps.args.end(), "BatchMode=yes") != vps.args.end());
    CHECK(vps.args[vps.args.size() - 4] == "-p");
    CHECK(vps.args[vps.args.size() - 3] == "2222");
    CHECK(vps.args[vps.args.size() - 2] == "x2@vps");
    CHECK(profile->X2ctl("lab", {"list"}).args == std::vector<std::string>{"-p", "22022", "lab", "x2ctl list"});
}

TEST_CASE("profile: a host's start and stop commands")
{
    std::string error;
    const auto profile = ParseProfile(R"({
        "hosts": [{"name": "vps", "destination": "x2@vps", "ssh": ["ssh"],
                   "start": "docker compose -f /opt/x2/compose.yml up -d {service}",
                   "stop": "docker compose -f /opt/x2/compose.yml stop {service}"},
                  {"name": "bare", "destination": "bare", "ssh": ["ssh"]}],
        "servers": [{"name": "LoginServer", "host": "vps", "service": "login", "control": "LoginServer"},
                    {"name": "GameServer", "host": "vps", "program": "./GameServer",
                     "start": "systemctl --user start x2@{name} # {control}"},
                    {"name": "Other", "host": "bare", "control": "Other"}]})",
                                      "", error);
    REQUIRE_MESSAGE(profile, error);
    const auto& servers = profile->servers;
    CHECK(profile->StartCommand(servers[0]) == "docker compose -f /opt/x2/compose.yml up -d login");
    CHECK(profile->StopCommand(servers[0]) == "docker compose -f /opt/x2/compose.yml stop login");
    // a server's own command replaces the host's; {service} is its name when not given
    CHECK(profile->StartCommand(servers[1]) == "systemctl --user start x2@GameServer # GameServer");
    CHECK(profile->StopCommand(servers[1]) == "docker compose -f /opt/x2/compose.yml stop GameServer");
    CHECK(profile->StartCommand(servers[2]).empty());
    CHECK(profile->StopCommand(servers[2]).empty());
    // the line goes to the remote shell as it is
    CHECK(profile->Shell("vps", "a 'b c'").args == std::vector<std::string>{"x2@vps", "a 'b c'"});
}

TEST_CASE("profile: a host's ssh key")
{
    std::string error;
    const auto profile = ParseProfile(R"({
        "hosts": [{"name": "vps", "destination": "x2@vps", "ssh": ["ssh"], "port": 2222, "identityFile": "~/.ssh/x2 key"}],
        "servers": [{"name": "Game", "host": "vps", "control": "GameServer"}]})",
                                      "", error);
    REQUIRE_MESSAGE(profile, error);
    CHECK(profile->hosts[0].identityFile == "~/.ssh/x2 key");    // as written: ssh expands ~
    CHECK(profile->X2ctl("vps", {"list"}).args ==
          std::vector<std::string>{"-p", "2222", "-i", "~/.ssh/x2 key", "-o", "IdentitiesOnly=yes", "x2@vps", "x2ctl list"});
}

TEST_CASE("profile: saved and read back the same")
{
    namespace fs = std::filesystem;
    const fs::path dir = fs::temp_directory_path() / "x2-profile-save";
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir / "dist");
    const std::string base = dir.string();
    const std::string outside = (dir.parent_path() / "x2-elsewhere" / "Game").lexically_normal().string();

    std::string error;
    const auto original = ParseProfile(std::string(R"({
        "x2ctl": "dist/x2ctl", "controlDir": "control",
        "hosts": [{"name": "win", "destination": "Administrator@win", "port": 2222, "shell": "cmd",
                   "x2ctl": "C:\\x2\\x2ctl.exe", "controlDir": "C:\\x2\\control", "identityFile": "~/.ssh/win",
                   "ssh": ["ssh", "-v"], "start": "schtasks /run /tn \"x2 {service}\"", "stop": "x"}],
        "servers": [{"name": "Login", "directory": "dist/LoginServer", "program": "./LoginServer", "args": ["DEV", "0"]},
                    {"name": "Here", "program": "./Here"},
                    {"name": "Far", "directory": )") + x2::platform::JsonString(outside) + R"(, "program": "./Far",
                     "after": ["Login"], "control": "FarServer"},
                    {"name": "Game", "host": "win", "control": "GameServer", "service": "game", "after": ["Login", "Far"],
                     "start": "a \"b\"", "stop": "c"}]})",
                                       base, error);
    REQUIRE_MESSAGE(original, error);
    const std::string file = (dir / "profile.json").string();
    REQUIRE_MESSAGE(SaveProfile(*original, file, error), error);
    CHECK_FALSE(fs::exists(file + ".tmp"));

    const auto loaded = LoadProfile(file, error);
    REQUIRE_MESSAGE(loaded, error);
    CHECK(SamePath(loaded->x2ctl, original->x2ctl));
    CHECK(SamePath(loaded->controlDir, original->controlDir));
    REQUIRE(loaded->hosts.size() == 1);
    const HostSpec& a = original->hosts[0];
    const HostSpec& b = loaded->hosts[0];
    CHECK(b.name == a.name);
    CHECK(b.destination == a.destination);
    CHECK(b.port == 2222);
    CHECK(b.shell == HostSpec::Shell::Cmd);
    CHECK(b.x2ctl == a.x2ctl);
    CHECK(b.controlDir == a.controlDir);
    CHECK(b.identityFile == a.identityFile);
    CHECK(b.ssh == a.ssh);
    CHECK(b.start == a.start);
    CHECK(b.stop == a.stop);
    REQUIRE(loaded->servers.size() == original->servers.size());
    for (std::size_t i = 0; i < loaded->servers.size(); ++i) {
        const ServerSpec& x = original->servers[i];
        const ServerSpec& y = loaded->servers[i];
        CAPTURE(x.name);
        CHECK(y.name == x.name);
        CHECK(y.host == x.host);
        CHECK(SamePath(y.directory, x.directory));
        CHECK(y.program == x.program);
        CHECK(y.args == x.args);
        CHECK(y.after == x.after);
        CHECK(y.control == x.control);
        CHECK(y.service == x.service);
        CHECK(y.start == x.start);
        CHECK(y.stop == x.stop);
    }

    // paths under the profile's folder stay relative to it, defaults are left out, and saving again changes nothing
    const std::string text = ProfileToJson(*loaded, base);
    CHECK(text.find(R"("x2ctl": "dist)") != std::string::npos);
    CHECK(text.find(R"("directory": "dist)") != std::string::npos);
    CHECK(text.find(x2::platform::JsonString(outside)) != std::string::npos);
    CHECK(text.find(R"("name": "Here",)") != std::string::npos);
    CHECK(text.find(R"("shell": "sh")") == std::string::npos);
    CHECK(text == ProfileToJson(*original, base));
    fs::remove_all(dir, ec);
}
