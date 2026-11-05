// The ServerManager window drawn headless (ImGui's null backends): every panel with a real server behind it, so ImGui's
// own checks (ID conflicts, unbalanced Begin/End, disabled stacks) run over the code paths the window uses.

#include "manager_ui.h"

#include "x2/app/imgui_layer.h"

#include <imgui.h>
#include <imgui_impl_null.h>

#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

using namespace x2::servermanager;
using namespace std::chrono_literals;

namespace {

struct NullImGui {
    NullImGui()
    {
        context = ImGui::CreateContext();
        ImGui::GetIO().IniFilename = nullptr;
        x2::app::ConfigureImGui({}, 1.0f);
        ImGui_ImplNullPlatform_Init();
        ImGui_ImplNullRender_Init();
        ImGui::GetIO().DisplaySize = ImVec2(1400, 850);
    }
    ~NullImGui()
    {
        ImGui_ImplNullRender_Shutdown();
        ImGui_ImplNullPlatform_Shutdown();
        ImGui::DestroyContext(context);
    }
    void Frame(ManagerUi& ui)
    {
        ui.Update();
        ImGui_ImplNullRender_NewFrame();
        ImGui_ImplNullPlatform_NewFrame();
        ImGui::NewFrame();
        ui.Draw();
        ImGui::Render();
        ImGui_ImplNullRender_RenderDrawData(ImGui::GetDrawData());
    }
    // Frames until `done` or 20 s.
    template <class Done>
    bool FramesUntil(ManagerUi& ui, Done done)
    {
        const auto deadline = std::chrono::steady_clock::now() + 20s;
        while (!done()) {
            if (std::chrono::steady_clock::now() > deadline)
                return false;
            if (Orchestrator* o = ui.GetOrchestrator())
                o->Events().Wait(20ms);
            Frame(ui);
        }
        Frame(ui);
        return true;
    }
    ImGuiContext* context;
};

std::string WriteProfile(const std::string& name, const std::string& json)
{
    const auto path = std::filesystem::temp_directory_path() / (name + ".json");
    std::ofstream(path) << json;
    return path.string();
}

std::string JsonPath(std::string path)
{
    for (char& c : path)
        if (c == '\\')
            c = '/';
    return path;
}

}  // namespace

TEST_CASE("ui: without a profile, and with a profile that does not load")
{
    NullImGui gui;
    ManagerUi ui;
    gui.Frame(ui);
    CHECK_FALSE(ui.GetOrchestrator());
    std::string error;
    CHECK_FALSE(ui.Load("/nonexistent/servermanager.json", error));
    CHECK_FALSE(error.empty());
    gui.Frame(ui);
    CHECK(ManagerUi::FindProfile({"ServerManager", "x.json"}) == "x.json");
}

TEST_CASE("ui: a failed server and a remote host that cannot be reached")
{
    NullImGui gui;
    ManagerUi ui;
    std::string error;
    const std::string path = WriteProfile("x2_ui_failed", R"({
        "hosts": [{"name": "far", "destination": "far.invalid", "ssh": ["x2-no-such-ssh"]}],
        "servers": [{"name": "Missing", "program": "./x2-no-such-server"},
                    {"name": "Far", "host": "far", "control": "Far"}]})");
    REQUIRE_MESSAGE(ui.Load(path, error), error);
    Orchestrator& o = *ui.GetOrchestrator();
    o.StartAll();
    CHECK(gui.FramesUntil(ui, [&] { return o.At(0).phase == Phase::Failed && o.At(1).phase == Phase::Failed; }));
    CHECK_FALSE(o.HostAt(0).running);
    ui.RequestQuit();
    CHECK(ui.ShouldQuit());     // nothing started here is running
    std::filesystem::remove(path);
}

#if defined(X2_TERMINAL_DEMO)
TEST_CASE("ui: a server started from the window, its log, its prompt and quitting")
{
    NullImGui gui;
    ManagerUi ui;
    const std::string name = "x2_ui_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count() % 1000000);
    const std::string path = WriteProfile(name, R"({"servers": [{"name": "Demo", "program": ")" +
                                                    JsonPath(X2_TERMINAL_DEMO) + R"(", "args": ["--ready-after=100"],
                                                    "control": ")" + name + R"("}]})");
    std::string error;
    REQUIRE_MESSAGE(ui.Load(path, error), error);
    Orchestrator& o = *ui.GetOrchestrator();
    o.StartAll();
    REQUIRE(gui.FramesUntil(ui, [&] { return o.At(0).phase == Phase::Ready; }));

    // a multi-line result and enough entries for the clipper
    bool done = false;
    o.At(0).session.RunLua("for i = 1, 300 do print('line ' .. i) end", [&](const ResultMessage&) { done = true; });
    REQUIRE(gui.FramesUntil(ui, [&] { return done; }));
    for (int i = 0; i < 3; ++i)
        gui.Frame(ui);

    // quitting asks first, then stops what it started
    ui.RequestQuit();
    CHECK_FALSE(ui.ShouldQuit());
    gui.Frame(ui);      // the dialog opens
    o.Stop(0);          // as the dialog's button does
    CHECK(gui.FramesUntil(ui, [&] { return o.At(0).phase == Phase::Offline; }));
    CHECK(o.At(0).exitCode == 0);
    std::filesystem::remove(path);
}
#endif
