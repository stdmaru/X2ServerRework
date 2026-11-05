// Win32 window-message names for the client's MsgProc(HWND, UINT, WPARAM, LPARAM) model (roadmap Phase 3): message
// parameter types, POINT/RECT, the WM_*, VK_*, MK_*, SIZE_*, GCS_* and SC_* constants, and the LOWORD/MAKELPARAM family.
//
// On Windows they come from <windows.h> (through types.h). Elsewhere they are defined here with the values and sizes they
// have on Windows x64, so the client code that switches on them compiles unchanged and the SDL3 translation
// (src/client/DXUT) produces the same numbers Windows would. Only what the client uses, plus the neighbours that keep a
// group complete (all function keys, all mouse buttons). Add names as more of the client is compiled.
#pragma once

#include "x2/platform/types.h"

#if !defined(_WIN32)

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

struct HICON__;
typedef HICON__* HICON;
struct HMENU__;
typedef HMENU__* HMENU;

typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT, *PPOINT, *LPPOINT;

typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT, *LPRECT;
typedef const RECT* LPCRECT;

// minwindef.h / windowsx.h
#define LOBYTE(w)           (static_cast<BYTE>(static_cast<DWORD_PTR>(w) & 0xff))
#define HIBYTE(w)           (static_cast<BYTE>((static_cast<DWORD_PTR>(w) >> 8) & 0xff))
#define LOWORD(l)           (static_cast<WORD>(static_cast<DWORD_PTR>(l) & 0xffff))
#define HIWORD(l)           (static_cast<WORD>((static_cast<DWORD_PTR>(l) >> 16) & 0xffff))
#ifndef MAKEWORD  // net.h has it too (Winsock versions)
#define MAKEWORD(a, b)      (static_cast<WORD>((static_cast<DWORD_PTR>(a) & 0xff) | ((static_cast<DWORD_PTR>(b) & 0xff) << 8)))
#endif
#define MAKELONG(a, b)      (static_cast<LONG>(static_cast<DWORD>((static_cast<DWORD_PTR>(a) & 0xffff) | ((static_cast<DWORD_PTR>(b) & 0xffff) << 16))))
#define MAKEWPARAM(l, h)    (static_cast<WPARAM>(static_cast<DWORD>(MAKELONG(l, h))))
#define MAKELPARAM(l, h)    (static_cast<LPARAM>(static_cast<DWORD>(MAKELONG(l, h))))
#define MAKELRESULT(l, h)   (static_cast<LRESULT>(static_cast<DWORD>(MAKELONG(l, h))))
#define GET_X_LPARAM(lp)    (static_cast<int>(static_cast<short>(LOWORD(lp))))
#define GET_Y_LPARAM(lp)    (static_cast<int>(static_cast<short>(HIWORD(lp))))
#define GET_WHEEL_DELTA_WPARAM(wp) (static_cast<short>(HIWORD(wp)))
#define GET_KEYSTATE_WPARAM(wp)    (LOWORD(wp))
#define GET_XBUTTON_WPARAM(wp)     (HIWORD(wp))
#define WHEEL_DELTA         120

#define CW_USEDEFAULT       (static_cast<int>(0x80000000))

// Window messages (winuser.h)
#define WM_NULL                 0x0000
#define WM_CREATE               0x0001
#define WM_DESTROY              0x0002
#define WM_MOVE                 0x0003
#define WM_SIZE                 0x0005
#define WM_ACTIVATE             0x0006
#define WM_SETFOCUS             0x0007
#define WM_KILLFOCUS            0x0008
#define WM_PAINT                0x000F
#define WM_CLOSE                0x0010
#define WM_QUIT                 0x0012
#define WM_ACTIVATEAPP          0x001C
#define WM_FONTCHANGE           0x001D
#define WM_SETCURSOR            0x0020
#define WM_GETMINMAXINFO        0x0024
#define WM_INPUTLANGCHANGE      0x0051
#define WM_NCHITTEST            0x0084
#define WM_NCLBUTTONDOWN        0x00A1
#define WM_NCRBUTTONDOWN        0x00A4
#define WM_NCMBUTTONDOWN        0x00A7
#define WM_KEYFIRST             0x0100
#define WM_KEYDOWN              0x0100
#define WM_KEYUP                0x0101
#define WM_CHAR                 0x0102
#define WM_SYSKEYDOWN           0x0104
#define WM_SYSKEYUP             0x0105
#define WM_SYSCHAR              0x0106
#define WM_IME_STARTCOMPOSITION 0x010D
#define WM_IME_ENDCOMPOSITION   0x010E
#define WM_IME_COMPOSITION      0x010F
#define WM_IME_KEYLAST          0x010F
#define WM_INITDIALOG           0x0110
#define WM_COMMAND              0x0111
#define WM_SYSCOMMAND           0x0112
#define WM_TIMER                0x0113
#define WM_MOUSEFIRST           0x0200
#define WM_MOUSEMOVE            0x0200
#define WM_LBUTTONDOWN          0x0201
#define WM_LBUTTONUP            0x0202
#define WM_LBUTTONDBLCLK        0x0203
#define WM_RBUTTONDOWN          0x0204
#define WM_RBUTTONUP            0x0205
#define WM_RBUTTONDBLCLK        0x0206
#define WM_MBUTTONDOWN          0x0207
#define WM_MBUTTONUP            0x0208
#define WM_MBUTTONDBLCLK        0x0209
#define WM_MOUSEWHEEL           0x020A
#define WM_XBUTTONDOWN          0x020B
#define WM_XBUTTONUP            0x020C
#define WM_XBUTTONDBLCLK        0x020D
#define WM_MOUSEHWHEEL          0x020E
#define WM_MOUSELAST            0x020E
#define WM_SIZING               0x0214
#define WM_CAPTURECHANGED       0x0215
#define WM_ENTERSIZEMOVE        0x0231
#define WM_EXITSIZEMOVE         0x0232
#define WM_IME_SETCONTEXT       0x0281
#define WM_IME_NOTIFY           0x0282
#define WM_IME_CHAR             0x0286
#define WM_USER                 0x0400
#define WM_APP                  0x8000

// WM_ACTIVATE
#define WA_INACTIVE    0
#define WA_ACTIVE      1
#define WA_CLICKACTIVE 2

// WM_SIZE
#define SIZE_RESTORED  0
#define SIZE_MINIMIZED 1
#define SIZE_MAXIMIZED 2

// WM_SYSCOMMAND
#define SC_MINIMIZE 0xF020
#define SC_MAXIMIZE 0xF030
#define SC_CLOSE    0xF060
#define SC_KEYMENU  0xF100
#define SC_RESTORE  0xF120

// Key state in the wParam of mouse messages
#define MK_LBUTTON  0x0001
#define MK_RBUTTON  0x0002
#define MK_SHIFT    0x0004
#define MK_CONTROL  0x0008
#define MK_MBUTTON  0x0010
#define MK_XBUTTON1 0x0020
#define MK_XBUTTON2 0x0040

// HIWORD of the wParam of WM_XBUTTON*
#define XBUTTON1 0x0001
#define XBUTTON2 0x0002

// WM_IME_COMPOSITION lParam (imm.h)
#define GCS_COMPREADSTR    0x0001
#define GCS_COMPREADATTR   0x0002
#define GCS_COMPREADCLAUSE 0x0004
#define GCS_COMPSTR        0x0008
#define GCS_COMPATTR       0x0010
#define GCS_COMPCLAUSE     0x0020
#define GCS_CURSORPOS      0x0080
#define GCS_DELTASTART     0x0100
#define GCS_RESULTREADSTR  0x0200
#define GCS_RESULTREADCLAUSE 0x0400
#define GCS_RESULTSTR      0x0800
#define GCS_RESULTCLAUSE   0x1000

// Virtual-key codes. Letters and digits are their ASCII upper-case values ('A'..'Z', '0'..'9').
#define VK_LBUTTON    0x01
#define VK_RBUTTON    0x02
#define VK_CANCEL     0x03
#define VK_MBUTTON    0x04
#define VK_XBUTTON1   0x05
#define VK_XBUTTON2   0x06
#define VK_BACK       0x08
#define VK_TAB        0x09
#define VK_CLEAR      0x0C
#define VK_RETURN     0x0D
#define VK_SHIFT      0x10
#define VK_CONTROL    0x11
#define VK_MENU       0x12
#define VK_PAUSE      0x13
#define VK_CAPITAL    0x14
#define VK_KANA       0x15
#define VK_HANGUL     0x15
#define VK_JUNJA      0x17
#define VK_FINAL      0x18
#define VK_HANJA      0x19
#define VK_KANJI      0x19
#define VK_ESCAPE     0x1B
#define VK_CONVERT    0x1C
#define VK_NONCONVERT 0x1D
#define VK_ACCEPT     0x1E
#define VK_MODECHANGE 0x1F
#define VK_SPACE      0x20
#define VK_PRIOR      0x21
#define VK_NEXT       0x22
#define VK_END        0x23
#define VK_HOME       0x24
#define VK_LEFT       0x25
#define VK_UP         0x26
#define VK_RIGHT      0x27
#define VK_DOWN       0x28
#define VK_SELECT     0x29
#define VK_PRINT      0x2A
#define VK_EXECUTE    0x2B
#define VK_SNAPSHOT   0x2C
#define VK_INSERT     0x2D
#define VK_DELETE     0x2E
#define VK_HELP       0x2F
#define VK_LWIN       0x5B
#define VK_RWIN       0x5C
#define VK_APPS       0x5D
#define VK_SLEEP      0x5F
#define VK_NUMPAD0    0x60
#define VK_NUMPAD1    0x61
#define VK_NUMPAD2    0x62
#define VK_NUMPAD3    0x63
#define VK_NUMPAD4    0x64
#define VK_NUMPAD5    0x65
#define VK_NUMPAD6    0x66
#define VK_NUMPAD7    0x67
#define VK_NUMPAD8    0x68
#define VK_NUMPAD9    0x69
#define VK_MULTIPLY   0x6A
#define VK_ADD        0x6B
#define VK_SEPARATOR  0x6C
#define VK_SUBTRACT   0x6D
#define VK_DECIMAL    0x6E
#define VK_DIVIDE     0x6F
#define VK_F1         0x70
#define VK_F2         0x71
#define VK_F3         0x72
#define VK_F4         0x73
#define VK_F5         0x74
#define VK_F6         0x75
#define VK_F7         0x76
#define VK_F8         0x77
#define VK_F9         0x78
#define VK_F10        0x79
#define VK_F11        0x7A
#define VK_F12        0x7B
#define VK_F13        0x7C
#define VK_F14        0x7D
#define VK_F15        0x7E
#define VK_F16        0x7F
#define VK_F17        0x80
#define VK_F18        0x81
#define VK_F19        0x82
#define VK_F20        0x83
#define VK_F21        0x84
#define VK_F22        0x85
#define VK_F23        0x86
#define VK_F24        0x87
#define VK_NUMLOCK    0x90
#define VK_SCROLL     0x91
#define VK_LSHIFT     0xA0
#define VK_RSHIFT     0xA1
#define VK_LCONTROL   0xA2
#define VK_RCONTROL   0xA3
#define VK_LMENU      0xA4
#define VK_RMENU      0xA5
#define VK_OEM_1      0xBA  // ;:
#define VK_OEM_PLUS   0xBB
#define VK_OEM_COMMA  0xBC
#define VK_OEM_MINUS  0xBD
#define VK_OEM_PERIOD 0xBE
#define VK_OEM_2      0xBF  // /?
#define VK_OEM_3      0xC0  // `~
#define VK_OEM_4      0xDB  // [{
#define VK_OEM_5      0xDC  // \|
#define VK_OEM_6      0xDD  // ]}
#define VK_OEM_7      0xDE  // '"
#define VK_OEM_102    0xE2  // the extra key of ISO keyboards
#define VK_PROCESSKEY 0xE5

#endif // !_WIN32
