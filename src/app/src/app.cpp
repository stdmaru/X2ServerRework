#include "x2/app/app.h"

namespace x2::app {

namespace {

std::string SdlError(const char* what)
{
    return std::string(what) + ": " + SDL_GetError();
}

}  // namespace

std::unique_ptr<Sdl> Sdl::Init(std::string& error, SDL_InitFlags flags)
{
    if (!SDL_Init(flags)) {
        error = SdlError("SDL_Init");
        return nullptr;
    }
    return std::unique_ptr<Sdl>(new Sdl);
}

Sdl::~Sdl()
{
    SDL_Quit();
}

std::unique_ptr<Window> Window::Create(const WindowOptions& options, std::string& error)
{
    std::unique_ptr<Window> self(new Window);

    const float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    const float s = scale > 0.0f ? scale : 1.0f;
    SDL_WindowFlags flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (options.resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    self->m_window = SDL_CreateWindow(options.title.c_str(), static_cast<int>(static_cast<float>(options.width) * s),
                                      static_cast<int>(static_cast<float>(options.height) * s), flags);
    if (!self->m_window) {
        error = SdlError("SDL_CreateWindow");
        return nullptr;
    }

    SDL_SetWindowPosition(self->m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(self->m_window);
    return self;
}

Window::~Window()
{
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

SDL_WindowID Window::Id() const
{
    return SDL_GetWindowID(m_window);
}

float Window::ContentScale() const
{
    const float scale = SDL_GetWindowDisplayScale(m_window);
    return scale > 0.0f ? scale : 1.0f;
}

bool Window::Minimized() const
{
    return (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED) != 0;
}

void Window::SetTitle(const std::string& title)
{
    SDL_SetWindowTitle(m_window, title.c_str());
}

bool PollEvents(const std::function<void(const SDL_Event&)>& handler, SDL_WindowID mainWindow)
{
    bool running = true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (handler) {
            handler(event);
        }
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                   (mainWindow == 0 || event.window.windowID == mainWindow)) {
            running = false;
        }
    }
    return running;
}

void WaitForEvents(int timeoutMs)
{
    SDL_WaitEventTimeout(nullptr, timeoutMs);
}

void WakeEventLoop()
{
    SDL_Event event{};
    event.type = SDL_EVENT_USER;
    SDL_PushEvent(&event);
}

}  // namespace x2::app
