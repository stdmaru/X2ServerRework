#include "SdlMessages.h"

#include "x2/platform/unicode.h"

#include <cmath>

namespace x2::dxut {

namespace {

// Bits of the lParam of WM_KEYDOWN/WM_KEYUP/WM_SYSKEY*.
constexpr LPARAM kExtendedBit = LPARAM(1) << 24;
constexpr LPARAM kContextBit = LPARAM(1) << 29;     // Alt held
constexpr LPARAM kPreviousBit = LPARAM(1) << 30;    // key was already down
constexpr LPARAM kTransitionBit = LPARAM(1) << 31;  // key is being released

LPARAM PointParam(float x, float y)
{
    // Client coordinates are signed 16-bit values (GET_X_LPARAM), negative while dragging outside the window.
    const auto ix = static_cast<int>(std::lround(x));
    const auto iy = static_cast<int>(std::lround(y));
    return MAKELPARAM(static_cast<WORD>(static_cast<short>(ix)), static_cast<WORD>(static_cast<short>(iy)));
}

WPARAM ModifierKeys(SDL_Keymod mod)
{
    WPARAM keys = 0;
    if (mod & SDL_KMOD_SHIFT) {
        keys |= MK_SHIFT;
    }
    if (mod & SDL_KMOD_CTRL) {
        keys |= MK_CONTROL;
    }
    return keys;
}

bool ForWindow(SDL_WindowID eventWindow, const TranslateContext& context)
{
    return context.window == 0 || eventWindow == context.window;
}

}  // namespace

VirtualKey ToVirtualKey(SDL_Scancode scancode, SDL_Keycode key, SDL_Keymod mod)
{
    // Letters and digits follow the keyboard layout, like Windows' VK codes (the key labelled A on AZERTY is VK 'A');
    // with a non-Latin layout, the position decides.
    static constexpr UINT kLetterScan[26] = {0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32,
                                             0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C};
    if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
        const int index = scancode - SDL_SCANCODE_A;
        UINT vk = static_cast<UINT>('A' + index);
        if (key >= 'a' && key <= 'z') {
            vk = static_cast<UINT>('A' + (key - 'a'));
        }
        return {vk, kLetterScan[index], false};
    }
    if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_0) {
        const int index = scancode - SDL_SCANCODE_1;  // 1..9, 0
        UINT vk = index == 9 ? '0' : static_cast<UINT>('1' + index);
        if (key >= '0' && key <= '9') {
            vk = static_cast<UINT>(key);
        }
        return {vk, static_cast<UINT>(0x02 + index), false};
    }
    if (scancode >= SDL_SCANCODE_F1 && scancode <= SDL_SCANCODE_F10) {
        return {static_cast<UINT>(VK_F1 + (scancode - SDL_SCANCODE_F1)), static_cast<UINT>(0x3B + (scancode - SDL_SCANCODE_F1)),
                false};
    }
    if (scancode >= SDL_SCANCODE_F13 && scancode <= SDL_SCANCODE_F24) {
        return {static_cast<UINT>(VK_F13 + (scancode - SDL_SCANCODE_F13)), 0, false};
    }

    // Keypad digits are navigation keys while Num Lock is off (not extended, unlike the separate arrow block).
    const bool numLock = (mod & SDL_KMOD_NUM) != 0;
    switch (scancode) {
    case SDL_SCANCODE_KP_1: return {numLock ? UINT(VK_NUMPAD1) : UINT(VK_END), 0x4F, false};
    case SDL_SCANCODE_KP_2: return {numLock ? UINT(VK_NUMPAD2) : UINT(VK_DOWN), 0x50, false};
    case SDL_SCANCODE_KP_3: return {numLock ? UINT(VK_NUMPAD3) : UINT(VK_NEXT), 0x51, false};
    case SDL_SCANCODE_KP_4: return {numLock ? UINT(VK_NUMPAD4) : UINT(VK_LEFT), 0x4B, false};
    case SDL_SCANCODE_KP_5: return {numLock ? UINT(VK_NUMPAD5) : UINT(VK_CLEAR), 0x4C, false};
    case SDL_SCANCODE_KP_6: return {numLock ? UINT(VK_NUMPAD6) : UINT(VK_RIGHT), 0x4D, false};
    case SDL_SCANCODE_KP_7: return {numLock ? UINT(VK_NUMPAD7) : UINT(VK_HOME), 0x47, false};
    case SDL_SCANCODE_KP_8: return {numLock ? UINT(VK_NUMPAD8) : UINT(VK_UP), 0x48, false};
    case SDL_SCANCODE_KP_9: return {numLock ? UINT(VK_NUMPAD9) : UINT(VK_PRIOR), 0x49, false};
    case SDL_SCANCODE_KP_0: return {numLock ? UINT(VK_NUMPAD0) : UINT(VK_INSERT), 0x52, false};
    case SDL_SCANCODE_KP_PERIOD: return {numLock ? UINT(VK_DECIMAL) : UINT(VK_DELETE), 0x53, false};
    default: break;
    }

    switch (scancode) {
    case SDL_SCANCODE_RETURN: return {VK_RETURN, 0x1C, false};
    case SDL_SCANCODE_ESCAPE: return {VK_ESCAPE, 0x01, false};
    case SDL_SCANCODE_BACKSPACE: return {VK_BACK, 0x0E, false};
    case SDL_SCANCODE_TAB: return {VK_TAB, 0x0F, false};
    case SDL_SCANCODE_SPACE: return {VK_SPACE, 0x39, false};
    case SDL_SCANCODE_MINUS: return {VK_OEM_MINUS, 0x0C, false};
    case SDL_SCANCODE_EQUALS: return {VK_OEM_PLUS, 0x0D, false};
    case SDL_SCANCODE_LEFTBRACKET: return {VK_OEM_4, 0x1A, false};
    case SDL_SCANCODE_RIGHTBRACKET: return {VK_OEM_6, 0x1B, false};
    case SDL_SCANCODE_BACKSLASH: return {VK_OEM_5, 0x2B, false};
    case SDL_SCANCODE_NONUSHASH: return {VK_OEM_5, 0x2B, false};
    case SDL_SCANCODE_SEMICOLON: return {VK_OEM_1, 0x27, false};
    case SDL_SCANCODE_APOSTROPHE: return {VK_OEM_7, 0x28, false};
    case SDL_SCANCODE_GRAVE: return {VK_OEM_3, 0x29, false};
    case SDL_SCANCODE_COMMA: return {VK_OEM_COMMA, 0x33, false};
    case SDL_SCANCODE_PERIOD: return {VK_OEM_PERIOD, 0x34, false};
    case SDL_SCANCODE_SLASH: return {VK_OEM_2, 0x35, false};
    case SDL_SCANCODE_NONUSBACKSLASH: return {VK_OEM_102, 0x56, false};
    case SDL_SCANCODE_CAPSLOCK: return {VK_CAPITAL, 0x3A, false};
    case SDL_SCANCODE_F11: return {VK_F11, 0x57, false};
    case SDL_SCANCODE_F12: return {VK_F12, 0x58, false};
    case SDL_SCANCODE_PRINTSCREEN: return {VK_SNAPSHOT, 0x37, true};
    case SDL_SCANCODE_SCROLLLOCK: return {VK_SCROLL, 0x46, false};
    case SDL_SCANCODE_PAUSE: return {VK_PAUSE, 0x45, false};
    case SDL_SCANCODE_INSERT: return {VK_INSERT, 0x52, true};
    case SDL_SCANCODE_HOME: return {VK_HOME, 0x47, true};
    case SDL_SCANCODE_PAGEUP: return {VK_PRIOR, 0x49, true};
    case SDL_SCANCODE_DELETE: return {VK_DELETE, 0x53, true};
    case SDL_SCANCODE_END: return {VK_END, 0x4F, true};
    case SDL_SCANCODE_PAGEDOWN: return {VK_NEXT, 0x51, true};
    case SDL_SCANCODE_RIGHT: return {VK_RIGHT, 0x4D, true};
    case SDL_SCANCODE_LEFT: return {VK_LEFT, 0x4B, true};
    case SDL_SCANCODE_DOWN: return {VK_DOWN, 0x50, true};
    case SDL_SCANCODE_UP: return {VK_UP, 0x48, true};
    case SDL_SCANCODE_NUMLOCKCLEAR: return {VK_NUMLOCK, 0x45, true};
    case SDL_SCANCODE_KP_DIVIDE: return {VK_DIVIDE, 0x35, true};
    case SDL_SCANCODE_KP_MULTIPLY: return {VK_MULTIPLY, 0x37, false};
    case SDL_SCANCODE_KP_MINUS: return {VK_SUBTRACT, 0x4A, false};
    case SDL_SCANCODE_KP_PLUS: return {VK_ADD, 0x4E, false};
    case SDL_SCANCODE_KP_ENTER: return {VK_RETURN, 0x1C, true};
    case SDL_SCANCODE_APPLICATION: return {VK_APPS, 0x5D, true};
    // WM_KEYDOWN reports the generic VK_SHIFT/VK_CONTROL/VK_MENU; the scan code and the extended bit tell the sides.
    case SDL_SCANCODE_LCTRL: return {VK_CONTROL, 0x1D, false};
    case SDL_SCANCODE_RCTRL: return {VK_CONTROL, 0x1D, true};
    case SDL_SCANCODE_LSHIFT: return {VK_SHIFT, 0x2A, false};
    case SDL_SCANCODE_RSHIFT: return {VK_SHIFT, 0x36, false};
    case SDL_SCANCODE_LALT: return {VK_MENU, 0x38, false};
    case SDL_SCANCODE_RALT: return {VK_MENU, 0x38, true};
    case SDL_SCANCODE_LGUI: return {VK_LWIN, 0x5B, true};
    case SDL_SCANCODE_RGUI: return {VK_RWIN, 0x5C, true};
    case SDL_SCANCODE_LANG1: return {VK_HANGUL, 0x72, false};  // Korean keyboards: Hangul/English
    case SDL_SCANCODE_LANG2: return {VK_HANJA, 0x71, false};
    default: return {};
    }
}

void SdlMessageTranslator::Translate(const SDL_Event& event, const TranslateContext& context, std::vector<WinMessage>& out)
{
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        if (ForWindow(event.key.windowID, context)) {
            Key(event.key, out);
        }
        break;

    case SDL_EVENT_TEXT_INPUT:
        if (ForWindow(event.text.windowID, context)) {
            Text(event.text, out);
        }
        break;

    case SDL_EVENT_TEXT_EDITING:
        if (ForWindow(event.edit.windowID, context)) {
            Editing(event.edit, out);
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (ForWindow(event.motion.windowID, context)) {
            out.push_back({WM_MOUSEMOVE, m_buttons | ModifierKeys(context.mod),
                           PointParam(event.motion.x * context.pixelDensity, event.motion.y * context.pixelDensity)});
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (ForWindow(event.button.windowID, context)) {
            Button(event.button, context, out);
        }
        break;

    case SDL_EVENT_MOUSE_WHEEL:
        if (ForWindow(event.wheel.windowID, context) && event.wheel.y != 0.0f) {
            float y = event.wheel.y;
            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                y = -y;
            }
            // Positive away from the user, in WHEEL_DELTA units; lParam holds screen coordinates.
            const auto delta = static_cast<short>(std::lround(y * WHEEL_DELTA));
            const WPARAM keys = m_buttons | ModifierKeys(context.mod);
            out.push_back({WM_MOUSEWHEEL, MAKEWPARAM(keys, static_cast<WORD>(delta)),
                           PointParam(context.windowX * context.pixelDensity + event.wheel.mouse_x * context.pixelDensity,
                                      context.windowY * context.pixelDensity + event.wheel.mouse_y * context.pixelDensity)});
        }
        break;

    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        if (ForWindow(event.window.windowID, context)) {
            out.push_back({WM_ACTIVATEAPP, TRUE, 0});
            out.push_back({WM_SETFOCUS, 0, 0});
        }
        break;

    case SDL_EVENT_WINDOW_FOCUS_LOST:
        if (ForWindow(event.window.windowID, context)) {
            ReleaseMouseButtons();
            out.push_back({WM_KILLFOCUS, 0, 0});
            out.push_back({WM_ACTIVATEAPP, FALSE, 0});
        }
        break;

    case SDL_EVENT_WINDOW_MINIMIZED:
        if (ForWindow(event.window.windowID, context)) {
            out.push_back({WM_SIZE, SIZE_MINIMIZED, 0});
        }
        break;

    case SDL_EVENT_WINDOW_MAXIMIZED:
    case SDL_EVENT_WINDOW_RESTORED:
        if (ForWindow(event.window.windowID, context)) {
            m_maximized = event.type == SDL_EVENT_WINDOW_MAXIMIZED;
            out.push_back({WM_SIZE, WPARAM(m_maximized ? SIZE_MAXIMIZED : SIZE_RESTORED),
                           MAKELPARAM(context.pixelWidth, context.pixelHeight)});
        }
        break;

    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        if (ForWindow(event.window.windowID, context)) {
            out.push_back({WM_SIZE, WPARAM(m_maximized ? SIZE_MAXIMIZED : SIZE_RESTORED),
                           MAKELPARAM(event.window.data1, event.window.data2)});
        }
        break;

    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        if (ForWindow(event.window.windowID, context)) {
            out.push_back({WM_CLOSE, 0, 0});
        }
        break;

    case SDL_EVENT_QUIT:  // e.g. SIGINT/SIGTERM, or the session ending: close the window like Windows does at log off
        out.push_back({WM_CLOSE, 0, 0});
        break;

    default:
        break;
    }
}

void SdlMessageTranslator::Key(const SDL_KeyboardEvent& key, std::vector<WinMessage>& out)
{
    const VirtualKey vk = ToVirtualKey(key.scancode, key.key, key.mod);
    if (vk.vk == 0) {
        return;
    }

    // Windows sends WM_SYS* while Alt is held without Ctrl (Ctrl+Alt is AltGr), and for F10 and Alt itself.
    const bool alt = (key.mod & SDL_KMOD_ALT) != 0;
    const bool ctrl = (key.mod & SDL_KMOD_CTRL) != 0;
    const bool system = (alt && !ctrl) || vk.vk == VK_F10 || (vk.vk == VK_MENU && !ctrl);

    LPARAM lParam = 1 | (static_cast<LPARAM>(vk.scanCode & 0xFF) << 16);
    if (vk.extended) {
        lParam |= kExtendedBit;
    }
    if (alt && !ctrl) {
        lParam |= kContextBit;
    }
    UINT message = 0;
    if (key.down) {
        if (key.repeat) {
            lParam |= kPreviousBit;
        }
        message = system ? WM_SYSKEYDOWN : WM_KEYDOWN;
    }
    else {
        lParam |= kPreviousBit | kTransitionBit;
        message = system ? WM_SYSKEYUP : WM_KEYUP;
    }
    out.push_back({message, vk.vk, lParam});
}

void SdlMessageTranslator::Text(const SDL_TextInputEvent& text, std::vector<WinMessage>& out)
{
    if (!text.text) {
        return;
    }
    const std::wstring chars = x2::platform::Utf8ToWide(text.text);
    if (chars.empty()) {
        return;
    }
    const bool fromIme = m_composing;
    if (fromIme) {
        // Committed IME text: the result string, the end of the composition, then (through DefWindowProc) the characters.
        m_result = chars;
        m_composing = false;
        m_composition.clear();
        m_compositionCursor = 0;
        out.push_back({WM_IME_COMPOSITION, static_cast<WPARAM>(chars.front()), GCS_RESULTSTR});
        out.push_back({WM_IME_ENDCOMPOSITION, 0, 0});
    }
    // One WM_CHAR per WCHAR: UTF-16 units on Windows, code points where wchar_t is 32-bit.
    for (const wchar_t c : chars) {
        out.push_back({WM_CHAR, static_cast<WPARAM>(c), 1, fromIme});
    }
}

void SdlMessageTranslator::Editing(const SDL_TextEditingEvent& editing, std::vector<WinMessage>& out)
{
    const std::wstring text = editing.text ? x2::platform::Utf8ToWide(editing.text) : std::wstring();
    if (text.empty()) {
        if (m_composing) {  // composition cancelled (or emptied with backspace)
            m_composing = false;
            m_composition.clear();
            m_compositionCursor = 0;
            out.push_back({WM_IME_COMPOSITION, 0, GCS_COMPSTR | GCS_CURSORPOS});
            out.push_back({WM_IME_ENDCOMPOSITION, 0, 0});
        }
        return;
    }
    if (!m_composing) {
        m_composing = true;
        out.push_back({WM_IME_STARTCOMPOSITION, 0, 0});
    }
    m_composition = text;
    // SDL counts the cursor in UTF-8 code points; std::wstring is code points on Linux, UTF-16 on Windows (Korean
    // syllables are in the BMP, so the two agree for the client's IME).
    m_compositionCursor = editing.start < 0 ? static_cast<int>(text.size()) : static_cast<int>(editing.start);
    out.push_back({WM_IME_COMPOSITION, static_cast<WPARAM>(text.back()), GCS_COMPSTR | GCS_COMPATTR | GCS_CURSORPOS});
}

void SdlMessageTranslator::Button(const SDL_MouseButtonEvent& button, const TranslateContext& context, std::vector<WinMessage>& out)
{
    UINT down = 0;
    UINT up = 0;
    UINT dblclk = 0;
    WPARAM mask = 0;
    WORD xbutton = 0;
    switch (button.button) {
    case SDL_BUTTON_LEFT: down = WM_LBUTTONDOWN; up = WM_LBUTTONUP; dblclk = WM_LBUTTONDBLCLK; mask = MK_LBUTTON; break;
    case SDL_BUTTON_RIGHT: down = WM_RBUTTONDOWN; up = WM_RBUTTONUP; dblclk = WM_RBUTTONDBLCLK; mask = MK_RBUTTON; break;
    case SDL_BUTTON_MIDDLE: down = WM_MBUTTONDOWN; up = WM_MBUTTONUP; dblclk = WM_MBUTTONDBLCLK; mask = MK_MBUTTON; break;
    case SDL_BUTTON_X1:
        down = WM_XBUTTONDOWN; up = WM_XBUTTONUP; dblclk = WM_XBUTTONDBLCLK; mask = MK_XBUTTON1; xbutton = XBUTTON1;
        break;
    case SDL_BUTTON_X2:
        down = WM_XBUTTONDOWN; up = WM_XBUTTONUP; dblclk = WM_XBUTTONDBLCLK; mask = MK_XBUTTON2; xbutton = XBUTTON2;
        break;
    default: return;
    }

    UINT message = 0;
    if (button.down) {
        m_buttons |= mask;
        // The DXUT window class has CS_DBLCLKS: the second click of a double click arrives as WM_*BUTTONDBLCLK.
        message = (button.clicks % 2 == 0) ? dblclk : down;
    }
    else {
        m_buttons &= ~mask;
        message = up;
    }
    // Key state after the change (down includes the button, up no longer does), plus the X button in the high word.
    const WPARAM keys = m_buttons | ModifierKeys(context.mod);
    out.push_back({message, xbutton ? MAKEWPARAM(keys, xbutton) : keys,
                   PointParam(button.x * context.pixelDensity, button.y * context.pixelDensity)});
}

}  // namespace x2::dxut
