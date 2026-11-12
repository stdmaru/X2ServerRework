#include "DXUT.h"

#include "SdlInput.h"
#include "SdlMessages.h"

#include "Device.h"

#include "x2/app/app.h"
#include "x2/render/render.h"
#include "x2/platform/unicode.h"

#include <SDL3/SDL.h>

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <memory>
#include <string>
#include <vector>

namespace {

struct DxutState {
    LPDXUTCALLBACKFRAMEMOVE frameMove = nullptr;
    void* frameMoveContext = nullptr;
    LPDXUTCALLBACKFRAMERENDER frameRender = nullptr;
    void* frameRenderContext = nullptr;
    LPDXUTCALLBACKKEYBOARD keyboard = nullptr;
    void* keyboardContext = nullptr;
    LPDXUTCALLBACKMSGPROC msgProc = nullptr;
    void* msgProcContext = nullptr;
    LPDXUTCALLBACKDEVICECREATED deviceCreated = nullptr;
    void* deviceCreatedContext = nullptr;
    LPDXUTCALLBACKDEVICERESET deviceReset = nullptr;
    void* deviceResetContext = nullptr;
    LPDXUTCALLBACKDEVICELOST deviceLost = nullptr;
    void* deviceLostContext = nullptr;
    LPDXUTCALLBACKDEVICEDESTROYED deviceDestroyed = nullptr;
    void* deviceDestroyedContext = nullptr;
    bool deviceHandedOut = false;  // DXUTCreateDevice called the Created callback: Destroyed is owed
    int resetWidth = 0;            // back buffer size the Reset callback last saw
    int resetHeight = 0;
    int resolutionWidth = 0;       // the game's resolution (pp.BackBufferWidth/Height), windowed or full screen
    int resolutionHeight = 0;

    bool handleAltEnter = true;
    std::vector<std::string> commandLine;
    bool showCursorWhenFullScreen = true;
    bool clipCursorWhenFullScreen = true;
    bool autoChangeAdapter = true;

    std::unique_ptr<x2::app::Sdl> sdl;
    // Lives with the window: created by DXUTCreateWindow, destroyed by ReleaseWindow. Never left to the static
    // destructor, which runs in exit() after the GPU driver and its layers have torn themselves down.
    std::unique_ptr<x2::render::Device> device;
    std::unique_ptr<x2::d3d9::Device> d3dDevice;    // KTDXLIB's D3D9 device on `device` (T38)
    std::unique_ptr<x2::app::Window> window;
    bool windowClaimed = false;
    HWND hwnd = nullptr;
    HINSTANCE hinstance = nullptr;
    x2::dxut::SdlMessageTranslator translator;
    x2::dxut::InputState input;  // DirectInput's view of the keyboard and mouse (CKTDIDevice)

    bool keys[256] = {};
    bool mouseButtons[5] = {};  // left, middle, right, X1, X2 (DXUT's order)
    bool active = true;
    bool minimized = false;
    bool windowed = true;
    bool quit = false;
    int exitCode = 0;

    double time = 0.0;
    float elapsedTime = 0.0f;
    float fps = 0.0f;
    double lastStatsTime = 0.0;
    DWORD framesSinceStats = 0;

    UINT presentInterval = D3DPRESENT_INTERVAL_DEFAULT;  // DXUTDeviceSettings::pp.PresentationInterval
    UINT forcedVsync = D3DPRESENT_INTERVAL_DEFAULT;      // DXUTGetVerticalSync
    float frameLimit = 0.f;                              // DXUTSetFrameLimit: frames per second, 0 for none
    std::uint64_t nextFrameNs = 0;                       // when the next frame is due under the limit
};

DxutState& State()
{
    static DxutState state;
    return state;
}

HWND HandleOf(SDL_Window* window)
{
#if defined(_WIN32)
    return static_cast<HWND>(
        SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
#else
    return reinterpret_cast<HWND>(window);
#endif
}

x2::dxut::TranslateContext ContextOf(SDL_Window* window)
{
    x2::dxut::TranslateContext context;
    context.window = SDL_GetWindowID(window);
    context.pixelDensity = SDL_GetWindowPixelDensity(window);
    if (context.pixelDensity <= 0.0f) {
        context.pixelDensity = 1.0f;
    }
    SDL_GetWindowPosition(window, &context.windowX, &context.windowY);
    SDL_GetWindowSizeInPixels(window, &context.pixelWidth, &context.pixelHeight);
    context.mod = SDL_GetModState();
    return context;
}

void UpdateMouseButtons(DxutState& state, WPARAM keys)
{
    state.mouseButtons[0] = (keys & MK_LBUTTON) != 0;
    state.mouseButtons[1] = (keys & MK_MBUTTON) != 0;
    state.mouseButtons[2] = (keys & MK_RBUTTON) != 0;
    state.mouseButtons[3] = (keys & MK_XBUTTON1) != 0;
    state.mouseButtons[4] = (keys & MK_XBUTTON2) != 0;
}

void UpdateFrameStats(DxutState& state)
{
    ++state.framesSinceStats;
    const double now = DXUTGetGlobalTimer()->GetAbsoluteTime();
    if (now - state.lastStatsTime > 1.0) {
        state.fps = static_cast<float>(state.framesSinceStats / (now - state.lastStatsTime));
        state.lastStatsTime = now;
        state.framesSinceStats = 0;
    }
}

// DXUT's Render3DEnvironment: time, FrameMove, FrameRender, then present (a cleared back buffer until KTDXLIB renders).
void Render3DEnvironment(DxutState& state)
{
    CDXUTTimer* timer = DXUTGetGlobalTimer();
    state.time = timer->GetTime();
    state.elapsedTime = static_cast<float>(timer->GetElapsedTime());
    UpdateFrameStats(state);

    if (state.frameMove) {
        state.frameMove(state.d3dDevice.get(), state.time, state.elapsedTime, state.frameMoveContext);
    }
    if (state.quit || !state.window) {  // DXUTShutdown from inside the callback
        return;
    }
    if (state.frameRender) {
        state.frameRender(state.d3dDevice.get(), state.time, state.elapsedTime, state.frameRenderContext);
    }
    if (state.quit || !state.window) {
        return;
    }
    // DXUT presents after the frame callback (its BeginScene/EndScene are the callback's own).
    if (!state.d3dDevice->Present()) {
        SDL_Log("DXUT: Present failed: %s", state.d3dDevice->LastError().c_str());
        state.exitCode = 1;
        state.quit = true;
    }
}

// X2_RENDER_BACKEND chooses the renderer ("sdlgpu", the default, or "null" for headless runs and tests; T36).
x2::render::Backend RenderBackend()
{
    x2::render::Backend backend = x2::render::Backend::SdlGpu;
    if (const char* name = SDL_getenv("X2_RENDER_BACKEND"); name && *name) {
        if (!x2::render::ParseBackend(name, backend)) {
            SDL_Log("DXUT: unknown X2_RENDER_BACKEND \"%s\", using sdlgpu", name);
        }
    }
    return backend;
}

// DXUT's DXUTCleanup3DEnvironment: the application lets go of the device before it is destroyed.
void ReleaseDeviceObjects(DxutState& state)
{
    if (!state.deviceHandedOut) {
        return;
    }
    state.deviceHandedOut = false;
    if (state.deviceLost) {
        state.deviceLost(state.deviceLostContext);
    }
    if (state.deviceDestroyed) {
        state.deviceDestroyed(state.deviceDestroyedContext);
    }
}

// D3D9's Reset after a mode change: the game sets its viewport, projection and UI scale and remakes its render
// targets in the Lost/Reset callbacks, so they run whenever the window's pixel size is no longer the one they saw
// (a resolution chosen in the options, full screen toggled, or the window manager resizing the window).
void ResetIfResized(DxutState& state)
{
    if (!state.deviceHandedOut || !state.window || !state.d3dDevice) {
        return;
    }
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(state.window->Handle(), &width, &height);
    if (width <= 0 || height <= 0 || (width == state.resetWidth && height == state.resetHeight)) {
        return;
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO, "DXUT: back buffer %dx%d", width, height);
    state.resetWidth = width;
    state.resetHeight = height;
    if (state.deviceLost) {
        state.deviceLost(state.deviceLostContext);
    }
    if (state.deviceReset) {
        state.deviceReset(state.d3dDevice.get(), DXUTGetBackBufferSurfaceDesc(), state.deviceResetContext);
    }
}

// Full screen shows the game's resolution: the closest display mode to it (emulated and scaled by the compositor on
// Wayland), or the desktop when no resolution is known.
bool SetFullscreenMode(SDL_Window* window, int width, int height)
{
    SDL_DisplayMode mode;
    if (width > 0 && height > 0 &&
        SDL_GetClosestFullscreenDisplayMode(SDL_GetDisplayForWindow(window), width, height, 0.f, true, &mode)) {
        return SDL_SetWindowFullscreenMode(window, &mode);
    }
    return SDL_SetWindowFullscreenMode(window, nullptr);
}

void ReleaseWindow(DxutState& state)
{
    ReleaseDeviceObjects(state);
    if (state.window) {
        if (state.windowClaimed) {
            state.d3dDevice.reset();
            state.device->WaitIdle();
            state.device->ReleaseWindow(state.window->Handle());
            state.windowClaimed = false;
        }
        SDL_StopTextInput(state.window->Handle());
    }
    state.d3dDevice.reset();
    state.device.reset();
    if (x2::dxut::BoundIme() == &state.translator.Ime()) {
        x2::dxut::BindIme(nullptr);
    }
    state.window.reset();
    state.hwnd = nullptr;
}

}  // namespace

void DXUTSetCallbackFrameMove(LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext)
{
    State().frameMove = pCallbackFrameMove;
    State().frameMoveContext = pUserContext;
}

void DXUTSetCallbackFrameRender(LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext)
{
    State().frameRender = pCallbackFrameRender;
    State().frameRenderContext = pUserContext;
}

void DXUTSetCallbackKeyboard(LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext)
{
    State().keyboard = pCallbackKeyboard;
    State().keyboardContext = pUserContext;
}

void DXUTSetCallbackMsgProc(LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext)
{
    State().msgProc = pCallbackMsgProc;
    State().msgProcContext = pUserContext;
}

void DXUTSetCallbackDeviceCreated(LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext)
{
    State().deviceCreated = pCallbackDeviceCreated;
    State().deviceCreatedContext = pUserContext;
}

void DXUTSetCallbackDeviceReset(LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext)
{
    State().deviceReset = pCallbackDeviceReset;
    State().deviceResetContext = pUserContext;
}

void DXUTSetCallbackDeviceLost(LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext)
{
    State().deviceLost = pCallbackDeviceLost;
    State().deviceLostContext = pUserContext;
}

void DXUTSetCallbackDeviceDestroyed(LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext)
{
    State().deviceDestroyed = pCallbackDeviceDestroyed;
    State().deviceDestroyedContext = pUserContext;
}

void DXUTSetCommandLine(int argc, char* argv[])
{
    State().commandLine.assign(argv, argv + argc);
}

const std::vector<std::string>& DXUTGetCommandLine()
{
    return State().commandLine;
}

void DXUTSetCloseOnAltF4(bool bClose)
{
    SDL_SetHint(SDL_HINT_WINDOWS_CLOSE_ON_ALT_F4, bClose ? "1" : "0");
}

HRESULT DXUTInit(bool /*bParseCommandLine*/, bool /*bHandleDefaultHotkeys*/, bool /*bShowMsgBoxOnError*/, bool bHandleAltEnter)
{
    DxutState& state = State();
    state.handleAltEnter = bHandleAltEnter;
    if (!state.sdl) {
        // KTDGUIIMEEditBox draws the composition string and the candidate list itself (it hid the IME windows on
        // WM_IME_SETCONTEXT): SDL sends them as events instead of showing the system's windows where it can.
        SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "composition,candidates");
        std::string error;
        state.sdl = x2::app::Sdl::Init(error, SDL_INIT_VIDEO);
        if (!state.sdl) {
            SDL_Log("DXUT: %s", error.c_str());
            return E_FAIL;
        }
        // Joysticks for CKTDIManager; the client runs without them, as it did without a DirectInput joystick.
        if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
            SDL_Log("DXUT: no joystick support: %s", SDL_GetError());
        }
    }
    DXUTGetGlobalTimer()->Reset();
    return S_OK;
}

namespace {

// Vsync on presents FIFO, one frame per vertical blank: the engine renders every loop between its fixed ticks (T41),
// so this follows the display's rate. Vsync off is immediate, or mailbox when tearing is not available.
x2::render::PresentMode PresentModeFor(x2::render::Device& device, SDL_Window* window, bool immediate)
{
    if (!immediate) {
        return x2::render::PresentMode::Vsync;
    }
    if (device.WindowSupportsPresentMode(window, x2::render::PresentMode::Immediate)) {
        return x2::render::PresentMode::Immediate;
    }
    if (device.WindowSupportsPresentMode(window, x2::render::PresentMode::Mailbox)) {
        return x2::render::PresentMode::Mailbox;
    }
    return x2::render::PresentMode::Vsync;
}

// The frame limit (DXUTSetFrameLimit): waits until the next frame is due. A frame that is late by more than one
// period starts the schedule again instead of hurrying to catch up.
void WaitForFrameLimit(DxutState& state)
{
    if (state.frameLimit <= 0.f) {
        state.nextFrameNs = 0;
        return;
    }
    const auto period = static_cast<std::uint64_t>(1e9 / state.frameLimit);
    const std::uint64_t now = SDL_GetTicksNS();
    if (state.nextFrameNs == 0 || now > state.nextFrameNs + period) {
        state.nextFrameNs = now + period;
        return;
    }
    if (now < state.nextFrameNs) {
        SDL_DelayPrecise(state.nextFrameNs - now);
    }
    state.nextFrameNs += period;
}

}  // namespace

HRESULT DXUTCreateWindow(const WCHAR* strWindowTitle, HINSTANCE /*hInstance*/, HICON /*hIcon*/, HMENU /*hMenu*/, int x, int y)
{
    DxutState& state = State();
    if (!state.sdl && FAILED(DXUTInit(false, false, true, true))) {
        return E_FAIL;
    }
    if (state.window) {
        return S_OK;
    }

    x2::app::WindowOptions options;
    options.title = strWindowTitle ? x2::platform::WideToUtf8(strWindowTitle) : std::string("X2");
    options.width = 1024;
    options.height = 768;
    // Fixed size (WS_OVERLAPPEDWINDOW was resizable): the game draws at the resolution of its options, which
    // DXUTCreateDeviceFromSettings applies. Tiling window managers (Hyprland, sway) float a window of fixed size, so the
    // game opens at its own size instead of a tile's (2026-11-12).
    options.resizable = false;
    std::string error;
    state.window = x2::app::Window::Create(options, error);
    if (!state.window) {
        SDL_Log("DXUT: %s", error.c_str());
        state.exitCode = 1;
        return E_FAIL;
    }
    SDL_Window* window = state.window->Handle();
    x2::render::DeviceOptions renderOptions;
    renderOptions.backend = RenderBackend();
    state.device = x2::render::Device::Create(renderOptions, error);
    if (!state.device) {
        SDL_Log("DXUT: %s", error.c_str());
        state.window.reset();
        state.exitCode = 1;
        return E_FAIL;
    }
    // D3DPRESENT_INTERVAL_DEFAULT: in step with the vertical blank, without tearing.
    if (!state.device->ClaimWindow(window) ||
        !state.device->SetSwapchainParameters(window, PresentModeFor(*state.device, window, false))) {
        SDL_Log("DXUT: %s", state.device->LastError().c_str());
        state.device->ReleaseWindow(window);
        state.device.reset();
        state.window.reset();
        state.exitCode = 1;
        return E_FAIL;
    }
    state.windowClaimed = true;
    state.d3dDevice = std::make_unique<x2::d3d9::Device>(*state.device, window);
    if (!state.d3dDevice->Ok()) {
        SDL_Log("DXUT: %s", state.d3dDevice->LastError().c_str());
        ReleaseWindow(state);
        state.exitCode = 1;
        return E_FAIL;
    }
    if (x != CW_USEDEFAULT && y != CW_USEDEFAULT) {
        SDL_SetWindowPosition(window, x, y);
    }
    state.hwnd = HandleOf(window);
#if defined(_WIN32)
    state.hinstance = static_cast<HINSTANCE>(
        SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr));
#endif
    // Windows delivers WM_CHAR and IME messages to any focused window; SDL only while text input is on.
    SDL_StartTextInput(window);
    // The window's input context for the IME edit box's IMM32 calls (Ime.h): what the IME does on request goes back to
    // SDL and, as the IME's messages, to the window procedure.
    x2::dxut::ImeContext& ime = state.translator.Ime();
    ime.clearComposition = [] {
        if (State().window) {
            SDL_ClearComposition(State().window->Handle());
        }
    };
    ime.send = [](const x2::dxut::WinMessage& message) {
        if (State().hwnd) {
            DXUTStaticWndProc(State().hwnd, message.message, message.wParam, message.lParam);
        }
    };
    x2::dxut::BindIme(&ime);
    state.active = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    state.input.SetFocused(state.active);
    state.quit = false;
    return S_OK;
}

HRESULT DXUTCreateDevice(UINT /*AdapterOrdinal*/, bool bWindowed, int nSuggestedWidth, int nSuggestedHeight)
{
    DxutState& state = State();
    if (!state.window || !state.d3dDevice) {
        return E_FAIL;
    }
    if (state.deviceHandedOut) {
        return S_OK;
    }
    SDL_Window* window = state.window->Handle();
    if (nSuggestedWidth > 0 && nSuggestedHeight > 0) {
        state.resolutionWidth = nSuggestedWidth;
        state.resolutionHeight = nSuggestedHeight;
    }
    if (bWindowed && nSuggestedWidth > 0 && nSuggestedHeight > 0) {
        SDL_SetWindowSize(window, nSuggestedWidth, nSuggestedHeight);
        SDL_SyncWindow(window);
    }
    if (!bWindowed && state.windowed) {
        DXUTToggleFullScreen();
    }
    state.deviceHandedOut = true;
    const D3DSURFACE_DESC* desc = DXUTGetBackBufferSurfaceDesc();
    state.resetWidth = desc ? static_cast<int>(desc->Width) : 0;
    state.resetHeight = desc ? static_cast<int>(desc->Height) : 0;
    if ((state.deviceCreated && FAILED(state.deviceCreated(state.d3dDevice.get(), desc, state.deviceCreatedContext))) ||
        (state.deviceReset && FAILED(state.deviceReset(state.d3dDevice.get(), desc, state.deviceResetContext)))) {
        SDL_Log("DXUT: the application could not set up the device");
        ReleaseWindow(state);
        state.exitCode = 1;
        return E_FAIL;
    }
    return S_OK;
}

namespace {

// DXUTStaticWndProc; `noFurtherProcessing` tells whether MsgProc stopped the message before DXUT's own handling (and,
// on Windows, DefWindowProc).
LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& noFurtherProcessing)
{
    DxutState& state = State();
    noFurtherProcessing = false;

    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
        const bool keyDown = uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN;
        const bool altDown = (lParam & (LPARAM(1) << 29)) != 0;
        state.keys[static_cast<BYTE>(wParam & 0xFF)] = keyDown;
        if (state.keyboard) {
            state.keyboard(static_cast<UINT>(wParam), keyDown, altDown, state.keyboardContext);
        }
    }
    if ((uMsg >= WM_LBUTTONDOWN && uMsg <= WM_XBUTTONDBLCLK) || uMsg == WM_MOUSEMOVE) {
        UpdateMouseButtons(state, LOWORD(wParam));
    }

    if (state.msgProc) {
        const LRESULT result = state.msgProc(hWnd, uMsg, wParam, lParam, &noFurtherProcessing, state.msgProcContext);
        if (noFurtherProcessing) {
            return result;
        }
    }

    switch (uMsg) {
    case WM_SIZE:
        state.minimized = wParam == SIZE_MINIMIZED;
        break;
    case WM_ACTIVATEAPP:
        state.active = wParam != FALSE;
        break;
    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && state.handleAltEnter) {
            DXUTToggleFullScreen();
            return 0;
        }
        break;
    case WM_CLOSE:  // DXUT destroys the window; WM_DESTROY then posts WM_QUIT
        ReleaseWindow(state);
        state.quit = true;
        return 0;
    default:
        break;
    }
    return 0;
}

}  // namespace

LRESULT CALLBACK DXUTStaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool noFurtherProcessing = false;
    return WndProc(hWnd, uMsg, wParam, lParam, noFurtherProcessing);
}

HRESULT DXUTMainLoop()
{
    DxutState& state = State();
    if (!state.window) {
        return E_FAIL;
    }

    std::vector<x2::dxut::WinMessage> messages;
    while (!state.quit) {
        SDL_Event event;
        while (!state.quit && SDL_PollEvent(&event)) {
            messages.clear();
            const x2::dxut::TranslateContext context = ContextOf(state.window->Handle());
            state.input.Handle(event, context);
            state.translator.Translate(event, context, messages);
            bool imeResultHandled = false;
            for (const x2::dxut::WinMessage& message : messages) {
                if (state.quit) {
                    break;
                }
                if (message.imeResultChar && imeResultHandled) {
                    continue;  // the WM_IME_COMPOSITION was handled: DefWindowProc never made these WM_CHARs
                }
                bool noFurtherProcessing = false;
                WndProc(state.hwnd, message.message, message.wParam, message.lParam, noFurtherProcessing);
                if (message.message == WM_IME_COMPOSITION && (message.lParam & GCS_RESULTSTR)) {
                    imeResultHandled = noFurtherProcessing;
                }
            }
        }
        if (state.quit) {
            break;
        }
        if (state.minimized) {
            // Nothing to draw: wait for the window to come back instead of spinning.
            SDL_WaitEventTimeout(nullptr, 100);
            continue;
        }
        ResetIfResized(state);
        Render3DEnvironment(state);
        WaitForFrameLimit(state);
    }
    ReleaseWindow(state);
    return S_OK;
}

void DXUTPumpEventsWhileBusy()
{
    // Long loads (the start-up, a stage) run on the main thread between frames. SDL_PumpEvents answers the compositor
    // (Wayland pings, X11 _NET_WM_PING) without taking events off the queue, which the main loop still handles in order.
    static std::uint64_t lastPump = 0;
    if (!State().window || !SDL_IsMainThread()) {
        return;
    }
    const std::uint64_t now = SDL_GetTicks();
    if (now - lastPump >= 100) {
        lastPump = now;
        SDL_PumpEvents();
    }
}

void DXUTShutdown(int nExitCode)
{
    DxutState& state = State();
    if (state.window) {  // SDL's dummy video driver has no Win32 HWND: test the window itself
        DXUTStaticWndProc(state.hwnd, WM_CLOSE, 0, 0);
    }
    state.exitCode = nExitCode;
    state.quit = true;
}

void DXUTSetCursorSettings(bool bShowCursorWhenFullScreen, bool bClipCursorWhenFullScreen)
{
    State().showCursorWhenFullScreen = bShowCursorWhenFullScreen;
    State().clipCursorWhenFullScreen = bClipCursorWhenFullScreen;
}

void DXUTSetMultimonSettings(bool bAutoChangeAdapter)
{
    State().autoChangeAdapter = bAutoChangeAdapter;
}

HRESULT DXUTToggleFullScreen()
{
    DxutState& state = State();
    if (!state.window) {
        return E_FAIL;
    }
    SDL_Window* window = state.window->Handle();
    if (state.windowed && !SetFullscreenMode(window, state.resolutionWidth, state.resolutionHeight)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "DXUT: full screen mode: %s", SDL_GetError());
    }
    if (!SDL_SetWindowFullscreen(window, state.windowed)) {
        return E_FAIL;
    }
    state.windowed = !state.windowed;
    if (state.windowed && state.resolutionWidth > 0 && state.resolutionHeight > 0) {
        SDL_SetWindowSize(window, state.resolutionWidth, state.resolutionHeight);
    }
    SDL_SyncWindow(window);
    ResetIfResized(state);
    return S_OK;
}

HINSTANCE DXUTGetHINSTANCE() { return State().hinstance; }
HWND DXUTGetHWND() { return State().hwnd; }
double DXUTGetTime() { return State().time; }
float DXUTGetElapsedTime() { return State().elapsedTime; }
float DXUTGetFPS() { return State().fps; }
bool DXUTIsWindowed() { return State().windowed; }
bool DXUTIsActive() { return State().active; }
int DXUTGetExitCode() { return State().exitCode; }
bool DXUTIsKeyDown(BYTE vKey) { return State().keys[vKey]; }

bool DXUTIsMouseButtonDown(BYTE vButton)
{
    const bool* buttons = State().mouseButtons;
    switch (vButton) {
    case VK_LBUTTON: return buttons[0];
    case VK_MBUTTON: return buttons[1];
    case VK_RBUTTON: return buttons[2];
    case VK_XBUTTON1: return buttons[3];
    case VK_XBUTTON2: return buttons[4];
    default: return false;
    }
}

x2::render::Device* DXUTGetRenderDevice()
{
    return State().device.get();
}

IDirect3DDevice9* DXUTGetD3DDevice()
{
    return State().d3dDevice.get();
}

const D3DSURFACE_DESC* DXUTGetBackBufferSurfaceDesc()
{
    SDL_Window* window = DXUTGetSDLWindow();
    if (window == nullptr) {
        return nullptr;
    }
    static D3DSURFACE_DESC desc;
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    desc = {};
    desc.Format = D3DFMT_A8R8G8B8;
    desc.Usage = D3DUSAGE_RENDERTARGET;
    desc.Pool = D3DPOOL_DEFAULT;
    desc.Width = static_cast<UINT>(width);
    desc.Height = static_cast<UINT>(height);
    return &desc;
}

bool DXUTGetDeviceObjectsReset()
{
    return State().d3dDevice != nullptr;
}

SDL_Window* DXUTGetSDLWindow()
{
    return State().window ? State().window->Handle() : nullptr;
}

x2::dxut::InputState& DXUTGetInputState()
{
    return State().input;
}

void DXUTOutputDebugString(const WCHAR* strMsg, ...)
{
    if (!strMsg) {
        return;
    }
    WCHAR buffer[512];
    va_list args;
    va_start(args, strMsg);
    const std::wstring format = x2::platform::TranslateWideFormat(strMsg);
    std::vswprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), format.c_str(), args);
    va_end(args);
    buffer[sizeof(buffer) / sizeof(buffer[0]) - 1] = L'\0';
    SDL_Log("%s", x2::platform::WideToUtf8(buffer).c_str());
}

// CDXUTTimer, on std::chrono::steady_clock (nanoseconds) instead of QueryPerformanceCounter.

CDXUTTimer::CDXUTTimer() = default;

std::int64_t CDXUTTimer::Now() const
{
    // While stopped, time stands at the moment it stopped.
    if (m_llStopTime != 0) {
        return m_llStopTime;
    }
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void CDXUTTimer::Reset()
{
    const std::int64_t now = Now();
    m_llBaseTime = now;
    m_llLastElapsedTime = now;
    m_llStopTime = 0;
    m_bTimerStopped = false;
}

void CDXUTTimer::Start()
{
    const std::int64_t now =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    if (m_bTimerStopped) {
        m_llBaseTime += now - m_llStopTime;
    }
    m_llStopTime = 0;
    m_llLastElapsedTime = now;
    m_bTimerStopped = false;
}

void CDXUTTimer::Stop()
{
    if (!m_bTimerStopped) {
        const std::int64_t now = Now();
        m_llStopTime = now;
        m_llLastElapsedTime = now;
        m_bTimerStopped = true;
    }
}

void CDXUTTimer::Advance()
{
    m_llStopTime += 100'000'000;  // 0.1 s
}

double CDXUTTimer::GetAbsoluteTime()
{
    return static_cast<double>(
               std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()) /
           1e9;
}

double CDXUTTimer::GetTime()
{
    return static_cast<double>(Now() - m_llBaseTime) / 1e9;
}

double CDXUTTimer::GetElapsedTime()
{
    const std::int64_t now = Now();
    double elapsed = static_cast<double>(now - m_llLastElapsedTime) / 1e9;
    m_llLastElapsedTime = now;
    // DXUT clamps negative values (a processor in power save mode, or moved to another processor).
    if (elapsed < 0.0) {
        elapsed = 0.0;
    }
    return elapsed;
}

bool CDXUTTimer::IsStopped()
{
    return m_bTimerStopped;
}

CDXUTTimer* DXUTGetGlobalTimer()
{
    static CDXUTTimer timer;
    return &timer;
}

DXUTDeviceSettings DXUTGetDeviceSettings()
{
    DxutState& state = State();
    DXUTDeviceSettings settings{};
    settings.AdapterOrdinal = 0;
    settings.DeviceType = D3DDEVTYPE_HAL;
    settings.AdapterFormat = D3DFMT_X8R8G8B8;
    if (const D3DSURFACE_DESC* desc = DXUTGetBackBufferSurfaceDesc()) {
        settings.pp.BackBufferWidth = desc->Width;
        settings.pp.BackBufferHeight = desc->Height;
    }
    settings.pp.BackBufferFormat = D3DFMT_A8R8G8B8;
    settings.pp.BackBufferCount = 1;
    settings.pp.MultiSampleType = D3DMULTISAMPLE_NONE;
    settings.pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    settings.pp.hDeviceWindow = state.hwnd;
    settings.pp.Windowed = state.windowed ? TRUE : FALSE;
    settings.pp.EnableAutoDepthStencil = TRUE;
    settings.pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    settings.pp.PresentationInterval = state.presentInterval;
    return settings;
}

HRESULT DXUTCreateDeviceFromSettings(DXUTDeviceSettings* pNewDeviceSettings, bool /*bPreserveInput*/,
                                     bool /*bClipWindowToSingleAdapter*/)
{
    DxutState& state = State();
    if (pNewDeviceSettings == nullptr || !state.window || !state.device) {
        return E_FAIL;
    }
    SDL_Window* window = state.window->Handle();
    const D3DPRESENT_PARAMETERS& pp = pNewDeviceSettings->pp;
    if (pp.BackBufferWidth > 0 && pp.BackBufferHeight > 0) {
        const int width = static_cast<int>(pp.BackBufferWidth);
        const int height = static_cast<int>(pp.BackBufferHeight);
        state.resolutionWidth = width;
        state.resolutionHeight = height;
        if (state.windowed && !SDL_SetWindowSize(window, width, height)) {
            SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "DXUT: SDL_SetWindowSize failed: %s", SDL_GetError());
            return E_FAIL;
        }
        if (!state.windowed && !SetFullscreenMode(window, width, height)) {
            SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "DXUT: full screen mode: %s", SDL_GetError());
            return E_FAIL;
        }
        SDL_SyncWindow(window);
    }
    if (pp.PresentationInterval != state.presentInterval) {
        const bool immediate = pp.PresentationInterval == static_cast<UINT>(D3DPRESENT_INTERVAL_IMMEDIATE);
        const x2::render::PresentMode mode = PresentModeFor(*state.device, window, immediate);
        if (!state.device->SetSwapchainParameters(window, mode)) {
            SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "DXUT: present mode %d: %s", static_cast<int>(mode),
                        state.device->LastError().c_str());
            return E_FAIL;
        }
        SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "DXUT: present mode %d (vsync %s)", static_cast<int>(mode),
                    immediate ? "off" : "on");
        state.presentInterval = pp.PresentationInterval;
    }
    ResetIfResized(state);
    return S_OK;
}

void DXUTSetWindowClientRectAtModeChange(int /*width*/, int /*height*/) {}
void DXUTSetFullscreenClientRectAtModeChange(int /*width*/, int /*height*/) {}

void DXUTSetFrameLimit(float fps)
{
    State().frameLimit = fps > 0.f ? fps : 0.f;
}

float DXUTGetFrameLimit()
{
    return State().frameLimit;
}

UINT DXUTGetVerticalSync()
{
    return State().forcedVsync;
}

void DXUTSetVerticalSync(int iVal_)
{
    DXUTDeviceSettings settings = DXUTGetDeviceSettings();
    const UINT interval = iVal_ == 1 ? static_cast<UINT>(D3DPRESENT_INTERVAL_IMMEDIATE) : D3DPRESENT_INTERVAL_DEFAULT;
    settings.pp.PresentationInterval = interval;
    State().forcedVsync = interval;
    DXUTCreateDeviceFromSettings(&settings);
}

const WCHAR* DXUTGetFrameStats()
{
    static std::wstring text;
    const DxutState& state = State();
    const D3DSURFACE_DESC* desc = DXUTGetBackBufferSurfaceDesc();
    const bool vsync = state.presentInterval != static_cast<UINT>(D3DPRESENT_INTERVAL_IMMEDIATE);
    char line[128];
    std::snprintf(line, sizeof(line), "%0.2f fps (%ux%u), vsync %s", state.fps, desc ? desc->Width : 0u,
                  desc ? desc->Height : 0u, vsync ? "on" : "off");
    text = x2::platform::Utf8ToWide(line);
    return text.c_str();
}

const WCHAR* DXUTGetDeviceStats()
{
    static std::wstring text;
    const DxutState& state = State();
    if (!state.device) {
        text.clear();
        return text.c_str();
    }
    std::string line(x2::render::BackendName(state.device->GetBackend()));
    if (const std::string_view driver = state.device->DriverName(); !driver.empty()) {
        line += " (";
        line += driver;
        line += ")";
    }
    text = x2::platform::Utf8ToWide(line);
    return text.c_str();
}

void DXUTResetStatsUpdateFrame()
{
    DxutState& state = State();
    state.lastStatsTime = DXUTGetGlobalTimer()->GetAbsoluteTime();
    state.framesSinceStats = 0;
    state.fps = 0.0f;
}

std::vector<D3DDISPLAYMODE> DXUTGetDisplayModes()
{
    std::vector<D3DDISPLAYMODE> modes;
    SDL_Window* window = DXUTGetSDLWindow();
    const SDL_DisplayID display = window != nullptr ? SDL_GetDisplayForWindow(window) : SDL_GetPrimaryDisplay();
    int count = 0;
    SDL_DisplayMode** list = SDL_GetFullscreenDisplayModes(display, &count);
    if (list == nullptr) {
        return modes;
    }
    for (int i = 0; i < count; ++i) {
        const SDL_DisplayMode& mode = *list[i];
        modes.push_back({static_cast<UINT>(mode.w), static_cast<UINT>(mode.h), static_cast<UINT>(mode.refresh_rate + 0.5f),
                         D3DFMT_X8R8G8B8});
    }
    SDL_free(list);
    return modes;
}
