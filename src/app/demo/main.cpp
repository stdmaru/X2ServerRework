// x2_app_demo: manual test of the SDL3 + ImGui foundation (docs/porting/app.md). Opens a window with the ImGui demo and a
// line of Korean text, to check the window, SDL_GPU, input, high DPI and the Hangul font.

#include "x2/app/imgui_layer.h"

#include <imgui.h>

#include <cstdio>

int main(int, char**)
{
    std::string error;
    auto sdl = x2::app::Sdl::Init(error);
    if (!sdl) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }

    x2::app::WindowOptions options;
    options.title = "x2_app_demo";
    auto window = x2::app::Window::Create(options, error);
    if (!window) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }

    auto gui = x2::app::ImGuiLayer::Create(*window, {}, error);
    if (!gui) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }

    const char* driver = SDL_GetGPUDeviceDriver(window->Device());
    bool showDemo = true;
    bool running = true;
    while (running) {
        running = x2::app::PollEvents([&](const SDL_Event& event) { gui->ProcessEvent(event); }, window->Id());
        if (window->Minimized()) {
            x2::app::WaitForEvents(100);
            continue;
        }

        gui->NewFrame();
        if (showDemo) {
            ImGui::ShowDemoWindow(&showDemo);
        }
        ImGui::Begin("x2::app");
        ImGui::Text("GPU driver: %s", driver ? driver : "?");
        ImGui::Text("Video driver: %s", SDL_GetCurrentVideoDriver());
        ImGui::Text("Content scale: %.2f", static_cast<double>(window->ContentScale()));
        ImGui::Text("Font: %s", gui->FontFile().empty() ? "(built-in, no Hangul)" : gui->FontFile().c_str());
        ImGui::TextUnformatted("한국어: 서버 준비 완료");
        ImGui::Checkbox("ImGui demo", &showDemo);
        ImGui::Text("%.1f FPS", static_cast<double>(ImGui::GetIO().Framerate));
        ImGui::End();

        x2::app::Frame frame;
        if (!window->BeginFrame(frame)) {
            std::fprintf(stderr, "frame: %s\n", SDL_GetError());
            break;
        }
        gui->Render(frame, SDL_FColor{0.10f, 0.11f, 0.13f, 1.0f});
        window->EndFrame(frame);
    }
    return 0;
}
