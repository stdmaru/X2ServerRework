// The client's live UI snapshot as the UI editor reads it (docs/tools/ui-editor.md).

#include "live.h"

#include <doctest/doctest.h>

#include <sstream>
#include <string>
#include <vector>

TEST_CASE("ui editor: live snapshot, records under their dialog, texts unescaped")
{
    std::istringstream in(
        "scene\tXS_VILLAGE_MAP\n"
        "screen\t2560\t1080\n"
        "control\t0\t1\n"    // before any dialog: dropped
        "dialog\tdlg_a.lua\t207\t-12\t3\t1\r\n"
        "control\t0\t1\n"
        "control\t1\t0\t0\t2\n"    // a static anchored right
        "picture\t1\t0\t0\tDLG_UI_Common.tga\tBg\n"
        "picture\t1\t1\t1\t\t\n"
        "text\t1\t0\tLv.\\t12\\\\x\\nend\n"
        "text\t1\t1\t\n"
        "dialog\tdlg_b.lua\t0\t0\t0\t0\t1\n");    // anchored left by code
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
    CHECK(a.anchor == -1);     // not written: older snapshots
    CHECK(a.controls[0].anchor == -1);
    CHECK(a.controls[1].anchor == 2);
    REQUIRE(a.pictures.size() == 2);
    CHECK(a.pictures[0].texture == "DLG_UI_Common.tga");
    CHECK(a.pictures[0].key == "Bg");
    CHECK_FALSE(a.pictures[0].shown);
    CHECK(a.pictures[1].texture.empty());
    REQUIRE(a.texts.size() == 2);
    CHECK(a.texts[0].text == "Lv.\t12\\x\nend");
    CHECK(a.texts[1].text.empty());
    CHECK(live.dialogs[1].controls.empty());
    CHECK(live.dialogs[1].anchor == 1);
}

TEST_CASE("ui editor: live snapshot, scripts code loaded and statics in full")
{
    std::istringstream in(
        "dialog\tdlg_ui_local_map_new.lua\t216\t132\t400\t1\n"
        "script\tdlg_map_local_ruben.lua\n"
        "control\t0\t1\t5\n"
        "picture\t0\t0\t1\tDLG_Map.tga\tBG\t0\t0\t100\t0\t0\t50\t100\t50\t0\t0\t0.5\t0\t0\t0.25\t0.5\t0.25\t1\t0.5\t0.5\t1\n"
        "text\t0\t0\t3\t17\t12\t8\t1\t1\t1\t1\tRelax\n"
        "text\t0\t1\t3\t17\t12\t8\t1\t1\t1\t1\t\n");
    const auto live = x2::ui_editor::LiveSnapshot::Parse(in);
    REQUIRE(live.dialogs.size() == 1);
    const auto& d = live.dialogs[0];
    CHECK(d.extraScripts == std::vector<std::string>{"dlg_map_local_ruben.lua"});
    CHECK(d.Key() == "dlg_ui_local_map_new.lua+dlg_map_local_ruben.lua");
    REQUIRE(d.controls.size() == 1);
    CHECK(d.controls[0].type == 5);
    REQUIRE(d.pictures.size() == 1);
    const auto& p = d.pictures[0];
    CHECK(p.full);
    CHECK(p.points[1][0] == 100.0f);
    CHECK(p.points[3][1] == 50.0f);
    CHECK(p.uv[1][0] == 0.5f);
    CHECK(p.uv[3][1] == 0.25f);
    CHECK(p.color[2] == 0.5f);
    CHECK(p.color[3] == 1.0f);
    REQUIRE(d.texts.size() == 2);
    CHECK(d.texts[0].full);
    CHECK(d.texts[0].font == 3);
    CHECK(d.texts[0].sortFlag == 17);
    CHECK(d.texts[0].pos[0] == 12.0f);
    CHECK(d.texts[0].text == "Relax");
    CHECK(d.texts[1].full);
    CHECK(d.texts[1].text.empty());
}
