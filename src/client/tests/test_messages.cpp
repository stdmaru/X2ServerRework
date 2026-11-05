// SDL3 events -> Win32 messages (src/client/DXUT/SdlMessages.*): the wParam/lParam Windows would send.

#include "SdlMessages.h"

#include <doctest/doctest.h>

#include <vector>

using x2::dxut::SdlMessageTranslator;
using x2::dxut::TranslateContext;
using x2::dxut::WinMessage;

namespace {

SDL_Event KeyEvent(bool down, SDL_Scancode scancode, SDL_Keycode key, SDL_Keymod mod = SDL_KMOD_NONE, bool repeat = false)
{
    SDL_Event event = {};
    event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    event.key.scancode = scancode;
    event.key.key = key;
    event.key.mod = mod;
    event.key.down = down;
    event.key.repeat = repeat;
    return event;
}

SDL_Event ButtonEvent(bool down, Uint8 button, float x, float y, Uint8 clicks = 1)
{
    SDL_Event event = {};
    event.type = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
    event.button.button = button;
    event.button.down = down;
    event.button.clicks = clicks;
    event.button.x = x;
    event.button.y = y;
    return event;
}

SDL_Event WindowEvent(SDL_EventType type, Sint32 data1 = 0, Sint32 data2 = 0)
{
    SDL_Event event = {};
    event.type = type;
    event.window.data1 = data1;
    event.window.data2 = data2;
    return event;
}

std::vector<WinMessage> Translate(SdlMessageTranslator& translator, const SDL_Event& event, const TranslateContext& context = {})
{
    std::vector<WinMessage> out;
    translator.Translate(event, context, out);
    return out;
}

}  // namespace

TEST_CASE("keys become WM_KEYDOWN/WM_KEYUP with the Windows lParam")
{
    SdlMessageTranslator translator;

    auto down = Translate(translator, KeyEvent(true, SDL_SCANCODE_A, SDLK_A));
    REQUIRE(down.size() == 1);
    CHECK(down[0].message == WM_KEYDOWN);
    CHECK(down[0].wParam == 'A');
    CHECK(down[0].lParam == (1 | (0x1E << 16)));  // repeat count 1, scan code

    auto repeat = Translate(translator, KeyEvent(true, SDL_SCANCODE_A, SDLK_A, SDL_KMOD_NONE, true));
    REQUIRE(repeat.size() == 1);
    CHECK(repeat[0].lParam == (1 | (0x1E << 16) | (1 << 30)));  // previous state: down

    auto up = Translate(translator, KeyEvent(false, SDL_SCANCODE_A, SDLK_A));
    REQUIRE(up.size() == 1);
    CHECK(up[0].message == WM_KEYUP);
    CHECK(static_cast<DWORD>(up[0].lParam) == (1u | (0x1Eu << 16) | (1u << 30) | (1u << 31)));

    // Extended keys and the generic modifier codes
    auto right = Translate(translator, KeyEvent(true, SDL_SCANCODE_RIGHT, SDLK_RIGHT));
    REQUIRE(right.size() == 1);
    CHECK(right[0].wParam == VK_RIGHT);
    CHECK((right[0].lParam & (1 << 24)) != 0);
    auto rctrl = Translate(translator, KeyEvent(true, SDL_SCANCODE_RCTRL, SDLK_RCTRL, SDL_KMOD_RCTRL));
    REQUIRE(rctrl.size() == 1);
    CHECK(rctrl[0].wParam == VK_CONTROL);
    CHECK((rctrl[0].lParam & (1 << 24)) != 0);
    auto lshift = Translate(translator, KeyEvent(true, SDL_SCANCODE_LSHIFT, SDLK_LSHIFT, SDL_KMOD_LSHIFT));
    REQUIRE(lshift.size() == 1);
    CHECK(lshift[0].wParam == VK_SHIFT);

    // Keys Windows has no virtual key for produce nothing
    CHECK(Translate(translator, KeyEvent(true, SDL_SCANCODE_MEDIA_PLAY, SDLK_MEDIA_PLAY)).empty());
}

TEST_CASE("letters follow the keyboard layout")
{
    // AZERTY: the key in the Q position is labelled A
    CHECK(x2::dxut::ToVirtualKey(SDL_SCANCODE_Q, SDLK_A, SDL_KMOD_NONE).vk == 'A');
    CHECK(x2::dxut::ToVirtualKey(SDL_SCANCODE_Q, SDLK_A, SDL_KMOD_NONE).scanCode == 0x10);
    // a non-Latin layout: the position decides
    CHECK(x2::dxut::ToVirtualKey(SDL_SCANCODE_Q, 0x3142, SDL_KMOD_NONE).vk == 'Q');
    CHECK(x2::dxut::ToVirtualKey(SDL_SCANCODE_0, SDLK_0, SDL_KMOD_NONE).vk == '0');
    CHECK(x2::dxut::ToVirtualKey(SDL_SCANCODE_F12, SDLK_F12, SDL_KMOD_NONE).vk == VK_F12);
    // keypad with and without Num Lock
    CHECK(x2::dxut::ToVirtualKey(SDL_SCANCODE_KP_7, SDLK_KP_7, SDL_KMOD_NUM).vk == VK_NUMPAD7);
    CHECK(x2::dxut::ToVirtualKey(SDL_SCANCODE_KP_7, SDLK_KP_7, SDL_KMOD_NONE).vk == VK_HOME);
}

TEST_CASE("Alt makes system keys")
{
    SdlMessageTranslator translator;

    auto alt = Translate(translator, KeyEvent(true, SDL_SCANCODE_LALT, SDLK_LALT, SDL_KMOD_LALT));
    REQUIRE(alt.size() == 1);
    CHECK(alt[0].message == WM_SYSKEYDOWN);
    CHECK(alt[0].wParam == VK_MENU);

    auto enter = Translate(translator, KeyEvent(true, SDL_SCANCODE_RETURN, SDLK_RETURN, SDL_KMOD_LALT));
    REQUIRE(enter.size() == 1);
    CHECK(enter[0].message == WM_SYSKEYDOWN);
    CHECK((enter[0].lParam & (1 << 29)) != 0);  // context code: Alt held

    // AltGr (Ctrl+Alt) types characters: ordinary key messages
    auto altgr = Translate(translator, KeyEvent(true, SDL_SCANCODE_Q, SDLK_Q, SDL_Keymod(SDL_KMOD_RALT | SDL_KMOD_LCTRL)));
    REQUIRE(altgr.size() == 1);
    CHECK(altgr[0].message == WM_KEYDOWN);

    auto f10 = Translate(translator, KeyEvent(true, SDL_SCANCODE_F10, SDLK_F10));
    REQUIRE(f10.size() == 1);
    CHECK(f10[0].message == WM_SYSKEYDOWN);
}

TEST_CASE("text input becomes WM_CHAR")
{
    SdlMessageTranslator translator;
    SDL_Event event = {};
    event.type = SDL_EVENT_TEXT_INPUT;
    event.text.text = "a\xED\x95\x9C";  // "a한"

    auto out = Translate(translator, event);
    REQUIRE(out.size() == 2);
    CHECK(out[0] == WinMessage{WM_CHAR, 'a', 1});
    CHECK(out[1] == WinMessage{WM_CHAR, 0xD55C, 1});
}

TEST_CASE("IME composition becomes WM_IME_* with the composition string")
{
    SdlMessageTranslator translator;
    SDL_Event editing = {};
    editing.type = SDL_EVENT_TEXT_EDITING;
    editing.edit.text = "\xE3\x85\x8E";  // "ㅎ"
    editing.edit.start = 1;

    auto start = Translate(translator, editing);
    REQUIRE(start.size() == 2);
    CHECK(start[0].message == WM_IME_STARTCOMPOSITION);
    CHECK(start[1].message == WM_IME_COMPOSITION);
    CHECK((start[1].lParam & GCS_COMPSTR) != 0);
    CHECK(translator.Composing());
    CHECK(translator.CompositionString() == L"\x314E");
    CHECK(translator.CompositionCursor() == 1);

    editing.edit.text = "\xED\x95\x9C";  // "한"
    auto update = Translate(translator, editing);
    REQUIRE(update.size() == 1);
    CHECK(update[0].message == WM_IME_COMPOSITION);
    CHECK(update[0].wParam == 0xD55C);

    SDL_Event commit = {};
    commit.type = SDL_EVENT_TEXT_INPUT;
    commit.text.text = "\xED\x95\x9C";
    auto result = Translate(translator, commit);
    REQUIRE(result.size() == 3);
    CHECK(result[0].message == WM_IME_COMPOSITION);
    CHECK(result[0].lParam == GCS_RESULTSTR);
    CHECK(result[1].message == WM_IME_ENDCOMPOSITION);
    CHECK(result[2].message == WM_CHAR);
    CHECK(result[2].imeResultChar);
    CHECK(translator.ResultString() == L"\xD55C");
    CHECK_FALSE(translator.Composing());

    // a cancelled composition ends without a result
    editing.edit.text = "\xE3\x85\x8E";
    Translate(translator, editing);
    editing.edit.text = "";
    auto cancel = Translate(translator, editing);
    REQUIRE(cancel.size() == 2);
    CHECK(cancel[1].message == WM_IME_ENDCOMPOSITION);
    CHECK(translator.CompositionString().empty());
}

TEST_CASE("mouse buttons, double clicks and pixel coordinates")
{
    SdlMessageTranslator translator;
    TranslateContext context;
    context.pixelDensity = 2.0f;

    auto down = Translate(translator, ButtonEvent(true, SDL_BUTTON_LEFT, 10.0f, 20.0f), context);
    REQUIRE(down.size() == 1);
    CHECK(down[0].message == WM_LBUTTONDOWN);
    CHECK(down[0].wParam == MK_LBUTTON);
    CHECK(GET_X_LPARAM(down[0].lParam) == 20);
    CHECK(GET_Y_LPARAM(down[0].lParam) == 40);

    auto up = Translate(translator, ButtonEvent(false, SDL_BUTTON_LEFT, 10.0f, 20.0f), context);
    REQUIRE(up.size() == 1);
    CHECK(up[0].message == WM_LBUTTONUP);
    CHECK(up[0].wParam == 0);

    auto dbl = Translate(translator, ButtonEvent(true, SDL_BUTTON_LEFT, 10.0f, 20.0f, 2), context);
    REQUIRE(dbl.size() == 1);
    CHECK(dbl[0].message == WM_LBUTTONDBLCLK);
    Translate(translator, ButtonEvent(false, SDL_BUTTON_LEFT, 10.0f, 20.0f, 2), context);

    context.mod = SDL_KMOD_LSHIFT;
    auto x2 = Translate(translator, ButtonEvent(true, SDL_BUTTON_X2, -3.0f, 5.0f), context);
    REQUIRE(x2.size() == 1);
    CHECK(x2[0].message == WM_XBUTTONDOWN);
    CHECK(GET_KEYSTATE_WPARAM(x2[0].wParam) == (MK_XBUTTON2 | MK_SHIFT));
    CHECK(GET_XBUTTON_WPARAM(x2[0].wParam) == XBUTTON2);
    CHECK(GET_X_LPARAM(x2[0].lParam) == -6);  // dragging outside the window: negative

    SDL_Event motion = {};
    motion.type = SDL_EVENT_MOUSE_MOTION;
    motion.motion.x = 1.0f;
    motion.motion.y = 2.0f;
    auto move = Translate(translator, motion, context);
    REQUIRE(move.size() == 1);
    CHECK(move[0].message == WM_MOUSEMOVE);
    CHECK(move[0].wParam == (MK_XBUTTON2 | MK_SHIFT));
    CHECK(move[0].lParam == MAKELPARAM(2, 4));
}

TEST_CASE("the wheel reports WHEEL_DELTA steps and screen coordinates")
{
    SdlMessageTranslator translator;
    TranslateContext context;
    context.windowX = 100;
    context.windowY = 50;

    SDL_Event wheel = {};
    wheel.type = SDL_EVENT_MOUSE_WHEEL;
    wheel.wheel.y = -1.0f;
    wheel.wheel.mouse_x = 10.0f;
    wheel.wheel.mouse_y = 20.0f;
    auto out = Translate(translator, wheel, context);
    REQUIRE(out.size() == 1);
    CHECK(out[0].message == WM_MOUSEWHEEL);
    CHECK(GET_WHEEL_DELTA_WPARAM(out[0].wParam) == -WHEEL_DELTA);
    CHECK(GET_X_LPARAM(out[0].lParam) == 110);
    CHECK(GET_Y_LPARAM(out[0].lParam) == 70);

    wheel.wheel.direction = SDL_MOUSEWHEEL_FLIPPED;
    out = Translate(translator, wheel, context);
    REQUIRE(out.size() == 1);
    CHECK(GET_WHEEL_DELTA_WPARAM(out[0].wParam) == WHEEL_DELTA);

    wheel.wheel.y = 0.0f;  // horizontal only
    wheel.wheel.x = 1.0f;
    CHECK(Translate(translator, wheel, context).empty());
}

TEST_CASE("focus, size and close")
{
    SdlMessageTranslator translator;
    TranslateContext context;
    context.pixelWidth = 1024;
    context.pixelHeight = 768;

    auto gained = Translate(translator, WindowEvent(SDL_EVENT_WINDOW_FOCUS_GAINED), context);
    REQUIRE(gained.size() == 2);
    CHECK(gained[0] == WinMessage{WM_ACTIVATEAPP, TRUE, 0});
    CHECK(gained[1].message == WM_SETFOCUS);

    Translate(translator, ButtonEvent(true, SDL_BUTTON_RIGHT, 1.0f, 1.0f), context);
    CHECK(translator.MouseButtons() == MK_RBUTTON);
    auto lost = Translate(translator, WindowEvent(SDL_EVENT_WINDOW_FOCUS_LOST), context);
    REQUIRE(lost.size() == 2);
    CHECK(lost[1] == WinMessage{WM_ACTIVATEAPP, FALSE, 0});
    CHECK(translator.MouseButtons() == 0);

    auto minimized = Translate(translator, WindowEvent(SDL_EVENT_WINDOW_MINIMIZED), context);
    REQUIRE(minimized.size() == 1);
    CHECK(minimized[0] == WinMessage{WM_SIZE, SIZE_MINIMIZED, 0});

    auto maximized = Translate(translator, WindowEvent(SDL_EVENT_WINDOW_MAXIMIZED), context);
    REQUIRE(maximized.size() == 1);
    CHECK(maximized[0] == WinMessage{WM_SIZE, SIZE_MAXIMIZED, MAKELPARAM(1024, 768)});

    auto resized = Translate(translator, WindowEvent(SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED, 1920, 1080), context);
    REQUIRE(resized.size() == 1);
    CHECK(resized[0] == WinMessage{WM_SIZE, SIZE_MAXIMIZED, MAKELPARAM(1920, 1080)});

    auto restored = Translate(translator, WindowEvent(SDL_EVENT_WINDOW_RESTORED), context);
    REQUIRE(restored.size() == 1);
    CHECK(restored[0].wParam == SIZE_RESTORED);

    auto close = Translate(translator, WindowEvent(SDL_EVENT_WINDOW_CLOSE_REQUESTED), context);
    REQUIRE(close.size() == 1);
    CHECK(close[0].message == WM_CLOSE);
}

TEST_CASE("events of other windows are ignored")
{
    SdlMessageTranslator translator;
    TranslateContext context;
    context.window = 7;

    SDL_Event event = KeyEvent(true, SDL_SCANCODE_A, SDLK_A);
    event.key.windowID = 8;
    CHECK(Translate(translator, event, context).empty());
    event.key.windowID = 7;
    CHECK(Translate(translator, event, context).size() == 1);
}
