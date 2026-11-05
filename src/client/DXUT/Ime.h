#pragma once

// The IME as KTDXLIB's IME edit box sees it (roadmap Phase 3). The original read the IME through IMM32: the composition
// string with ImmGetCompositionStringW on WM_IME_COMPOSITION, the candidate list with ImmGetCandidateListW on
// WM_IME_NOTIFY (IMN_OPENCANDIDATE/IMN_CHANGECANDIDATE), and controlled it with ImmNotifyIME and ImmAssociateContext.
// SDL3 owns the real IME on every platform (on Windows it handles the IMM32 messages itself), so the port keeps that
// API on the state SDL reports: ImeContext holds what the SDL events said, and x2::dxut::imm has the IMM32 functions
// the edit box calls through its _Imm* pointers, answering from the bound ImeContext. See docs/porting/client-ime.md.

#include "x2/platform/ime.h"
#include "x2/platform/win32.h"

#include <SDL3/SDL_events.h>

#include <functional>
#include <string>
#include <vector>

namespace x2::dxut {

struct WinMessage;

class ImeContext {
public:
    // --- Composition (SDL_EVENT_TEXT_EDITING / SDL_EVENT_TEXT_INPUT)
    bool Composing() const { return m_composing; }
    const std::wstring& CompositionString() const { return m_composition; }
    int CompositionCursor() const { return m_cursor; }
    // GCS_COMPATTR: one ATTR_* per character of the composition string.
    const std::vector<BYTE>& CompositionAttributes() const { return m_attributes; }
    const std::wstring& ResultString() const { return m_result; }

    // `targetStart`/`targetLength`: the clause being converted, as SDL reports it (start/length of the selected
    // editing text; -1 or a zero length when there is none).
    void SetComposition(std::wstring text, int cursor, int targetStart, int targetLength);
    void ClearComposition();
    void SetResult(std::wstring result) { m_result = std::move(result); }

    // --- Candidate list (SDL_EVENT_TEXT_EDITING_CANDIDATES)
    bool CandidatesOpen() const { return m_candidatesOpen; }
    // The candidates of the current page, without the index SDL puts in front of them on Windows.
    const std::vector<std::wstring>& Candidates() const { return m_candidates; }
    // The selected candidate, relative to the page; -1 when none is.
    int SelectedCandidate() const { return m_selected; }
    bool HorizontalCandidates() const { return m_horizontal; }

    // Returns the WM_IME_NOTIFY wParam the change means: IMN_OPENCANDIDATE, IMN_CHANGECANDIDATE, IMN_CLOSECANDIDATE,
    // or 0 for none (an empty list while none is open).
    UINT SetCandidates(const SDL_TextEditingCandidatesEvent& event);
    // Returns IMN_CLOSECANDIDATE if a list was open, 0 otherwise.
    UINT CloseCandidates();

    // --- Input-context state the IMM32 functions keep
    bool Associated() const { return m_associated; }
    void SetAssociated(bool associated) { m_associated = associated; }
    bool Open() const { return m_open; }
    void SetOpen(bool open) { m_open = open; }
    DWORD Conversion() const { return m_conversion; }
    DWORD Sentence() const { return m_sentence; }
    void SetConversion(DWORD conversion, DWORD sentence)
    {
        m_conversion = conversion;
        m_sentence = sentence;
    }

    // What the IME does on Windows when the application asks it to (ImmNotifyIME). `clearComposition` resets SDL's
    // composition (SDL_ClearComposition); `send` delivers the messages the IME would send back to the window
    // (WM_IME_NOTIFY, WM_IME_ENDCOMPOSITION), synchronously, as SendMessage did. Both may be empty.
    std::function<void()> clearComposition;
    std::function<void(const WinMessage&)> send;

    // ImmNotifyIME(NI_COMPOSITIONSTR, CPS_CANCEL) and ImmNotifyIME(NI_CLOSECANDIDATE).
    void CancelComposition();
    void CloseCandidateWindow();

private:
    bool m_composing = false;
    std::wstring m_composition;
    int m_cursor = 0;
    std::vector<BYTE> m_attributes;
    std::wstring m_result;

    bool m_candidatesOpen = false;
    std::vector<std::wstring> m_candidates;
    int m_selected = -1;
    int m_pageSize = 0;  // largest page seen since the list opened (see SetCandidates)
    bool m_horizontal = false;

    bool m_associated = true;
    bool m_open = true;
    DWORD m_conversion = IME_CMODE_NATIVE;
    DWORD m_sentence = 0;
};

// The ImeContext the imm functions answer from (the window's input context), or nullptr. DXUT binds its translator's.
void BindIme(ImeContext* context);
ImeContext* BoundIme();

// The IMM32 functions KTDGUIIMEEditBox loads from imm32.dll, with the same names and signatures, on the bound
// ImeContext. The HIMC is the ImeContext itself. Functions with no SDL counterpart do what IMM32 does without an IME.
namespace imm {

INPUTCONTEXT* WINAPI ImmLockIMC(HIMC);
BOOL WINAPI ImmUnlockIMC(HIMC);
LPVOID WINAPI ImmLockIMCC(HIMCC);
BOOL WINAPI ImmUnlockIMCC(HIMCC);
BOOL WINAPI ImmDisableTextFrameService(DWORD);
LONG WINAPI ImmGetCompositionStringW(HIMC himc, DWORD index, LPVOID buffer, DWORD bufferBytes);
DWORD WINAPI ImmGetCandidateListW(HIMC himc, DWORD index, LPCANDIDATELIST list, DWORD bufferBytes);
HIMC WINAPI ImmGetContext(HWND);
BOOL WINAPI ImmReleaseContext(HWND, HIMC);
HIMC WINAPI ImmAssociateContext(HWND, HIMC himc);
BOOL WINAPI ImmGetOpenStatus(HIMC himc);
BOOL WINAPI ImmSetOpenStatus(HIMC himc, BOOL open);
BOOL WINAPI ImmGetConversionStatus(HIMC himc, LPDWORD conversion, LPDWORD sentence);
HWND WINAPI ImmGetDefaultIMEWnd(HWND);
UINT WINAPI ImmGetIMEFileNameA(HKL, LPSTR buffer, UINT bufferLength);
UINT WINAPI ImmGetVirtualKey(HWND);
BOOL WINAPI ImmNotifyIME(HIMC himc, DWORD action, DWORD index, DWORD value);
BOOL WINAPI ImmSetConversionStatus(HIMC himc, DWORD conversion, DWORD sentence);
BOOL WINAPI ImmSimulateHotKey(HWND, DWORD);
BOOL WINAPI ImmIsIME(HKL hkl);

}  // namespace imm

}  // namespace x2::dxut
