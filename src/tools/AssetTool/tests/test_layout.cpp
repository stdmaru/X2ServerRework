#include "x2/assets/layout.h"

#include <doctest/doctest.h>

using namespace x2::assets;

TEST_CASE("assets: glob patterns")
{
    CHECK(GlobMatch("*", ""));
    CHECK(GlobMatch("*", "anything.tga"));
    CHECK(GlobMatch("DLG_*.lua", "dlg_ui_main.LUA"));
    CHECK(GlobMatch("*.x", "Motion_Elsword.X"));
    CHECK_FALSE(GlobMatch("*.x", "Motion_Elsword.xet"));
    CHECK(GlobMatch("mesh_*_cash*", "Mesh_Arme_Cash_Foot360A.tga"));
    CHECK(GlobMatch("data0?4.kom", "data034.kom"));
    CHECK_FALSE(GlobMatch("data0?4.kom", "data0344.kom"));
    CHECK(GlobMatch("a*b*c", "aXXbYYbZZc"));
    CHECK_FALSE(GlobMatch("a*b*c", "aXXbYYbZZ"));
    CHECK_FALSE(GlobMatch("", "x"));
    CHECK(GlobMatch("data0[12]?.kom", "DATA015.kom"));
    CHECK(GlobMatch("data0[12]?.kom", "data020.kom"));
    CHECK_FALSE(GlobMatch("data0[12]?.kom", "data035.kom"));
    CHECK(GlobMatch("data00[5-9].kom", "data007.kom"));
    CHECK_FALSE(GlobMatch("data00[5-9].kom", "data004.kom"));
    CHECK(GlobMatch("[a-c]*", "Bat.x"));
    CHECK(GlobMatch("x[-]y", "x-y"));
    CHECK(GlobMatch("a[b", "a[b"));  // no closing bracket: literal
    CHECK(GlobMatch("*[0-9].tga", "Title_10620.tga"));
}

TEST_CASE("assets: layout rules")
{
    Layout layout;
    std::string error;
    REQUIRE_MESSAGE(layout.Parse("# comment\n"
                                 "\n"
                                 "ui/scripts      DLG_*.lua\r\n"
                                 "sound/{archive} *.ogg        data07*.kom   # trailing comment\n"
                                 "sound/other     *.ogg\n"
                                 "lua/{archive}x  *.lua\n",
                                 error),
                    error);
    REQUIRE(layout.Rules().size() == 4);
    CHECK(layout.Rules()[1].line == 4);

    CHECK(layout.Classify("DLG_Main.lua", "data034.kom") == "ui/scripts");          // first match wins
    CHECK(layout.Classify("Item.lua", "data036.kom") == "lua/data036x");
    CHECK(layout.Classify("Hit.ogg", "DATA070.KOM") == "sound/DATA070");           // archive pattern, placeholder
    CHECK(layout.Classify("Hit.ogg", "data102.kom") == "sound/other");
    CHECK(layout.Classify("Wall.dds", "datai001.kom") == "_unsorted/datai001");    // fallback

    Layout empty;
    CHECK(empty.Classify("Wall.dds", "data001.kom") == "_unsorted/data001");
}

TEST_CASE("assets: bad layout rules")
{
    Layout layout;
    std::string error;
    CHECK_FALSE(layout.Parse("ui\n", error));
    CHECK(error.rfind("1:", 0) == 0);
    CHECK_FALSE(layout.Parse("ui *.lua data034.kom extra\n", error));
    for (const char* folder : {"/abs", "a/../b", "a//b", "a/", "./a", "a\\b", "c:/x", ".kom", ".kom/blobs"})
    {
        CAPTURE(folder);
        CHECK_FALSE(layout.Parse(std::string("ok *\n") + folder + " *.lua\n", error));
        CHECK(error.rfind("2:", 0) == 0);
    }
    CHECK(layout.Parse("ui/.kom *\n", error));  // only the top-level .kom is reserved
}
