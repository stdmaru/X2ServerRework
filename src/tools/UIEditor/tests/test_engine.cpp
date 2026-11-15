// The UI editor's engine, headless on the null render backend: a dialog script from a loose data folder is opened,
// drawn and inspected (docs/tools/ui-editor.md).

#include "engine.h"

#include <doctest/doctest.h>

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
    engine->ApplyAnchors(0, entry, {});
    auto dialogs = engine->Inspect();
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
    CHECK(dialogs[0].controls[0].pictures[0].point.line == 11);
    CHECK(dialogs[0].controls[0].pictures[0].point.shared == 1);
    CHECK(dialogs[0].controls[1].pictures[0].point.line == 17);
    CHECK(dialogs[0].controls[0].points.empty());   // a static's points are its pictures'
    CHECK_FALSE(dialogs[1].posSource.Known());      // DLG_Text.lua sets no position
    REQUIRE(dialogs[1].controls[0].strings.size() == 1);
    CHECK(dialogs[1].controls[0].strings[0].source.line == 5);
    CHECK(dialogs[1].controls[0].strings[0].source.kind == x2::ui_editor::CallKind::String);
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

    // Undone: the file's text again, reloaded in the game's state.
    engine->SetScriptText("DLG_Test.lua", nullptr);
    REQUIRE_MESSAGE(engine->ReloadDialog(0, error), error);
    dialogs = engine->Inspect();
    CHECK(dialogs[0].pos[0] == 40.0f);
    CHECK(dialogs[0].controls[0].pictures[0].points[0][0] == 0.0f);
    CHECK(x2::ui_editor::FindLooseScript(game.root, "dlg_TEST.lua") == game.root / "data" / "ui" / "scripts" / "DLG_Test.lua");
    CHECK(x2::ui_editor::FindLooseScript(game.root, "DLG_Hidden.lua").empty());     // .kom/ skipped
}
