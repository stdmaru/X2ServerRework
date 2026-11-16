// UIAnchors.lua as the UI editor reads and edits it: each edit changes only the text it must (docs/tools/ui-editor.md).

#include "anchors_file.h"

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>
#include <string>

using x2::ui_editor::AnchorEntry;
using x2::ui_editor::AnchorsFile;

namespace {

// Laid out as the client_data's: comments, one-line entries, and controls on lines of their own.
const std::string kFile = "\xEF\xBB\xBF-- Horizontal anchors (T44).\n"
                          "UI_ANCHORS =\n"
                          "{\n"
                          "\t-- HUD ---\n"
                          "\n"
                          "\t-- Chat, bottom left.\n"
                          "\t[\"DLG_ChatBox_New.lua\"] = { DIALOG = \"LEFT\" },\n"
                          "\t[\"DLG_UI_Quick_Slot.lua\"] = { DIALOG = \"LEFT\" },\t-- the slots\n"
                          "\t[\"DLG_Menu_Button_NEW.lua\"] = { STRETCH_ZONES = { { 49, 494 }, { 569, 1011 } }, CONTROLS = {\n"
                          "\t\tStatic = \"STRETCH\", Exp = \"STRETCH\",\n"
                          "\t\tCharac = \"RIGHT\", Inven = \"RIGHT\",\n"
                          "\t} },\n"
                          "\t[\"DLG_Loading.lua\"] = { CONTROLS = { Bar = \"STRETCH\", Version = \"LEFT\" } },\n"
                          "}\n";

std::string Replaced(std::string text, const std::string& from, const std::string& to)
{
    const std::size_t at = text.find(from);
    REQUIRE(at != std::string::npos);
    return text.replace(at, from.size(), to);
}

}  // namespace

TEST_CASE("ui editor: UIAnchors.lua is read by script, without folder or case")
{
    const AnchorsFile file = AnchorsFile::Parse(kFile);
    CHECK(file.Problem().empty());
    CHECK(file.Get("dlg_chatbox_new.lua").dialog == "LEFT");
    CHECK(file.Get("UI/Scripts/DLG_UI_Quick_Slot.lua").dialog == "LEFT");
    const AnchorEntry menu = file.Get("DLG_Menu_Button_NEW.lua");
    CHECK(menu.dialog.empty());
    CHECK(menu.zones == std::vector<std::pair<float, float>>{{49.0f, 494.0f}, {569.0f, 1011.0f}});
    REQUIRE(menu.controls.size() == 4);
    CHECK(menu.controls[0] == std::pair<std::string, std::string>{"Static", "STRETCH"});
    CHECK(menu.Control("Inven") == "RIGHT");
    CHECK_FALSE(file.Has("DLG_Other.lua"));
    CHECK(file.Get("DLG_Other.lua").Empty());

    AnchorsFile same = AnchorsFile::Parse(kFile);
    same.Set("DLG_Menu_Button_NEW.lua", menu);
    CHECK_FALSE(same.Changed());
    CHECK(same.Text() == kFile);

    CHECK_FALSE(AnchorsFile::Parse("UI_ANCHORS = { [\"x.lua\"] = { DIALOG = \"LEFT\" ").Problem().empty());
}

TEST_CASE("ui editor: an edit to UIAnchors.lua changes only its own text")
{
    AnchorsFile file = AnchorsFile::Parse(kFile);

    SUBCASE("a value") {
        AnchorEntry e = file.Get("DLG_Menu_Button_NEW.lua");
        e.SetControl("Inven", "LEFT");
        file.Set("DLG_Menu_Button_NEW.lua", e);
        CHECK(file.Changed());
        CHECK(file.Text() == Replaced(kFile, "Inven = \"RIGHT\"", "Inven = \"LEFT\""));
    }
    SUBCASE("a control removed, then the last on its line, then its line") {
        AnchorEntry e = file.Get("DLG_Menu_Button_NEW.lua");
        e.SetControl("Static", "");
        file.Set("DLG_Menu_Button_NEW.lua", e);
        CHECK(file.Text() == Replaced(kFile, "Static = \"STRETCH\", Exp", "Exp"));
        e.SetControl("Inven", "");
        e.SetControl("Charac", "");
        file.Set("DLG_Menu_Button_NEW.lua", e);
        CHECK(file.Text() == Replaced(kFile, "\t\tStatic = \"STRETCH\", Exp = \"STRETCH\",\n\t\tCharac = \"RIGHT\", Inven = \"RIGHT\",\n",
                                      "\t\tExp = \"STRETCH\",\n"));
    }
    SUBCASE("the last control of a line with more lines after it") {
        AnchorEntry e = file.Get("DLG_Menu_Button_NEW.lua");
        e.SetControl("Exp", "");
        file.Set("DLG_Menu_Button_NEW.lua", e);
        CHECK(file.Text() == Replaced(kFile, "\"STRETCH\", Exp = \"STRETCH\",\n", "\"STRETCH\",\n"));
    }
    SUBCASE("a control added, after a trailing comma and on one line") {
        AnchorEntry e = file.Get("DLG_Menu_Button_NEW.lua");
        e.SetControl("Quest", "RIGHT");
        file.Set("DLG_Menu_Button_NEW.lua", e);
        CHECK(file.Text() == Replaced(kFile, "Inven = \"RIGHT\",\n", "Inven = \"RIGHT\", Quest = \"RIGHT\",\n"));
        AnchorEntry l = file.Get("DLG_Loading.lua");
        l.SetControl("Odd Name", "RIGHT");
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text().find("Version = \"LEFT\", [\"Odd Name\"] = \"RIGHT\" } },") != std::string::npos);
        CHECK(file.Get("DLG_Loading.lua").Control("Odd Name") == "RIGHT");
    }
    SUBCASE("the last control of a line removed") {
        AnchorEntry l = file.Get("DLG_Loading.lua");
        l.SetControl("Version", "");
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text() == Replaced(kFile, "Bar = \"STRETCH\", Version = \"LEFT\" }", "Bar = \"STRETCH\" }"));
    }
    SUBCASE("the dialog's anchor added, changed and removed") {
        AnchorEntry l = file.Get("DLG_Loading.lua");
        l.dialog = "RIGHT";
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text() == Replaced(kFile, "{ CONTROLS = { Bar", "{ DIALOG = \"RIGHT\", CONTROLS = { Bar"));
        l.dialog = "";
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text() == kFile);
    }
    SUBCASE("stretch zones replaced, added before the controls and removed") {
        AnchorEntry e = file.Get("DLG_Menu_Button_NEW.lua");
        e.zones = {{10.0f, 20.5f}};
        file.Set("DLG_Menu_Button_NEW.lua", e);
        CHECK(file.Text() == Replaced(kFile, "{ { 49, 494 }, { 569, 1011 } }", "{ { 10, 20.5 } }"));
        AnchorEntry l = file.Get("DLG_Loading.lua");
        l.zones = {{10.0f, 1014.0f}};
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text().find("{ STRETCH_ZONES = { { 10, 1014 } }, CONTROLS = { Bar") != std::string::npos);
        l.zones.clear();
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text().find("[\"DLG_Loading.lua\"] = { CONTROLS = { Bar") != std::string::npos);
    }
    SUBCASE("backdrops added before the controls, changed, read back and removed") {
        AnchorEntry l = file.Get("DLG_Loading.lua");
        l.SetBackdrop("Bg", 64.0f);
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text() == Replaced(kFile, "{ CONTROLS = { Bar", "{ BACKDROPS = { Bg = 64 }, CONTROLS = { Bar"));
        l.SetBackdrop("Bg", 32.5f);
        l.SetBackdrop("Second", 0.0f);
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text().find("{ BACKDROPS = { Bg = 32.5, Second = 0 }, CONTROLS") != std::string::npos);
        CHECK(file.Problem().empty());
        CHECK(file.Get("DLG_Loading.lua").Backdrop("Bg") == 32.5f);
        CHECK(file.Get("DLG_Loading.lua").Backdrop("Second") == 0.0f);
        CHECK(file.Get("DLG_Loading.lua").Backdrop("Bar") < 0.0f);
        l.backdrops.clear();
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text() == kFile);

        AnchorEntry n;
        n.SetBackdrop("Login", 64.0f);
        file.Set("DLG_Login_Back.lua", n);
        CHECK(file.Text().find("\t[\"DLG_Login_Back.lua\"] = { BACKDROPS = { Login = 64 } },\n") != std::string::npos);
        CHECK(file.Get("dlg_login_back.lua") == n);
        CHECK_FALSE(AnchorsFile::Parse("UI_ANCHORS = { [\"x.lua\"] = { BACKDROPS = { A = \"LEFT\" } } }").Problem().empty());
    }
    SUBCASE("every control removed, then the whole entry") {
        AnchorEntry l = file.Get("DLG_Loading.lua");
        l.controls.clear();
        l.dialog = "LEFT";
        file.Set("DLG_Loading.lua", l);
        CHECK(file.Text() == Replaced(kFile, "{ CONTROLS = { Bar = \"STRETCH\", Version = \"LEFT\" } }", "{ DIALOG = \"LEFT\" }"));
        file.Set("DLG_Loading.lua", {});
        CHECK(file.Text() == Replaced(kFile, "\t[\"DLG_Loading.lua\"] = { CONTROLS = { Bar = \"STRETCH\", Version = \"LEFT\" } },\n", ""));
        CHECK_FALSE(file.Has("DLG_Loading.lua"));
    }
    SUBCASE("an entry with a comment after it removed: the comment stays") {
        file.Set("DLG_UI_Quick_Slot.lua", {});
        CHECK(file.Text() == Replaced(kFile, "\t[\"DLG_UI_Quick_Slot.lua\"] = { DIALOG = \"LEFT\" },\t-- the slots\n",
                                      "\t\t-- the slots\n"));
    }
    SUBCASE("new entries go at the end, under the editor's section") {
        AnchorEntry e;
        e.dialog = "RIGHT";
        file.Set("DLG_New.lua", e);
        AnchorEntry wide;
        for (int i = 0; i < 12; ++i) {
            wide.SetControl("Control_Number_" + std::to_string(i), "RIGHT");
        }
        file.Set("DLG_Wide.lua", wide);
        const std::string& text = file.Text();
        const std::size_t section = text.find("\t-- Set in the UI editor (tools/ui-editor) ---");
        REQUIRE(section != std::string::npos);
        CHECK(text.rfind(kFile.substr(0, kFile.size() - 2) + "\n", 0) == 0);     // all of it but the closing brace
        CHECK(text.find("\t[\"DLG_New.lua\"] = { DIALOG = \"RIGHT\" },\n", section) != std::string::npos);
        CHECK(text.find("\t[\"DLG_Wide.lua\"] = { CONTROLS = {\n\t\tControl_Number_0 = \"RIGHT\",", section) !=
              std::string::npos);
        CHECK(text.substr(text.size() - 8) == "\t} },\n}\n");
        CHECK(text.find("Set in the UI editor", section + 5) == std::string::npos);    // one section
        CHECK(file.Problem().empty());
        CHECK(file.Get("DLG_Wide.lua") == wide);
        CHECK(file.Get("DLG_New.lua") == e);
    }
}

TEST_CASE("ui editor: UIAnchors.lua's particle anchors stay as they are when dialog entries change")
{
    const std::string particles = "\nUI_PARTICLE_ANCHORS =\n{\n\tNewComboDamage = \"LEFT\", OutLine = \"STRETCH\",\n}\n";
    AnchorsFile file = AnchorsFile::Parse(kFile + particles);
    CHECK(file.Problem().empty());
    AnchorEntry e;
    e.dialog = "RIGHT";
    file.Set("DLG_New.lua", e);
    file.Set("DLG_ChatBox_New.lua", AnchorEntry{});
    const std::string& text = file.Text();
    CHECK(text.find("DLG_New.lua") < text.find("UI_PARTICLE_ANCHORS"));
    CHECK(text.find("DLG_ChatBox_New.lua") == std::string::npos);
    CHECK(text.size() >= particles.size());
    CHECK(text.compare(text.size() - particles.size(), particles.size(), particles) == 0);
}

TEST_CASE("ui editor: a missing UIAnchors.lua is written with its header")
{
    AnchorsFile file = AnchorsFile::Parse({});
    CHECK_FALSE(file.Changed());
    AnchorEntry e;
    e.SetControl("A", "STRETCH");
    file.Set("DLG_A.lua", e);
    CHECK(file.Text().compare(0, 3, "\xEF\xBB\xBF") == 0);
    CHECK(file.Text().find("UI_ANCHORS =\n{\n") != std::string::npos);
    CHECK(AnchorsFile::Parse(file.Text()).Get("dlg_a.lua") == e);
}

TEST_CASE("ui editor: UIAnchors.lua is found by name, and a new one goes with the dialog scripts")
{
    namespace fs = std::filesystem;
    const fs::path game = fs::temp_directory_path() / "x2_ui_editor_find";
    fs::remove_all(game);
    fs::create_directories(game / "data" / ".kom");
    fs::create_directories(game / "data" / "screens" / "town");
    std::ofstream(game / "data" / "screens" / "town" / "DLG_Shop.lua") << "";
    CHECK(AnchorsFile::Find(game) == game / "data" / "screens" / "town" / "UIAnchors.lua");
    fs::create_directories(game / "data" / "misc");
    std::ofstream(game / "data" / "misc" / "uianchors.LUA") << "";
    CHECK(AnchorsFile::Find(game) == game / "data" / "misc" / "uianchors.LUA");
    fs::remove_all(game / "data" / "misc");
    fs::remove_all(game / "data" / "screens");
    CHECK(AnchorsFile::Find(game) == game / "data" / "UIAnchors.lua");
    fs::remove_all(game);
}

TEST_CASE("ui editor: UIAnchors.lua's particle anchors are set, changed and removed in place")
{
    const std::string particles = "\nUI_PARTICLE_ANCHORS =\n{\n\tNewComboDamage = \"LEFT\", OutLine = \"STRETCH\"\n}\n";
    AnchorsFile file = AnchorsFile::Parse(kFile + particles);
    CHECK(file.Particle("NewComboDamage") == "LEFT");
    CHECK(file.Particle("Missing").empty());
    file.SetParticle("OutLine", "SPAN");
    CHECK(file.Text() == Replaced(kFile + particles, "\"STRETCH\"\n}", "\"SPAN\"\n}"));
    file.SetParticle("CutIn_Talk_Right", "RIGHT");
    CHECK(file.Particle("CutIn_Talk_Right") == "RIGHT");
    CHECK(file.Text().find("\"SPAN\",\n\tCutIn_Talk_Right = \"RIGHT\",\n}") != std::string::npos);
    file.SetParticle("NewComboDamage", "");
    CHECK(file.Particle("NewComboDamage").empty());
    CHECK(file.Text().find("NewComboDamage") == std::string::npos);
    CHECK(AnchorsFile::Parse(file.Text()).Particle("OutLine") == "SPAN");
    CHECK(file.Problem().empty());
}

TEST_CASE("ui editor: a particle anchor makes UI_PARTICLE_ANCHORS when the file has none")
{
    AnchorsFile file = AnchorsFile::Parse(kFile);
    file.SetParticle("OutLine", "STRETCH");
    CHECK(file.Text().compare(0, kFile.size(), kFile) == 0);
    CHECK(file.Text().find("UI_PARTICLE_ANCHORS") != std::string::npos);
    const AnchorsFile again = AnchorsFile::Parse(file.Text());
    CHECK(again.Problem().empty());
    CHECK(again.Particle("OutLine") == "STRETCH");
    CHECK(again.Get("DLG_ChatBox_New.lua").dialog == "LEFT");
}

TEST_CASE("ui editor: UIAnchors.lua's picture anchors are read and written by id")
{
    AnchorsFile file = AnchorsFile::Parse(kFile);
    AnchorEntry e = file.Get("DLG_Loading.lua");
    e.SetPicture("PromotionMovie/BG_ALPHA_01", "STRETCH");
    e.SetPicture("Static/#2", "RIGHT");
    file.Set("DLG_Loading.lua", e);
    CHECK(file.Text().find("PICTURES = {") < file.Text().find("CONTROLS = { Bar"));
    const AnchorsFile again = AnchorsFile::Parse(file.Text());
    CHECK(again.Problem().empty());
    CHECK(again.Get("dlg_loading.lua").Picture("PromotionMovie/BG_ALPHA_01") == "STRETCH");
    CHECK(again.Get("dlg_loading.lua").Picture("Static/#2") == "RIGHT");
    CHECK(again.Get("dlg_loading.lua").Control("Bar") == file.Get("DLG_Loading.lua").Control("Bar"));
    e.SetPicture("Static/#2", "");
    e.SetPicture("PromotionMovie/BG_ALPHA_01", "");
    file.Set("DLG_Loading.lua", e);
    CHECK(file.Text() == kFile);

    AnchorEntry fresh;
    fresh.SetPicture("A/B", "LEFT");
    file.Set("DLG_Fresh.lua", fresh);
    CHECK(AnchorsFile::Parse(file.Text()).Get("DLG_Fresh.lua") == fresh);
}
