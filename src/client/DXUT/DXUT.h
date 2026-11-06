#pragma once

// Replacement for the DXUT framework the original client was built on (KTDXLIB/DXUT, not imported): the window, the
// message pump and the frame loop, now on SDL3 through x2::app (decision T32). Same names and callback signatures as
// DXUT's, so X2.cpp and KTDXLIB call it unchanged; only the subset the client uses, grown as layers are compiled.
// Device functions (DXUTCreateDevice, DXUTGetD3DDevice, DXUTGetBackBufferSurfaceDesc, device settings, lost/reset)
// arrive with the graphics port (Phase 4); until then the callbacks receive a null device.
// See docs/porting/client-dxut.md.

#include "x2/platform/win32.h"

#include <SDL3/SDL_video.h>

#include <cstdint>

struct IDirect3DDevice9;  // d3d9.h on Windows; never defined elsewhere (the device pointer is null for now)

// Callbacks (DXUT.h)
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMEMOVE)(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMERENDER)(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
typedef void    (CALLBACK *LPDXUTCALLBACKKEYBOARD)(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
typedef LRESULT (CALLBACK *LPDXUTCALLBACKMSGPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);

void DXUTSetCallbackFrameMove(LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext = nullptr);
void DXUTSetCallbackFrameRender(LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext = nullptr);
void DXUTSetCallbackKeyboard(LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext = nullptr);
void DXUTSetCallbackMsgProc(LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext = nullptr);

// Initialization. The command line is not parsed (the client passes false); default hotkeys are not implemented (the
// client passes false, and DXUT's were all commented out but Pause). Alt+Enter toggles full screen, as in DXUT.
HRESULT DXUTInit(bool bParseCommandLine = true, bool bHandleDefaultHotkeys = true, bool bShowMsgBoxOnError = true,
                 bool bHandleAltEnter = true);
// Creates the window, with a client area of 1024x768 (what X2.cpp later asks DXUTCreateDevice for) times the display
// scale. hInstance, hIcon and hMenu are ignored: the window takes the executable's icon (x2_windows_icon).
HRESULT DXUTCreateWindow(const WCHAR* strWindowTitle = L"Direct3D Window", HINSTANCE hInstance = nullptr,
                         HICON hIcon = nullptr, HMENU hMenu = nullptr, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
// Pumps messages and runs frames (FrameMove, FrameRender, present) until the window is closed.
HRESULT DXUTMainLoop();
// The window procedure: every translated SDL event goes through it, as DXUT's own WndProc did. Exposed, as in DXUT.
LRESULT CALLBACK DXUTStaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DXUTShutdown(int nExitCode = 0);

// Settings that are stored but have no effect yet (full screen cursor, multi-monitor adapter changes).
void DXUTSetCursorSettings(bool bShowCursorWhenFullScreen, bool bClipCursorWhenFullScreen);
void DXUTSetMultimonSettings(bool bAutoChangeAdapter);
HRESULT DXUTToggleFullScreen();

// State
HINSTANCE DXUTGetHINSTANCE();
HWND      DXUTGetHWND();  // the real HWND on Windows; elsewhere the SDL_Window*, as an opaque handle
double    DXUTGetTime();
float     DXUTGetElapsedTime();
float     DXUTGetFPS();
bool      DXUTIsWindowed();
bool      DXUTIsActive();
int       DXUTGetExitCode();
bool      DXUTIsKeyDown(BYTE vKey);         // a virtual-key code, as last seen in WM_KEYDOWN/WM_KEYUP
bool      DXUTIsMouseButtonDown(BYTE vButton);  // VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2

// Port additions: the SDL window behind DXUTGetHWND (null before DXUTCreateWindow), the x2::render device that draws
// into it (created and destroyed with the window, backend from X2_RENDER_BACKEND; null without a window), and the keyboard and mouse state
// the main loop keeps for KTDXLIB's DirectInput layer (SdlInput.h, docs/porting/client-input.md).
SDL_Window* DXUTGetSDLWindow();
namespace x2::render {
class Device;
}
x2::render::Device* DXUTGetRenderDevice();
namespace x2::dxut {
class InputState;
}
x2::dxut::InputState& DXUTGetInputState();

// Timer (DXUTmisc.h). Seconds, from a monotonic clock.
class CDXUTTimer {
public:
    CDXUTTimer();

    void Reset();              // resets the timer
    void Start();              // starts the timer
    void Stop();               // stops (or pauses) the timer
    void Advance();            // advances the timer by 0.1 seconds
    double GetAbsoluteTime();  // the absolute system time
    double GetTime();          // the current time
    double GetElapsedTime();   // time since the previous GetElapsedTime call
    bool IsStopped();          // true if the timer is stopped

private:
    std::int64_t Now() const;

    bool m_bTimerStopped = true;
    std::int64_t m_llStopTime = 0;
    std::int64_t m_llLastElapsedTime = 0;
    std::int64_t m_llBaseTime = 0;
};

CDXUTTimer* DXUTGetGlobalTimer();

// Debug output (DXUTmisc.h): DXUTTRACE prints in debug builds only.
#if defined(DEBUG) || defined(_DEBUG)
#define DXUTTRACE DXUTOutputDebugString
#else
#define DXUTTRACE(...) ((void)0)
#endif
void DXUTOutputDebugString(const WCHAR* strMsg, ...);

// PIX events (DXUTmisc.h): nothing to mark without D3D9.
#define DXUT_BeginPerfEvent(color, pstrMessage) ((void)0)
#define DXUT_EndPerfEvent() ((void)0)
