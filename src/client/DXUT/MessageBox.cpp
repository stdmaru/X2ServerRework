#include "MessageBox.h"

#if !defined(_WIN32)

#include "x2/platform/unicode.h"

#include <SDL3/SDL_messagebox.h>

#include <string>
#include <vector>

namespace {

int Show(HWND hWnd, const std::string& text, const std::string& caption, UINT type)
{
    struct Button { int id; const char* label; };
    std::vector<Button> buttons;
    switch (type & 0xF) {
    case MB_OKCANCEL:         buttons = {{IDOK, "OK"}, {IDCANCEL, "Cancel"}}; break;
    case MB_ABORTRETRYIGNORE: buttons = {{IDABORT, "Abort"}, {IDRETRY, "Retry"}, {IDIGNORE, "Ignore"}}; break;
    case MB_YESNOCANCEL:      buttons = {{IDYES, "Yes"}, {IDNO, "No"}, {IDCANCEL, "Cancel"}}; break;
    case MB_YESNO:            buttons = {{IDYES, "Yes"}, {IDNO, "No"}}; break;
    case MB_RETRYCANCEL:      buttons = {{IDRETRY, "Retry"}, {IDCANCEL, "Cancel"}}; break;
    default:                  buttons = {{IDOK, "OK"}}; break;
    }

    std::vector<SDL_MessageBoxButtonData> data;
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        SDL_MessageBoxButtonFlags flags = 0;
        if (i == 0) {
            flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;   // Win32's default button is the first one
        }
        if (buttons[i].id == IDCANCEL || (buttons.size() == 1 && buttons[i].id == IDOK)) {
            flags |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
        }
        data.push_back({flags, buttons[i].id, buttons[i].label});
    }

    SDL_MessageBoxFlags flags = SDL_MESSAGEBOX_INFORMATION;
    switch (type & 0xF0) {
    case MB_ICONHAND:        flags = SDL_MESSAGEBOX_ERROR; break;
    case MB_ICONEXCLAMATION: flags = SDL_MESSAGEBOX_WARNING; break;
    default: break;
    }
    flags |= SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;

    const SDL_MessageBoxData box{flags, reinterpret_cast<SDL_Window*>(hWnd), caption.c_str(), text.c_str(),
                                 static_cast<int>(data.size()), data.data(), nullptr};
    int pressed = 0;
    if (!SDL_ShowMessageBox(&box, &pressed)) {
        return 0;   // Win32 returns 0 on failure
    }
    // Closing the box without a button: Win32 answers IDCANCEL when there is a Cancel button, else IDOK for MB_OK
    if (pressed < 0) {
        for (const Button& button : buttons) {
            if (button.id == IDCANCEL) {
                return IDCANCEL;
            }
        }
        return buttons.size() == 1 ? IDOK : 0;
    }
    return pressed;
}

}  // namespace

int MessageBoxW(HWND hWnd, const wchar_t* text, const wchar_t* caption, UINT type)
{
    return MessageBoxA(hWnd, x2::platform::WideToUtf8(text ? text : L"").c_str(),
                       x2::platform::WideToUtf8(caption ? caption : L"Error").c_str(), type);
}

int MessageBoxA(HWND hWnd, const char* text, const char* caption, UINT type)
{
    // Win32 text breaks lines with CRLF (the string table turns "\\n" into CRLF); the dialogs SDL uses want LF.
    std::string body(text ? text : "");
    for (std::size_t at = body.find("\r\n"); at != std::string::npos; at = body.find("\r\n", at)) {
        body.erase(at, 1);
    }
    return Show(hWnd, body, caption ? caption : "Error", type);
}

#endif
