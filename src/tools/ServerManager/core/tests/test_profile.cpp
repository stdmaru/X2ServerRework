#include "x2/servermanager/profile.h"

#include <doctest/doctest.h>

using namespace x2::servermanager;

TEST_CASE("profile: parsing and paths")
{
    std::string error;
    const auto profile = ParseProfile(R"({
        "x2ctl": "./x2ctl",
        "servers": [
            {"name": "A", "directory": "A", "program": "./A", "args": ["DEV"]},
            {"name": "B", "directory": "/abs/B", "program": "./B.exe", "args": ["DEV", "0"], "after": ["A"],
             "control": "Bee"}
        ]})", "/dist", error);
    REQUIRE_MESSAGE(profile, error);
    CHECK(profile->x2ctl == "/dist/x2ctl");
    REQUIRE(profile->servers.size() == 2);
    CHECK(profile->servers[0].directory == "/dist/A");
    CHECK(profile->servers[0].ControlName() == "A");
    CHECK(profile->servers[1].directory == "/abs/B");
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
    CHECK(profile.servers[4].directory == "/dist/GameServer");
    CHECK(profile.x2ctl == "/dist/x2ctl");

    // listed out of order: dependencies still come first
    std::string error;
    const auto shuffled = ParseProfile(R"({"servers": [{"name": "C", "program": "c", "after": ["B"]},
        {"name": "A", "program": "a"}, {"name": "B", "program": "b", "after": ["A"]}]})", "", error);
    REQUIRE(shuffled);
    CHECK(shuffled->StartOrder() == std::vector<int>{1, 2, 0});
}
