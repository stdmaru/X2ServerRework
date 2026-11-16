// The UI editor's engine, headless on the null render backend: a dialog script from a loose data folder is opened,
// drawn and inspected (docs/tools/ui-editor.md).

#include "engine.h"
#include "layout_edit.h"
#include "tet_file.h"

#include <cmath>
#include <doctest/doctest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace {

struct TempGame {
    std::filesystem::path previous = std::filesystem::current_path();
    std::filesystem::path root = std::filesystem::temp_directory_path() /
                                 ("x2_ui_editor_test_" + std::to_string(std::hash<std::string>{}(previous.string())));
    TempGame()
    {
        std::filesystem::remove_all(root);
        std::filesystem::create_directories(root / "data" / "ui" / "scripts");
        std::filesystem::create_directories(root / "data" / ".kom");
        std::ofstream(root / "data" / ".kom" / "DLG_Hidden.lua") << "";
        // A 4 x 4 white, opaque, uncompressed 32-bit TGA for the pictures (pictures without a texture draw nothing).
        std::ofstream tga(root / "data" / "ui" / "DLG_Test.tga", std::ios::binary);
        const unsigned char header[18] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0, 32, 8};
        tga.write(reinterpret_cast<const char*>(header), sizeof(header));
        for (int i = 0; i < 16 * 4; ++i) {
            tga.put(static_cast<char>(0xFF));
        }
        // A dialog at (100, 50) with a named 200 x 100 static and an unnamed button-sized static, no textures.
        // Client scripts are UTF-8 with BOM, which the Lua loader expects (client_data.md#lua).
        std::ofstream(root / "data" / "ui" / "scripts" / "DLG_Test.lua") << "\xEF\xBB\xBF" << R"lua(
FALSE = 0 -- Enum.lua's, read as numbers
g_pUIDialog:SetName("DLG_Test")
g_pUIDialog:SetPos(100, 50)
g_pStatic = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pStatic)
g_pStatic:SetName("Panel")
g_pPicture = g_pUIDialog:CreatePicture()
g_pStatic:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_Test.tga")
g_pPicture:SetPoint({ USE_TEXTURE_SIZE = FALSE, LEFT_TOP = D3DXVECTOR2(0, 0), RIGHT_BOTTOM = D3DXVECTOR2(200, 100) })
g_pStatic2 = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pStatic2)
g_pPicture2 = g_pUIDialog:CreatePicture()
g_pStatic2:AddPicture(g_pPicture2)
g_pPicture2:SetTex("DLG_Test.tga")
g_pPicture2:SetPoint({ USE_TEXTURE_SIZE = FALSE, LEFT_TOP = D3DXVECTOR2(10, 20), RIGHT_BOTTOM = D3DXVECTOR2(60, 40) })
g_pCX2SlotManager:AddSlot(g_pCX2SlotManager:CreateItemSlot())
)lua";
        // Texts need the game's fonts to draw, which the tests do not have: this one is only inspected.
        std::ofstream(root / "data" / "ui" / "scripts" / "DLG_Text.lua") << "\xEF\xBB\xBF" << R"lua(
g_pStatic = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pStatic)
g_pStatic:SetName("Label")
g_pStatic:AddString({ STRING = "Hello", FONT_INDEX = 3, POS = D3DXVECTOR2(12, 8) })
)lua";
        // Anchors the script declares itself, which UIAnchors.lua does not override, beside a control it leaves.
        std::ofstream(root / "data" / "ui" / "scripts" / "DLG_Anchored.lua") << "\xEF\xBB\xBF" << R"lua(
g_pUIDialog:SetAnchor("RIGHT")
g_pOwn = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pOwn)
g_pOwn:SetName("Own")
g_pOwn:SetAnchor("LEFT")
g_pFree = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pFree)
g_pFree:SetName("Free")
)lua";
        // A picture wider than the screen panned over it, as the field maps (DLG_UI_FIELD_MAP_*.lua).
        std::ofstream(root / "data" / "ui" / "scripts" / "DLG_Pan.lua") << "\xEF\xBB\xBF" << R"lua(
g_pUIDialog:SetPos(0, 0)
g_pUIDialog:SetEnableMoveByDrag(true)
g_pUIDialog:SetSize(1536, 768)
g_pUIDialog:SetEnableDraggingRange(true)
g_pUIDialog:SetDraggingRangeCenter(512, 384)
g_pUIDialog:SetDraggingRange(512, 0)
)lua";
        std::ofstream(root / "data" / "ui" / "scripts" / "dlg_another.lua") << "\xEF\xBB\xBFg_pUIDialog:SetName(\"x\")\n";
    }
    ~TempGame()
    {
        std::filesystem::current_path(previous);
        std::error_code ec;
        std::filesystem::remove_all(root, ec);
    }
};

std::unique_ptr<x2::render::Device> NullRender()
{
    std::string error;
    x2::render::DeviceOptions options;
    options.backend = x2::render::Backend::Null;
    auto device = x2::render::Device::Create(options, error);
    REQUIRE_MESSAGE(device, error);
    return device;
}

}  // namespace

TEST_CASE("ui editor: dialog scripts are listed from the data folder, sorted, .kom/ skipped")
{
    TempGame game;
    const auto scripts = x2::ui_editor::FindDialogScripts(game.root);
    REQUIRE(scripts.size() == 5);
    CHECK(scripts[0] == "DLG_Anchored.lua");
    CHECK(scripts[1] == "dlg_another.lua");
    CHECK(scripts[2] == "DLG_Pan.lua");
    CHECK(scripts[3] == "DLG_Test.lua");
    CHECK(scripts[4] == "DLG_Text.lua");
}

TEST_CASE("ui editor: a dialog opens, draws at 16:9 and reports its controls in layout units and pixels")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);

    std::string openError;
    CHECK(engine->OpenDialog("DLG_Missing.lua", openError) == -1);
    CHECK_FALSE(openError.empty());

    openError.clear();
    const int index = engine->OpenDialog("dlg_test.lua", openError);
    REQUIRE(index == 0);
    CHECK_MESSAGE(openError.empty(), openError);
    REQUIRE(engine->Render(1280, 720, 0.016f, error));
    CHECK(engine->TargetWidth() == 1280);
    CHECK(engine->TargetHeight() == 720);
    CHECK(engine->UIScale() == doctest::Approx(720.0 / 768.0));
    CHECK(engine->UIOriginX() == doctest::Approx(160.0));

    const auto dialogs = engine->Inspect();
    REQUIRE(dialogs.size() == 1);
    const auto& dialog = dialogs[0];
    CHECK(dialog.loaded);
    CHECK(dialog.name == "DLG_Test");
    CHECK(dialog.pos[0] == 100.0f);
    CHECK(dialog.anchor == "CENTER");
    CHECK_FALSE(dialog.anchorDeclared);
    REQUIRE(dialog.controls.size() == 2);
    const auto& panel = dialog.controls[0];
    CHECK(panel.name == "Panel");
    CHECK(panel.named);
    CHECK(panel.typeName == "static");
    REQUIRE(panel.drawn);
    INFO("drawn ", panel.drawnMin[0], ",", panel.drawnMin[1], " - ", panel.drawnMax[0], ",", panel.drawnMax[1]);
    CHECK(panel.drawnMax[0] - panel.drawnMin[0] == doctest::Approx(200.0));
    CHECK(panel.pixelMin[0] == doctest::Approx(160.0 + (100.0 + panel.drawnMin[0]) * 720.0 / 768.0));
    CHECK(panel.pixelMin[1] == doctest::Approx((50.0 + panel.drawnMin[1]) * 720.0 / 768.0));
    REQUIRE(panel.pictures.size() == 1);
    CHECK(panel.pictures[0].texture == "DLG_Test.tga");
    CHECK(panel.pictures[0].points[3][0] == 200.0f);
    CHECK(panel.pictures[0].pixelMin[0] == doctest::Approx(panel.pixelMin[0]));     // the panel's only picture
    CHECK(panel.pictures[0].pixelMax[1] == doctest::Approx(panel.pixelMax[1]));
    CHECK(dialog.controls[1].name == "#1");
    CHECK_FALSE(dialog.controls[1].named);

    // Another size: the canvas follows, the dialog stays in layout units.
    REQUIRE(engine->Render(1024, 768, 0.016f, error));
    CHECK(engine->UIScale() == doctest::Approx(1.0));
    CHECK(engine->UIOriginX() == doctest::Approx(0.0));
    CHECK(engine->Inspect()[0].controls[0].pixelMin[0] == doctest::Approx(100.0 + panel.drawnMin[0]));

    engine->SetDialogPos(0, 0.0f, 0.0f);
    CHECK(engine->Inspect()[0].pos[0] == 0.0f);
    engine->CloseDialog(0);
    CHECK(engine->DialogCount() == 0);
}

TEST_CASE("ui editor: a static's texts are inspected with their string, position and font")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Text.lua", error) == 0);
    const auto dialogs = engine->Inspect();
    REQUIRE(dialogs.size() == 1);
    REQUIRE(dialogs[0].controls.size() == 1);
    const auto& strings = dialogs[0].controls[0].strings;
    REQUIRE(strings.size() == 1);
    CHECK(strings[0].text == "Hello");
    CHECK(strings[0].stringId == -1);
    CHECK(strings[0].pos[0] == 12.0f);
    CHECK(strings[0].font == 3);
}

TEST_CASE("ui editor: the game's live state goes into an open dialog of the same script")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Text.lua", error) == 0);
    CHECK(engine->DialogScript(0) == "DLG_Text.lua");

    x2::ui_editor::LiveSnapshot::Dialog live;
    live.script = "dlg_text.lua";
    live.pos[0] = 300.0f;
    live.pos[1] = 40.0f;
    live.controls = {{0, false, -1}};
    live.texts = {{0, 0, "Lv. 12"}};
    engine->ApplyLive(0, live);
    engine->ApplyLive(5, live);                  // no such dialog: ignored

    const auto dialogs = engine->Inspect();
    REQUIRE(dialogs.size() == 1);
    CHECK(dialogs[0].pos[0] == 300.0f);
    CHECK(dialogs[0].pos[1] == 40.0f);
    REQUIRE(dialogs[0].controls.size() == 1);
    CHECK_FALSE(dialogs[0].controls[0].shown);
    REQUIRE(dialogs[0].controls[0].strings.size() == 1);
    CHECK(dialogs[0].controls[0].strings[0].text == "Lv. 12");
    // Marked as the game's code's (2026-11-16): hidden by it, a text it filled.
    CHECK(dialogs[0].controls[0].game.shown);
    CHECK(dialogs[0].controls[0].game.scriptShown);
    CHECK_FALSE(dialogs[0].controls[0].game.added);
    CHECK(dialogs[0].controls[0].strings[0].game.text);
}

TEST_CASE("ui editor: statics as the game holds them, those code added and pictures it removed included")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Test.lua", error) == 0);
    REQUIRE(engine->LoadScriptInto(0, "DLG_Text.lua", error));     // as code adds a script to a dialog
    CHECK(engine->DialogExtraScripts(0) == std::vector<std::string>{"DLG_Text.lua"});
    CHECK_FALSE(engine->LoadScriptInto(0, "DLG_Missing.lua", error));
    const auto before = engine->Inspect();
    REQUIRE(before.size() == 1);
    const int scripted = static_cast<int>(before[0].controls.size());    // Panel, the unnamed static, the text
    REQUIRE(scripted == 3);

    x2::ui_editor::LiveSnapshot::Dialog live;
    live.script = "dlg_test.lua";
    live.extraScripts = {"dlg_text.lua"};
    live.controls = {{0, true, 5}, {scripted + 1, true, 5}};    // a static code added, after another control
    // No picture of control 0 in the snapshot: code removed the Panel's.
    x2::ui_editor::LiveSnapshot::Picture added;
    added.control = scripted + 1;
    added.texture = "DLG_Test.tga";
    added.points[1][0] = 40.0f;
    added.points[3][0] = 40.0f;
    added.points[2][1] = 20.0f;
    added.points[3][1] = 20.0f;
    added.full = true;
    live.pictures.push_back(added);
    x2::ui_editor::LiveSnapshot::Text text;
    text.control = scripted + 1;
    text.text = "Relax";
    text.font = 2;
    text.pos[0] = 5.0f;
    text.full = true;
    live.texts.push_back(text);
    engine->ApplyLive(0, live);

    const auto after = engine->Inspect();
    REQUIRE(after.size() == 1);
    REQUIRE(after[0].controls.size() == static_cast<std::size_t>(scripted + 2));
    CHECK(after[0].controls[0].pictures.empty());               // the game holds no picture there
    CHECK_FALSE(after[0].controls[static_cast<std::size_t>(scripted)].shown);   // a filler keeps the indexes
    const auto& code = after[0].controls[static_cast<std::size_t>(scripted + 1)];
    CHECK(code.shown);
    REQUIRE(code.pictures.size() == 1);
    CHECK(code.pictures[0].texture == "DLG_Test.tga");
    CHECK(code.pictures[0].points[1][0] == 40.0f);
    CHECK(code.pictures[0].points[3][1] == 20.0f);
    REQUIRE(code.strings.size() == 1);
    CHECK(code.strings[0].text == "Relax");
    CHECK(code.strings[0].font == 2);
    CHECK(code.strings[0].pos[0] == 5.0f);
    CHECK(code.game.added);
    CHECK(code.pictures[0].game.added);
    CHECK(code.strings[0].game.added);
    CHECK_FALSE(after[0].controls[1].game.Any());               // as the script left it
}

TEST_CASE("ui editor: UIAnchors.lua entries go into open dialogs, the script's own anchors kept")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Test.lua", error) == 0);
    REQUIRE(engine->OpenDialog("DLG_Anchored.lua", error) == 1);
    REQUIRE(engine->Render(1920, 1080, 0.016f, error));

    x2::ui_editor::AnchorEntry entry;
    entry.dialog = "LEFT";
    entry.zones = {{10.0f, 50.0f}};
    entry.SetControl("Panel", "STRETCH");
    entry.SetControl("Missing", "RIGHT");   // skipped
    entry.SetBackdrop("Panel", 48.0f);
    entry.SetBackdrop("Missing", 10.0f);    // skipped
    engine->ApplyAnchors(0, entry, {});
    REQUIRE(engine->Render(2520, 1080, 0.016f, error));    // 21:9: the backdrop's bars and fades draw
    auto dialogs = engine->Inspect();
    CHECK(dialogs[0].controls[0].backdropFade == 48.0f);
    CHECK(dialogs[0].controls[1].backdropFade < 0.0f);
    CHECK(dialogs[0].anchor == "LEFT");
    CHECK(dialogs[0].anchorDeclared);
    CHECK_FALSE(dialogs[0].anchorFromScript);
    CHECK(dialogs[0].stretchZones == std::vector<std::pair<float, float>>{{10.0f, 50.0f}});
    CHECK(dialogs[0].controls[0].anchor == "STRETCH");
    CHECK_FALSE(dialogs[0].controls[1].anchorDeclared);

    // What the previous entry declared and the new one does not goes back to none.
    engine->ApplyAnchors(0, {}, entry);
    dialogs = engine->Inspect();
    CHECK_FALSE(dialogs[0].anchorDeclared);
    CHECK(dialogs[0].anchor == "CENTER");
    CHECK(dialogs[0].stretchZones.empty());
    CHECK_FALSE(dialogs[0].controls[0].anchorDeclared);
    CHECK(dialogs[0].controls[0].backdropFade < 0.0f);

    CHECK(dialogs[1].anchor == "RIGHT");
    CHECK(dialogs[1].anchorFromScript);
    CHECK(dialogs[1].controls[0].anchorFromScript);
    x2::ui_editor::AnchorEntry other;
    other.dialog = "LEFT";
    other.SetControl("Own", "RIGHT");
    other.SetControl("Free", "RIGHT");
    engine->ApplyAnchors(1, other, {});
    dialogs = engine->Inspect();
    CHECK(dialogs[1].anchor == "RIGHT");
    CHECK(dialogs[1].controls[0].anchor == "LEFT");
    CHECK(dialogs[1].controls[1].anchor == "RIGHT");
    CHECK_FALSE(dialogs[1].controls[1].anchorFromScript);
}

TEST_CASE("ui editor: a picture wider than the screen pans only as far as it still covers the screen")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Pan.lua", error) == 0);

    // At 4:3 the script's range holds: from 0 (its left end at the screen's left) to -512.
    REQUIRE(engine->Render(1024, 768, 0.016f, error));
    CHECK(engine->Inspect()[0].pos[0] == doctest::Approx(0.0));
    engine->SetDialogPos(0, -600.0f, 0.0f);
    REQUIRE(engine->Render(1024, 768, 0.016f, error));
    CHECK(engine->Inspect()[0].pos[0] == doctest::Approx(-512.0));

    // At 16:9 the canvas is 1365.3 wide, 170.7 to each side of the column: the picture's ends stop at its edges.
    engine->SetDialogPos(0, 0.0f, 0.0f);
    REQUIRE(engine->Render(1280, 720, 0.016f, error));
    CHECK(engine->Inspect()[0].pos[0] == doctest::Approx(-170.67).epsilon(0.001));
    engine->SetDialogPos(0, -600.0f, 0.0f);
    REQUIRE(engine->Render(1280, 720, 0.016f, error));
    CHECK(engine->Inspect()[0].pos[0] == doctest::Approx(1024.0 + 170.67 - 1536.0).epsilon(0.001));

    // Wider than the picture (21:9, 1820 wide): it stays centered.
    engine->SetDialogPos(0, 0.0f, 0.0f);
    REQUIRE(engine->Render(2560, 1080, 0.016f, error));
    CHECK(engine->Inspect()[0].pos[0] == doctest::Approx(-256.0));
}

TEST_CASE("ui editor: the engine knows the script line that set each position")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Test.lua", error) == 0);
    REQUIRE(engine->OpenDialog("DLG_Text.lua", error) == 1);
    const auto dialogs = engine->Inspect();
    REQUIRE(dialogs.size() == 2);
    CHECK(dialogs[0].posSource.script == "DLG_Test.lua");
    CHECK(dialogs[0].posSource.line == 4);
    CHECK(dialogs[0].posSource.kind == x2::ui_editor::CallKind::DialogPos);
    REQUIRE(dialogs[0].controls.size() == 2);
    REQUIRE(dialogs[0].controls[0].pictures.size() == 1);
    CHECK_FALSE(dialogs[0].controls[0].pictures[0].game.Any());     // no game state in it
    CHECK(dialogs[0].controls[0].pictures[0].point.line == 11);
    CHECK(dialogs[0].controls[0].pictures[0].point.shared == 1);
    CHECK(dialogs[0].controls[1].pictures[0].point.line == 17);
    CHECK(dialogs[0].controls[0].points.empty());   // a static's points are its pictures'
    CHECK(dialogs[0].controls[0].add.line == 6);
    CHECK(dialogs[0].controls[0].add.kind == x2::ui_editor::CallKind::AddControl);
    CHECK(dialogs[0].controls[0].pictures[0].add.line == 9);
    CHECK(dialogs[0].controls[0].pictures[0].add.kind == x2::ui_editor::CallKind::AddPicture);
    CHECK(dialogs[0].controls[1].add.line == 13);
    CHECK_FALSE(dialogs[1].posSource.Known());      // DLG_Text.lua sets no position
    REQUIRE(dialogs[1].controls[0].strings.size() == 1);
    CHECK(dialogs[1].controls[0].strings[0].source.line == 5);
    CHECK(dialogs[1].controls[0].strings[0].source.kind == x2::ui_editor::CallKind::String);
}

TEST_CASE("ui editor: named placements put a dialog at a spot or follow a control of another dialog")
{
    TempGame game;
    const auto scripts = game.root / "data" / "ui" / "scripts";
    // The target: a centered dialog with a centered control and one at the right edge (offset 30, 40).
    std::ofstream(scripts / "DLG_Target.lua") << "\xEF\xBB\xBF" << R"lua(
g_pUIDialog:SetPos(100, 50)
g_pMiddle = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pMiddle)
g_pMiddle:SetName("Middle")
g_pRight = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pRight)
g_pRight:SetName("Right")
g_pRight:SetAnchor("RIGHT")
g_pRight:SetOffsetPos(30, 40)
)lua";
    std::ofstream(scripts / "DLG_Pointer.lua") << "\xEF\xBB\xBF" << R"lua(
g_pUIDialog:SetPos(0, 0)
g_pUIDialog:AddPlacement({ NAME = "MIDDLE", POS = D3DXVECTOR2(250, 300) })
g_pUIDialog:AddPlacement({ NAME = "AT_MIDDLE", TARGET = "dlg_target.LUA", CONTROL = "Middle", POS = D3DXVECTOR2(10, 20) })
g_pUIDialog:AddPlacement({ NAME = "AT_RIGHT", TARGET = "DLG_Target.lua", CONTROL = "Right", POS = D3DXVECTOR2(5, 5) })
g_pUIDialog:AddPlacement({ NAME = "LEFT_EDGE", POS = D3DXVECTOR2(0, 600), ANCHOR = "LEFT" })
g_pUIDialog:AddPlacement({ NAME = "NOWHERE", TARGET = "DLG_Missing.lua", POS = D3DXVECTOR2(1, 1) })
)lua";
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Target.lua", error) == 0);
    REQUIRE(engine->OpenDialog("DLG_Pointer.lua", error) == 1);
    REQUIRE(engine->Render(1920, 1080, 0.016f, error));     // 16:9: the right edge is 170.67 units right of 4:3's

    auto dialogs = engine->Inspect();
    REQUIRE(dialogs[1].placements.size() == 5);
    const auto& placements = dialogs[1].placements;
    CHECK(placements[0].name == "MIDDLE");
    CHECK(placements[0].target.empty());
    CHECK(placements[0].source.line == 3);
    CHECK(placements[0].source.kind == x2::ui_editor::CallKind::Placement);
    CHECK(placements[1].target == "dlg_target.lua");
    CHECK(placements[1].control == "Middle");
    CHECK(placements[3].anchor == "LEFT");
    CHECK(placements[4].source.line == 7);
    CHECK_FALSE(placements[4].resolved);
    CHECK(dialogs[1].placement.empty());

    CHECK_FALSE(engine->ApplyPlacement(1, "MISSING"));
    REQUIRE(engine->ApplyPlacement(1, "MIDDLE"));
    dialogs = engine->Inspect();
    CHECK(dialogs[1].placement == "MIDDLE");
    CHECK(dialogs[1].pos[0] == 250.0f);
    CHECK(dialogs[1].pos[1] == 300.0f);

    // A centered control: where the 4:3 layout puts it, at any width.
    REQUIRE(engine->ApplyPlacement(1, "AT_MIDDLE"));
    CHECK(engine->Inspect()[1].pos[0] == doctest::Approx(110.0));
    CHECK(engine->Inspect()[1].pos[1] == doctest::Approx(70.0));
    // The target moves: the dialog follows at the next step.
    engine->SetDialogPos(0, 200.0f, 60.0f);
    REQUIRE(engine->Render(1920, 1080, 0.016f, error));
    CHECK(engine->Inspect()[1].pos[0] == doctest::Approx(210.0));
    CHECK(engine->Inspect()[1].pos[1] == doctest::Approx(80.0));

    // A control at the right edge: its anchor's shift comes along.
    REQUIRE(engine->ApplyPlacement(1, "AT_RIGHT"));
    dialogs = engine->Inspect();
    CHECK(dialogs[1].pos[0] == doctest::Approx(200.0 + 30.0 + 5.0 + 1920.0 / 1080.0 * 768.0 / 2.0 - 512.0).epsilon(0.001));
    CHECK(dialogs[1].pos[1] == doctest::Approx(60.0 + 40.0 + 5.0));
    CHECK(dialogs[1].anchor == "CENTER");

    // A placement's own anchor, given back when another placement takes over.
    REQUIRE(engine->ApplyPlacement(1, "LEFT_EDGE"));
    dialogs = engine->Inspect();
    CHECK(dialogs[1].anchor == "LEFT");
    CHECK(dialogs[1].canvasPos[0] == doctest::Approx(512.0 - 1920.0 / 1080.0 * 768.0 / 2.0).epsilon(0.001));
    REQUIRE(engine->ApplyPlacement(1, "MIDDLE"));
    CHECK(engine->Inspect()[1].anchor == "CENTER");
    CHECK_FALSE(engine->Inspect()[1].anchorDeclared);

    // A target not shown: the dialog stays where it is until it is.
    REQUIRE(engine->ApplyPlacement(1, "AT_MIDDLE"));
    engine->SetDialogShown(0, false);
    engine->SetDialogPos(0, 300.0f, 60.0f);
    REQUIRE(engine->Render(1920, 1080, 0.016f, error));
    CHECK(engine->Inspect()[1].pos[0] == doctest::Approx(210.0));
    engine->SetDialogShown(0, true);
    REQUIRE(engine->Render(1920, 1080, 0.016f, error));
    CHECK(engine->Inspect()[1].pos[0] == doctest::Approx(310.0));
    REQUIRE(engine->ApplyPlacement(1, ""));
    CHECK(engine->Inspect()[1].placement.empty());

    // An edited placement: the dialog built again from the edited script is where it puts it now.
    REQUIRE(engine->ApplyPlacement(1, "MIDDLE"));
    std::string text;
    {
        std::ifstream in(scripts / "DLG_Pointer.lua", std::ios::binary);
        text.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }
    REQUIRE(x2::ui_editor::MoveCall(text, 3, x2::ui_editor::CallKind::Placement, -50, 0, error));
    engine->SetScriptText("DLG_Pointer.lua", &text);
    REQUIRE_MESSAGE(engine->ReloadDialog(1, error), error);
    dialogs = engine->Inspect();
    CHECK(dialogs[1].placement == "MIDDLE");
    CHECK(dialogs[1].pos[0] == 200.0f);
}

namespace {

std::string EmptyTet()
{
    const unsigned header[3] = {0x20544554, 0x100, 0};
    return std::string(reinterpret_cast<const char*>(header), sizeof(header));
}

x2::ui_editor::TetRect Rect(float l, float t, float r, float b)
{
    x2::ui_editor::TetRect rect;
    rect.leftTop[0] = l, rect.leftTop[1] = t;
    rect.rightBottom[0] = r, rect.rightBottom[1] = b;
    rect.leftBottom[0] = l, rect.leftBottom[1] = b;
    rect.rightTop[0] = r, rect.rightTop[1] = t;
    rect.size[0] = r - l, rect.size[1] = b - t;
    return rect;
}

}  // namespace

TEST_CASE("ui editor: a picture's key comes from its texture's .tet, which edits change in its dialogs")
{
    TempGame game;
    x2::ui_editor::TetFile tet;
    std::string error;
    REQUIRE(x2::ui_editor::TetFile::Parse(EmptyTet(), tet, error));
    REQUIRE(tet.Add("HALF", Rect(0, 0, 2, 4), error));
    {
        const std::string bytes = tet.Bytes();
        std::ofstream(game.root / "data" / "ui" / "DLG_Test.TET", std::ios::binary).write(bytes.data(), bytes.size());
        std::ofstream(game.root / "data" / "ui" / "scripts" / "DLG_Keyed.lua") << "\xEF\xBB\xBF" << R"lua(
g_pStatic = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pStatic)
g_pPicture = g_pUIDialog:CreatePicture()
g_pStatic:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_Test.tga", "HALF")
g_pPicture:SetPoint({ LEFT_TOP = D3DXVECTOR2(0, 0) })
)lua";
    }
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Keyed.lua", error) == 0);
    auto picture = engine->Inspect()[0].controls[0].pictures.at(0);
    CHECK(picture.key == "HALF");
    CHECK(picture.texSource.line == 6);
    CHECK(picture.texSource.kind == x2::ui_editor::CallKind::Texture);
    CHECK(picture.textureSize[0] == 2);
    CHECK(picture.textureSize[1] == 4);

    std::string bytes;
    REQUIRE_MESSAGE(engine->TetBytes("DLG_Test.tga", bytes, error), error);
    CHECK(bytes == tet.Bytes());
    x2::ui_editor::TetRect grown = *tet.Find("HALF");
    REQUIRE(grown.Resize(2, 0));
    REQUIRE(tet.Set("HALF", grown));
    REQUIRE_MESSAGE(engine->SetTet("DLG_Test.tga", tet.Bytes(), error), error);
    CHECK(engine->DialogUsesTexture(0, "dlg_test.TGA"));
    REQUIRE(engine->ReloadDialog(0, error));
    picture = engine->Inspect()[0].controls[0].pictures.at(0);
    CHECK(picture.textureSize[0] == 4);
    REQUIRE(engine->TetBytes("DLG_Test.tga", bytes, error));
    CHECK(bytes == tet.Bytes());
    CHECK_FALSE(engine->SetTet("DLG_Test.tga", "nope", error));

    // After the game's state went in (watched, then unchecked), a key the script changed to is drawn, at its size and
    // with its rectangle, not the game's (2026-11-16).
    x2::ui_editor::LiveSnapshot::Dialog live;
    live.script = "dlg_keyed.lua";
    live.controls = {{0, true, -1}};
    x2::ui_editor::LiveSnapshot::Picture shown;
    shown.texture = "DLG_Test.tga";
    shown.key = "HALF";
    shown.points[1][0] = shown.points[3][0] = 4.0f;
    shown.points[2][1] = shown.points[3][1] = 4.0f;
    shown.uv[1][0] = shown.uv[3][0] = 1.0f;
    shown.uv[2][1] = shown.uv[3][1] = 1.0f;
    shown.full = true;
    live.pictures = {shown};
    engine->ApplyLive(0, live);
    REQUIRE(tet.Add("OTHER", Rect(0, 2, 1, 3), error));
    REQUIRE_MESSAGE(engine->SetTet("DLG_Test.tga", tet.Bytes(), error), error);
    std::string text;
    REQUIRE_MESSAGE(engine->ScriptText("DLG_Keyed.lua", text, error), error);
    REQUIRE_MESSAGE(x2::ui_editor::SetTexCall(text, 6, "DLG_Test.tga", "OTHER", error), error);
    engine->SetScriptText("DLG_Keyed.lua", &text);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    picture = engine->Inspect()[0].controls[0].pictures.at(0);
    CHECK(picture.key == "OTHER");
    // The game's 4 by what the edits changed since the game started: OTHER's 1 x 1 instead of HALF's first 2 x 4.
    CHECK(picture.points[3][0] == doctest::Approx(3.0f));
    CHECK(picture.points[3][1] == doctest::Approx(1.0f));
    CHECK(picture.uv[0][1] == doctest::Approx(0.5f));       // OTHER's rectangle: from y 2 of 4

    // The texture itself, for Interactive Select: its size in texels, as the .tet's.
    x2::ui_editor::Engine::TextureImage image;
    REQUIRE_MESSAGE(engine->LoadTextureImage("DLG_Test.tga", image, error), error);
    CHECK(image.texture);
    CHECK(image.width == 4);
    CHECK(image.height == 4);
    CHECK_FALSE(engine->LoadTextureImage("DLG_Missing.tga", image, error));
}

TEST_CASE("ui editor: a key the game's code set takes its edited rectangle, at the size it changed to")
{
    TempGame game;
    x2::ui_editor::TetFile tet;
    std::string error;
    REQUIRE(x2::ui_editor::TetFile::Parse(EmptyTet(), tet, error));
    REQUIRE(tet.Add("SCRIPT", Rect(0, 0, 1, 1), error));
    REQUIRE(tet.Add("NAME", Rect(0, 0, 2, 1), error));
    {
        const std::string bytes = tet.Bytes();
        std::ofstream(game.root / "data" / "ui" / "DLG_Test.TET", std::ios::binary).write(bytes.data(), bytes.size());
        std::ofstream(game.root / "data" / "ui" / "scripts" / "DLG_Named.lua") << "\xEF\xBB\xBF" << R"lua(
g_pStatic = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pStatic)
g_pPicture = g_pUIDialog:CreatePicture()
g_pStatic:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_Test.tga", "SCRIPT")
g_pPicture:SetPoint({ LEFT_TOP = D3DXVECTOR2(10, 10) })
)lua";
    }
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Named.lua", error) == 0);
    // The game's code drew NAME there, 2 x 1 (Add's name_noclass in character creation).
    x2::ui_editor::LiveSnapshot::Dialog live;
    live.script = "dlg_named.lua";
    live.controls = {{0, true, -1}};
    x2::ui_editor::LiveSnapshot::Picture shown;
    shown.texture = "DLG_Test.tga";
    shown.key = "NAME";
    shown.points[0][0] = shown.points[2][0] = 10.0f;
    shown.points[1][0] = shown.points[3][0] = 12.0f;
    shown.points[0][1] = shown.points[1][1] = 10.0f;
    shown.points[2][1] = shown.points[3][1] = 11.0f;
    shown.uv[1][0] = shown.uv[3][0] = 0.5f;
    shown.uv[2][1] = shown.uv[3][1] = 0.25f;
    shown.full = true;
    live.pictures = {shown};
    engine->ApplyLive(0, live);

    // NAME widened to 3 x 1 in the .tet: drawn 3 wide, with the new rectangle.
    x2::ui_editor::TetRect wide = *tet.Find("NAME");
    REQUIRE(wide.Resize(1, 0));
    REQUIRE(tet.Set("NAME", wide));
    REQUIRE_MESSAGE(engine->SetTet("DLG_Test.tga", tet.Bytes(), error), error);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    const auto picture = engine->Inspect()[0].controls[0].pictures.at(0);
    CHECK(picture.key == "NAME");
    CHECK(picture.points[0][0] == doctest::Approx(10.0f));
    CHECK(picture.points[3][0] == doctest::Approx(13.0f));
    CHECK(picture.points[3][1] == doctest::Approx(11.0f));
    CHECK(picture.uv[3][0] == doctest::Approx(0.75f));      // 3 of the texture's 4 texels
    CHECK(picture.game.texture);                            // the game's code set NAME; the script says SCRIPT
    CHECK(picture.game.scriptKey == "SCRIPT");
    CHECK_FALSE(picture.game.added);
}

TEST_CASE("ui editor: a dialog grows at a line: crossed pictures grow, those past it would move")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Test.lua", error) == 0);
    REQUIRE(engine->Render(1024, 768, 0.016f, error));
    auto dialog = engine->Inspect()[0];

    // The panel's picture spans 0-200, the other static's 10-60: a line at 100 crosses the first only.
    auto plan = x2::ui_editor::PlanCut(dialog, true, 100.0f, 15.0f);
    REQUIRE(plan.grows.size() == 1);
    CHECK(plan.moves.empty());
    CHECK(plan.stays == 1);
    REQUIRE(plan.edits.size() == 1);
    CHECK(plan.edits[0].resize);
    CHECK(plan.edits[0].d[2] == 15.0f);
    CHECK(plan.edits[0].ref.line == 11);
    // A line at 5 moves the other picture, past it, and grows the panel's.
    plan = x2::ui_editor::PlanCut(dialog, true, 5.0f, 15.0f);
    CHECK(plan.grows.size() == 1);
    REQUIRE(plan.moves.size() == 1);
    CHECK(plan.stays == 0);

    // Made: the panel's picture is 15 wider, the other moved 15 right.
    std::string text;
    REQUIRE(engine->ScriptText("DLG_Test.lua", text, error));
    for (const auto& edit : plan.edits) {
        const bool ok = edit.resize ? x2::ui_editor::ResizeCall(text, edit.ref.line, edit.d[0], edit.d[1], edit.d[2], edit.d[3], error)
                                    : x2::ui_editor::MoveCall(text, edit.ref.line, edit.ref.kind, edit.d[0], edit.d[1], error);
        REQUIRE_MESSAGE(ok, error);
    }
    engine->SetScriptText("DLG_Test.lua", &text);
    REQUIRE(engine->ReloadDialog(0, error));
    dialog = engine->Inspect()[0];
    CHECK(dialog.controls[0].pictures[0].points[3][0] == 215.0f);
    CHECK(dialog.controls[1].pictures[0].points[0][0] == 25.0f);
    float extent[4];
    REQUIRE(engine->Render(1024, 768, 0.016f, error));
    REQUIRE(x2::ui_editor::DialogExtent(engine->Inspect()[0], extent));
    CHECK(extent[2] - extent[0] == doctest::Approx(215.0));
}

TEST_CASE("ui editor: an edited script reloads into its dialog, moved where it was and in the game's state")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Test.lua", error) == 0);
    engine->SetDialogPos(0, 300, 200);      // placed elsewhere than its script says (a recorded position)

    std::string text;
    REQUIRE_MESSAGE(engine->ScriptText("dlg_test.lua", text, error), error);
    CHECK(text.rfind("\xEF\xBB\xBF", 0) == 0);
    REQUIRE_MESSAGE(x2::ui_editor::MoveCall(text, 4, x2::ui_editor::CallKind::DialogPos, 10, 5, error), error);
    REQUIRE_MESSAGE(x2::ui_editor::MoveCall(text, 11, x2::ui_editor::CallKind::Point, 7, 0, error), error);
    engine->SetScriptText("DLG_Test.lua", &text);
    CHECK(engine->DialogUses(0, "dlg_test.lua"));
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    auto dialogs = engine->Inspect();
    REQUIRE(dialogs.size() == 1);
    CHECK(dialogs[0].pos[0] == 310.0f);     // moved by what the edit moved
    CHECK(dialogs[0].pos[1] == 205.0f);
    CHECK(dialogs[0].controls[0].pictures[0].points[0][0] == 7.0f);
    CHECK(dialogs[0].controls[0].pictures[0].points[3][0] == 207.0f);

    // The game, started before the edit, still draws the picture where the file had it: the edit moves it too.
    x2::ui_editor::LiveSnapshot::Dialog live;
    live.script = "dlg_test.lua";
    live.pos[0] = 40.0f;
    live.pos[1] = 30.0f;
    live.controls = {{0, true, -1}, {1, true, -1}};
    x2::ui_editor::LiveSnapshot::Picture picture;
    picture.texture = "DLG_Test.tga";
    picture.points[1][0] = picture.points[3][0] = 200.0f;
    picture.points[2][1] = picture.points[3][1] = 100.0f;
    picture.full = true;
    live.pictures = {picture};
    engine->ApplyLive(0, live);
    dialogs = engine->Inspect();
    CHECK(dialogs[0].pos[0] == 50.0f);      // the game's 40 and the edit's 10
    CHECK(dialogs[0].controls[0].pictures[0].points[0][0] == 7.0f);

    // Resized after the game's state went in (still there with Watch game unchecked): it grows from the game's
    // corners, each by what the edit changed (2026-11-16: the size stayed the game's).
    REQUIRE_MESSAGE(x2::ui_editor::ResizeCall(text, 11, 0, 0, 30, 10, error), error);
    engine->SetScriptText("DLG_Test.lua", &text);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    dialogs = engine->Inspect();
    CHECK(dialogs[0].controls[0].pictures[0].points[0][0] == 7.0f);
    CHECK(dialogs[0].controls[0].pictures[0].points[3][0] == 237.0f);   // the game's 200, the move's 7, the edit's 30
    CHECK(dialogs[0].controls[0].pictures[0].points[3][1] == 110.0f);

    // Undone: the file's text again, reloaded in the game's state.
    engine->SetScriptText("DLG_Test.lua", nullptr);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    dialogs = engine->Inspect();
    CHECK(dialogs[0].pos[0] == 40.0f);
    CHECK(dialogs[0].controls[0].pictures[0].points[0][0] == 0.0f);
    CHECK(x2::ui_editor::FindLooseScript(game.root, "dlg_TEST.lua") == game.root / "data" / "ui" / "scripts" / "DLG_Test.lua");
    CHECK(x2::ui_editor::FindLooseScript(game.root, "DLG_Hidden.lua").empty());     // .kom/ skipped
}

TEST_CASE("ui editor: a picture moved to a static of its own is drawn where it was, by a control of its own")
{
    TempGame game;
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_Test.lua", error) == 0);
    REQUIRE_MESSAGE(engine->Render(1280, 720, 0.0f, error), error);
    const auto before = engine->Inspect();
    REQUIRE(before[0].controls.size() == 2);
    const auto& picture = before[0].controls[1].pictures[0];

    std::string text;
    REQUIRE_MESSAGE(engine->ScriptText("DLG_Test.lua", text, error), error);
    REQUIRE_MESSAGE(x2::ui_editor::PictureToNewStatic(text, picture.add.line, before[0].controls[1].add.line, "Own", true,
                                                      error),
                    error);
    engine->SetScriptText("DLG_Test.lua", &text);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    REQUIRE_MESSAGE(engine->Render(1280, 720, 0.0f, error), error);
    const auto after = engine->Inspect();
    REQUIRE(after[0].controls.size() == 3);
    CHECK(after[0].controls[0].name == "Panel");
    CHECK(after[0].controls[1].name == "Own");          // just before the static it came from: drawn under it
    CHECK(after[0].controls[2].pictures.empty());
    REQUIRE(after[0].controls[1].pictures.size() == 1);
    CHECK(after[0].controls[1].pictures[0].points[0][0] == picture.points[0][0]);
    CHECK(after[0].controls[1].pictures[0].pixelMin[0] == picture.pixelMin[0]);
    CHECK(after[0].controls[1].pictures[0].pixelMax[1] == picture.pixelMax[1]);
    CHECK(after[0].controls[1].add.Known());

    // Removed: the dialog builds without it.
    REQUIRE_MESSAGE(x2::ui_editor::RemoveControl(text, after[0].controls[1].add.line, error), error);
    engine->SetScriptText("DLG_Test.lua", &text);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    CHECK(engine->Inspect()[0].controls.size() == 2);
}

// The user's game folder, when X2_UI_EDITOR_GAME names it (not in CI): every position its dialog scripts set through
// a call the engine notes can be moved in the script's text.
TEST_CASE("ui editor: the client_data's tutorial pointer follows the quick slots" *
          doctest::skip(std::getenv("X2_UI_EDITOR_GAME") == nullptr))
{
    const std::filesystem::path folder = std::getenv("X2_UI_EDITOR_GAME");
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = folder;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE_MESSAGE(engine->OpenDialog("DLG_UI_Quick_Slot.lua", error) == 0, error);
    REQUIRE_MESSAGE(engine->OpenDialog("DLG_Mouse_Click.lua", error) == 1, error);
    const auto opened = engine->Inspect();
    REQUIRE(opened[1].placements.size() == 11);
    for (const auto& placement : opened[1].placements) {
        CHECK_MESSAGE(placement.source.Known(), placement.name);
    }
    REQUIRE(engine->ApplyPlacement(1, "SUMMON_CARD_SLOT"));
    REQUIRE(engine->Render(1024, 768, 0.016f, error));
    CHECK(engine->Inspect()[1].canvasPos[0] == doctest::Approx(107.0));     // where the original put it
    CHECK(engine->Inspect()[1].canvasPos[1] == doctest::Approx(763.0));
    REQUIRE(engine->Render(1920, 1080, 0.016f, error));
    REQUIRE(engine->Render(1920, 1080, 0.016f, error));
    const auto dialogs = engine->Inspect();
    // The quick slots are LEFT: the pointer goes to the screen's left edge with them.
    CHECK(dialogs[1].canvasPos[0] == doctest::Approx(dialogs[0].canvasPos[0] + 107.0));
    CHECK(dialogs[0].canvasPos[0] < -100.0);
}

TEST_CASE("ui editor: the positions the client_data's scripts set can be moved" *
          doctest::skip(std::getenv("X2_UI_EDITOR_GAME") == nullptr))
{
    const std::filesystem::path folder = std::getenv("X2_UI_EDITOR_GAME");
    const std::filesystem::path previous = std::filesystem::current_path();
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = folder;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    int calls = 0, shared = 0, failed = 0;
    const char* filter = std::getenv("X2_UI_EDITOR_FILTER");     // a name prefix, to look at a few
    for (const std::string& script : x2::ui_editor::FindDialogScripts(folder)) {
        if (filter != nullptr && script.rfind(filter, 0) != 0) {
            continue;
        }
        if (engine->OpenDialog(script, error) < 0) {
            continue;
        }
        std::vector<x2::ui_editor::SourceRef> refs;
        for (const auto& dialog : engine->Inspect()) {
            refs.push_back(dialog.posSource);
            for (const auto& control : dialog.controls) {
                refs.insert(refs.end(), control.points.begin(), control.points.end());
                for (const auto& picture : control.pictures) {
                    refs.push_back(picture.point);
                }
                for (const auto& string : control.strings) {
                    refs.push_back(string.source);
                }
            }
        }
        for (const auto& ref : refs) {
            if (!ref.Known()) {
                continue;
            }
            std::string text;
            if (!engine->ScriptText(ref.script, text, error)) {
                MESSAGE(error);
                ++failed;
                continue;
            }
            ++calls;
            shared += ref.shared > 1 ? 1 : 0;
            if (!x2::ui_editor::MoveCall(text, ref.line, ref.kind, 1, 1, error)) {
                if (++failed <= 40) {
                    MESSAGE(script << " -> " << ref.script << ": " << error);
                }
            }
        }
        engine->CloseAll();
    }
    MESSAGE(calls << " calls, " << shared << " of them on lines that set several things, " << failed << " not movable");
    CHECK(calls > 0);
    engine.reset();
    std::filesystem::current_path(previous);
}

// The user's game folder, when X2_UI_EDITOR_GAME names it (not in CI): every picture of a static its dialog scripts
// add can be moved to a static of its own in the text, and the first of each dialog is drawn where it was after.
TEST_CASE("ui editor: the client_data's pictures can be moved to statics of their own" *
          doctest::skip(std::getenv("X2_UI_EDITOR_GAME") == nullptr))
{
    const std::filesystem::path folder = std::getenv("X2_UI_EDITOR_GAME");
    const std::filesystem::path previous = std::filesystem::current_path();
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = folder;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    int pictures = 0, unknown = 0, shared = 0, failed = 0, drawn = 0, moved = 0;
    const char* filter = std::getenv("X2_UI_EDITOR_FILTER");
    // The new static gets the old one's anchor, as the editor gives it.
    const x2::ui_editor::AnchorsFile anchors =
        x2::ui_editor::AnchorsFile::Load(x2::ui_editor::AnchorsFile::Find(folder), error);
    for (const std::string& script : x2::ui_editor::FindDialogScripts(folder)) {
        if (filter != nullptr && script.rfind(filter, 0) != 0) {
            continue;
        }
        if (engine->OpenDialog(script, error) < 0 || !engine->Render(1280, 720, 0.0f, error)) {
            engine->CloseAll();
            continue;
        }
        const auto dialogs = engine->Inspect();
        bool checked = false;
        for (const auto& control : dialogs[0].controls) {
            for (std::size_t p = 0; p < control.pictures.size(); ++p) {
                const auto& picture = control.pictures[p];
                ++pictures;
                if (!picture.add.Known() || !control.add.Known() || picture.add.script != control.add.script) {
                    ++unknown;
                    continue;
                }
                if (picture.add.shared > 1 || control.add.shared > 1) {
                    ++shared;
                    continue;
                }
                std::string text;
                REQUIRE(engine->ScriptText(picture.add.script, text, error));
                if (!x2::ui_editor::PictureToNewStatic(text, picture.add.line, control.add.line, "Moved_By_Test", p == 0,
                                                       error)) {
                    if (++failed <= 40) {
                        MESSAGE(script << ": " << error);
                    }
                    continue;
                }
                if (checked || !picture.shown || picture.pixelMax[0] <= picture.pixelMin[0]) {
                    continue;
                }
                // Drawn where it was: the first shown picture of each dialog.
                checked = true;
                engine->SetScriptText(picture.add.script, &text);
                ++drawn;
                const int movedBefore = moved;
                const x2::ui_editor::AnchorEntry entry = anchors.Get(script);
                x2::ui_editor::AnchorEntry withNew = entry;
                if (control.named && !entry.Control(control.name).empty()) {
                    withNew.SetControl("Moved_By_Test", entry.Control(control.name));
                }
                const bool reloaded = engine->ReloadDialog(0, error);
                if (reloaded) {
                    engine->ApplyAnchors(0, withNew, entry);
                }
                if (reloaded && engine->Render(1280, 720, 0.0f, error)) {
                    const auto reloaded = engine->Inspect();
                    for (const auto& c : reloaded[0].controls) {
                        if (c.name == "Moved_By_Test" && c.pictures.size() == 1 &&
                            std::fabs(c.pictures[0].pixelMin[0] - picture.pixelMin[0]) < 0.01f &&
                            std::fabs(c.pictures[0].pixelMin[1] - picture.pixelMin[1]) < 0.01f) {
                            ++moved;
                        }
                    }
                }
                if (moved == movedBefore && drawn - moved <= 30) {
                    MESSAGE(script << ": " << control.name << " picture " << p << " (" << picture.key
                                   << ") is not drawn where it was; anchor " << control.anchor
                                   << std::string(control.anchorDeclared ? " declared" : "") << ", backdrop " << control.backdropFade);
                }
                engine->SetScriptText(picture.add.script, nullptr);
            }
        }
        engine->CloseAll();
    }
    MESSAGE(pictures << " pictures: " << unknown << " not added by a script call seen, " << shared
                     << " added in loops or functions, " << failed << " refused; " << moved << " of " << drawn
                     << " moved and drawn where they were");
    CHECK(pictures > 0);
    engine.reset();
    std::filesystem::current_path(previous);
}

// TEMP: timing of a control move
TEST_CASE("ui editor: a picture placed by a SetPos after its SetPoint moves and grows by it")
{
    TempGame game;
    // As the character selection's background (DLG_UI_Character_Selection_Back_New.lua), whose SetPos puts it back
    // wherever its SetPoint is moved to (2026-11-16).
    std::ofstream(game.root / "data" / "ui" / "scripts" / "DLG_PicPos.lua") << "\xEF\xBB\xBF" << R"lua(
FALSE = 0
g_pStatic = g_pUIDialog:CreateStatic()
g_pUIDialog:AddControl(g_pStatic)
g_pStatic:SetName("Back")
g_pPicture = g_pUIDialog:CreatePicture()
g_pStatic:AddPicture(g_pPicture)
g_pPicture:SetTex("DLG_Test.tga")
g_pPicture:SetPoint({ USE_TEXTURE_SIZE = FALSE, LEFT_TOP = D3DXVECTOR2(0, 0), RIGHT_BOTTOM = D3DXVECTOR2(200, 100) })
g_pPicture:SetPos(D3DXVECTOR2(-245, 0))
)lua";
    auto render = NullRender();
    x2::ui_editor::EngineOptions options;
    options.gameFolder = game.root;
    options.mountArchives = false;
    std::string error;
    auto engine = x2::ui_editor::Engine::Create(*render, options, error);
    REQUIRE_MESSAGE(engine, error);
    REQUIRE(engine->OpenDialog("DLG_PicPos.lua", error) == 0);
    auto dialogs = engine->Inspect();
    const auto& picture = dialogs[0].controls[0].pictures[0];
    CHECK(picture.point.line == 9);
    REQUIRE(picture.pos.Known());
    CHECK(picture.pos.line == 10);
    CHECK(picture.pos.kind == x2::ui_editor::CallKind::PicturePos);
    CHECK(picture.points[0][0] == -245.0f);

    std::string text;
    REQUIRE_MESSAGE(engine->ScriptText("DLG_PicPos.lua", text, error), error);
    REQUIRE_MESSAGE(x2::ui_editor::MoveCall(text, picture.pos.line, picture.pos.kind, 45, 10, error), error);
    CHECK(text.find("g_pPicture:SetPos(D3DXVECTOR2(-200, 10))") != std::string::npos);
    engine->SetScriptText("DLG_PicPos.lua", &text);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    dialogs = engine->Inspect();
    CHECK(dialogs[0].controls[0].pictures[0].points[0][0] == -200.0f);
    CHECK(dialogs[0].controls[0].pictures[0].points[0][1] == 10.0f);
    CHECK(dialogs[0].controls[0].pictures[0].points[3][0] == 0.0f);    // still 200 wide
    engine.reset();
}
