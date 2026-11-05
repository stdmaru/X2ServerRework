// The DXUT replacement (src/client/DXUT/DXUT.*): messages reach the client's callbacks in DXUT's order, without a window.

#include "DXUT.h"

#include <SDL3/SDL.h>

#include <doctest/doctest.h>

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

TEST_CASE("without a GPU the window fails cleanly")
{
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
    CHECK(DXUTInit(false, false, true) == S_OK);
    CHECK(DXUTGetHWND() == nullptr);
    // The dummy video driver has no GPU device: creation fails with a message instead of crashing.
    CHECK(FAILED(DXUTCreateWindow(L"X2")));
    CHECK(DXUTGetHWND() == nullptr);
    CHECK(DXUTGetSDLWindow() == nullptr);
    CHECK(FAILED(DXUTMainLoop()));
}
