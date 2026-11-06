#include "DXUT.h"

#include "x2/platform/unicode.h"

#include <SDL3/SDL.h>

#include <cstdint>
#include <cwchar>

#if !defined(_WIN32)

namespace {

SDL_Window* CaptureWindow = nullptr;

bool IsMouseButton(int vKey)
{
    return vKey == VK_LBUTTON || vKey == VK_RBUTTON || vKey == VK_MBUTTON || vKey == VK_XBUTTON1 ||
           vKey == VK_XBUTTON2;
}

}  // namespace

HWND SetCapture(HWND hWnd)
{
    HWND previous = GetCapture();
    SDL_Window* window = DXUTGetSDLWindow();
    if (window != nullptr && hWnd == DXUTGetHWND() && SDL_CaptureMouse(true)) {
        CaptureWindow = window;
    }
    return previous;
}

BOOL ReleaseCapture()
{
    if (CaptureWindow != nullptr) {
        SDL_CaptureMouse(false);
        CaptureWindow = nullptr;
    }
    return TRUE;
}

HWND GetCapture()
{
    return CaptureWindow != nullptr && CaptureWindow == DXUTGetSDLWindow() ? DXUTGetHWND() : nullptr;
}

SHORT GetKeyState(int nVirtKey)
{
    if (nVirtKey < 0 || nVirtKey > 0xFF) {
        return 0;
    }
    const BYTE vKey = static_cast<BYTE>(nVirtKey);
    const bool down = IsMouseButton(nVirtKey) ? DXUTIsMouseButtonDown(vKey) : DXUTIsKeyDown(vKey);
    SHORT state = down ? static_cast<SHORT>(0x8000) : 0;
    const SDL_Keymod mods = SDL_GetModState();
    if ((nVirtKey == VK_CAPITAL && (mods & SDL_KMOD_CAPS) != 0) ||
        (nVirtKey == VK_NUMLOCK && (mods & SDL_KMOD_NUM) != 0) ||
        (nVirtKey == VK_SCROLL && (mods & SDL_KMOD_SCROLL) != 0)) {
        state = static_cast<SHORT>(state | 1);
    }
    return state;
}

SHORT GetAsyncKeyState(int vKey)
{
    return static_cast<SHORT>(GetKeyState(vKey) & static_cast<SHORT>(0x8000));
}

HWND GetFocus()
{
    SDL_Window* window = DXUTGetSDLWindow();
    return window != nullptr && SDL_GetKeyboardFocus() == window ? DXUTGetHWND() : nullptr;
}

BOOL GetClientRect(HWND /*hWnd*/, RECT* lpRect)
{
    if (lpRect == nullptr) {
        return FALSE;
    }
    int width = 0;
    int height = 0;
    if (SDL_Window* window = DXUTGetSDLWindow(); window != nullptr) {
        SDL_GetWindowSizeInPixels(window, &width, &height);
    }
    *lpRect = RECT{0, 0, width, height};
    return TRUE;
}

UINT GetCaretBlinkTime()
{
    return 530;
}

BOOL SystemParametersInfoW(UINT uiAction, UINT /*uiParam*/, void* pvParam, UINT /*fWinIni*/)
{
    if (uiAction == SPI_GETWHEELSCROLLLINES && pvParam != nullptr) {
        *static_cast<UINT*>(pvParam) = 3;
        return TRUE;
    }
    return FALSE;
}

namespace {

constexpr WORD kEnglishUs = 0x0409;

}  // namespace

HKL GetKeyboardLayout(DWORD /*idThread*/)
{
    return reinterpret_cast<HKL>(static_cast<std::uintptr_t>(0x04090409));
}

int GetKeyboardLayoutList(int nBuff, HKL* lpList)
{
    if (nBuff > 0 && lpList != nullptr) {
        lpList[0] = GetKeyboardLayout(0);
    }
    return 1;
}

int GetLocaleInfoW(LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData)
{
    if (LOWORD(Locale) != kEnglishUs) {
        return 0;
    }
    const wchar_t* value = LCType == LOCALE_SABBREVLANGNAME ? L"ENU"
                           : LCType == LOCALE_SLANGUAGE     ? L"English (United States)"
                                                            : nullptr;
    if (value == nullptr) {
        return 0;
    }
    const int length = static_cast<int>(std::wcslen(value)) + 1;
    if (cchData == 0) {
        return length;
    }
    if (lpLCData == nullptr || cchData < length) {
        return 0;
    }
    std::wmemcpy(lpLCData, value, static_cast<std::size_t>(length));
    return length;
}

#endif

std::wstring DXUTGetClipboardText()
{
    char* text = SDL_GetClipboardText();
    std::wstring result = text != nullptr ? x2::platform::Utf8ToWide(text) : std::wstring();
    SDL_free(text);
    return result;
}

bool DXUTSetClipboardText(const std::wstring& text)
{
    return SDL_SetClipboardText(x2::platform::WideToUtf8(text).c_str());
}
