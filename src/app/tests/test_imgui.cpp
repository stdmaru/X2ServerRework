// ImGui setup without a window: ImGui's null backends render to nothing, which still builds the draw lists.

#include "x2/app/imgui_layer.h"

#include <imgui.h>
#include <imgui_impl_null.h>

#include <doctest/doctest.h>

#include <filesystem>

namespace {

struct NullImGui {
    NullImGui()
    {
        context = ImGui::CreateContext();
        ImGui::GetIO().IniFilename = nullptr;
    }
    void Init()
    {
        ImGui_ImplNullPlatform_Init();
        ImGui_ImplNullRender_Init();
    }
    ~NullImGui()
    {
        ImGui_ImplNullRender_Shutdown();
        ImGui_ImplNullPlatform_Shutdown();
        ImGui::DestroyContext(context);
    }
    ImGuiContext* context;
};

}  // namespace

TEST_CASE("imgui: missing fonts fall back to the built-in font")
{
    NullImGui gui;
    x2::app::ImGuiOptions options;
    options.fontFiles = {"/nonexistent/font.ttf"};
    options.hangulFontFiles = {"/nonexistent/hangul.ttf"};
    CHECK(x2::app::ConfigureImGui(options, 1.0f).empty());
    CHECK(ImGui::GetIO().Fonts->Fonts.Size == 1);
}

TEST_CASE("imgui: the first existing font file is loaded")
{
    NullImGui gui;
    std::string found;
    for (const std::string& file : x2::app::DefaultFontFiles()) {
        if (std::filesystem::is_regular_file(file)) {
            found = file;
            break;
        }
    }
    if (found.empty()) {
        MESSAGE("no system CJK font installed; skipped");
        return;
    }
    x2::app::ImGuiOptions options;
    options.fontFiles = {"/nonexistent/font.ttf", found};
    CHECK(x2::app::ConfigureImGui(options, 1.0f) == found);
}

TEST_CASE("imgui: the Hangul font is merged into a Latin main font, or is the main font without one")
{
    std::string latin, hangul;
    for (const std::string& file : x2::app::DefaultFontFiles())
        if (latin.empty() && std::filesystem::is_regular_file(file))
            latin = file;
    for (const std::string& file : x2::app::DefaultHangulFontFiles())
        if (hangul.empty() && std::filesystem::is_regular_file(file))
            hangul = file;
    if (hangul.empty()) {
        MESSAGE("no system CJK font installed; skipped");
        return;
    }
    {
        NullImGui gui;
        x2::app::ImGuiOptions options;
        options.fontFiles = {"/nonexistent/font.ttf"};
        options.hangulFontFiles = {hangul};
        CHECK(x2::app::ConfigureImGui(options, 1.0f) == hangul);
        REQUIRE(ImGui::GetIO().Fonts->Fonts.Size == 1);
        CHECK(ImGui::GetIO().Fonts->Fonts[0]->Sources.Size == 1);
    }
    if (latin.empty() || latin == hangul)
        return;     // Linux: the CJK font is the main font
    NullImGui gui;
    CHECK(x2::app::ConfigureImGui({}, 1.0f) == latin);
    REQUIRE(ImGui::GetIO().Fonts->Fonts.Size == 1);
    CHECK(ImGui::GetIO().Fonts->Fonts[0]->Sources.Size == 2);   // Latin first: its own backslash
}

TEST_CASE("imgui: a frame with Korean text renders with the null backends")
{
    NullImGui gui;
    x2::app::ConfigureImGui({}, 2.0f);
    gui.Init();
    ImGui::GetIO().DisplaySize = ImVec2(800, 600);

    for (int i = 0; i < 2; ++i) {
        ImGui_ImplNullRender_NewFrame();
        ImGui_ImplNullPlatform_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("test");
        ImGui::TextUnformatted("서버 준비 완료");
        ImGui::End();
        ImGui::Render();
        ImGui_ImplNullRender_RenderDrawData(ImGui::GetDrawData());
    }
    CHECK(ImGui::GetDrawData()->TotalVtxCount > 0);
    CHECK(ImGui::GetStyle().FontScaleDpi == doctest::Approx(2.0f));
}
