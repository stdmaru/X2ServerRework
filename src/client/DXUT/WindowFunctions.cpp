#include "DXUT.h"

#include "x2/platform/unicode.h"

#include <SDL3/SDL.h>

#include <cstdint>
#include <cstring>
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

int GetSystemMetrics(int nIndex)
{
    SDL_Window* window = DXUTGetSDLWindow();
    const SDL_DisplayID display = window != nullptr ? SDL_GetDisplayForWindow(window) : SDL_GetPrimaryDisplay();
    SDL_Rect bounds{};
    if (!SDL_GetDisplayBounds(display, &bounds))
        return 0;
    switch (nIndex) {
    case SM_CXSCREEN: return bounds.w;
    case SM_CYSCREEN: return bounds.h;
    default: return 0;
    }
}

BOOL GetCursorPos(POINT* lpPoint)
{
    if (lpPoint == nullptr)
        return FALSE;
    float x = 0, y = 0;
    SDL_GetGlobalMouseState(&x, &y);
    lpPoint->x = static_cast<LONG>(x);
    lpPoint->y = static_cast<LONG>(y);
    return TRUE;
}

BOOL ScreenToClient(HWND /*hWnd*/, POINT* lpPoint)
{
    SDL_Window* window = DXUTGetSDLWindow();
    int x = 0, y = 0;
    if (lpPoint == nullptr || window == nullptr || !SDL_GetWindowPosition(window, &x, &y))
        return FALSE;
    lpPoint->x -= x;
    lpPoint->y -= y;
    return TRUE;
}

BOOL GetWindowRect(HWND /*hWnd*/, RECT* lpRect)
{
    SDL_Window* window = DXUTGetSDLWindow();
    int x = 0, y = 0, w = 0, h = 0;
    if (lpRect == nullptr || window == nullptr || !SDL_GetWindowPosition(window, &x, &y) || !SDL_GetWindowSize(window, &w, &h))
        return FALSE;
    int top = 0, left = 0, bottom = 0, right = 0;
    SDL_GetWindowBordersSize(window, &top, &left, &bottom, &right);  // zeros when the system does not say
    *lpRect = {x - left, y - top, x + w + right, y + h + bottom};
    return TRUE;
}

HWND WindowFromPoint(POINT Point)
{
    POINT client = Point;
    RECT rect{};
    if (!ScreenToClient(DXUTGetHWND(), &client) || !GetClientRect(DXUTGetHWND(), &rect))
        return nullptr;
    return client.x >= rect.left && client.x < rect.right && client.y >= rect.top && client.y < rect.bottom ? DXUTGetHWND() : nullptr;
}

HWND GetActiveWindow()
{
    return GetFocus();
}

BOOL SetForegroundWindow(HWND /*hWnd*/)
{
    SDL_Window* window = DXUTGetSDLWindow();
    return window != nullptr && SDL_RaiseWindow(window) ? TRUE : FALSE;
}

HWND SetFocus(HWND hWnd)
{
    HWND previous = GetFocus();
    SetForegroundWindow(hWnd);
    return previous;
}

BOOL SetWindowTextW(HWND /*hWnd*/, LPCWSTR lpString)
{
    SDL_Window* window = DXUTGetSDLWindow();
    return window != nullptr && lpString != nullptr && SDL_SetWindowTitle(window, x2::platform::WideToUtf8(lpString).c_str()) ? TRUE : FALSE;
}

void PostQuitMessage(int /*nExitCode*/)
{
    SDL_Event event{};
    event.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&event);
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

namespace {

DXUTCursor* ShownCursor = nullptr;

std::uint32_t ReadU32(const unsigned char* p) { return p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<std::uint32_t>(p[3]) << 24); }
std::uint16_t ReadU16(const unsigned char* p) { return static_cast<std::uint16_t>(p[0] | (p[1] << 8)); }

}  // namespace

// The .cur layout: ICONDIR (6 bytes, type 2), the first ICONDIRENTRY (16 bytes: size, hot spot, data size and offset),
// then a BITMAPINFOHEADER whose height counts the XOR and AND images, the bottom-up B, G, R, A rows, and the 1-bit AND
// mask (rows padded to 4 bytes). When every alpha is 0 (cursors drawn before alpha channels), the mask gives it.
DXUTCursor* DXUTCreateCursorFromCur(const unsigned char* data, std::size_t size)
{
    if (data == nullptr || size < 6 + 16 + 40 || ReadU16(data + 2) != 2 || ReadU16(data + 4) == 0)
        return nullptr;
    const unsigned char* entry = data + 6;
    const int hotX = ReadU16(entry + 4);
    const int hotY = ReadU16(entry + 6);
    const std::uint32_t offset = ReadU32(entry + 12);
    if (offset + 40 > size)
        return nullptr;
    const unsigned char* header = data + offset;
    const int width = static_cast<int>(ReadU32(header + 4));
    const int height = static_cast<int>(ReadU32(header + 8)) / 2;
    const int bitCount = ReadU16(header + 14);
    const std::uint32_t headerSize = ReadU32(header);
    const std::size_t maskPitch = ((static_cast<std::size_t>(width) + 31) / 32) * 4;
    const std::size_t pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4;
    if (bitCount != 32 || width <= 0 || height <= 0 || offset + headerSize + pixels + maskPitch * height > size)
        return nullptr;
    const unsigned char* xorImage = header + headerSize;
    const unsigned char* andMask = xorImage + pixels;

    SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_BGRA32);
    if (surface == nullptr)
        return nullptr;
    bool anyAlpha = false;
    for (std::size_t i = 3; i < pixels; i += 4)
        anyAlpha = anyAlpha || xorImage[i] != 0;
    for (int y = 0; y < height; ++y) {
        const unsigned char* src = xorImage + static_cast<std::size_t>(height - 1 - y) * width * 4;
        const unsigned char* mask = andMask + static_cast<std::size_t>(height - 1 - y) * maskPitch;
        auto* dst = static_cast<unsigned char*>(surface->pixels) + static_cast<std::size_t>(y) * surface->pitch;
        for (int x = 0; x < width; ++x) {
            std::memcpy(dst + x * 4, src + x * 4, 4);
            if (!anyAlpha)
                dst[x * 4 + 3] = (mask[x / 8] & (0x80 >> (x % 8))) != 0 ? 0 : 255;
        }
    }
    SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotX, hotY);
    SDL_DestroySurface(surface);
    return reinterpret_cast<DXUTCursor*>(cursor);
}

void DXUTDestroyCursor(DXUTCursor* cursor)
{
    if (cursor == ShownCursor)
        ShownCursor = nullptr;
    SDL_DestroyCursor(reinterpret_cast<SDL_Cursor*>(cursor));
}

void DXUTSetCursor(DXUTCursor* cursor)
{
    ShownCursor = cursor;
    if (cursor == nullptr) {
        SDL_HideCursor();
        return;
    }
    SDL_SetCursor(reinterpret_cast<SDL_Cursor*>(cursor));
    SDL_ShowCursor();
}

DXUTCursor* DXUTGetCursor()
{
    return ShownCursor;
}
