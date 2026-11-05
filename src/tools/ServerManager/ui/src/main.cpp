// ServerManager: the window over the servers (decisions T29, T30; docs/tools/servermanager.md#window).
//   ServerManager [profile.json | dist folder]
// Without an argument: servermanager.json, else dist/, in the current folder; or one loaded from the window.

#include "manager_ui.h"

#include "x2/app/imgui_layer.h"

#include <SDL3/SDL_main.h>   // WinMain on Windows, where the program has no console (GUI subsystem)
#include <imgui.h>

#include <memory>
#include <string>
#include <vector>

namespace {

void Fatal(const std::string& message)
{
    SDL_Log("%s", message.c_str());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ServerManager", message.c_str(), nullptr);
}

// Window positions and the log splitter, per user.
std::string IniFile()
{
    char* dir = SDL_GetPrefPath("x2", "ServerManager");
    if (!dir)
        return {};
    std::string path = std::string(dir) + "imgui.ini";
    SDL_free(dir);
    return path;
}

}  // namespace

int main(int argc, char* argv[])
{
    std::string error;
    auto sdl = x2::app::Sdl::Init(error);
    if (!sdl) {
        Fatal(error);
        return 1;
    }
    x2::app::WindowOptions options;
    options.title = "ServerManager";
    options.width = 1400;
    options.height = 850;
    auto window = x2::app::Window::Create(options, error);
    if (!window) {
        Fatal(error);
        return 1;
    }
    x2::app::ImGuiOptions guiOptions;
    guiOptions.iniFile = IniFile();
    auto gui = x2::app::ImGuiLayer::Create(*window, guiOptions, error);
    if (!gui) {
        Fatal(error);
        return 1;
    }

    x2::servermanager::ManagerUi ui([] { x2::app::WakeEventLoop(); });
    // SDL passes the arguments as UTF-8 on every platform
    std::vector<std::string> args(argv, argv + argc);
    const std::string profile = x2::servermanager::ManagerUi::FindProfile(args);
    if (!profile.empty() && !ui.Load(profile, error))
        Fatal(error);

    std::string title;
    int frames = 2;     // ImGui settles layout and hover over a couple of frames after an event
    while (!ui.ShouldQuit()) {
        x2::app::WaitForEvents(frames > 0 ? 0 : ui.IdleTimeoutMs());
        bool input = false;
        if (!x2::app::PollEvents([&](const SDL_Event& event) {
                gui->ProcessEvent(event);
                input = true;
            }, window->Id()))
            ui.RequestQuit();
        const bool changed = ui.Update();
        frames = input || changed ? 3 : frames - 1;
        if (window->Minimized())
            continue;

        const std::string wanted = ui.ProfilePath().empty() ? "ServerManager" : "ServerManager - " + ui.ProfilePath();
        if (wanted != title) {
            title = wanted;
            window->SetTitle(title);
        }
        gui->NewFrame();
        ui.Draw();
        x2::app::Frame frame;
        if (!window->BeginFrame(frame)) {
            Fatal(std::string("GPU: ") + SDL_GetError());
            break;
        }
        gui->Render(frame, SDL_FColor{0.08f, 0.09f, 0.10f, 1.0f});
        window->EndFrame(frame);
    }
    return 0;
}
