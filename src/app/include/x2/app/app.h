#pragma once

// SDL3 application foundation (decision T32): window and events, shared by the client and the tools. The GPU device is
// x2::render's (T36), created for the window. No ImGui here (that is x2/app/imgui_layer.h, tools only).
// See docs/porting/app.md.

#include <SDL3/SDL.h>

#include <functional>
#include <memory>
#include <string>

namespace x2::app {

// SDL for the whole process: SDL_Init on creation, SDL_Quit on destruction. Create it once, before any window.
class Sdl {
public:
    static std::unique_ptr<Sdl> Init(std::string& error, SDL_InitFlags flags = SDL_INIT_VIDEO);
    ~Sdl();

    Sdl(const Sdl&) = delete;
    Sdl& operator=(const Sdl&) = delete;

private:
    Sdl() = default;
};

struct WindowOptions {
    std::string title = "X2";
    int width = 1280;       // logical size; multiplied by the display content scale
    int height = 800;
    bool resizable = true;
};

// A window. Drawing into it goes through an x2::render::Device that claims it (x2::render::WindowClaim).
class Window {
public:
    static std::unique_ptr<Window> Create(const WindowOptions& options, std::string& error);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    SDL_Window* Handle() const { return m_window; }
    SDL_WindowID Id() const;
    float ContentScale() const;     // display scale (1.0 = 96 dpi)
    bool Minimized() const;
    void SetTitle(const std::string& title);

private:
    Window() = default;

    SDL_Window* m_window = nullptr;
};

// Handles every pending event. Returns false when the application should quit: SDL_EVENT_QUIT, or a close request for
// `mainWindow` (0: any window).
bool PollEvents(const std::function<void(const SDL_Event&)>& handler, SDL_WindowID mainWindow = 0);

// Waits up to `timeoutMs` for an event (it stays queued for PollEvents). For tools that redraw only when something
// changed; wake it from another thread with WakeEventLoop.
void WaitForEvents(int timeoutMs);
void WakeEventLoop();

}  // namespace x2::app
