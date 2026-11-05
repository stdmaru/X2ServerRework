#include "SdlInput.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_stdinc.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>

namespace x2::dxut {

namespace {

#define X2_DIK(name, scancode) DirectInputKeyInfo{#name, name, scancode}

constexpr DirectInputKeyInfo kDirectInputKeys[] = {
    X2_DIK(DIK_ESCAPE, SDL_SCANCODE_ESCAPE),
    X2_DIK(DIK_1, SDL_SCANCODE_1),
    X2_DIK(DIK_2, SDL_SCANCODE_2),
    X2_DIK(DIK_3, SDL_SCANCODE_3),
    X2_DIK(DIK_4, SDL_SCANCODE_4),
    X2_DIK(DIK_5, SDL_SCANCODE_5),
    X2_DIK(DIK_6, SDL_SCANCODE_6),
    X2_DIK(DIK_7, SDL_SCANCODE_7),
    X2_DIK(DIK_8, SDL_SCANCODE_8),
    X2_DIK(DIK_9, SDL_SCANCODE_9),
    X2_DIK(DIK_0, SDL_SCANCODE_0),
    X2_DIK(DIK_MINUS, SDL_SCANCODE_MINUS),
    X2_DIK(DIK_EQUALS, SDL_SCANCODE_EQUALS),
    X2_DIK(DIK_BACK, SDL_SCANCODE_BACKSPACE),
    X2_DIK(DIK_TAB, SDL_SCANCODE_TAB),
    X2_DIK(DIK_Q, SDL_SCANCODE_Q),
    X2_DIK(DIK_W, SDL_SCANCODE_W),
    X2_DIK(DIK_E, SDL_SCANCODE_E),
    X2_DIK(DIK_R, SDL_SCANCODE_R),
    X2_DIK(DIK_T, SDL_SCANCODE_T),
    X2_DIK(DIK_Y, SDL_SCANCODE_Y),
    X2_DIK(DIK_U, SDL_SCANCODE_U),
    X2_DIK(DIK_I, SDL_SCANCODE_I),
    X2_DIK(DIK_O, SDL_SCANCODE_O),
    X2_DIK(DIK_P, SDL_SCANCODE_P),
    X2_DIK(DIK_LBRACKET, SDL_SCANCODE_LEFTBRACKET),
    X2_DIK(DIK_RBRACKET, SDL_SCANCODE_RIGHTBRACKET),
    X2_DIK(DIK_RETURN, SDL_SCANCODE_RETURN),
    X2_DIK(DIK_LCONTROL, SDL_SCANCODE_LCTRL),
    X2_DIK(DIK_A, SDL_SCANCODE_A),
    X2_DIK(DIK_S, SDL_SCANCODE_S),
    X2_DIK(DIK_D, SDL_SCANCODE_D),
    X2_DIK(DIK_F, SDL_SCANCODE_F),
    X2_DIK(DIK_G, SDL_SCANCODE_G),
    X2_DIK(DIK_H, SDL_SCANCODE_H),
    X2_DIK(DIK_J, SDL_SCANCODE_J),
    X2_DIK(DIK_K, SDL_SCANCODE_K),
    X2_DIK(DIK_L, SDL_SCANCODE_L),
    X2_DIK(DIK_SEMICOLON, SDL_SCANCODE_SEMICOLON),
    X2_DIK(DIK_APOSTROPHE, SDL_SCANCODE_APOSTROPHE),
    X2_DIK(DIK_GRAVE, SDL_SCANCODE_GRAVE),
    X2_DIK(DIK_LSHIFT, SDL_SCANCODE_LSHIFT),
    X2_DIK(DIK_BACKSLASH, SDL_SCANCODE_BACKSLASH),
    X2_DIK(DIK_Z, SDL_SCANCODE_Z),
    X2_DIK(DIK_X, SDL_SCANCODE_X),
    X2_DIK(DIK_C, SDL_SCANCODE_C),
    X2_DIK(DIK_V, SDL_SCANCODE_V),
    X2_DIK(DIK_B, SDL_SCANCODE_B),
    X2_DIK(DIK_N, SDL_SCANCODE_N),
    X2_DIK(DIK_M, SDL_SCANCODE_M),
    X2_DIK(DIK_COMMA, SDL_SCANCODE_COMMA),
    X2_DIK(DIK_PERIOD, SDL_SCANCODE_PERIOD),
    X2_DIK(DIK_SLASH, SDL_SCANCODE_SLASH),
    X2_DIK(DIK_RSHIFT, SDL_SCANCODE_RSHIFT),
    X2_DIK(DIK_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY),
    X2_DIK(DIK_LMENU, SDL_SCANCODE_LALT),
    X2_DIK(DIK_SPACE, SDL_SCANCODE_SPACE),
    X2_DIK(DIK_CAPITAL, SDL_SCANCODE_CAPSLOCK),
    X2_DIK(DIK_F1, SDL_SCANCODE_F1),
    X2_DIK(DIK_F2, SDL_SCANCODE_F2),
    X2_DIK(DIK_F3, SDL_SCANCODE_F3),
    X2_DIK(DIK_F4, SDL_SCANCODE_F4),
    X2_DIK(DIK_F5, SDL_SCANCODE_F5),
    X2_DIK(DIK_F6, SDL_SCANCODE_F6),
    X2_DIK(DIK_F7, SDL_SCANCODE_F7),
    X2_DIK(DIK_F8, SDL_SCANCODE_F8),
    X2_DIK(DIK_F9, SDL_SCANCODE_F9),
    X2_DIK(DIK_F10, SDL_SCANCODE_F10),
    X2_DIK(DIK_NUMLOCK, SDL_SCANCODE_NUMLOCKCLEAR),
    X2_DIK(DIK_SCROLL, SDL_SCANCODE_SCROLLLOCK),
    X2_DIK(DIK_NUMPAD7, SDL_SCANCODE_KP_7),
    X2_DIK(DIK_NUMPAD8, SDL_SCANCODE_KP_8),
    X2_DIK(DIK_NUMPAD9, SDL_SCANCODE_KP_9),
    X2_DIK(DIK_SUBTRACT, SDL_SCANCODE_KP_MINUS),
    X2_DIK(DIK_NUMPAD4, SDL_SCANCODE_KP_4),
    X2_DIK(DIK_NUMPAD5, SDL_SCANCODE_KP_5),
    X2_DIK(DIK_NUMPAD6, SDL_SCANCODE_KP_6),
    X2_DIK(DIK_ADD, SDL_SCANCODE_KP_PLUS),
    X2_DIK(DIK_NUMPAD1, SDL_SCANCODE_KP_1),
    X2_DIK(DIK_NUMPAD2, SDL_SCANCODE_KP_2),
    X2_DIK(DIK_NUMPAD3, SDL_SCANCODE_KP_3),
    X2_DIK(DIK_NUMPAD0, SDL_SCANCODE_KP_0),
    X2_DIK(DIK_DECIMAL, SDL_SCANCODE_KP_PERIOD),
    X2_DIK(DIK_OEM_102, SDL_SCANCODE_NONUSBACKSLASH),
    X2_DIK(DIK_F11, SDL_SCANCODE_F11),
    X2_DIK(DIK_F12, SDL_SCANCODE_F12),
    X2_DIK(DIK_F13, SDL_SCANCODE_F13),
    X2_DIK(DIK_F14, SDL_SCANCODE_F14),
    X2_DIK(DIK_F15, SDL_SCANCODE_F15),
    X2_DIK(DIK_KANA, SDL_SCANCODE_INTERNATIONAL2),
    X2_DIK(DIK_ABNT_C1, SDL_SCANCODE_INTERNATIONAL1),
    X2_DIK(DIK_CONVERT, SDL_SCANCODE_INTERNATIONAL4),
    X2_DIK(DIK_NOCONVERT, SDL_SCANCODE_INTERNATIONAL5),
    X2_DIK(DIK_YEN, SDL_SCANCODE_INTERNATIONAL3),
    X2_DIK(DIK_ABNT_C2, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_NUMPADEQUALS, SDL_SCANCODE_KP_EQUALS),
    X2_DIK(DIK_PREVTRACK, SDL_SCANCODE_MEDIA_PREVIOUS_TRACK),
    X2_DIK(DIK_AT, SDL_SCANCODE_UNKNOWN),         // NEC PC-98 keys, which SDL does not know
    X2_DIK(DIK_COLON, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_UNDERLINE, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_KANJI, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_STOP, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_AX, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_UNLABELED, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_NEXTTRACK, SDL_SCANCODE_MEDIA_NEXT_TRACK),
    X2_DIK(DIK_NUMPADENTER, SDL_SCANCODE_KP_ENTER),
    X2_DIK(DIK_RCONTROL, SDL_SCANCODE_RCTRL),
    X2_DIK(DIK_MUTE, SDL_SCANCODE_MUTE),
    X2_DIK(DIK_CALCULATOR, SDL_SCANCODE_UNKNOWN),  // SDL3 dropped the application-launch scancodes
    X2_DIK(DIK_PLAYPAUSE, SDL_SCANCODE_MEDIA_PLAY_PAUSE),
    X2_DIK(DIK_MEDIASTOP, SDL_SCANCODE_MEDIA_STOP),
    X2_DIK(DIK_VOLUMEDOWN, SDL_SCANCODE_VOLUMEDOWN),
    X2_DIK(DIK_VOLUMEUP, SDL_SCANCODE_VOLUMEUP),
    X2_DIK(DIK_WEBHOME, SDL_SCANCODE_AC_HOME),
    X2_DIK(DIK_NUMPADCOMMA, SDL_SCANCODE_KP_COMMA),
    X2_DIK(DIK_DIVIDE, SDL_SCANCODE_KP_DIVIDE),
    X2_DIK(DIK_SYSRQ, SDL_SCANCODE_PRINTSCREEN),
    X2_DIK(DIK_RMENU, SDL_SCANCODE_RALT),
    X2_DIK(DIK_PAUSE, SDL_SCANCODE_PAUSE),
    X2_DIK(DIK_HOME, SDL_SCANCODE_HOME),
    X2_DIK(DIK_UP, SDL_SCANCODE_UP),
    X2_DIK(DIK_PRIOR, SDL_SCANCODE_PAGEUP),
    X2_DIK(DIK_LEFT, SDL_SCANCODE_LEFT),
    X2_DIK(DIK_RIGHT, SDL_SCANCODE_RIGHT),
    X2_DIK(DIK_END, SDL_SCANCODE_END),
    X2_DIK(DIK_DOWN, SDL_SCANCODE_DOWN),
    X2_DIK(DIK_NEXT, SDL_SCANCODE_PAGEDOWN),
    X2_DIK(DIK_INSERT, SDL_SCANCODE_INSERT),
    X2_DIK(DIK_DELETE, SDL_SCANCODE_DELETE),
    X2_DIK(DIK_LWIN, SDL_SCANCODE_LGUI),
    X2_DIK(DIK_RWIN, SDL_SCANCODE_RGUI),
    X2_DIK(DIK_APPS, SDL_SCANCODE_APPLICATION),
    X2_DIK(DIK_POWER, SDL_SCANCODE_POWER),
    X2_DIK(DIK_SLEEP, SDL_SCANCODE_SLEEP),
    X2_DIK(DIK_WAKE, SDL_SCANCODE_WAKE),
    X2_DIK(DIK_WEBSEARCH, SDL_SCANCODE_AC_SEARCH),
    X2_DIK(DIK_WEBFAVORITES, SDL_SCANCODE_AC_BOOKMARKS),
    X2_DIK(DIK_WEBREFRESH, SDL_SCANCODE_AC_REFRESH),
    X2_DIK(DIK_WEBSTOP, SDL_SCANCODE_AC_STOP),
    X2_DIK(DIK_WEBFORWARD, SDL_SCANCODE_AC_FORWARD),
    X2_DIK(DIK_WEBBACK, SDL_SCANCODE_AC_BACK),
    X2_DIK(DIK_MYCOMPUTER, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_MAIL, SDL_SCANCODE_UNKNOWN),
    X2_DIK(DIK_MEDIASELECT, SDL_SCANCODE_MEDIA_SELECT),

    // Alternate names: the same codes, listed so that every DIK_* spelling the client uses can be checked.
    X2_DIK(DIK_BACKSPACE, SDL_SCANCODE_BACKSPACE),
    X2_DIK(DIK_NUMPADSTAR, SDL_SCANCODE_KP_MULTIPLY),
    X2_DIK(DIK_LALT, SDL_SCANCODE_LALT),
    X2_DIK(DIK_CAPSLOCK, SDL_SCANCODE_CAPSLOCK),
    X2_DIK(DIK_NUMPADMINUS, SDL_SCANCODE_KP_MINUS),
    X2_DIK(DIK_NUMPADPLUS, SDL_SCANCODE_KP_PLUS),
    X2_DIK(DIK_NUMPADPERIOD, SDL_SCANCODE_KP_PERIOD),
    X2_DIK(DIK_NUMPADSLASH, SDL_SCANCODE_KP_DIVIDE),
    X2_DIK(DIK_RALT, SDL_SCANCODE_RALT),
    X2_DIK(DIK_UPARROW, SDL_SCANCODE_UP),
    X2_DIK(DIK_PGUP, SDL_SCANCODE_PAGEUP),
    X2_DIK(DIK_LEFTARROW, SDL_SCANCODE_LEFT),
    X2_DIK(DIK_RIGHTARROW, SDL_SCANCODE_RIGHT),
    X2_DIK(DIK_DOWNARROW, SDL_SCANCODE_DOWN),
    X2_DIK(DIK_PGDN, SDL_SCANCODE_PAGEDOWN),
    X2_DIK(DIK_CIRCUMFLEX, SDL_SCANCODE_MEDIA_PREVIOUS_TRACK),
};

#undef X2_DIK

struct KeyTables {
    std::array<BYTE, SDL_SCANCODE_COUNT> dikOfScancode{};
    std::array<SDL_Scancode, 256> scancodeOfDik{};

    KeyTables()
    {
        scancodeOfDik.fill(SDL_SCANCODE_UNKNOWN);
        for (const DirectInputKeyInfo& key : kDirectInputKeys) {
            if (key.scancode == SDL_SCANCODE_UNKNOWN) {
                continue;
            }
            if (dikOfScancode[key.scancode] == 0) {
                dikOfScancode[key.scancode] = key.dik;
            }
            if (scancodeOfDik[key.dik] == SDL_SCANCODE_UNKNOWN) {
                scancodeOfDik[key.dik] = key.scancode;
            }
        }
        // The key left of Enter on ISO keyboards: Windows reports it with the backslash's scan code (0x2B).
        dikOfScancode[SDL_SCANCODE_NONUSHASH] = DIK_BACKSLASH;
    }
};

const KeyTables& Tables()
{
    static const KeyTables tables;
    return tables;
}

constexpr float kWheelDelta = 120.0f;  // WHEEL_DELTA: DirectInput's lZ moves 120 per notch

// Takes the whole part of an accumulated delta, leaving the fraction for the next read.
LONG TakeWhole(float& accumulated)
{
    const float whole = std::trunc(accumulated);
    accumulated -= whole;
    return static_cast<LONG>(whole);
}

int ButtonIndex(Uint8 button)
{
    switch (button) {
    case SDL_BUTTON_LEFT: return 0;    // MOUSE_LBUTTON
    case SDL_BUTTON_RIGHT: return 1;   // MOUSE_RBUTTON
    case SDL_BUTTON_MIDDLE: return 2;  // MOUSE_MBUTTON
    case SDL_BUTTON_X1: return 3;
    default: return -1;
    }
}

bool ForWindow(SDL_WindowID eventWindow, const TranslateContext& context)
{
    return context.window == 0 || eventWindow == context.window;
}

// DIPROP_RANGE -1024..1024 and DIPROP_DEADZONE 128 (of 10000) on X and Y, as CKTDIDevice::Create set them.
LONG StickAxis(Sint16 value)
{
    constexpr float kDeadZone = 128.0f / 10000.0f;
    const float v = std::clamp(static_cast<float>(value) / 32767.0f, -1.0f, 1.0f);
    const float magnitude = std::fabs(v);
    if (magnitude <= kDeadZone) {
        return 0;
    }
    const float scaled = (magnitude - kDeadZone) / (1.0f - kDeadZone) * 1024.0f;
    return static_cast<LONG>(std::lround(v < 0.0f ? -scaled : scaled));
}

// DirectInput's default range, 0..65535, for the axes CKTDIDevice did not set.
LONG FullAxis(Sint16 value)
{
    return static_cast<LONG>(value) + 32768;
}

DWORD Pov(bool up, bool right, bool down, bool left)
{
    if (up && down) {
        up = down = false;
    }
    if (left && right) {
        left = right = false;
    }
    if (up) {
        return right ? 4500 : left ? 31500 : 0;
    }
    if (down) {
        return right ? 13500 : left ? 22500 : 18000;
    }
    if (right) {
        return 9000;
    }
    if (left) {
        return 27000;
    }
    return 0xFFFFFFFF;
}

BYTE Button(bool down)
{
    return down ? 0x80 : 0;
}

}  // namespace

std::span<const DirectInputKeyInfo> DirectInputKeys()
{
    return kDirectInputKeys;
}

BYTE DirectInputKeyFromScancode(SDL_Scancode scancode)
{
    if (scancode < 0 || scancode >= SDL_SCANCODE_COUNT) {
        return 0;
    }
    return Tables().dikOfScancode[scancode];
}

SDL_Scancode ScancodeFromDirectInputKey(BYTE dik)
{
    return Tables().scancodeOfDik[dik];
}

void InputState::Handle(const SDL_Event& event, const TranslateContext& context)
{
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        if (ForWindow(event.key.windowID, context)) {
            if (const BYTE dik = DirectInputKeyFromScancode(event.key.scancode)) {
                m_keys[dik] = Button(event.type == SDL_EVENT_KEY_DOWN);
            }
        }
        break;
    case SDL_EVENT_MOUSE_MOTION:
        if (ForWindow(event.motion.windowID, context)) {
            m_cursor.x = static_cast<LONG>(std::lround(event.motion.x * context.pixelDensity));
            m_cursor.y = static_cast<LONG>(std::lround(event.motion.y * context.pixelDensity));
            m_deltaX += event.motion.xrel * context.pixelDensity;
            m_deltaY += event.motion.yrel * context.pixelDensity;
        }
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (ForWindow(event.button.windowID, context)) {
            m_cursor.x = static_cast<LONG>(std::lround(event.button.x * context.pixelDensity));
            m_cursor.y = static_cast<LONG>(std::lround(event.button.y * context.pixelDensity));
            if (const int index = ButtonIndex(event.button.button); index >= 0) {
                m_buttons[index] = Button(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            }
        }
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        if (ForWindow(event.wheel.windowID, context)) {
            m_wheel += event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -event.wheel.y : event.wheel.y;
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        if (ForWindow(event.window.windowID, context)) {
            SetFocused(event.type == SDL_EVENT_WINDOW_FOCUS_GAINED);
        }
        break;
    default:
        break;
    }
}

void InputState::SetFocused(bool focused)
{
    if (!focused) {
        Reset();
    }
    m_focused = focused;
}

DIMOUSESTATE InputState::ReadMouse()
{
    DIMOUSESTATE state = {};
    state.lX = TakeWhole(m_deltaX);
    state.lY = TakeWhole(m_deltaY);
    float wheel = m_wheel * kWheelDelta;
    state.lZ = TakeWhole(wheel);
    m_wheel = wheel / kWheelDelta;
    std::copy(std::begin(m_buttons), std::end(m_buttons), std::begin(state.rgbButtons));
    return state;
}

void InputState::Reset()
{
    std::fill(std::begin(m_keys), std::end(m_keys), BYTE(0));
    std::fill(std::begin(m_buttons), std::end(m_buttons), BYTE(0));
    m_deltaX = m_deltaY = m_wheel = 0.0f;
}

JoystickReader::~JoystickReader()
{
    Close();
}

void JoystickReader::Close()
{
    if (m_gamepad) {
        SDL_CloseGamepad(m_gamepad);  // also closes its joystick
    } else if (m_joystick) {
        SDL_CloseJoystick(m_joystick);
    }
    m_gamepad = nullptr;
    m_joystick = nullptr;
}

bool JoystickReader::Open()
{
    if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0 || !SDL_HasJoystick()) {
        return false;
    }
    int count = 0;
    SDL_JoystickID* ids = SDL_GetJoysticks(&count);
    if (!ids) {
        return false;
    }
    const SDL_JoystickID id = count > 0 ? ids[0] : 0;
    SDL_free(ids);
    if (id == 0) {
        return false;
    }
    if (SDL_WasInit(SDL_INIT_GAMEPAD) != 0 && SDL_IsGamepad(id)) {
        m_gamepad = SDL_OpenGamepad(id);
        m_joystick = m_gamepad ? SDL_GetGamepadJoystick(m_gamepad) : nullptr;
    }
    if (!m_joystick) {
        m_gamepad = nullptr;
        m_joystick = SDL_OpenJoystick(id);
    }
    return m_joystick != nullptr;
}

DIJOYSTATE JoystickReader::Neutral()
{
    DIJOYSTATE state = {};
    state.lZ = 32767;
    std::fill(std::begin(state.rgdwPOV), std::end(state.rgdwPOV), 0xFFFFFFFF);
    return state;
}

bool JoystickReader::Read(DIJOYSTATE& state)
{
    state = Neutral();
    if (m_joystick && !SDL_JoystickConnected(m_joystick)) {
        Close();
    }
    if (!m_joystick && !Open()) {
        return false;
    }

    if (m_gamepad) {
        SDL_Gamepad* pad = m_gamepad;
        state.lX = StickAxis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTX));
        state.lY = StickAxis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTY));
        const LONG left = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);    // 0..32767
        const LONG right = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
        state.lZ = std::clamp<LONG>(32767 + left - right, 0, 65535);
        state.lRx = FullAxis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTX));
        state.lRy = FullAxis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTY));
        state.rgdwPOV[0] = Pov(SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_UP),
                               SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT),
                               SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_DOWN),
                               SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_LEFT));
        constexpr SDL_GamepadButton kButtons[] = {
            SDL_GAMEPAD_BUTTON_SOUTH,          SDL_GAMEPAD_BUTTON_EAST,           SDL_GAMEPAD_BUTTON_WEST,
            SDL_GAMEPAD_BUTTON_NORTH,          SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,  SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
            SDL_GAMEPAD_BUTTON_BACK,           SDL_GAMEPAD_BUTTON_START,          SDL_GAMEPAD_BUTTON_LEFT_STICK,
            SDL_GAMEPAD_BUTTON_RIGHT_STICK,
        };
        for (size_t i = 0; i < std::size(kButtons); ++i) {
            state.rgbButtons[i] = Button(SDL_GetGamepadButton(pad, kButtons[i]));
        }
        return true;
    }

    SDL_Joystick* joystick = m_joystick;
    const int axes = SDL_GetNumJoystickAxes(joystick);
    LONG* const fullAxes[] = {&state.lZ, &state.lRx, &state.lRy, &state.lRz, &state.rglSlider[0], &state.rglSlider[1]};
    for (int axis = 0; axis < axes && axis < 2 + static_cast<int>(std::size(fullAxes)); ++axis) {
        const Sint16 value = SDL_GetJoystickAxis(joystick, axis);
        if (axis == 0) {
            state.lX = StickAxis(value);
        } else if (axis == 1) {
            state.lY = StickAxis(value);
        } else {
            *fullAxes[axis - 2] = FullAxis(value);
        }
    }
    const int hats = std::min(SDL_GetNumJoystickHats(joystick), static_cast<int>(std::size(state.rgdwPOV)));
    for (int hat = 0; hat < hats; ++hat) {
        const Uint8 value = SDL_GetJoystickHat(joystick, hat);
        state.rgdwPOV[hat] = Pov((value & SDL_HAT_UP) != 0, (value & SDL_HAT_RIGHT) != 0, (value & SDL_HAT_DOWN) != 0,
                                 (value & SDL_HAT_LEFT) != 0);
    }
    const int buttons = std::min(SDL_GetNumJoystickButtons(joystick), static_cast<int>(std::size(state.rgbButtons)));
    for (int button = 0; button < buttons; ++button) {
        state.rgbButtons[button] = Button(SDL_GetJoystickButton(joystick, button));
    }
    return true;
}

}  // namespace x2::dxut
