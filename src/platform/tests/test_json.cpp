#include "x2/platform/json.h"

#include <doctest/doctest.h>

#include <string>

using x2::platform::Json;
using x2::platform::ParseJson;

TEST_CASE("json: a log line of the control protocol")
{
    const auto doc = ParseJson(
        R"({"type":"log","time":"2026-10-31 20:21:31","level":"note","line":181,"message":"한글 \"q\"",)"
        R"("fields":[{"name":"iSocket","value":"3"},{"name":"wstr","value":"é😀"}],"text":["a","b"]})");
    REQUIRE(doc);
    CHECK(doc->GetType() == Json::Object);
    CHECK(doc->GetString("type") == "log");
    CHECK(doc->GetInteger("line") == 181);
    CHECK(doc->GetString("message") == "한글 \"q\"");
    const Json* fields = doc->Find("fields");
    REQUIRE(fields);
    REQUIRE(fields->Items().size() == 2);
    CHECK(fields->Items()[1].GetString("value") == "é😀");
    CHECK(doc->Find("text")->Items()[1].Text() == "b");
    CHECK(doc->Find("missing") == nullptr);
    CHECK(doc->GetString("line", "x") == "x");     // wrong type: fallback
}

TEST_CASE("json: scalars, nesting and member order")
{
    const auto doc = ParseJson(R"( {"b":true,"n":null,"x":-1.5e3,"i":-42,"z":0,"a":[[],{}],"k":1,"k":2} )");
    REQUIRE(doc);
    CHECK(doc->GetBool("b"));
    CHECK(doc->Find("n")->IsNull());
    CHECK(doc->Find("x")->AsNumber() == doctest::Approx(-1500.0));
    CHECK_FALSE(doc->Find("x")->AsInteger());
    CHECK(doc->GetInteger("i") == -42);
    CHECK(doc->GetInteger("z", 7) == 0);
    CHECK(doc->Find("a")->Items()[1].GetType() == Json::Object);
    CHECK(doc->GetInteger("k") == 2);   // the last repeated key
    CHECK(doc->Members().front().first == "b");
    CHECK(ParseJson("\"s\"")->Text() == "s");
    CHECK(ParseJson("[1,2]")->Items().size() == 2);
    CHECK(ParseJson("12345678901234567890")->AsInteger() == std::nullopt);    // does not fit
}

TEST_CASE("json: syntax errors")
{
    for (const char* bad : {"", "{", "{\"a\":}", "[1,]", "{\"a\":1,}", "01", "1.", "-", "1e", "tru", "\"a", "{\"a\" 1}",
                            "[1] x", "{'a':1}", "\"\x01\"", "print(1)"})
        CHECK_MESSAGE(!ParseJson(bad), bad);
    std::string deep(65, '[');
    deep += std::string(65, ']');
    CHECK_FALSE(ParseJson(deep));
    std::string ok(64, '[');
    ok += std::string(64, ']');
    CHECK(ParseJson(ok));
}
