#include "script_edit.h"

#include <doctest/doctest.h>

using x2::ui_editor::CallKind;
using x2::ui_editor::FormatNumber;
using x2::ui_editor::MoveCall;
using x2::ui_editor::ShiftExpression;

TEST_CASE("script edit: numbers are written as a script would")
{
    CHECK(FormatNumber(12.0f) == "12");
    CHECK(FormatNumber(-3.5f) == "-3.5");
    CHECK(FormatNumber(0.125f) == "0.125");
    CHECK(FormatNumber(-0.0f) == "0");
}

TEST_CASE("script edit: an expression keeps its constants")
{
    CHECK(ShiftExpression("746", 4) == "750");
    CHECK(ShiftExpression(" 746", -746) == " 0");
    CHECK(ShiftExpression("746 + MOVE_BG_Y", 4) == "750 + MOVE_BG_Y");
    CHECK(ShiftExpression("MOVE_BG_Y + 746", -6) == "MOVE_BG_Y + 740");
    CHECK(ShiftExpression("X - 10", 4) == "X - 6");
    CHECK(ShiftExpression("X - 10", 14) == "X + 4");
    CHECK(ShiftExpression("X + 10", -10) == "X");
    CHECK(ShiftExpression("10 + X", -10) == "X");
    CHECK(ShiftExpression("10 - X", -10) == "-X");
    CHECK(ShiftExpression("-5", 2) == "-3");
    CHECK(ShiftExpression("X", 4) == "X + 4");
    CHECK(ShiftExpression("X ", -4) == "X - 4 ");
    CHECK(ShiftExpression("A * 2", 1) == "A * 2 + 1");          // 2 is not a term of its own
    CHECK(ShiftExpression("(A + 2) * 3", 1) == "(A + 2) * 3 + 1");
    CHECK(ShiftExpression("1.5e2 + X", 1) == "151 + X");
    CHECK(ShiftExpression("T.Y + 3", 1) == "T.Y + 4");
    CHECK(ShiftExpression("X", 0) == "X");
}

TEST_CASE("script edit: a dialog's SetPos")
{
    std::string text = "\xEF\xBB\xBFg_pUIDialog:SetName(\"A\")\ng_pUIDialog:SetPos(100, BASE_Y + 20) -- (comment)\n";
    std::string error;
    REQUIRE_MESSAGE(MoveCall(text, 2, CallKind::DialogPos, 5, -20, error), error);
    CHECK(text == "\xEF\xBB\xBFg_pUIDialog:SetName(\"A\")\ng_pUIDialog:SetPos(105, BASE_Y) -- (comment)\n");
    CHECK_FALSE(MoveCall(text, 1, CallKind::DialogPos, 1, 1, error));
    CHECK(error == "no SetPos call on line 1");
    CHECK_FALSE(MoveCall(text, 9, CallKind::DialogPos, 1, 1, error));
}

TEST_CASE("script edit: a point table, on one line and over several")
{
    std::string error;
    std::string one = "p:SetPoint({ CHANGE_TIME = 0, ADD_SIZE_Y = WIDE_BG_Y, LEFT_TOP = D3DXVECTOR2(0, 746 + MOVE_BG_Y) })\n";
    REQUIRE_MESSAGE(MoveCall(one, 1, CallKind::Point, 3, 4, error), error);
    CHECK(one == "p:SetPoint({ CHANGE_TIME = 0, ADD_SIZE_Y = WIDE_BG_Y, LEFT_TOP = D3DXVECTOR2(3, 750 + MOVE_BG_Y) })\n");

    std::string several = "b:SetNormalPoint{\n"
                          "\tLEFT_TOP = D3DXVECTOR2(10, 20),\n"
                          "\t-- RIGHT_BOTTOM = D3DXVECTOR2(1, 1),\n"
                          "\tRIGHT_BOTTOM = D3DXVECTOR2(50, 60),\n"
                          "\tNAME = \"LEFT_TOP = D3DXVECTOR2(0, 0)\",\n"
                          "}\n";
    REQUIRE_MESSAGE(MoveCall(several, 1, CallKind::Point, -10, 1, error), error);
    CHECK(several == "b:SetNormalPoint{\n"
                     "\tLEFT_TOP = D3DXVECTOR2(0, 21),\n"
                     "\t-- RIGHT_BOTTOM = D3DXVECTOR2(1, 1),\n"
                     "\tRIGHT_BOTTOM = D3DXVECTOR2(40, 61),\n"
                     "\tNAME = \"LEFT_TOP = D3DXVECTOR2(0, 0)\",\n"
                     "}\n");

    std::string corners = "p:SetPoint({ IS_RECT = false, LEFT_TOP = D3DXVECTOR2(0, 0), RIGHT_TOP = D3DXVECTOR2(8, 0),\n"
                          "\tLEFT_BOTTOM = D3DXVECTOR2(0, 8), RIGHT_BOTTOM = D3DXVECTOR2(8, 8) })\n";
    REQUIRE_MESSAGE(MoveCall(corners, 1, CallKind::Point, 1, 2, error), error);
    CHECK(corners == "p:SetPoint({ IS_RECT = false, LEFT_TOP = D3DXVECTOR2(1, 2), RIGHT_TOP = D3DXVECTOR2(9, 2),\n"
                     "\tLEFT_BOTTOM = D3DXVECTOR2(1, 10), RIGHT_BOTTOM = D3DXVECTOR2(9, 10) })\n");
}

TEST_CASE("script edit: a point table without a corner gets a LEFT_TOP")
{
    std::string error;
    std::string one = "p:SetPoint({ USE_TEXTURE_SIZE = true })";
    REQUIRE_MESSAGE(MoveCall(one, 1, CallKind::Point, 3, 4, error), error);
    CHECK(one == "p:SetPoint({ LEFT_TOP = D3DXVECTOR2(3, 4), USE_TEXTURE_SIZE = true })");
    std::string empty = "p:SetPoint({})";
    REQUIRE_MESSAGE(MoveCall(empty, 1, CallKind::Point, 3, 4, error), error);
    CHECK(empty == "p:SetPoint({ LEFT_TOP = D3DXVECTOR2(3, 4) })");
    std::string several = "p:SetPoint({\n\tCHANGE_TIME = 0,\n})";
    REQUIRE_MESSAGE(MoveCall(several, 1, CallKind::Point, 3, 4, error), error);
    CHECK(several == "p:SetPoint({\n\tLEFT_TOP = D3DXVECTOR2(3, 4),\n\tCHANGE_TIME = 0,\n})");
}

TEST_CASE("script edit: AddString's POS")
{
    std::string error;
    std::string text = "s:AddString({\n\tMSG = \"(x)\",\n\tPOS = D3DXVECTOR2(0, 755 + MOVE_TEXT_Y),\n})\n";
    REQUIRE_MESSAGE(MoveCall(text, 1, CallKind::String, 2, 2, error), error);
    CHECK(text == "s:AddString({\n\tMSG = \"(x)\",\n\tPOS = D3DXVECTOR2(2, 757 + MOVE_TEXT_Y),\n})\n");
}

TEST_CASE("script edit: lines it cannot edit are refused, unchanged")
{
    std::string error;
    std::string two = "a:SetPoint({ LEFT_TOP = D3DXVECTOR2(0, 0) }) b:SetPoint({ LEFT_TOP = D3DXVECTOR2(1, 1) })";
    const std::string before = two;
    CHECK_FALSE(MoveCall(two, 1, CallKind::Point, 1, 1, error));
    CHECK(error == "more than one point table (Set...Point) call on line 1");
    CHECK(two == before);
    std::string notVector = "a:SetPoint({ LEFT_TOP = ORIGIN })";
    CHECK_FALSE(MoveCall(notVector, 1, CallKind::Point, 1, 1, error));
    CHECK(error == "LEFT_TOP is not a D3DXVECTOR2(x, y) on line 1");
    std::string commented = "-- a:SetPoint({ LEFT_TOP = D3DXVECTOR2(0, 0) })";
    CHECK_FALSE(MoveCall(commented, 1, CallKind::Point, 1, 1, error));
}
