// The ServerManager window drawn headless (ImGui's null backends): every panel with a real server behind it, so ImGui's
// own checks (ID conflicts, unbalanced Begin/End, disabled stacks) run over the code paths the window uses.

#include "manager_ui.h"

#include "x2/platform/process.h"

#include "x2/app/imgui_layer.h"

#include <imgui.h>
#include <imgui_impl_null.h>

#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
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

TEST_CASE("ui: a profile made in the Profile tab, saved next to the program and applied")
{
    namespace fs = std::filesystem;
    const fs::path dir = fs::temp_directory_path() / "x2_ui_profile_tab";
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir / "dist" / "LoginServer");
    const std::string file = (dir / "profile.json").string();
    CHECK(ManagerUi::FindProfile({"ServerManager"}, file).empty() == !fs::is_directory("dist"));

    NullImGui gui;
    ManagerUi ui;
    ui.SetDefaultProfileFile(file);         // no profile yet: an empty one in the Profile tab
    gui.Frame(ui);
    CHECK(ui.Editor().File() == file);
    CHECK_FALSE(ui.Editor().Dirty());

    Profile& draft = ui.Editor().Draft();
    HostSpec win;
    win.name = "win";
    win.destination = "Administrator@win.invalid";
    win.shell = HostSpec::Shell::Cmd;
    win.identityFile = (dir / "key").string();
    win.ssh = {"x2-no-such-ssh"};
    draft.hosts.push_back(win);
    ServerSpec login;
    login.name = "Login";
    login.directory = (dir / "dist" / "LoginServer").string();
    login.program = "./x2-no-such-server";
    login.args = {"DEV", "0"};
    draft.servers.push_back(login);
    ServerSpec game;
    game.name = "Game";
    game.host = "win";
    game.control = "GameServer";
    game.after = {"Login"};
    draft.servers.push_back(game);
    CHECK(ui.Editor().Dirty());
    ui.ShowTab(ManagerUi::Tab::Profile);
    gui.Frame(ui);      // general, the selected host and server, with a remote one and dependencies
    gui.Frame(ui);

    std::string error;
    REQUIRE_MESSAGE(ui.ApplyProfile(error), error);
    CHECK_FALSE(ui.Editor().Dirty());
    REQUIRE(ui.GetOrchestrator());
    CHECK(ui.GetOrchestrator()->Size() == 2);
    CHECK(ui.ProfilePath() == file);
    CHECK(ManagerUi::FindProfile({"ServerManager"}, file) == file);   // next start: this file
    std::ifstream in(file);
    const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    CHECK(text.find(R"("directory": "dist)") != std::string::npos);    // relative to the file's folder
    CHECK(text.find(R"("shell": "cmd")") != std::string::npos);
    gui.Frame(ui);

    // an invalid draft is not saved
    ui.Editor().Draft().servers[1].after = {"Nobody"};
    gui.Frame(ui);
    CHECK_FALSE(ui.SaveProfile(error));
    CHECK(error.find("Nobody") != std::string::npos);
    ui.ShowTab(ManagerUi::Tab::Servers);
    gui.Frame(ui);
    fs::remove_all(dir, ec);
}

TEST_CASE("ui: a dist folder is saved to the default profile file")
{
    namespace fs = std::filesystem;
    const fs::path dir = fs::temp_directory_path() / "x2_ui_profile_dist";
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir / "dist");
    NullImGui gui;
    ManagerUi ui;
    ui.SetDefaultProfileFile((dir / "profile.json").string());
    std::string error;
    REQUIRE_MESSAGE(ui.Load((dir / "dist").string(), error), error);
    CHECK(ui.Editor().File() == (dir / "profile.json").string());
    CHECK(ui.Editor().Draft().servers.size() == 6);
    CHECK_FALSE(ui.Editor().Dirty());
    ui.ShowTab(ManagerUi::Tab::Profile);
    gui.Frame(ui);
    REQUIRE_MESSAGE(ui.SaveProfile(error), error);
    const auto loaded = LoadProfile((dir / "profile.json").string(), error);
    REQUIRE_MESSAGE(loaded, error);
    CHECK(loaded->servers.size() == 6);
    fs::remove_all(dir, ec);
}

#if defined(X2_ASKPASS_HELPER)
TEST_CASE("ui: ssh's questions in a dialog")
{
    NullImGui gui;
    ManagerUi ui;
    std::string error;
    REQUIRE_MESSAGE(ui.EnableAskpass(X2_ASKPASS_HELPER, error), error);
    const auto ask = [](const std::string& prompt) {
        x2::platform::ProcessOptions options;
        options.program = X2_ASKPASS_HELPER;
        options.args = {prompt};
        std::string startError;
        auto p = x2::platform::Process::Start(options, startError);
        REQUIRE_MESSAGE(p, startError);
        return p;
    };
    // each kind is drawn, then answered as its buttons would
    const std::pair<std::string, std::optional<std::string>> cases[] = {
        {"x2@vps's password: ", std::string("pw")},
        {"Are you sure you want to continue connecting (yes/no/[fingerprint])? ", std::string("yes")},
        {"Enter passphrase for key '/home/x2/.ssh/id_ed25519': ", std::nullopt},
    };
    for (const auto& [prompt, answer] : cases) {
        auto helper = ask(prompt);
        const auto deadline = std::chrono::steady_clock::now() + 15s;
        while (ui.Askpass().Pending().empty() && std::chrono::steady_clock::now() < deadline)
            std::this_thread::sleep_for(10ms);
        REQUIRE(ui.Askpass().Pending().size() == 1);
        gui.Frame(ui);      // opens the dialog
        gui.Frame(ui);
        ui.Askpass().Answer(ui.Askpass().Pending().front().id, answer);
        gui.Frame(ui);      // closes it
        std::string line;
        if (answer) {
            REQUIRE(helper->ReadLine(line));
            CHECK(line == *answer);
        }
        CHECK(helper->Wait() == (answer ? 0 : 1));
    }
    ui.Askpass().Stop();
}
#endif

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
