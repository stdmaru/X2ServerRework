#pragma once

// The Win32 window functions KTDXLIB's UI calls (mouse capture, key state, the client rectangle, caret blink and wheel
// settings), outside Windows, on SDL3 and the DXUT replacement's input state. On Windows <windows.h> provides the real
// ones. The clipboard helpers are port additions on SDL's clipboard on every platform: the edit boxes call them in
// place of OpenClipboard/GetClipboardData/SetClipboardData. See docs/porting/client-dxut.md.

#include "x2/platform/types.h"

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

#endif

// The clipboard's text (empty when it has none), and setting it. Port additions on SDL, on every platform.
std::wstring DXUTGetClipboardText();
bool DXUTSetClipboardText(const std::wstring& text);
