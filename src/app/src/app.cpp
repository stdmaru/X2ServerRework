#include "x2/app/app.h"

#include <cstdint>
#include <cstring>

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

bool Window::SetIcon(const unsigned char* ico, std::size_t size, std::string& error)
{
    SDL_Surface* image = LoadIcoImage(ico, size, error);
    if (!image) {
        return false;
    }
    const bool ok = SDL_SetWindowIcon(m_window, image);
    if (!ok) {
        error = SdlError("SDL_SetWindowIcon");
    }
    SDL_DestroySurface(image);
    return ok;
}

SDL_Surface* LoadIcoImage(const unsigned char* ico, std::size_t size, std::string& error)
{
    auto u16 = [ico](std::size_t at) { return static_cast<std::uint32_t>(ico[at] | (ico[at + 1] << 8)); };
    auto u32 = [&u16](std::size_t at) { return u16(at) | (u16(at + 2) << 16); };
    static constexpr unsigned char kPngSignature[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};

    // ICONDIR: reserved 0, type 1, count; then 16-byte ICONDIRENTRYs (width, height: 0 means 256; ...; bytes, offset).
    if (size < 6 || u16(0) != 0 || u16(2) != 1) {
        error = "not an .ico file";
        return nullptr;
    }
    const std::size_t count = u16(4);
    std::size_t bestOffset = 0;
    std::size_t bestBytes = 0;
    std::uint32_t bestArea = 0;
    for (std::size_t i = 0; i < count && 6 + 16 * (i + 1) <= size; ++i) {
        const std::size_t entry = 6 + 16 * i;
        const std::uint32_t width = ico[entry] ? ico[entry] : 256;
        const std::uint32_t height = ico[entry + 1] ? ico[entry + 1] : 256;
        const std::size_t bytes = u32(entry + 8);
        const std::size_t offset = u32(entry + 12);
        if (offset > size || bytes > size - offset || bytes < sizeof(kPngSignature) ||
            std::memcmp(ico + offset, kPngSignature, sizeof(kPngSignature)) != 0) {
            continue;
        }
        if (width * height > bestArea) {
            bestArea = width * height;
            bestOffset = offset;
            bestBytes = bytes;
        }
    }
    if (bestArea == 0) {
        error = "no PNG image in the .ico file";
        return nullptr;
    }
    SDL_IOStream* stream = SDL_IOFromConstMem(ico + bestOffset, bestBytes);
    SDL_Surface* image = stream ? SDL_LoadPNG_IO(stream, true) : nullptr;
    if (!image) {
        error = SdlError("SDL_LoadPNG_IO");
    }
    return image;
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
