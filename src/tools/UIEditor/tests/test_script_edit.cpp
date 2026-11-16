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

TEST_CASE("script edit: a picture's SetPos")
{
    std::string text = "P:SetPos(D3DXVECTOR2(-245, BASE_Y))\nP:SetPos(1, 2)\n";
    std::string error;
    REQUIRE_MESSAGE(MoveCall(text, 1, CallKind::PicturePos, 45, 3, error), error);
    CHECK(text == "P:SetPos(D3DXVECTOR2(-200, BASE_Y + 3))\nP:SetPos(1, 2)\n");
    CHECK_FALSE(MoveCall(text, 2, CallKind::PicturePos, 1, 1, error));
    CHECK(error == "SetPos on line 2 does not take a D3DXVECTOR2(x, y)");
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

    std::string navigation = "g_pNavi:SetBGPoint_LUA({\n\tLEFT_TOP = D3DXVECTOR2(398, 95),\n})";   // the C++ name
    REQUIRE_MESSAGE(MoveCall(navigation, 1, CallKind::Point, 2, 0, error), error);
    CHECK(navigation == "g_pNavi:SetBGPoint_LUA({\n\tLEFT_TOP = D3DXVECTOR2(400, 95),\n})");

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

TEST_CASE("script edit: a picture's SetTex names another texture and key")
{
    std::string text = "p:SetTex(\"A.TGA\", \"KEY\")\np:SetTex( 'B.tga' , 'OTHER' ) -- SetTex(\"x\", \"y\")\n";
    std::string error;
    REQUIRE(x2::ui_editor::SetTexCall(text, 1, "A.TGA", "NEW_KEY", error));
    REQUIRE(x2::ui_editor::SetTexCall(text, 2, "C.TGA", "K2", error));
    CHECK(text == "p:SetTex(\"A.TGA\", \"NEW_KEY\")\np:SetTex( 'C.TGA' , 'K2' ) -- SetTex(\"x\", \"y\")\n");

    std::string named = "p:SetTex(TEX, \"KEY\")\n";
    CHECK_FALSE(x2::ui_editor::SetTexCall(named, 1, "A.TGA", "K", error));
    CHECK(named == "p:SetTex(TEX, \"KEY\")\n");
    std::string two = "p:SetTex(\"A\", \"B\") q:SetTex(\"A\", \"B\")\n";
    CHECK_FALSE(x2::ui_editor::SetTexCall(two, 1, "A", "K", error));
    std::string plain = "p:SetTex(\"A\", \"B\")\n";
    CHECK_FALSE(x2::ui_editor::SetTexCall(plain, 1, "A", "K\"", error));
    CHECK_FALSE(MoveCall(plain, 1, CallKind::Texture, 1, 1, error));
}

TEST_CASE("script edit: a point table grows by its edges")
{
    std::string error;
    // The texture's size: LEFT_TOP moves with the left and top edges, ADD_SIZE_X/Y take the growth.
    std::string text = "p:SetPoint({ ADD_SIZE_Y = 480, CHANGE_TIME = 0, LEFT_TOP = D3DXVECTOR2(0, 55) })\n";
    REQUIRE(x2::ui_editor::ResizeCall(text, 1, 0, 0, 0, 20, error));
    CHECK(text == "p:SetPoint({ ADD_SIZE_Y = 500, CHANGE_TIME = 0, LEFT_TOP = D3DXVECTOR2(0, 55) })\n");
    REQUIRE(x2::ui_editor::ResizeCall(text, 1, -10, 0, 0, 0, error));
    CHECK(text == "p:SetPoint({ ADD_SIZE_X = 10, ADD_SIZE_Y = 500, CHANGE_TIME = 0, LEFT_TOP = D3DXVECTOR2(-10, 55) })\n");

    std::string bare = "p:SetPoint({ CHANGE_TIME = 0 })\n";
    REQUIRE(x2::ui_editor::ResizeCall(bare, 1, 0, 5, 0, 0, error));
    CHECK(bare == "p:SetPoint({ ADD_SIZE_Y = -5, LEFT_TOP = D3DXVECTOR2(0, 5), CHANGE_TIME = 0 })\n");

    // Corners given: each moves with its edges.
    std::string corners = "p:SetPoint({ USE_TEXTURE_SIZE = FALSE, LEFT_TOP = D3DXVECTOR2(10, 20), "
                          "RIGHT_BOTTOM = D3DXVECTOR2(60, 40 + Y) })\n";
    REQUIRE(x2::ui_editor::ResizeCall(corners, 1, 0, -2, 5, 3, error));
    CHECK(corners == "p:SetPoint({ USE_TEXTURE_SIZE = FALSE, LEFT_TOP = D3DXVECTOR2(10, 18), "
                     "RIGHT_BOTTOM = D3DXVECTOR2(65, 43 + Y) })\n");
    std::string four = "p:SetPoint({ USE_TEXTURE_SIZE = false, IS_RECT = false, LEFT_TOP = D3DXVECTOR2(0, 0),\n"
                       "  RIGHT_TOP = D3DXVECTOR2(10, 0), LEFT_BOTTOM = D3DXVECTOR2(0, 10), RIGHT_BOTTOM = D3DXVECTOR2(10, 10) })\n";
    REQUIRE(x2::ui_editor::ResizeCall(four, 1, 0, 0, 2, 4, error));
    CHECK(four == "p:SetPoint({ USE_TEXTURE_SIZE = false, IS_RECT = false, LEFT_TOP = D3DXVECTOR2(0, 0),\n"
                  "  RIGHT_TOP = D3DXVECTOR2(12, 0), LEFT_BOTTOM = D3DXVECTOR2(0, 14), RIGHT_BOTTOM = D3DXVECTOR2(12, 14) })\n");

    std::string noCorner = "p:SetPoint({ USE_TEXTURE_SIZE = FALSE, LEFT_TOP = D3DXVECTOR2(0, 0) })\n";
    CHECK_FALSE(x2::ui_editor::ResizeCall(noCorner, 1, 0, 0, 1, 1, error));
    std::string unknown = "p:SetPoint({ USE_TEXTURE_SIZE = flag, LEFT_TOP = D3DXVECTOR2(0, 0) })\n";
    CHECK_FALSE(x2::ui_editor::ResizeCall(unknown, 1, 0, 0, 1, 1, error));
    CHECK(unknown == "p:SetPoint({ USE_TEXTURE_SIZE = flag, LEFT_TOP = D3DXVECTOR2(0, 0) })\n");
}

namespace {

// DLG_Dungeon_Ending_Event_Speech.lua's start, as the client_data had it (2026-11-16).
const std::string kSpeech = "\xEF\xBB\xBF"
                            "g_pUIDialog:SetName(\"DLG_Dungeon_Ending_Event_Speech\")\n"
                            "g_pStaticSpeech = g_pUIDialog:CreateStatic()\n"
                            "g_pUIDialog:AddControl(g_pStaticSpeech)\n"
                            "g_pStaticSpeech:SetName(\"Static_Ending_Speech\")\n"
                            "g_pPictureSpeech_Shadow = g_pUIDialog:CreatePicture()\n"
                            "g_pStaticSpeech:AddPicture(g_pPictureSpeech_Shadow)\n"
                            "g_pPictureSpeech_Shadow:SetTex(\"DLG_Common_New_Texture64.tga\", \"Shadow\")\n"
                            "g_pPictureSpeech_Shadow:SetPoint({ ADD_SIZE_X = 1024, LEFT_TOP = D3DXVECTOR2(0, 530) })\n"
                            "g_pPictureSpeech_Left = g_pUIDialog:CreatePicture()\n"
                            "g_pStaticSpeech:AddPicture(g_pPictureSpeech_Left)\n"
                            "g_pPictureSpeech_Left:SetTex(\"DLG_Common_New_Texture64.tga\", \"Speech_Popup_Left\")\n"
                            "g_pPictureSpeech_Left:SetPoint({ LEFT_TOP = D3DXVECTOR2(59, 562) })\n"
                            "g_pStaticSpeech:AddString({\n"
                            "\tPOS = D3DXVECTOR2(90, 592),\n"
                            "})\n";

}  // namespace

TEST_CASE("script edit: a picture moved to a static of its own")
{
    std::string error;
    std::string before = kSpeech;
    REQUIRE_MESSAGE(x2::ui_editor::PictureToNewStatic(before, 6, 3, "Static_Ending_Speech_Shadow", true, error), error);
    CHECK(before == "\xEF\xBB\xBF"
                    "g_pUIDialog:SetName(\"DLG_Dungeon_Ending_Event_Speech\")\n"
                    "g_pStaticSpeech = g_pUIDialog:CreateStatic()\n"
                    "g_pStatic_Static_Ending_Speech_Shadow = g_pUIDialog:CreateStatic()\n"
                    "g_pStatic_Static_Ending_Speech_Shadow:SetName(\"Static_Ending_Speech_Shadow\")\n"
                    "g_pUIDialog:AddControl(g_pStatic_Static_Ending_Speech_Shadow)\n"
                    "g_pUIDialog:AddControl(g_pStaticSpeech)\n"
                    "g_pStaticSpeech:SetName(\"Static_Ending_Speech\")\n"
                    "g_pPictureSpeech_Shadow = g_pUIDialog:CreatePicture()\n"
                    "g_pStatic_Static_Ending_Speech_Shadow:AddPicture(g_pPictureSpeech_Shadow)\n"
                    "g_pPictureSpeech_Shadow:SetTex(\"DLG_Common_New_Texture64.tga\", \"Shadow\")\n"
                    "g_pPictureSpeech_Shadow:SetPoint({ ADD_SIZE_X = 1024, LEFT_TOP = D3DXVECTOR2(0, 530) })\n"
                    "g_pPictureSpeech_Left = g_pUIDialog:CreatePicture()\n"
                    "g_pStaticSpeech:AddPicture(g_pPictureSpeech_Left)\n"
                    "g_pPictureSpeech_Left:SetTex(\"DLG_Common_New_Texture64.tga\", \"Speech_Popup_Left\")\n"
                    "g_pPictureSpeech_Left:SetPoint({ LEFT_TOP = D3DXVECTOR2(59, 562) })\n"
                    "g_pStaticSpeech:AddString({\n"
                    "\tPOS = D3DXVECTOR2(90, 592),\n"
                    "})\n");

    // After: added past the old one, drawn over it.
    std::string after = kSpeech;
    REQUIRE_MESSAGE(x2::ui_editor::PictureToNewStatic(after, 10, 3, "Left", false, error), error);
    CHECK(after.find("g_pUIDialog:AddControl(g_pStaticSpeech)\n"
                     "g_pUIDialog:AddControl(g_pStatic_Left)\n") != std::string::npos);
    CHECK(after.find("g_pStaticSpeech = g_pUIDialog:CreateStatic()\n"
                     "g_pStatic_Left = g_pUIDialog:CreateStatic()\n"
                     "g_pStatic_Left:SetName(\"Left\")\n") != std::string::npos);
    CHECK(after.find("g_pStatic_Left:AddPicture(g_pPictureSpeech_Left)\n") != std::string::npos);

    // The picture added before the static is: the new static is made before both.
    std::string late = "s = g_pUIDialog:CreateStatic()\np = g_pUIDialog:CreatePicture()\n\ts:AddPicture(p)\n"
                       "g_pUIDialog:AddControl(s)\n";
    REQUIRE_MESSAGE(x2::ui_editor::PictureToNewStatic(late, 3, 4, "P", true, error), error);
    CHECK(late == "s = g_pUIDialog:CreateStatic()\np = g_pUIDialog:CreatePicture()\n"
                  "\tg_pStatic_P = g_pUIDialog:CreateStatic()\n\tg_pStatic_P:SetName(\"P\")\n\tg_pStatic_P:AddPicture(p)\n"
                  "g_pUIDialog:AddControl(g_pStatic_P)\ng_pUIDialog:AddControl(s)\n");

    // The old static's offset goes with it.
    std::string offset = "s = g_pUIDialog:CreateStatic()\ng_pUIDialog:AddControl(s)\ns:SetOffsetPos(-290, -227)\n"
                         "p = g_pUIDialog:CreatePicture()\ns:AddPicture(p)\n";
    REQUIRE_MESSAGE(x2::ui_editor::PictureToNewStatic(offset, 5, 2, "P", false, error), error);
    CHECK(offset == "s = g_pUIDialog:CreateStatic()\n"
                    "g_pStatic_P = g_pUIDialog:CreateStatic()\ng_pStatic_P:SetName(\"P\")\ng_pStatic_P:SetOffsetPos(-290, -227)\n"
                    "g_pUIDialog:AddControl(s)\ng_pUIDialog:AddControl(g_pStatic_P)\ns:SetOffsetPos(-290, -227)\n"
                    "p = g_pUIDialog:CreatePicture()\ng_pStatic_P:AddPicture(p)\n");

    // Refused: not the calls, another static, a quote in the name; a name the text has gets a number.
    std::string text = kSpeech;
    CHECK_FALSE(x2::ui_editor::PictureToNewStatic(text, 7, 3, "X", true, error));
    CHECK(error == "no AddPicture call on line 7");
    CHECK_FALSE(x2::ui_editor::PictureToNewStatic(text, 6, 3, "X\"", true, error));
    std::string other = "a = g_pUIDialog:CreateStatic()\ng_pUIDialog:AddControl(a)\nb:AddPicture(p)\n";
    CHECK_FALSE(x2::ui_editor::PictureToNewStatic(other, 3, 2, "X", true, error));
    CHECK(text == kSpeech);
    std::string taken = "g_pStatic_X = 1\n" + kSpeech.substr(3);
    REQUIRE_MESSAGE(x2::ui_editor::PictureToNewStatic(taken, 7, 4, "X", true, error), error);
    CHECK(taken.find("g_pStatic_X_2:AddPicture(g_pPictureSpeech_Shadow)") != std::string::npos);
}

TEST_CASE("script edit: statics and pictures added")
{
    std::string error;
    std::string text = "g_pUIDialog:SetName(\"A\")";
    REQUIRE_MESSAGE(x2::ui_editor::AddStatic(text, "New", "T.tga", "KEY", 10, 20.5f, error), error);
    CHECK(text == "g_pUIDialog:SetName(\"A\")\n"
                  "g_pStatic_New = g_pUIDialog:CreateStatic()\n"
                  "g_pStatic_New:SetName(\"New\")\n"
                  "g_pUIDialog:AddControl(g_pStatic_New)\n"
                  "g_pPicture_New = g_pUIDialog:CreatePicture()\n"
                  "g_pStatic_New:AddPicture(g_pPicture_New)\n"
                  "g_pPicture_New:SetTex(\"T.tga\", \"KEY\")\n"
                  "g_pPicture_New:SetPoint({ USE_TEXTURE_SIZE = true, LEFT_TOP = D3DXVECTOR2(10, 20.5) })\n");
    std::string whole = "";
    REQUIRE_MESSAGE(x2::ui_editor::AddStatic(whole, "W", "Whole.dds", "", 0, 0, error), error);
    CHECK(whole.find("g_pPicture_W:SetTex2(\"Whole.dds\")\n") != std::string::npos);

    // A picture after the static's last: past that picture's own lines.
    std::string speech = kSpeech;
    REQUIRE_MESSAGE(x2::ui_editor::AddPictureAfter(speech, 10, CallKind::AddPicture, "T.tga", "K", 1, 2, error), error);
    CHECK(speech.find("g_pPictureSpeech_Left:SetPoint({ LEFT_TOP = D3DXVECTOR2(59, 562) })\n"
                      "g_pPicture_g_pStaticSpeech = g_pUIDialog:CreatePicture()\n"
                      "g_pStaticSpeech:AddPicture(g_pPicture_g_pStaticSpeech)\n"
                      "g_pPicture_g_pStaticSpeech:SetTex(\"T.tga\", \"K\")\n"
                      "g_pPicture_g_pStaticSpeech:SetPoint({ USE_TEXTURE_SIZE = true, LEFT_TOP = D3DXVECTOR2(1, 2) })\n"
                      "g_pStaticSpeech:AddString({\n") != std::string::npos);
    // A static without pictures: after its lines.
    std::string empty = "s = g_pUIDialog:CreateStatic()\ng_pUIDialog:AddControl(s)\ns:SetName(\"S\")\nx = 1\n";
    REQUIRE_MESSAGE(x2::ui_editor::AddPictureAfter(empty, 2, CallKind::AddControl, "T.tga", "K", 0, 0, error), error);
    CHECK(empty == "s = g_pUIDialog:CreateStatic()\ng_pUIDialog:AddControl(s)\ns:SetName(\"S\")\n"
                   "g_pPicture_s = g_pUIDialog:CreatePicture()\ns:AddPicture(g_pPicture_s)\n"
                   "g_pPicture_s:SetTex(\"T.tga\", \"K\")\n"
                   "g_pPicture_s:SetPoint({ USE_TEXTURE_SIZE = true, LEFT_TOP = D3DXVECTOR2(0, 0) })\nx = 1\n");
    CHECK_FALSE(x2::ui_editor::AddPictureAfter(empty, 1, CallKind::AddControl, "T.tga", "K", 0, 0, error));
    CHECK_FALSE(x2::ui_editor::AddStatic(empty, "S", "a\"b", "K", 0, 0, error));
}

TEST_CASE("script edit: pictures and statics duplicated")
{
    std::string error;
    std::string text = kSpeech;
    REQUIRE_MESSAGE(x2::ui_editor::DuplicatePicture(text, 10, 10, 0, error), error);
    CHECK(text.find("g_pPictureSpeech_Left:SetPoint({ LEFT_TOP = D3DXVECTOR2(59, 562) })\n"
                    "g_pPictureSpeech_Left_Copy = g_pUIDialog:CreatePicture()\n"
                    "g_pStaticSpeech:AddPicture(g_pPictureSpeech_Left_Copy)\n"
                    "g_pPictureSpeech_Left_Copy:SetTex(\"DLG_Common_New_Texture64.tga\", \"Speech_Popup_Left\")\n"
                    "g_pPictureSpeech_Left_Copy:SetPoint({ LEFT_TOP = D3DXVECTOR2(69, 562) })\n"
                    "g_pStaticSpeech:AddString({\n") != std::string::npos);

    std::string copy = kSpeech;
    REQUIRE_MESSAGE(x2::ui_editor::DuplicateStatic(copy, 3, "Speech2", 0, 5, error), error);
    CHECK(copy == kSpeech +
                      "g_pStatic_Speech2 = g_pUIDialog:CreateStatic()\n"
                      "g_pStatic_Speech2:SetName(\"Speech2\")\n"
                      "g_pPictureSpeech_Shadow_Copy = g_pUIDialog:CreatePicture()\n"
                      "g_pStatic_Speech2:AddPicture(g_pPictureSpeech_Shadow_Copy)\n"
                      "g_pPictureSpeech_Shadow_Copy:SetTex(\"DLG_Common_New_Texture64.tga\", \"Shadow\")\n"
                      "g_pPictureSpeech_Shadow_Copy:SetPoint({ ADD_SIZE_X = 1024, LEFT_TOP = D3DXVECTOR2(0, 535) })\n"
                      "g_pPictureSpeech_Left_Copy = g_pUIDialog:CreatePicture()\n"
                      "g_pStatic_Speech2:AddPicture(g_pPictureSpeech_Left_Copy)\n"
                      "g_pPictureSpeech_Left_Copy:SetTex(\"DLG_Common_New_Texture64.tga\", \"Speech_Popup_Left\")\n"
                      "g_pPictureSpeech_Left_Copy:SetPoint({ LEFT_TOP = D3DXVECTOR2(59, 567) })\n"
                      "g_pStatic_Speech2:AddString({\n"
                      "\tPOS = D3DXVECTOR2(90, 597),\n"
                      "})\n"
                      "g_pUIDialog:AddControl(g_pStatic_Speech2)\n");
}

TEST_CASE("script edit: pictures and controls removed")
{
    std::string error;
    std::string text = kSpeech;
    REQUIRE_MESSAGE(x2::ui_editor::RemovePicture(text, 6, error), error);
    CHECK(text == "\xEF\xBB\xBF"
                  "g_pUIDialog:SetName(\"DLG_Dungeon_Ending_Event_Speech\")\n"
                  "g_pStaticSpeech = g_pUIDialog:CreateStatic()\n"
                  "g_pUIDialog:AddControl(g_pStaticSpeech)\n"
                  "g_pStaticSpeech:SetName(\"Static_Ending_Speech\")\n"
                  "g_pPictureSpeech_Left = g_pUIDialog:CreatePicture()\n"
                  "g_pStaticSpeech:AddPicture(g_pPictureSpeech_Left)\n"
                  "g_pPictureSpeech_Left:SetTex(\"DLG_Common_New_Texture64.tga\", \"Speech_Popup_Left\")\n"
                  "g_pPictureSpeech_Left:SetPoint({ LEFT_TOP = D3DXVECTOR2(59, 562) })\n"
                  "g_pStaticSpeech:AddString({\n"
                  "\tPOS = D3DXVECTOR2(90, 592),\n"
                  "})\n");

    std::string control = kSpeech + "x = 1\n";
    REQUIRE_MESSAGE(x2::ui_editor::RemoveControl(control, 3, error), error);
    CHECK(control == "\xEF\xBB\xBFg_pUIDialog:SetName(\"DLG_Dungeon_Ending_Event_Speech\")\nx = 1\n");

    // Used elsewhere (inside an if): only the call that adds it goes.
    std::string used = "p = g_pUIDialog:CreatePicture()\ns:AddPicture(p)\np:SetTex(\"T.tga\", \"K\")\n"
                       "if a then p:SetShow(false) end\n";
    REQUIRE_MESSAGE(x2::ui_editor::RemovePicture(used, 2, error), error);
    CHECK(used == "p = g_pUIDialog:CreatePicture()\np:SetTex(\"T.tga\", \"K\")\nif a then p:SetShow(false) end\n");

    // A name used again for the next picture: only the first picture's lines go.
    std::string reused = "p = g_pUIDialog:CreatePicture()\ns:AddPicture(p)\np:SetTex(\"A.tga\", \"K\")\n"
                         "p = g_pUIDialog:CreatePicture()\ns:AddPicture(p)\np:SetTex(\"B.tga\", \"K\")\n";
    REQUIRE_MESSAGE(x2::ui_editor::RemovePicture(reused, 2, error), error);
    CHECK(reused == "p = g_pUIDialog:CreatePicture()\ns:AddPicture(p)\np:SetTex(\"B.tga\", \"K\")\n");
    CHECK_FALSE(x2::ui_editor::RemoveControl(reused, 1, error));
}
