// The anchor recorder's file as the UI editor reads it (docs/tools/ui-editor.md).

#include "record.h"

#include <doctest/doctest.h>

#include <sstream>
#include <string>
#include <vector>

TEST_CASE("ui editor: recorded positions by script, without folder or case, each once")
{
    std::istringstream in(
        "# script\tcontrol\ttype\tmin_x\tmin_y\tmax_x\tmax_y\tdialog_positions\tplaced_by_code\n"
        "dlg_a.lua\tBg\t5\t0\t0\t10\t10\t207,100\t0\n"
        "dlg_b.lua\tSlot\t5\t0\t0\t10\t10\t373,138;582,138\t1\r\n"
        "dlg_b.lua\t#2\t5\t0\t0\t10\t10\t373,138;797,469\t0\n"
        "short\tline\n");
    const auto record = x2::ui_editor::Record::Parse(in);
    CHECK(record.Size() == 2);
    const auto* a = record.Positions("ui/scripts/DLG_A.lua");
    REQUIRE(a != nullptr);
    REQUIRE(a->size() == 1);
    CHECK(a->front().x == 207.0f);
    CHECK(a->front().y == 100.0f);
    CHECK_FALSE(record.Of("DLG_A.lua").placedByCode);
    const auto* b = record.Positions("DLG_B.lua");
    REQUIRE(b != nullptr);
    REQUIRE(b->size() == 3);
    CHECK((*b)[2].x == 797.0f);
    CHECK(record.Of("DLG_B.lua").placedByCode);
    CHECK(record.Positions("DLG_C.lua") == nullptr);
}

TEST_CASE("ui editor: recorded scenes, and the scripts seen in each")
{
    std::istringstream in(
        "dlg_a.lua\tBg\t5\t0\t0\t10\t10\t207,100\t0\tXS_LOGIN\n"
        "dlg_b.lua\tSlot\t5\t0\t0\t10\t10\t373,138\t1\tXS_VILLAGE_MAP,XS_LOGIN\r\n"
        "dlg_b.lua\t#2\t5\t0\t0\t10\t10\t373,138\t0\tXS_DUNGEON_GAME\n"
        "dlg_c.lua\tOld\t5\t0\t0\t10\t10\t1,1\t0\n");
    const auto record = x2::ui_editor::Record::Parse(in);
    CHECK(record.Scenes() == std::vector<std::string>{"XS_DUNGEON_GAME", "XS_LOGIN", "XS_VILLAGE_MAP"});
    CHECK(record.ScriptsIn("XS_LOGIN") == std::vector<std::string>{"dlg_a.lua", "dlg_b.lua"});
    CHECK(record.ScriptsIn("XS_DUNGEON_GAME") == std::vector<std::string>{"dlg_b.lua"});
    CHECK(record.Of("dlg_b.lua").scenes.size() == 3);
    CHECK(record.Of("dlg_c.lua").scenes.empty());
}
