#pragma once

// SDL3 events -> Win32 window messages (roadmap Phase 3). The client keeps the original MsgProc(HWND, UINT, WPARAM,
// LPARAM) model: each SDL event becomes the message(s) Windows would have sent to the DXUT window, with the same wParam
// and lParam encoding, so CKTDXApp::MsgProc and the stages switch on them unchanged. See docs/porting/client-dxut.md.

#include "Ime.h"

#include "x2/platform/win32.h"

#include <SDL3/SDL_events.h>

#include <string>
#include <vector>

namespace x2::dxut {

struct WinMessage {
    UINT message = 0;
    WPARAM wParam = 0;
    LPARAM lParam = 0;
    // A WM_CHAR that Windows would produce only through DefWindowProc of the WM_IME_COMPOSITION before it (the IME
    // result). The dispatcher drops it when that WM_IME_COMPOSITION was handled with no further processing.
    bool imeResultChar = false;

    bool operator==(const WinMessage&) const = default;
};

// What the translation needs to know about the window and the keyboard at the time of the event. Filled from SDL by the
// main loop; tests fill it by hand.
struct TranslateContext {
    SDL_WindowID window = 0;    // only events of this window (0: any)
    float pixelDensity = 1.0f;  // window coordinates -> pixels (client coordinates are pixels, like the back buffer)
    int windowX = 0;            // window position on screen, for the screen coordinates of WM_MOUSEWHEEL
    int windowY = 0;
    int pixelWidth = 0;         // client size in pixels, for WM_SIZE
    int pixelHeight = 0;
    SDL_Keymod mod = SDL_KMOD_NONE;  // modifiers for MK_SHIFT/MK_CONTROL of mouse messages
};

// Windows virtual-key code, set-1 scan code and extended-key flag of an SDL key, as WM_KEYDOWN reports them.
struct VirtualKey {
    UINT vk = 0;
    UINT scanCode = 0;
    bool extended = false;
};
VirtualKey ToVirtualKey(SDL_Scancode scancode, SDL_Keycode key, SDL_Keymod mod);

class SdlMessageTranslator {
public:
    // Appends the messages for `event` to `out` (none for events Windows has no message for).
    void Translate(const SDL_Event& event, const TranslateContext& context, std::vector<WinMessage>& out);

    // The IME state the WM_IME_COMPOSITION and WM_IME_NOTIFY messages refer to (what ImmGetCompositionString and
    // ImmGetCandidateList return on Windows).
    ImeContext& Ime() { return m_ime; }
    const ImeContext& Ime() const { return m_ime; }
    bool Composing() const { return m_ime.Composing(); }
    const std::wstring& CompositionString() const { return m_ime.CompositionString(); }
    int CompositionCursor() const { return m_ime.CompositionCursor(); }
    const std::wstring& ResultString() const { return m_ime.ResultString(); }

    // MK_* bits of the mouse buttons held now.
    WPARAM MouseButtons() const { return m_buttons; }

    // Forgets held buttons (focus lost: Windows stops sending their release to this window).
    void ReleaseMouseButtons() { m_buttons = 0; }

private:
    void Key(const SDL_KeyboardEvent& key, std::vector<WinMessage>& out);
    void Text(const SDL_TextInputEvent& text, std::vector<WinMessage>& out);
    void Editing(const SDL_TextEditingEvent& editing, std::vector<WinMessage>& out);
    void Candidates(const SDL_TextEditingCandidatesEvent& candidates, std::vector<WinMessage>& out);
    void CloseCandidates(std::vector<WinMessage>& out);
    void Button(const SDL_MouseButtonEvent& button, const TranslateContext& context, std::vector<WinMessage>& out);

    WPARAM m_buttons = 0;  // MK_LBUTTON | MK_RBUTTON | ...
    bool m_maximized = false;
    ImeContext m_ime;
};

}  // namespace x2::dxut
