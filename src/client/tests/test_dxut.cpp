// The DXUT replacement (src/client/DXUT/DXUT.*): messages reach the client's callbacks in DXUT's order, without a window.

#include "DXUT.h"

#include "x2/render/null.h"

#include <SDL3/SDL.h>

#include <doctest/doctest.h>

#include <string>
#include <vector>

namespace {

struct Received {
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
};
std::vector<Received> g_messages;
std::vector<UINT> g_order;  // 1: keyboard callback, 2: MsgProc
bool g_stopMessages = false;

LRESULT CALLBACK TestMsgProc(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
    CHECK(pUserContext == &g_messages);
    g_messages.push_back({uMsg, wParam, lParam});
    g_order.push_back(2);
    *pbNoFurtherProcessing = g_stopMessages;
    return 0;
}

UINT g_lastKey = 0;
bool g_lastDown = false;
bool g_lastAlt = false;

void CALLBACK TestKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void*)
{
    g_lastKey = nChar;
    g_lastDown = bKeyDown;
    g_lastAlt = bAltDown;
    g_order.push_back(1);
}

void Reset()
{
    g_messages.clear();
    g_order.clear();
    g_stopMessages = false;
    DXUTSetCallbackMsgProc(TestMsgProc, &g_messages);
    DXUTSetCallbackKeyboard(TestKeyboard);
}

}  // namespace

TEST_CASE("key messages go to the keyboard callback, then MsgProc")
{
    Reset();
    DXUTStaticWndProc(nullptr, WM_KEYDOWN, VK_HOME, 1 | (0x47 << 16) | (1 << 24));
    CHECK(g_lastKey == VK_HOME);
    CHECK(g_lastDown);
    CHECK_FALSE(g_lastAlt);
    CHECK(DXUTIsKeyDown(VK_HOME));
    REQUIRE(g_order.size() == 2);
    CHECK(g_order[0] == 1);
    CHECK(g_order[1] == 2);
    REQUIRE(g_messages.size() == 1);
    CHECK(g_messages[0].message == WM_KEYDOWN);

    DXUTStaticWndProc(nullptr, WM_SYSKEYUP, VK_HOME, LPARAM(1) | (LPARAM(1) << 29) | (LPARAM(1) << 30) | (LPARAM(1) << 31));
    CHECK_FALSE(g_lastDown);
    CHECK(g_lastAlt);
    CHECK_FALSE(DXUTIsKeyDown(VK_HOME));
}

TEST_CASE("mouse messages keep the button state")
{
    Reset();
    DXUTStaticWndProc(nullptr, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(5, 6));
    CHECK(DXUTIsMouseButtonDown(VK_RBUTTON));
    CHECK_FALSE(DXUTIsMouseButtonDown(VK_LBUTTON));
    DXUTStaticWndProc(nullptr, WM_RBUTTONUP, 0, MAKELPARAM(5, 6));
    CHECK_FALSE(DXUTIsMouseButtonDown(VK_RBUTTON));
    CHECK(g_messages.size() == 2);
}

TEST_CASE("activation follows WM_ACTIVATEAPP unless MsgProc stops it")
{
    Reset();
    DXUTStaticWndProc(nullptr, WM_ACTIVATEAPP, FALSE, 0);
    CHECK_FALSE(DXUTIsActive());
    DXUTStaticWndProc(nullptr, WM_ACTIVATEAPP, TRUE, 0);
    CHECK(DXUTIsActive());

    g_stopMessages = true;  // no further processing: DXUT's own handling is skipped
    DXUTStaticWndProc(nullptr, WM_ACTIVATEAPP, FALSE, 0);
    CHECK(DXUTIsActive());
    CHECK(g_messages.size() == 3);
}

TEST_CASE("the timer measures elapsed time and stops")
{
    CDXUTTimer timer;
    timer.Reset();
    SDL_Delay(5);
    const double elapsed = timer.GetElapsedTime();
    CHECK(elapsed > 0.0);
    CHECK(elapsed < 5.0);
    CHECK(timer.GetTime() >= elapsed);

    timer.Stop();
    CHECK(timer.IsStopped());
    const double stopped = timer.GetTime();
    SDL_Delay(2);
    CHECK(timer.GetTime() == stopped);
    timer.Advance();
    CHECK(timer.GetTime() == doctest::Approx(stopped + 0.1));
    timer.Start();
    CHECK_FALSE(timer.IsStopped());
    CHECK(timer.GetTime() >= stopped);
}

TEST_CASE("without a GPU the sdlgpu window fails cleanly")
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    CHECK(DXUTInit(false, false, true) == S_OK);
    // The IME edit box draws the composition and the candidates itself (docs/porting/client-ime.md).
    const char* imeUi = SDL_GetHint(SDL_HINT_IME_IMPLEMENTED_UI);
    REQUIRE(imeUi != nullptr);
    CHECK(std::string(imeUi) == "composition,candidates");
    CHECK(DXUTGetHWND() == nullptr);
    // The dummy video driver has no GPU swapchain: creation fails with a message instead of crashing.
    SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "X2_RENDER_BACKEND", "sdlgpu", true);
    CHECK(FAILED(DXUTCreateWindow(L"X2")));
    CHECK(DXUTGetRenderDevice() == nullptr);
    CHECK(DXUTGetHWND() == nullptr);
    CHECK(DXUTGetSDLWindow() == nullptr);
    CHECK(FAILED(DXUTMainLoop()));
}

namespace {

int g_frames = 0;
std::vector<std::string> g_commands;  // what the null device recorded, before DXUTShutdown destroys it
size_t g_liveResources = 0;
size_t g_liveAfterFirstFrame = 0;
bool g_gotDevice = true;

void CALLBACK CountFrames(IDirect3DDevice9* device, double, float, void*)
{
    g_gotDevice = g_gotDevice && device == DXUTGetD3DDevice() && device != nullptr;
    if (++g_frames == 2) {  // the D3D9 device's own resources and the depth buffer exist
        g_liveAfterFirstFrame = x2::render::null::LiveResourceCount(*DXUTGetRenderDevice());
    }
    if (g_frames == 3) {  // after two presented frames
        g_commands = x2::render::null::Commands(*DXUTGetRenderDevice());
        g_liveResources = x2::render::null::LiveResourceCount(*DXUTGetRenderDevice());
        DXUTShutdown(0);
    }
}

}  // namespace

TEST_CASE("the null renderer clears every frame to black")
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "X2_RENDER_BACKEND", "null", true);
    REQUIRE(DXUTInit(false, false, true) == S_OK);
    REQUIRE(SUCCEEDED(DXUTCreateWindow(L"X2")));
    x2::render::Device* device = DXUTGetRenderDevice();
    REQUIRE(device != nullptr);
    CHECK(device->GetBackend() == x2::render::Backend::Null);
    CHECK(DXUTGetSDLWindow() != nullptr);

    g_frames = 0;
    DXUTSetCallbackFrameMove(CountFrames);
    DXUTSetCallbackFrameRender(nullptr);
    DXUTSetCallbackKeyboard(nullptr);
    DXUTSetCallbackMsgProc(nullptr);
    DXUTStaticWndProc(DXUTGetHWND(), WM_SIZE, SIZE_RESTORED, 0);  // earlier tests may have left it minimized
    x2::render::null::ClearCommands(*device);
    CHECK(SUCCEEDED(DXUTMainLoop()));
    DXUTSetCallbackFrameMove(nullptr);
    CHECK(DXUTGetExitCode() == 0);
    CHECK(DXUTGetSDLWindow() == nullptr);
    CHECK(DXUTGetRenderDevice() == nullptr);  // destroyed with the window, not at exit

    int clears = 0, submits = 0;
    for (const std::string& line : g_commands) {
        clears += line.find("BeginRenderPass color[0]=") == 0 && line.find("load=clear(0,0,0,1)") != std::string::npos;
        submits += line == "Submit";
    }
    CHECK(clears == 2);
    CHECK(submits == 2);
    CHECK(g_gotDevice);
    CHECK(g_liveResources == g_liveAfterFirstFrame);  // nothing created per frame
}

namespace {

std::vector<std::string> g_deviceEvents;
bool g_deviceHadBackBuffer = false;

HRESULT CALLBACK TestDeviceCreated(IDirect3DDevice9* device, const D3DSURFACE_DESC* desc, void*)
{
    g_deviceEvents.push_back("created");
    g_deviceHadBackBuffer = device == DXUTGetD3DDevice() && desc != nullptr && desc->Width > 0 && desc->Height > 0;
    return S_OK;
}

HRESULT CALLBACK TestDeviceReset(IDirect3DDevice9* device, const D3DSURFACE_DESC* desc, void*)
{
    g_deviceEvents.push_back(device != nullptr && desc != nullptr ? "reset" : "reset without device");
    return S_OK;
}

void CALLBACK TestDeviceLost(void*)
{
    g_deviceEvents.push_back(DXUTGetD3DDevice() != nullptr ? "lost" : "lost after the device");
}

void CALLBACK TestDeviceDestroyed(void*)
{
    g_deviceEvents.push_back(DXUTGetD3DDevice() != nullptr ? "destroyed" : "destroyed after the device");
}

}  // namespace

TEST_CASE("the device callbacks run once each, around the device's life")
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "X2_RENDER_BACKEND", "null", true);
    REQUIRE(DXUTInit(false, false, true) == S_OK);
    CHECK(FAILED(DXUTCreateDevice(D3DADAPTER_DEFAULT, true, 1024, 768)));  // no window yet
    REQUIRE(SUCCEEDED(DXUTCreateWindow(L"X2")));

    g_deviceEvents.clear();
    DXUTSetCallbackDeviceCreated(TestDeviceCreated);
    DXUTSetCallbackDeviceReset(TestDeviceReset);
    DXUTSetCallbackDeviceLost(TestDeviceLost);
    DXUTSetCallbackDeviceDestroyed(TestDeviceDestroyed);
    CHECK(SUCCEEDED(DXUTCreateDevice(D3DADAPTER_DEFAULT, true, 1024, 768)));
    CHECK(SUCCEEDED(DXUTCreateDevice(D3DADAPTER_DEFAULT, true, 1024, 768)));  // already handed out: nothing again
    CHECK(g_deviceHadBackBuffer);
    DXUTShutdown(0);
    DXUTSetCallbackDeviceCreated(nullptr);
    DXUTSetCallbackDeviceReset(nullptr);
    DXUTSetCallbackDeviceLost(nullptr);
    DXUTSetCallbackDeviceDestroyed(nullptr);
    CHECK(DXUTGetRenderDevice() == nullptr);
    CHECK(g_deviceEvents == std::vector<std::string>{"created", "reset", "lost", "destroyed"});
}
