#include "x2/servermanager/profile.h"

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
             R"({"hosts": [{"name": "h", "destination": "a"}], "servers": [{"name": "A", "host": "h"}]})",  // no socket name
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
