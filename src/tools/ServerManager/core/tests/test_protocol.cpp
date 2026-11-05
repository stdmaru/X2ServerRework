// Lines as the servers write them (docs/porting/console.md and dbg::logfile::RenderJson).

#include "x2/servermanager/protocol.h"

#include "x2/platform/json.h"

#include <doctest/doctest.h>

using namespace x2::servermanager;

TEST_CASE("protocol: hello, status, output")
{
    auto m = ParseServerMessage(R"({"type":"hello","protocol":1,"server":"ChannelServer","pid":577544,"state":"loading"})");
    REQUIRE(std::holds_alternative<HelloMessage>(m));
    const auto& hello = std::get<HelloMessage>(m);
    CHECK(hello.protocol == 1);
    CHECK(hello.server == "ChannelServer");
    CHECK(hello.pid == 577544);
    CHECK(hello.state == ServerState::Loading);

    m = ParseServerMessage(R"({"type":"status","state":"ready"})");
    REQUIRE(std::holds_alternative<StatusMessage>(m));
    CHECK(std::get<StatusMessage>(m).state == ServerState::Ready);
    CHECK(std::get<StatusMessage>(ParseServerMessage(R"({"type":"status","state":"weird"})")).state == ServerState::Unknown);

    m = ParseServerMessage(R"({"type":"output","text":"hello 한글"})");
    REQUIRE(std::holds_alternative<OutputMessage>(m));
    CHECK(std::get<OutputMessage>(m).text == "hello 한글");
    CHECK_FALSE(std::get<OutputMessage>(m).seq);
    CHECK_FALSE(hello.seq);
}

TEST_CASE("protocol: line numbers for the history")
{
    auto m = ParseServerMessage(R"({"type":"hello","protocol":1,"server":"A","pid":1,"state":"ready","seq":42})");
    CHECK(std::get<HelloMessage>(m).seq == 42);
    m = ParseServerMessage(R"({"seq":7,"type":"log","level":"warn","message":"m"})");
    CHECK(std::get<LogMessage>(m).seq == 7);
    m = ParseServerMessage(R"({"seq":8,"type":"output","text":"t"})");
    CHECK(std::get<OutputMessage>(m).seq == 8);
    m = ParseServerMessage(R"({"seq":-1,"type":"output","text":"t"})");
    CHECK_FALSE(std::get<OutputMessage>(m).seq);
}

TEST_CASE("protocol: log with fields and text")
{
    const auto m = ParseServerMessage(
        R"({"type":"log","time":"2026-10-31 20:21:31","level":"warn","stream":"cwarn","func":"OnLogin",)"
        R"("file":"/src/x.cpp","line":181,"message":"Packet Auth Fail Limit : 100",)"
        R"("fields":[{"name":"iUnitUID","value":"42"}],"text":["more"]})");
    REQUIRE(std::holds_alternative<LogMessage>(m));
    const auto& log = std::get<LogMessage>(m);
    CHECK(log.time == "2026-10-31 20:21:31");
    CHECK(log.level == LogLevel::Warn);
    CHECK(log.stream == "cwarn");
    CHECK(log.func == "OnLogin");
    CHECK(log.file == "/src/x.cpp");
    CHECK(log.line == 181);
    CHECK(log.message == "Packet Auth Fail Limit : 100");
    REQUIRE(log.fields.size() == 1);
    CHECK(log.fields[0].name == "iUnitUID");
    CHECK(log.fields[0].value == "42");
    REQUIRE(log.text.size() == 1);
    CHECK(log.text[0] == "more");
}

TEST_CASE("protocol: results")
{
    auto m = ParseServerMessage(R"({"type":"result","id":1,"ok":true,"output":"127.0.0.1\n","error":""})");
    REQUIRE(std::holds_alternative<ResultMessage>(m));
    CHECK(std::get<ResultMessage>(m).id == 1);
    CHECK(std::get<ResultMessage>(m).ok);
    CHECK(std::get<ResultMessage>(m).output == "127.0.0.1\n");

    m = ParseServerMessage(R"({"type":"result","id":null,"ok":false,"output":"","error":"not a command"})");
    CHECK_FALSE(std::get<ResultMessage>(m).id);
    CHECK_FALSE(std::get<ResultMessage>(m).ok);
    CHECK(std::get<ResultMessage>(m).error == "not a command");
    CHECK_FALSE(std::get<ResultMessage>(ParseServerMessage(R"({"type":"result","id":"abc","ok":true})")).id);
}

TEST_CASE("protocol: anything else is a raw line")
{
    for (const char* line : {"Segmentation fault", "", "[1,2]", R"({"type":"future"})", R"({"no":"type"})", "{bad"})
        CHECK_MESSAGE(std::holds_alternative<RawLine>(ParseServerMessage(line)), line);
    CHECK(std::get<RawLine>(ParseServerMessage("x y")).text == "x y");
}

TEST_CASE("protocol: requests the servers parse")
{
    CHECK(LuaRequest(7, "print(\"a\")\n") == R"({"id":7,"lua":"print(\"a\")\n"})");
    CHECK(StopRequest(8) == R"({"id":8,"stop":true})");
    CHECK(HistoryRequest(3, 120) == R"({"id":3,"history":true,"upto":120})");
    const auto history = x2::platform::ParseJsonObject(HistoryRequest(3, 120));
    REQUIRE(history);
    CHECK(history->at("history").text == "true");
    CHECK(history->at("upto").text == "120");
    // the servers read commands with ParseJsonObject (flat objects)
    const auto object = x2::platform::ParseJsonObject(LuaRequest(9, "x = '한글'"));
    REQUIRE(object);
    CHECK(object->at("lua").text == "x = '한글'");
    CHECK(object->at("id").text == "9");
}
