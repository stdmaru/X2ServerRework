#pragma once

// SDL3 keyboard, mouse and joystick state in DirectInput's shape, for KTDXLIB's CKTDIDevice (the original read DirectInput
// devices in immediate mode, foreground and non-exclusive). The DXUT main loop feeds every SDL event to the InputState
// returned by DXUTGetInputState(); CKTDIDevice::Read takes a snapshot each frame. See docs/porting/client-input.md.

#include "DirectInputKeys.h"
#include "SdlMessages.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>

#include <span>

namespace x2::dxut {

// One DIK_* of dinput.h, alternate names included, and the SDL scancode of that key (SDL_SCANCODE_UNKNOWN for the few
// keys SDL has no scancode for).
struct DirectInputKeyInfo {
    const char* name;
    BYTE dik;
    SDL_Scancode scancode;
};
std::span<const DirectInputKeyInfo> DirectInputKeys();

BYTE DirectInputKeyFromScancode(SDL_Scancode scancode);  // 0 when the key has no DIK_*
SDL_Scancode ScancodeFromDirectInputKey(BYTE dik);       // SDL_SCANCODE_UNKNOWN when none

class InputState {
public:
    // Updates the state from one event. Keyboard and mouse events of other windows than context.window are ignored.
    void Handle(const SDL_Event& event, const TranslateContext& context);

    // Without focus, DirectInput's foreground devices are not acquired: CKTDIDevice::Read fails until focus returns.
    // Losing focus releases every key and button.
    void SetFocused(bool focused);
    bool Focused() const { return m_focused; }

    // 256 bytes indexed by DIK_*, 0x80 when down (GetDeviceState of c_dfDIKeyboard).
    const BYTE* Keys() const { return m_keys; }

    // Mouse motion and wheel since the previous call, and the buttons held now (GetDeviceState of c_dfDIMouse).
    DIMOUSESTATE ReadMouse();

    // Cursor in client pixels (GetCursorPos + ScreenToClient).
    POINT Cursor() const { return m_cursor; }

    void Reset();

private:
    BYTE m_keys[256] = {};
    BYTE m_buttons[4] = {};
    float m_deltaX = 0.0f;  // window coordinates x pixel density, kept fractional between reads
    float m_deltaY = 0.0f;
    float m_wheel = 0.0f;   // notches
    POINT m_cursor = {0, 0};
    bool m_focused = true;
};

// The first SDL joystick in DirectInput's c_dfDIJoystick layout, with the ranges CKTDIDevice::Create set: X and Y in
// -1024..1024 with a 1.28% dead zone, other axes in 0..65535. A gamepad is reported the way DirectInput reported an
// XInput controller: left stick X/Y, triggers together on Z (left above 32767, right below), right stick Rx/Ry, the
// d-pad as POV 0 and buttons A, B, X, Y, LB, RB, Back, Start, left stick, right stick. Other joysticks report their axes,
// first hat and buttons in order. Opens the first joystick when none is open, so one plugged in later is picked up.
class JoystickReader {
public:
    JoystickReader() = default;
    JoystickReader(const JoystickReader&) = delete;
    JoystickReader& operator=(const JoystickReader&) = delete;
    ~JoystickReader();

    // false (and `state` neutral) without a joystick or with the joystick subsystem off.
    bool Read(DIJOYSTATE& state);
    bool IsOpen() const { return m_joystick != nullptr; }
    void Close();

    // Centered axes (Z included, so neither trigger reads as pressed), centered POVs, no button.
    static DIJOYSTATE Neutral();

private:
    bool Open();

    SDL_Joystick* m_joystick = nullptr;
    SDL_Gamepad* m_gamepad = nullptr;
};

}  // namespace x2::dxut
