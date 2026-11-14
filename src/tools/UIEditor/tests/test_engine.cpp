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
    REQUIRE(scripts.size() == 3);
    CHECK(scripts[0] == "dlg_another.lua");
    CHECK(scripts[1] == "DLG_Test.lua");
    CHECK(scripts[2] == "DLG_Text.lua");
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
