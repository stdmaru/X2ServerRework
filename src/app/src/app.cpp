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

    // SPIR-V (Vulkan), DXIL (D3D12) and MSL (Metal): whatever the platform has.
    self->m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
#if X2_DEV_TOOLS
                                         true,   // validation layers when available
#else
                                         false,
#endif
                                         nullptr);
    if (!self->m_device) {
        error = SdlError("SDL_CreateGPUDevice");
        return nullptr;
    }
    if (!SDL_ClaimWindowForGPUDevice(self->m_device, self->m_window)) {
        error = SdlError("SDL_ClaimWindowForGPUDevice");
        return nullptr;
    }
    SDL_SetGPUSwapchainParameters(self->m_device, self->m_window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                  options.vsync ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_IMMEDIATE);

    SDL_SetWindowPosition(self->m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(self->m_window);
    return self;
}

Window::~Window()
{
    if (m_device) {
        SDL_WaitForGPUIdle(m_device);
        if (m_window) {
            SDL_ReleaseWindowFromGPUDevice(m_device, m_window);
        }
        SDL_DestroyGPUDevice(m_device);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

SDL_WindowID Window::Id() const
{
    return SDL_GetWindowID(m_window);
}

SDL_GPUTextureFormat Window::SwapchainFormat() const
{
    return SDL_GetGPUSwapchainTextureFormat(m_device, m_window);
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

bool Window::BeginFrame(Frame& frame)
{
    frame = {};
    frame.commands = SDL_AcquireGPUCommandBuffer(m_device);
    if (!frame.commands) {
        return false;
    }
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(frame.commands, m_window, &frame.target, &frame.width, &frame.height)) {
        SDL_CancelGPUCommandBuffer(frame.commands);
        frame = {};
        return false;
    }
    return true;
}

void Window::EndFrame(Frame& frame)
{
    if (frame.commands) {
        SDL_SubmitGPUCommandBuffer(frame.commands);
    }
    frame = {};
}

void Window::WaitIdle()
{
    SDL_WaitForGPUIdle(m_device);
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
