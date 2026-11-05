// The DXUT replacement's DirectInput view (src/client/DXUT/SdlInput.*): the DIK_* table, the keyboard and mouse state
// fed by SDL events, and joysticks read through SDL (virtual devices, no hardware).

#include "SdlInput.h"

#include <SDL3/SDL.h>

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>
#include <string>

namespace {

using x2::dxut::DirectInputKeyFromScancode;
using x2::dxut::DirectInputKeys;
using x2::dxut::InputState;
using x2::dxut::JoystickReader;
using x2::dxut::ScancodeFromDirectInputKey;
using x2::dxut::TranslateContext;

bool IdentifierChar(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

// Every DIK_* spelled in the client's sources (X2, X2Lib, KTDXLIB).
std::set<std::string> DirectInputKeysInSources()
{
    std::set<std::string> names;
    const std::filesystem::path root = X2_CLIENT_SOURCE_DIR;
    for (const char* layer : {"X2", "X2Lib", "KTDXLIB"}) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root / layer)) {
            const std::filesystem::path& path = entry.path();
            if (!entry.is_regular_file() || (path.extension() != ".cpp" && path.extension() != ".h")) {
                continue;
            }
            std::ifstream file(path, std::ios::binary);
            const std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            for (size_t at = text.find("DIK_"); at != std::string::npos; at = text.find("DIK_", at + 4)) {
                if (at > 0 && IdentifierChar(text[at - 1])) {
                    continue;
                }
                size_t end = at + 4;
                while (end < text.size() && IdentifierChar(text[end])) {
                    ++end;
                }
                if (end > at + 4) {  // not "DIK_*" in a comment
                    names.insert(text.substr(at, end - at));
                }
            }
        }
    }
    return names;
}

SDL_Event KeyEvent(SDL_Scancode scancode, bool down, SDL_WindowID window = 0)
{
    SDL_Event event{};
    event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    event.key.windowID = window;
    event.key.scancode = scancode;
    event.key.down = down;
    return event;
}

SDL_Event MotionEvent(float x, float y, float dx, float dy)
{
    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_MOTION;
    event.motion.x = x;
    event.motion.y = y;
    event.motion.xrel = dx;
    event.motion.yrel = dy;
    return event;
}

SDL_Event ButtonEvent(Uint8 button, bool down)
{
    SDL_Event event{};
    event.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
    event.button.button = button;
    event.button.down = down;
    return event;
}

SDL_Event WheelEvent(float y, SDL_MouseWheelDirection direction = SDL_MOUSEWHEEL_NORMAL)
{
    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_WHEEL;
    event.wheel.y = y;
    event.wheel.direction = direction;
    return event;
}

}  // namespace

// The values of dinput.h that differ from "set-1 scan code | 0x80 when extended".
static_assert(DIK_NUMLOCK == 0x45 && DIK_PAUSE == 0xC5 && DIK_SYSRQ == 0xB7);
static_assert(DIK_NUMPADENTER == 0x9C && DIK_RCONTROL == 0x9D && DIK_RMENU == 0xB8 && DIK_DIVIDE == 0xB5);
static_assert(DIK_LWIN == 0xDB && DIK_RWIN == 0xDC && DIK_APPS == 0xDD);
static_assert(DIK_BACKSPACE == DIK_BACK && DIK_PGDN == DIK_NEXT && DIK_LALT == DIK_LMENU);
static_assert(DIJOFS_BUTTON0 == 48 && DIJOFS_BUTTON(31) == 79 && DIJOFS_Z == 8 && DIJOFS_POV(0) == 32);

TEST_CASE("every DIK_ the client uses has an SDL scancode")
{
    std::set<std::string> table;
    for (const auto& key : DirectInputKeys()) {
        CHECK_MESSAGE(table.insert(key.name).second, key.name);
    }
    const std::set<std::string> used = DirectInputKeysInSources();
    CHECK(used.size() >= 100);  // 102 names on 2026-11-05
    for (const std::string& name : used) {
        INFO(name);
        bool found = false;
        for (const auto& key : DirectInputKeys()) {
            if (name == key.name) {
                found = true;
                CHECK(key.scancode != SDL_SCANCODE_UNKNOWN);
            }
        }
        CHECK(found);
    }
}

TEST_CASE("DIK_ <-> SDL scancode round trip")
{
    for (const auto& key : DirectInputKeys()) {
        INFO(key.name);
        if (key.scancode == SDL_SCANCODE_UNKNOWN) {
            CHECK(ScancodeFromDirectInputKey(key.dik) == SDL_SCANCODE_UNKNOWN);
            continue;
        }
        CHECK(DirectInputKeyFromScancode(key.scancode) == key.dik);
        CHECK(ScancodeFromDirectInputKey(key.dik) == key.scancode);
    }
    CHECK(DirectInputKeyFromScancode(SDL_SCANCODE_NONUSHASH) == DIK_BACKSLASH);
    CHECK(DirectInputKeyFromScancode(SDL_SCANCODE_UNKNOWN) == 0);
    CHECK(DirectInputKeyFromScancode(SDL_SCANCODE_F24) == 0);
    CHECK(ScancodeFromDirectInputKey(0) == SDL_SCANCODE_UNKNOWN);
}

TEST_CASE("DIK_ matches the scan codes of WM_KEYDOWN")
{
    // DirectInput's key index is the set-1 scan code, with 0x80 for E0 keys: the same scan codes the message translator
    // puts in lParam. Pause and Num Lock swap the extended bit (Pause is E1 1D 45), Print Screen is E0 37.
    for (const auto& key : DirectInputKeys()) {
        const auto vk = x2::dxut::ToVirtualKey(key.scancode, SDLK_UNKNOWN, SDL_KMOD_NUM);
        if (key.scancode == SDL_SCANCODE_UNKNOWN || vk.scanCode == 0 || key.dik == DIK_PAUSE || key.dik == DIK_NUMLOCK) {
            continue;
        }
        INFO(key.name);
        CHECK(key.dik == (vk.scanCode | (vk.extended ? 0x80u : 0u)));
    }
}

TEST_CASE("keyboard state")
{
    InputState input;
    TranslateContext context;
    context.window = 7;
    input.Handle(KeyEvent(SDL_SCANCODE_Z, true, 7), context);
    input.Handle(KeyEvent(SDL_SCANCODE_RCTRL, true, 7), context);
    input.Handle(KeyEvent(SDL_SCANCODE_X, true, 8), context);  // another window
    CHECK(input.Keys()[DIK_Z] == 0x80);
    CHECK(input.Keys()[DIK_RCONTROL] == 0x80);
    CHECK(input.Keys()[DIK_X] == 0);

    input.Handle(KeyEvent(SDL_SCANCODE_Z, false, 7), context);
    CHECK(input.Keys()[DIK_Z] == 0);

    SDL_Event focus{};
    focus.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    focus.window.windowID = 7;
    input.Handle(focus, context);
    CHECK_FALSE(input.Focused());
    CHECK(input.Keys()[DIK_RCONTROL] == 0);
    focus.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
    input.Handle(focus, context);
    CHECK(input.Focused());
}

TEST_CASE("mouse state")
{
    InputState input;
    TranslateContext context;
    context.pixelDensity = 2.0f;

    input.Handle(MotionEvent(10.0f, 20.0f, 3.25f, -1.0f), context);
    CHECK(input.Cursor().x == 20);
    CHECK(input.Cursor().y == 40);
    input.Handle(ButtonEvent(SDL_BUTTON_RIGHT, true), context);
    input.Handle(WheelEvent(1.0f), context);

    DIMOUSESTATE mouse = input.ReadMouse();
    CHECK(mouse.lX == 6);  // 6.5 pixels: the half stays for the next read
    CHECK(mouse.lY == -2);
    CHECK(mouse.lZ == 120);
    CHECK(mouse.rgbButtons[0] == 0);
    CHECK(mouse.rgbButtons[1] == 0x80);  // MOUSE_RBUTTON
    CHECK(mouse.rgbButtons[2] == 0);

    input.Handle(MotionEvent(10.0f, 20.0f, 0.25f, 0.0f), context);
    input.Handle(WheelEvent(0.5f, SDL_MOUSEWHEEL_FLIPPED), context);
    input.Handle(WheelEvent(-1.0f, SDL_MOUSEWHEEL_FLIPPED), context);
    input.Handle(ButtonEvent(SDL_BUTTON_RIGHT, false), context);
    input.Handle(ButtonEvent(SDL_BUTTON_MIDDLE, true), context);
    mouse = input.ReadMouse();
    CHECK(mouse.lX == 1);  // 0.5 + 0.5
    CHECK(mouse.lY == 0);
    CHECK(mouse.lZ == 60);
    CHECK(mouse.rgbButtons[1] == 0);
    CHECK(mouse.rgbButtons[2] == 0x80);  // MOUSE_MBUTTON

    mouse = input.ReadMouse();
    CHECK(mouse.lX == 0);
    CHECK(mouse.lZ == 0);
}

TEST_CASE("joystick state without a joystick")
{
    const DIJOYSTATE neutral = JoystickReader::Neutral();
    CHECK(neutral.lX == 0);
    CHECK(neutral.lZ == 32767);
    CHECK(neutral.rgdwPOV[0] == 0xFFFFFFFF);

    JoystickReader reader;
    DIJOYSTATE state{};
    if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0) {
        CHECK_FALSE(reader.Read(state));
        CHECK(state.rgdwPOV[0] == 0xFFFFFFFF);
    }
}

namespace {

// SDL's joystick subsystem with virtual devices only, for the length of a test.
struct VirtualJoysticks {
    bool ok = false;

    VirtualJoysticks()
    {
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "0");
        ok = SDL_InitSubSystem(SDL_INIT_GAMEPAD);
    }
    ~VirtualJoysticks()
    {
        if (ok) {
            SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
        }
    }

    static SDL_JoystickID Attach(SDL_JoystickType type, int axes, int buttons, int hats)
    {
        SDL_VirtualJoystickDesc desc;
        SDL_INIT_INTERFACE(&desc);
        desc.type = static_cast<Uint16>(type);
        desc.naxes = static_cast<Uint16>(axes);
        desc.nbuttons = static_cast<Uint16>(buttons);
        desc.nhats = static_cast<Uint16>(hats);
        desc.name = "x2 test";
        return SDL_AttachVirtualJoystick(&desc);
    }
};

}  // namespace

TEST_CASE("gamepad read as DirectInput saw an XInput controller")
{
    VirtualJoysticks joysticks;
    REQUIRE(joysticks.ok);
    const SDL_JoystickID id =
        VirtualJoysticks::Attach(SDL_JOYSTICK_TYPE_GAMEPAD, SDL_GAMEPAD_AXIS_COUNT, SDL_GAMEPAD_BUTTON_COUNT, 0);
    REQUIRE(id != 0);
    REQUIRE(SDL_IsGamepad(id));

    JoystickReader reader;
    DIJOYSTATE state{};
    REQUIRE(reader.Read(state));  // opens it
    SDL_Joystick* joystick = SDL_GetJoystickFromID(id);
    REQUIRE(joystick != nullptr);

    SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFTX, 32767);
    SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFTY, 200);  // inside the dead zone
    SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFT_TRIGGER, 32767);
    SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_RIGHTX, -32768);
    SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_WEST, true);
    SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_START, true);
    SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_DPAD_UP, true);
    SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_DPAD_RIGHT, true);
    SDL_UpdateJoysticks();

    REQUIRE(reader.Read(state));
    CHECK(state.lX == 1024);
    CHECK(state.lY == 0);
    CHECK(state.lZ == 65534);  // left trigger above the centre
    CHECK(state.lRx == 0);
    CHECK(state.rgdwPOV[0] == 4500);
    CHECK(state.rgbButtons[0] == 0);
    CHECK(state.rgbButtons[2] == 0x80);  // X
    CHECK(state.rgbButtons[7] == 0x80);  // Start

    SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_LEFT_TRIGGER, -32768);  // released: a trigger rests at the minimum
    SDL_SetJoystickVirtualAxis(joystick, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, 32767);
    SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_DPAD_UP, false);
    SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_DPAD_RIGHT, false);
    SDL_UpdateJoysticks();
    REQUIRE(reader.Read(state));
    CHECK(state.lZ == 0);
    CHECK(state.rgdwPOV[0] == 0xFFFFFFFF);

    SDL_DetachVirtualJoystick(id);
    SDL_UpdateJoysticks();
    CHECK_FALSE(reader.Read(state));
    CHECK_FALSE(reader.IsOpen());
}

TEST_CASE("other joysticks read axis by axis")
{
    VirtualJoysticks joysticks;
    REQUIRE(joysticks.ok);
    const SDL_JoystickID id = VirtualJoysticks::Attach(SDL_JOYSTICK_TYPE_UNKNOWN, 3, 4, 1);
    REQUIRE(id != 0);

    JoystickReader reader;
    DIJOYSTATE state{};
    REQUIRE(reader.Read(state));
    SDL_Joystick* joystick = SDL_GetJoystickFromID(id);
    REQUIRE(joystick != nullptr);
    if (SDL_IsGamepad(id)) {
        MESSAGE("SDL maps this virtual joystick as a gamepad; raw layout not checked");
        SDL_DetachVirtualJoystick(id);
        return;
    }

    SDL_SetJoystickVirtualAxis(joystick, 0, -32768);
    SDL_SetJoystickVirtualAxis(joystick, 1, 16384);
    SDL_SetJoystickVirtualAxis(joystick, 2, 0);
    SDL_SetJoystickVirtualButton(joystick, 3, true);
    SDL_SetJoystickVirtualHat(joystick, 0, SDL_HAT_LEFTDOWN);
    SDL_UpdateJoysticks();

    REQUIRE(reader.Read(state));
    CHECK(state.lX == -1024);
    CHECK(state.lY > 500);
    CHECK(state.lY < 520);
    CHECK(state.lZ == 32768);
    CHECK(state.rgdwPOV[0] == 22500);
    CHECK(state.rgbButtons[3] == 0x80);
    CHECK(state.rgbButtons[4] == 0);

    SDL_DetachVirtualJoystick(id);
}
