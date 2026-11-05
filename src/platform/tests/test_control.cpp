#include "x2/platform/control.h"
#include "x2/platform/json.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace x2::platform;

TEST_CASE("JSON strings are escaped and stay UTF-8")
{
    CHECK(JsonString("a\"b\\c\n\t\x01") == "\"a\\\"b\\\\c\\n\\t\\u0001\"");
    CHECK(JsonString("\xED\x95\x9C") == "\"\xED\x95\x9C\"");
    CHECK(JsonString("bad\xFF") == "\"bad\xEF\xBF\xBD\"");
}

TEST_CASE("flat JSON objects are parsed")
{
    const auto obj = ParseJsonObject(R"( {"id": 12, "lua":"print(\"x\")\n", "stop":true, "n":null, "k":"é😀"} )");
    REQUIRE(obj);
    CHECK(obj->at("id").kind == JsonValue::Number);
    CHECK(obj->at("id").text == "12");
    CHECK(obj->at("lua").text == "print(\"x\")\n");
    CHECK(obj->at("stop").kind == JsonValue::Bool);
    CHECK(obj->at("n").kind == JsonValue::Null);
    CHECK(obj->at("k").text == "\xC3\xA9\xF0\x9F\x98\x80");
    CHECK(ParseJsonObject("{}"));
    CHECK_FALSE(ParseJsonObject("print(1)"));
    CHECK_FALSE(ParseJsonObject(R"({"a":[1]})"));
    CHECK_FALSE(ParseJsonObject(R"({"a":1} x)"));
}

TEST_CASE("control socket: greeting, lines both ways, broadcast, a second server refused")
{
    const std::string name = "x2-test-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count() % 100000);
    ControlServer server;
    std::mutex mutex;
    std::vector<std::string> received;
    std::string error;
    REQUIRE(server.Start(
        name, [] { return std::string("hello"); },
        [&](int client, std::string line) {
            std::lock_guard lock(mutex);
            received.push_back(line);
            server.Send(client, "echo " + line);
        },
        error));

    ControlServer second;
    CHECK_FALSE(second.Start(name, {}, {}, error));

    const std::vector<std::string> names = ListControlSockets();
    CHECK(std::find(names.begin(), names.end(), name) != names.end());

    ControlClient client;
    REQUIRE(client.Connect(name, error));
    std::string line;
    REQUIRE(client.ReadLine(line));
    CHECK(line == "hello");
    REQUIRE(client.WriteLine("한글 command"));
    REQUIRE(client.ReadLine(line));
    CHECK(line == "echo 한글 command");
    server.Broadcast("to everyone");
    REQUIRE(client.ReadLine(line));
    CHECK(line == "to everyone");
    {
        std::lock_guard lock(mutex);
        REQUIRE(received.size() == 1);
        CHECK(received[0] == "한글 command");
    }

    // a client that went away does not stop the next one
    client.Close();
    ControlClient again;
    REQUIRE(again.Connect(name, error));
    REQUIRE(again.ReadLine(line));
    CHECK(line == "hello");

    // stopping ends the connections and removes the socket
    server.Stop();
    CHECK_FALSE(again.ReadLine(line));
    ControlClient late;
    CHECK_FALSE(late.Connect(name, error));
}

TEST_CASE("control socket: a folder of its own (x2ctl --dir, X2_CONTROL_DIR)")
{
    const std::string name = "x2-dir-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count() % 100000);
    const std::filesystem::path dir = std::filesystem::temp_directory_path() / ("x2-ctl-" + name) / "x2";
    SetControlSocketDir(dir.string());
    CHECK(ControlSocketDir() == dir.string());
    CHECK(std::filesystem::is_directory(dir));     // created on demand

    std::string error;
    {
        ControlServer server;
        REQUIRE_MESSAGE(server.Start(name, [] { return std::string("hello"); }, {}, error), error);
        CHECK(std::filesystem::path(server.Path()).parent_path() == dir);
        const std::vector<std::string> names = ListControlSockets(dir.string());
        CHECK(std::find(names.begin(), names.end(), name) != names.end());
        ControlClient client;
        REQUIRE_MESSAGE(client.Connect(name, error), error);
        std::string line;
        REQUIRE(client.ReadLine(line));
        CHECK(line == "hello");

        SetControlSocketDir({});
        const std::vector<std::string> elsewhere = ListControlSockets();
        CHECK(std::find(elsewhere.begin(), elsewhere.end(), name) == elsewhere.end());
        SetControlSocketDir(dir.string());
    }
    SetControlSocketDir({});
    std::error_code ec;
    std::filesystem::remove_all(dir.parent_path(), ec);
}
