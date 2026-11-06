#pragma once

// Win32's MessageBox outside Windows, on SDL_ShowMessageBox: the client shows errors and a few questions with it
// (93 calls in 37 files, 2026-11-06). On Windows <windows.h> provides the real one. The owner window is the HWND that
// DXUTGetHWND() returns (the SDL_Window* outside Windows) or NULL. The buttons, the icon and the return codes follow
// Win32; MB_TASKMODAL, MB_TOPMOST and the other modality flags are accepted and ignored.

#include "x2/platform/types.h"

#if !defined(_WIN32)

#define MB_OK                0x00000000L
#define MB_OKCANCEL          0x00000001L
#define MB_ABORTRETRYIGNORE  0x00000002L
#define MB_YESNOCANCEL       0x00000003L
#define MB_YESNO             0x00000004L
#define MB_RETRYCANCEL       0x00000005L
#define MB_ICONHAND          0x00000010L
#define MB_ICONQUESTION      0x00000020L
#define MB_ICONEXCLAMATION   0x00000030L
#define MB_ICONASTERISK      0x00000040L
#define MB_ICONERROR         MB_ICONHAND
#define MB_ICONSTOP          MB_ICONHAND
#define MB_ICONWARNING       MB_ICONEXCLAMATION
#define MB_ICONINFORMATION   MB_ICONASTERISK
#define MB_APPLMODAL         0x00000000L
#define MB_SYSTEMMODAL       0x00001000L
#define MB_TASKMODAL         0x00002000L
#define MB_SETFOREGROUND     0x00010000L
#define MB_TOPMOST           0x00040000L

#define IDOK     1
#define IDCANCEL 2
#define IDABORT  3
#define IDRETRY  4
#define IDIGNORE 5
#define IDYES    6
#define IDNO     7

int MessageBoxW(HWND hWnd, const wchar_t* text, const wchar_t* caption, UINT type);
int MessageBoxA(HWND hWnd, const char* text, const char* caption, UINT type);  // UTF-8 (narrow strings are UTF-8)
#define MessageBox MessageBoxW

#endif
