#include "x2/platform/xml.h"

#include <doctest/doctest.h>

#include <string>

using x2::platform::ParseXml;

TEST_CASE("xml: elements, attributes, children in order")
{
    // The shape of the client's AutoSetupGameOptionWeightConfig.xml (CRLF, tabs, spaces around '=').
    const std::string doc =
        "<AutoSetupGameOptionWeightConfig version=\"1.0\">\r\n"
        "\t<GRAPHIC_UNIT GpuWeight = \"0.8\" >\r\n"
        "\t\t<High score=\"80\" />\r\n\t\t<Middle score=\"40\" />\r\n\t\t<Low score=\"0\" />\r\n"
        "\t</GRAPHIC_UNIT>\r\n\r\n"
        "\t<ShowAllPlayer GpuWeight = \"1.0\" >\r\n\t\t<On score=\"50\" />\r\n\t\t<Off score=\"0\" />\r\n"
        "\t</ShowAllPlayer>\r\n"
        "</AutoSetupGameOptionWeightConfig>\r\n";
    const auto root = ParseXml(doc);
    REQUIRE(root);
    CHECK(root->name == "AutoSetupGameOptionWeightConfig");
    REQUIRE(root->Attribute("version"));
    CHECK(*root->Attribute("version") == "1.0");
    REQUIRE(root->children.size() == 2);
    const auto& unit = root->children[0];
    CHECK(unit.name == "GRAPHIC_UNIT");
    CHECK(*unit.Attribute("GpuWeight") == "0.8");
    REQUIRE(unit.children.size() == 3);
    CHECK(unit.children[1].name == "Middle");
    CHECK(*unit.children[1].Attribute("score") == "40");
    CHECK(unit.Child("Low") == &unit.children[2]);
    CHECK(unit.Child("Missing") == nullptr);
    CHECK(unit.Attribute("score") == nullptr);
    CHECK(root->children[1].Child("On")->Attribute("score")->compare("50") == 0);
}

TEST_CASE("xml: prolog, comments, text, entities, CDATA")
{
    const auto root = ParseXml("\xEF\xBB\xBF<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!-- c -->\n"
                               "<!DOCTYPE a [<!ELEMENT a ANY>]>\n"
                               "<a k='x &amp; &lt;y&gt; &#65;&#x42;'>one <!-- skip --><b/> two<![CDATA[<raw>]]></a>\n"
                               "<!-- end -->");
    REQUIRE(root);
    CHECK(*root->Attribute("k") == "x & <y> AB");
    CHECK(root->text == "one  two<raw>");
    REQUIRE(root->children.size() == 1);
    CHECK(root->children[0].name == "b");
}

TEST_CASE("xml: malformed documents fail with an offset")
{
    std::string error;
    CHECK_FALSE(ParseXml("<a><b></a>", &error));
    CHECK(error.find("mismatched") != std::string::npos);
    CHECK_FALSE(ParseXml("<a x=1/>", &error));
    CHECK_FALSE(ParseXml("<a>&nope;</a>", &error));
    CHECK_FALSE(ParseXml("<a/><b/>", &error));
    CHECK_FALSE(ParseXml("", &error));
    CHECK_FALSE(ParseXml("<a>", &error));
    CHECK(error.find("byte") != std::string::npos);
}
