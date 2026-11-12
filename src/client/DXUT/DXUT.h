#pragma once

// Replacement for the DXUT framework the original client was built on (KTDXLIB/DXUT, not imported): the window, the
// message pump and the frame loop, now on SDL3 through x2::app (decision T32). Same names and callback signatures as
// DXUT's, so X2.cpp and KTDXLIB call it unchanged; only the subset the client uses, grown as layers are compiled.
// The device is KTDXLIB's D3D9 device on x2::render (decision T38), created with the window; the callbacks receive it.
// See docs/porting/client-dxut.md.

#include "x2/platform/win32.h"

#include <SDL3/SDL_video.h>

#include <cstdint>
#include <string>
#include <vector>

#include "Device.h"   // KTDXLIB/D3D9: IDirect3DDevice9 is x2::d3d9::Device
#include "MessageBox.h"   // Win32 MessageBox outside Windows
#include "WindowFunctions.h"   // the other Win32 window functions the UI calls, outside Windows
#include "GrowableArray.h"   // DXUT's CGrowableArray
#include "Camera.h"   // DXUT's CFirstPersonCamera (DXUTmisc.h)

// Callbacks (DXUT.h)
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMEMOVE)(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
typedef void    (CALLBACK *LPDXUTCALLBACKFRAMERENDER)(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
typedef void    (CALLBACK *LPDXUTCALLBACKKEYBOARD)(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
typedef LRESULT (CALLBACK *LPDXUTCALLBACKMSGPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext);
typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICECREATED)(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
typedef HRESULT (CALLBACK *LPDXUTCALLBACKDEVICERESET)(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
typedef void    (CALLBACK *LPDXUTCALLBACKDEVICELOST)(void* pUserContext);
typedef void    (CALLBACK *LPDXUTCALLBACKDEVICEDESTROYED)(void* pUserContext);

void DXUTSetCallbackFrameMove(LPDXUTCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext = nullptr);
void DXUTSetCallbackFrameRender(LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext = nullptr);
void DXUTSetCallbackKeyboard(LPDXUTCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext = nullptr);
void DXUTSetCallbackMsgProc(LPDXUTCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext = nullptr);
// The device's life (2026-11-12): DXUTCreateDevice calls Created then Reset; closing the window calls Lost then
// Destroyed before the device goes. KTDXLIB's device is never lost (T38), so they run once each.
void DXUTSetCallbackDeviceCreated(LPDXUTCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext = nullptr);
void DXUTSetCallbackDeviceReset(LPDXUTCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext = nullptr);
void DXUTSetCallbackDeviceLost(LPDXUTCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext = nullptr);
void DXUTSetCallbackDeviceDestroyed(LPDXUTCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext = nullptr);

// Initialization. The command line is not parsed (the client passes false); default hotkeys are not implemented (the
// client passes false, and DXUT's were all commented out but Pause). Alt+Enter toggles full screen, as in DXUT.
HRESULT DXUTInit(bool bParseCommandLine = true, bool bHandleDefaultHotkeys = true, bool bShowMsgBoxOnError = true,
                 bool bHandleAltEnter = true);
// Creates the window, with a client area of 1024x768 (what X2.cpp later asks DXUTCreateDevice for) times the display
// scale. hInstance, hIcon and hMenu are ignored: the window takes the executable's icon (x2_windows_icon).
HRESULT DXUTCreateWindow(const WCHAR* strWindowTitle = L"Direct3D Window", HINSTANCE hInstance = nullptr,
                         HICON hIcon = nullptr, HMENU hMenu = nullptr, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
#ifndef D3DADAPTER_DEFAULT
#define D3DADAPTER_DEFAULT 0
#endif
// Hands the window's device to the application: sizes the client area (when windowed and a size is given), then calls
// the Created and Reset callbacks. Fails without a window, or when a callback fails (the window is closed then).
// DXUT's device enumeration and its IsDeviceAcceptable/ModifyDeviceSettings callbacks are gone: there is one device.
HRESULT DXUTCreateDevice(UINT AdapterOrdinal = D3DADAPTER_DEFAULT, bool bWindowed = true, int nSuggestedWidth = 0,
                         int nSuggestedHeight = 0);
// Pumps messages and runs frames (FrameMove, FrameRender, present) until the window is closed.
HRESULT DXUTMainLoop();
// The window procedure: every translated SDL event goes through it, as DXUT's own WndProc did. Exposed, as in DXUT.
LRESULT CALLBACK DXUTStaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DXUTShutdown(int nExitCode = 0);
// Port addition (2026-11-12): keeps the window responsive during long loads on the main thread (at most every 100 ms;
// no-op on other threads). Without it, compositors flag the client as not responding while it loads.
void DXUTPumpEventsWhileBusy();

// The executable's arguments, argv[0] first (MSVC's __argc / __argv): main() stores them before DXUTInit.
void DXUTSetCommandLine(int argc, char* argv[]);
const std::vector<std::string>& DXUTGetCommandLine();
// Whether Alt+F4 closes the window (the client turns it off, as its RegisterHotKey( MOD_ALT, VK_F4 ) did). Only
// Windows lets an application keep it; elsewhere the window manager handles Alt+F4.
void DXUTSetCloseOnAltF4(bool bClose);

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
// KTDXLIB's D3D9 device (null without a window).
IDirect3DDevice9* DXUTGetD3DDevice();
// The back buffer: the window's client area in pixels, A8R8G8B8 (null without a window). Updated on each call.
const D3DSURFACE_DESC* DXUTGetBackBufferSurfaceDesc();
// DXUT's device settings (2026-11-07). The port has one device, so they describe the window: the back buffer is its
// client area (A8R8G8B8 on an X8R8G8B8 adapter, D24S8 depth, no multisampling) and the interval its present mode.
struct DXUTDeviceSettings {
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT AdapterFormat;
    DWORD BehaviorFlags;
    D3DPRESENT_PARAMETERS pp;
};
DXUTDeviceSettings DXUTGetDeviceSettings();
// Applies the back buffer size (the windowed client area, or the full-screen window's) and the interval
// (D3DPRESENT_INTERVAL_IMMEDIATE: no vertical sync). The device is kept: nothing is lost or reset.
HRESULT DXUTCreateDeviceFromSettings(DXUTDeviceSettings* pNewDeviceSettings, bool bPreserveInput = false,
                                     bool bClipWindowToSingleAdapter = true);
// The client areas DXUT restored on a full-screen/windowed switch. Kept for the caller; the window keeps its size.
void DXUTSetWindowClientRectAtModeChange(int width, int height);
void DXUTSetFullscreenClientRectAtModeChange(int width, int height);
// KOG's DXUT additions: the forced interval (D3DPRESENT_INTERVAL_DEFAULT or _IMMEDIATE), and setting it (1: immediate,
// anything else: the vertical blank).
UINT DXUTGetVerticalSync();  // the present interval (D3DPRESENT_INTERVAL_*)
void DXUTSetFrameLimit(float fps);  // T41: at most `fps` frames per second (0: no limit; vsync still applies)
float DXUTGetFrameLimit();
void DXUTSetVerticalSync(int iVal_);
// The statistics lines of DXUT's HUD (X2.cpp's debug text): "<fps> fps (<width>x<height>), vsync on|off" and the
// renderer ("<backend> (<driver>)"). Valid until the next call.
const WCHAR* DXUTGetFrameStats();
const WCHAR* DXUTGetDeviceStats();
// Restarts the frame-rate count (KOG's addition, called when a battlefield loads).
void DXUTResetStatsUpdateFrame();
// The display's modes, for the resolution list (in place of DXUT's CD3DEnumeration: X2Lib read the adapter's modes
// through it). Every mode reports the adapter format of DXUTGetDeviceSettings.
std::vector<D3DDISPLAYMODE> DXUTGetDisplayModes();
// KTDXLIB's device is never lost or reset (T38): its objects exist while it does.
bool DXUTGetDeviceObjectsReset();
inline bool DXUTGetDeviceLost() { return false; }
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

// dxerr.h's trace macros: print the failure in debug builds and give back the HRESULT.
#define DXTRACE_ERR(str, hr)        (DXUTTRACE(L"%s failed (0x%08x)\n", (str), static_cast<unsigned>(hr)), (hr))
#define DXTRACE_ERR_MSGBOX(str, hr) DXTRACE_ERR(str, hr)

// DXUTgui.h's control states, which KTDXLIB's widgets (DXUT's controls with KOG's changes) still use.
enum DXUT_CONTROL_STATE {
    DXUT_STATE_NORMAL = 0,
    DXUT_STATE_DISABLED,
    DXUT_STATE_HIDDEN,
    DXUT_STATE_FOCUS,
    DXUT_STATE_MOUSEOVER,
    DXUT_STATE_PRESSED,
};
#define MAX_CONTROL_STATES 6

// PIX events (DXUTmisc.h): nothing to mark without D3D9.
#define DXUT_BeginPerfEvent(color, pstrMessage) ((void)0)
#define DXUT_EndPerfEvent() ((void)0)
