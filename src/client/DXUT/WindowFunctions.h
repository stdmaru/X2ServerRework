#pragma once

// The Win32 window functions KTDXLIB's UI calls (mouse capture, key state, the client rectangle, caret blink and wheel
// settings), outside Windows, on SDL3 and the DXUT replacement's input state. On Windows <windows.h> provides the real
// ones. The clipboard helpers are port additions on SDL's clipboard on every platform: the edit boxes call them in
// place of OpenClipboard/GetClipboardData/SetClipboardData. See docs/porting/client-dxut.md.

#include "x2/platform/ime.h"
#include "x2/platform/types.h"

#include <cstddef>
#include <string>

#if !defined(_WIN32)

#define SPI_GETWHEELSCROLLLINES 0x0068

// Mouse capture: SDL_CaptureMouse on the DXUT window. hWnd is the window DXUTGetHWND() returns (or NULL).
HWND SetCapture(HWND hWnd);
BOOL ReleaseCapture();
HWND GetCapture();
// Negative (the high bit set) while the key is down, as last seen in WM_KEYDOWN/WM_KEYUP or the mouse buttons; the low
// bit is the toggle state for VK_CAPITAL, VK_NUMLOCK and VK_SCROLL.
SHORT GetKeyState(int nVirtKey);
SHORT GetAsyncKeyState(int vKey);
HWND GetFocus();  // the DXUT window when it has the keyboard focus, else NULL
BOOL GetClientRect(HWND hWnd, RECT* lpRect);  // the DXUT window's client area in pixels
UINT GetCaretBlinkTime();  // 530 ms, Windows' default
// Only SPI_GETWHEELSCROLLLINES (3 lines, Windows' default); the others fail.
BOOL SystemParametersInfoW(UINT uiAction, UINT uiParam, void* pvParam, UINT fWinIni);
#define SystemParametersInfo SystemParametersInfoW

// Keyboard layouts: SDL reports none, so there is one, English (United States), 0x04090409. The IME edit box names
// the input language with it (docs/porting/client-ime.md).
#define SORT_DEFAULT            0x0
#define MAKELCID(lgid, srtid)   (static_cast<DWORD>((static_cast<DWORD>(static_cast<WORD>(srtid)) << 16) | static_cast<WORD>(lgid)))
#define LOCALE_SLANGUAGE        0x00000002
#define LOCALE_SABBREVLANGNAME  0x00000003
typedef DWORD LCID;
typedef DWORD LCTYPE;
HKL GetKeyboardLayout(DWORD idThread);
int GetKeyboardLayoutList(int nBuff, HKL* lpList);
// LOCALE_SABBREVLANGNAME and LOCALE_SLANGUAGE of English (United States); 0 (failure) for anything else.
int GetLocaleInfoW(LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData);

// The window and the mouse for X2Lib (2026-11-07). Screen coordinates are SDL's global ones; hWnd is the DXUT window.
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define HTCLIENT    1
int GetSystemMetrics(int nIndex);  // SM_CXSCREEN / SM_CYSCREEN: the size of the display the window is on
BOOL GetCursorPos(POINT* lpPoint);
BOOL ScreenToClient(HWND hWnd, POINT* lpPoint);
BOOL GetWindowRect(HWND hWnd, RECT* lpRect);  // the window with its borders, in screen coordinates
HWND WindowFromPoint(POINT Point);  // the DXUT window when the point is on it, else NULL
HWND GetActiveWindow();  // the DXUT window when it has the keyboard focus, else NULL
BOOL SetForegroundWindow(HWND hWnd);  // raises the DXUT window
HWND SetFocus(HWND hWnd);  // raises the DXUT window; returns the window that had the focus
BOOL SetWindowTextW(HWND hWnd, LPCWSTR lpString);  // the window title
#define SetWindowText SetWindowTextW
void PostQuitMessage(int nExitCode);  // an SDL quit event: the main loop closes the window

#endif

// The mouse cursor, on SDL on every platform (SDL owns the window's cursor, on Windows too). The original loaded
// IDC_CURSOR1, a .cur resource of x2.exe, with LoadCursor and switched it with SetCursor (2026-11-07).
struct DXUTCursor;
// A cursor from the bytes of a 32-bit .cur file (the hot spot comes from the file). NULL when the file is not one.
DXUTCursor* DXUTCreateCursorFromCur(const unsigned char* data, std::size_t size);
void DXUTDestroyCursor(DXUTCursor* cursor);
// SetCursor: shows `cursor`, or hides the system cursor when it is NULL (the game then draws its own).
void DXUTSetCursor(DXUTCursor* cursor);
DXUTCursor* DXUTGetCursor();  // the cursor shown, or NULL while hidden

// The clipboard's text (empty when it has none), and setting it. Port additions on SDL, on every platform.
std::wstring DXUTGetClipboardText();
bool DXUTSetClipboardText(const std::wstring& text);
