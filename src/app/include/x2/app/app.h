#pragma once

// SDL3 application foundation (decision T32): window, events and the SDL_GPU device (T19), shared by the client and the
// tools. No ImGui here (that is x2/app/imgui_layer.h, tools only). See docs/porting/app.md.

#include <SDL3/SDL.h>

#include <cstdint>
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
    bool vsync = true;
};

// A frame being recorded: the command buffer and the swapchain texture to draw into. `target` is null when there is
// nothing to draw into (minimized window, swapchain not ready); the frame must still be ended.
struct Frame {
    SDL_GPUCommandBuffer* commands = nullptr;
    SDL_GPUTexture* target = nullptr;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

// A window with its own SDL_GPU device claimed for it.
class Window {
public:
    static std::unique_ptr<Window> Create(const WindowOptions& options, std::string& error);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    SDL_Window* Handle() const { return m_window; }
    SDL_GPUDevice* Device() const { return m_device; }
    SDL_WindowID Id() const;
    SDL_GPUTextureFormat SwapchainFormat() const;
    float ContentScale() const;     // display scale (1.0 = 96 dpi)
    bool Minimized() const;
    void SetTitle(const std::string& title);

    // BeginFrame acquires a command buffer and waits for the swapchain texture; EndFrame submits it.
    // BeginFrame returns false (and no frame) only if the device failed: see SDL_GetError().
    bool BeginFrame(Frame& frame);
    void EndFrame(Frame& frame);
    void WaitIdle();

private:
    Window() = default;

    SDL_Window* m_window = nullptr;
    SDL_GPUDevice* m_device = nullptr;
};

// Handles every pending event. Returns false when the application should quit: SDL_EVENT_QUIT, or a close request for
// `mainWindow` (0: any window).
bool PollEvents(const std::function<void(const SDL_Event&)>& handler, SDL_WindowID mainWindow = 0);

// Waits up to `timeoutMs` for an event (it stays queued for PollEvents). For tools that redraw only when something
// changed; wake it from another thread with WakeEventLoop.
void WaitForEvents(int timeoutMs);
void WakeEventLoop();

}  // namespace x2::app
