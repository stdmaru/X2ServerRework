// The client's live UI snapshot as the UI editor reads it (docs/tools/ui-editor.md).

#include "live.h"

#include <doctest/doctest.h>

#include <sstream>

TEST_CASE("ui editor: live snapshot, records under their dialog, texts unescaped")
{
    std::istringstream in(
        "scene\tXS_VILLAGE_MAP\n"
        "screen\t2560\t1080\n"
        "control\t0\t1\n"    // before any dialog: dropped
        "dialog\tdlg_a.lua\t207\t-12\t3\t1\r\n"
        "control\t0\t1\n"
        "control\t1\t0\n"
        "picture\t1\t0\t0\tDLG_UI_Common.tga\tBg\n"
        "picture\t1\t1\t1\t\t\n"
        "text\t1\t0\tLv.\\t12\\\\x\\nend\n"
        "text\t1\t1\t\n"
        "dialog\tdlg_b.lua\t0\t0\t0\t0\n");
    const auto live = x2::ui_editor::LiveSnapshot::Parse(in);
    CHECK(live.scene == "XS_VILLAGE_MAP");
    CHECK(live.screen[0] == 2560);
    CHECK(live.screen[1] == 1080);
    REQUIRE(live.dialogs.size() == 2);
    const auto& a = live.dialogs[0];
    CHECK(a.script == "dlg_a.lua");
    CHECK(a.pos[0] == 207.0f);
    CHECK(a.pos[1] == -12.0f);
    CHECK(a.layer == 3);
    CHECK(a.front);
    REQUIRE(a.controls.size() == 2);
    CHECK_FALSE(a.controls[1].shown);
    REQUIRE(a.pictures.size() == 2);
    CHECK(a.pictures[0].texture == "DLG_UI_Common.tga");
    CHECK(a.pictures[0].key == "Bg");
    CHECK_FALSE(a.pictures[0].shown);
    CHECK(a.pictures[1].texture.empty());
    REQUIRE(a.texts.size() == 2);
    CHECK(a.texts[0].text == "Lv.\t12\\x\nend");
    CHECK(a.texts[1].text.empty());
    CHECK(live.dialogs[1].controls.empty());
}
