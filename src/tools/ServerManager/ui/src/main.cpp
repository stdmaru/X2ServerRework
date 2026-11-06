// ServerManager: the window over the servers (decisions T29, T30; docs/tools/servermanager.md#window).
//   ServerManager [profile.json | dist folder]
// Without an argument: profile.json next to the program, else dist/ in the current folder; or one loaded or made in
// the window (saved to profile.json next to the program).
//   ServerManager <prompt>   (run by ssh as its SSH_ASKPASS: docs/tools/servermanager.md#ssh-prompts)

#include "manager_ui.h"

#include "x2/servermanager/askpass.h"

#include "x2/app/imgui_layer.h"

#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_main.h>   // WinMain on Windows, where the program has no console (GUI subsystem)
#include <imgui.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace {

void Fatal(const std::string& message)
{
    SDL_Log("%s", message.c_str());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ServerManager", message.c_str(), nullptr);
}

// The user's folder for ServerManager's own files (ImGui's window state, crash.txt).
std::string PrefFile(const char* name)
{
    char* dir = SDL_GetPrefPath("x2", "ServerManager");
    if (!dir)
        return {};
    std::string path = std::string(dir) + name;
    SDL_free(dir);
    return path;
}

// Without a console (Windows GUI subsystem, a desktop launcher) an exception that escapes, on any thread, would only
// show "abort() has been called": say which one, in a message box and in crash.txt.
[[noreturn]] void OnTerminate()
{
    std::string text = "terminate called without an exception";
    if (const std::exception_ptr current = std::current_exception()) {
        try {
            std::rethrow_exception(current);
        } catch (const std::exception& e) {
            text = std::string("unhandled exception ") + typeid(e).name() + ": " + e.what();
        } catch (...) {
            text = "unhandled exception of an unknown type";
        }
    }
    const std::string file = PrefFile("crash.txt");
    if (std::FILE* out = file.empty() ? nullptr : std::fopen(file.c_str(), "w")) {
        std::fprintf(out, "%s\n", text.c_str());
        std::fclose(out);
    }
    Fatal(text + (file.empty() ? std::string() : "\n\n(also in " + file + ")"));
    std::abort();
}

// profile.json in the program's folder
std::string DefaultProfileFile()
{
    const char* base = SDL_GetBasePath();     // owned by SDL; ends with the separator
    return base ? std::string(base) + "profile.json" : std::string("profile.json");
}

// SDL's file and folder dialogs (the desktop portal or zenity on Linux): the answer comes on another thread.
void Pick(SDL_Window* window, x2::servermanager::ProfileEditor::PickKind kind, const std::string& start,
          std::function<void(std::string)> done)
{
    auto* callback = new std::function<void(std::string)>(std::move(done));
    const auto answer = [](void* user, const char* const* files, int) {
        std::unique_ptr<std::function<void(std::string)>> callback(static_cast<std::function<void(std::string)>*>(user));
        (*callback)(files && files[0] ? std::string(files[0]) : std::string());
        x2::app::WakeEventLoop();
    };
    const char* location = start.empty() ? nullptr : start.c_str();
    if (kind == x2::servermanager::ProfileEditor::PickKind::Folder)
        SDL_ShowOpenFolderDialog(answer, callback, window, location, false);
    else
        SDL_ShowOpenFileDialog(answer, callback, window, nullptr, 0, location, false);
}

}  // namespace

int main(int argc, char* argv[])
{
    // started by ssh as its SSH_ASKPASS: ask the window that started that ssh, print the answer, and exit
    if (x2::servermanager::IsAskpassInvocation())
        return x2::servermanager::RunAskpass(argc, argv);
    std::set_terminate(OnTerminate);
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
    auto device = x2::render::Device::Create({}, error);
    if (!device) {
        Fatal(error);
        return 1;
    }
    x2::render::WindowClaim claim(*device, window->Handle());
    if (!claim.Ok()) {
        Fatal(device->LastError());
        return 1;
    }
    x2::app::ImGuiOptions guiOptions;
    guiOptions.iniFile = PrefFile("imgui.ini");
    auto gui = x2::app::ImGuiLayer::Create(*window, *device, guiOptions, error);
    if (!gui) {
        Fatal(error);
        return 1;
    }

    x2::servermanager::ManagerUi ui([] { x2::app::WakeEventLoop(); });
    // SDL passes the arguments as UTF-8 on every platform
    std::vector<std::string> args(argv, argv + argc);
    const std::string defaultFile = DefaultProfileFile();
    ui.SetDefaultProfileFile(defaultFile);
    ui.SetPicker([handle = window->Handle()](auto kind, const std::string& start, auto done) {
        Pick(handle, kind, start, std::move(done));
    });
    // ssh's questions in a dialog: ssh runs this program again as its askpass (the start of main)
    if (const char* base = SDL_GetBasePath()) {
#if defined(_WIN32)
        const std::string self = std::string(base) + "ServerManager.exe";
#else
        const std::string self = std::string(base) + "ServerManager";
#endif
        if (!ui.EnableAskpass(self, error))
            SDL_Log("ssh prompts disabled: %s", error.c_str());
    }
    const std::string profile = x2::servermanager::ManagerUi::FindProfile(args, defaultFile);
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
        x2::render::CommandBuffer* commands = device->AcquireCommandBuffer();
        x2::render::SwapchainTexture swapchain;
        if (!commands || !commands->WaitAndAcquireSwapchainTexture(window->Handle(), swapchain)) {
            if (commands)
                commands->Cancel();
            Fatal("GPU: " + device->LastError());
            break;
        }
        gui->Render(*commands, swapchain.texture, {0.08f, 0.09f, 0.10f, 1.0f});
        commands->Submit();
    }
    return 0;
}
