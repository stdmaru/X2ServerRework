// KTDXLIB's input layer (CKTDIManager, CKTDIDevice) on the DXUT replacement's SDL state: key locks, double presses,
// the action map, the mouse, focus and a virtual gamepad, the way X2Lib reads them each frame.

#include "KTDIManager.h"

#include "DXUT.h"
#include "SdlInput.h"

#include <SDL3/SDL.h>

#include <doctest/doctest.h>

#include <map>

namespace {

constexpr float kFrame = 1.0f / 60.0f;

void Key(SDL_Scancode scancode, bool down)
{
    SDL_Event event{};
    event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    event.key.scancode = scancode;
    event.key.down = down;
    DXUTGetInputState().Handle(event, x2::dxut::TranslateContext{});
}

// A manager set up as CKTDXApp does, on a clean, focused input state.
struct Manager {
    CKTDIManager manager{nullptr, nullptr};

    Manager()
    {
        DXUTGetInputState().Reset();
        DXUTGetInputState().SetFocused(true);
        manager.CreateKeyboard();
        manager.CreateMouse(TRUE);
        manager.CreateJoystic();
        manager.SetGamePadDefaultMap();
        manager.SetDefaultMap();
    }
    ~Manager() { DXUTGetInputState().Reset(); }

    void Frame(float elapsed = kFrame) { manager.OnFrameMove(0.0, elapsed); }
    CKTDIDevice& Keyboard() { return *manager.Getkeyboard(); }
};

}  // namespace

TEST_CASE("GetKeyState reports a press once until the key is released")
{
    Manager m;
    Key(SDL_SCANCODE_F9, true);
    m.Frame();
    CHECK(m.Keyboard().GetKeyState(DIK_F9) == TRUE);
    CHECK(m.Keyboard().GetKeyState(DIK_F9) == FALSE);  // locked
    CHECK(m.Keyboard().GetPureKeyState(DIK_F9) == TRUE);
    CHECK(m.Keyboard().GetNumPureKeyPresses() == 1);
    CHECK(m.Keyboard().GetNumKeyPresses() == 0);
    m.Frame();
    CHECK(m.Keyboard().GetKeyState(DIK_F9) == FALSE);  // still held

    Key(SDL_SCANCODE_F9, false);
    m.Frame();
    CHECK(m.Keyboard().GetPureKeyState(DIK_F9) == FALSE);
    Key(SDL_SCANCODE_F9, true);
    m.Frame();
    CHECK(m.Keyboard().GetKeyState(DIK_F9) == TRUE);

    m.Keyboard().SetLock(DIK_F9, FALSE);
    CHECK(m.Keyboard().GetKeyState(DIK_F9) == TRUE);
}

TEST_CASE("double presses")
{
    Manager m;
    CKTDIDevice& keyboard = m.Keyboard();

    Key(SDL_SCANCODE_RIGHT, true);
    m.Frame();
    CHECK(keyboard.GetDoubleKeyState(DIK_RIGHT) == FALSE);
    Key(SDL_SCANCODE_RIGHT, false);
    m.Frame();
    CHECK(keyboard.GetDoubleKeyState(DIK_RIGHT) == FALSE);
    Key(SDL_SCANCODE_RIGHT, true);
    m.Frame();
    CHECK(keyboard.GetDoubleKeyState(DIK_RIGHT) == TRUE);
    m.Frame();
    CHECK(keyboard.GetDoubleKeyState(DIK_RIGHT) == FALSE);  // reported once

    // Too slow: the second press comes after DOUBLE_INPUT_GAP (0.4 s).
    Key(SDL_SCANCODE_RIGHT, false);
    m.Frame();
    keyboard.SetDoubleKeyReset(DIK_RIGHT);
    Key(SDL_SCANCODE_RIGHT, true);
    m.Frame();
    CHECK(keyboard.GetDoubleKeyState(DIK_RIGHT) == FALSE);
    Key(SDL_SCANCODE_RIGHT, false);
    m.Frame();
    CHECK(keyboard.GetDoubleKeyState(DIK_RIGHT) == FALSE);
    m.Frame(0.5f);
    Key(SDL_SCANCODE_RIGHT, true);
    m.Frame();
    CHECK(keyboard.GetDoubleKeyState(DIK_RIGHT) == FALSE);
}

TEST_CASE("actions through the default key map")
{
    Manager m;
    CHECK(m.Keyboard().GetActionKey(GA_ATTACK_FAST) == DIK_Z);
    CHECK(m.Keyboard().GetActionKey(GA_AWAKE) == DIK_LCONTROL);

    Key(SDL_SCANCODE_Z, true);
    m.Frame();
    CHECK(m.manager.GetActionState(GA_ATTACK_FAST) == TRUE);
    CHECK(m.manager.GetActionState(GA_ATTACK_FAST) == FALSE);
    CHECK(m.manager.GetPureActionState(GA_ATTACK_FAST) == TRUE);
    CHECK(m.manager.GetPureActionState(GA_ATTACK_STRONG) == FALSE);
    m.Frame();
    CHECK(m.manager.GetActionState(GA_ATTACK_FAST) == FALSE);
    Key(SDL_SCANCODE_Z, false);
    m.Frame();
    CHECK(m.manager.GetPureActionState(GA_ATTACK_FAST) == FALSE);

    // A key moved to another action leaves the first one without a key.
    m.Keyboard().SetAction(GA_SKILL1, DIK_Z);
    CHECK(m.Keyboard().GetActionKey(GA_ATTACK_FAST) == 0);
    Key(SDL_SCANCODE_Z, true);
    m.Frame();
    CHECK(m.manager.GetPureActionState(GA_SKILL1) == TRUE);
    CHECK(m.manager.GetPureActionState(GA_ATTACK_FAST) == FALSE);

    std::map<short, short> keys;
    m.Keyboard().GetActionKeys(keys);
    CHECK(keys[GA_SKILL1] == DIK_Z);
    keys[GA_SKILL1] = DIK_A;
    m.Keyboard().SetActionKeys(keys);
    CHECK(m.Keyboard().GetActionKey(GA_SKILL1) == DIK_A);

    unsigned char pressed = 0;
    m.Keyboard().SetLock(DIK_Z, FALSE);
    CHECK(m.manager.ListenDeviceInput(pressed, KEYBOARD));
    CHECK(pressed == DIK_Z);
}

TEST_CASE("disabled manager and lost focus")
{
    Manager m;
    Key(SDL_SCANCODE_UP, true);
    m.Frame();
    CHECK(m.manager.GetPureActionState(GA_UP) == TRUE);

    // Without focus the devices are not read (DirectInput could not acquire them) until the app deactivates them.
    DXUTGetInputState().SetFocused(false);
    m.Frame();
    CHECK(m.manager.GetPureActionState(GA_UP) == TRUE);
    m.manager.DeactivateApp();
    CHECK_FALSE(m.manager.IsActivated());
    m.Frame();
    CHECK(m.manager.GetPureActionState(GA_UP) == FALSE);

    DXUTGetInputState().SetFocused(true);
    m.manager.ActivateApp();
    Key(SDL_SCANCODE_UP, true);
    m.manager.SetEnable(false);
    m.Frame();
    CHECK(m.manager.GetPureActionState(GA_UP) == FALSE);
    m.manager.SetEnable(true);
    m.Frame();
    CHECK(m.manager.GetPureActionState(GA_UP) == TRUE);
}

TEST_CASE("mouse position, motion, buttons and wheel")
{
    Manager m;
    CKTDIDevice& mouse = *m.manager.GetMouse();
    x2::dxut::InputState& input = DXUTGetInputState();
    const x2::dxut::TranslateContext context;

    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_MOTION;
    event.motion.x = 300.0f;
    event.motion.y = 200.0f;
    event.motion.xrel = 5.0f;
    event.motion.yrel = -3.0f;
    input.Handle(event, context);
    event = {};
    event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    event.button.button = SDL_BUTTON_LEFT;
    event.button.down = true;
    event.button.x = 300.0f;
    event.button.y = 200.0f;
    input.Handle(event, context);
    event = {};
    event.type = SDL_EVENT_MOUSE_WHEEL;
    event.wheel.y = -1.0f;
    input.Handle(event, context);
    m.Frame();

    CHECK(mouse.GetXPos() == 300);
    CHECK(mouse.GetYPos() == 200);
    CHECK(mouse.GetXDelta() == 5);
    CHECK(mouse.GetYDelta() == -3);
    CHECK(mouse.GetZPos() == -120);
    CHECK(mouse.GetMouseDeltaPosition().x == 300);  // from (0, 0) before the first frame
    CHECK(mouse.GetButtonState(MOUSE_LBUTTON) == TRUE);
    CHECK(mouse.GetButtonState(MOUSE_LBUTTON) == FALSE);
    CHECK(mouse.GetPureButtonState(MOUSE_LBUTTON) == TRUE);
    CHECK(mouse.GetPureButtonState(MOUSE_RBUTTON) == FALSE);

    m.Frame();
    CHECK(mouse.GetXDelta() == 0);
    CHECK(mouse.GetZPos() == 0);
    CHECK(mouse.GetMouseDeltaPosition().x == 0);
}

TEST_CASE("gamepad actions through the default pad map")
{
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "0");
    REQUIRE(SDL_InitSubSystem(SDL_INIT_GAMEPAD));
    SDL_VirtualJoystickDesc desc;
    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.naxes = SDL_GAMEPAD_AXIS_COUNT;
    desc.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
    desc.name = "x2 test pad";
    const SDL_JoystickID id = SDL_AttachVirtualJoystick(&desc);
    REQUIRE(id != 0);

    {
        Manager m;
        m.manager.SetEnableJoyStick(true);
        m.Frame();  // opens the pad
        SDL_Joystick* joystick = SDL_GetJoystickFromID(id);
        REQUIRE(joystick != nullptr);
        CHECK(m.manager.GetPureActionState(GA_UP) == FALSE);
        CHECK(m.manager.GetPureActionState(GA_ATTACK_FAST) == FALSE);

        SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_WEST, true);  // button 2: JB_BUTTON2
        SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_DPAD_LEFT, true);
        SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFTY, -32768);
        SDL_UpdateJoysticks();
        m.Frame();
        CHECK(m.manager.GetActionState(GA_ATTACK_FAST) == TRUE);
        CHECK(m.manager.GetPureActionState(GA_LEFT) == TRUE);
        CHECK(m.manager.GetPureActionState(GA_UP) == TRUE);
        CHECK(m.manager.GetPureActionState(GA_RIGHT) == FALSE);
        CHECK(m.manager.GetJoystic()->GetPOV() == 27000);

        unsigned char pressed = 0;
        CHECK(m.manager.ListenDeviceInput(pressed, JOYSTICK));
        CHECK(pressed == DIJOFS_BUTTON(2));

        SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_WEST, false);
        SDL_UpdateJoysticks();
        m.Frame();
        CHECK(m.manager.GetPureActionState(GA_ATTACK_FAST) == FALSE);

        // Turned off in the options: the pad no longer drives actions.
        m.manager.SetEnableJoyStick(false);
        m.Frame();
        CHECK(m.manager.GetPureActionState(GA_LEFT) == FALSE);
    }

    SDL_DetachVirtualJoystick(id);
    SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}
