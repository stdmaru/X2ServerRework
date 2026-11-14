// The anchor recorder's file as the UI editor reads it (docs/tools/ui-editor.md).

#include "record.h"

#include <doctest/doctest.h>

#include <sstream>

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
