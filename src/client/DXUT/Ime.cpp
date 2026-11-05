#include "Ime.h"

#include "SdlMessages.h"

#include "x2/platform/unicode.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace x2::dxut {

namespace {

ImeContext* g_bound = nullptr;

ImeContext* ContextOf(HIMC himc)
{
    return himc && reinterpret_cast<ImeContext*>(himc) == g_bound ? g_bound : nullptr;
}

// SDL's Windows backend writes each candidate as "<digit> <candidate>", the digit counting from the page's first entry
// (1 for most IMEs). The edit box numbers the candidates itself, so the prefix is dropped when every entry has it.
bool HasIndexPrefixes(const std::vector<std::wstring>& candidates)
{
    if (candidates.empty()) {
        return false;
    }
    for (std::size_t i = 0; i < candidates.size(); ++i) {
        const std::wstring& candidate = candidates[i];
        if (candidate.size() < 3 || candidate[1] != L' ' || candidate[0] < L'0' || candidate[0] > L'9') {
            return false;
        }
        const int first = candidates[0][0] - L'0';
        if (candidate[0] - L'0' != static_cast<int>((first + i) % 10)) {
            return false;
        }
    }
    return true;
}

// Copies `bytes` bytes of `data` into the caller's buffer the way ImmGetCompositionStringW does: a zero-length buffer
// asks for the size; a short buffer gets what fits.
LONG CopyOut(const void* data, std::size_t bytes, LPVOID buffer, DWORD bufferBytes)
{
    if (bufferBytes == 0 || !buffer) {
        return static_cast<LONG>(bytes);
    }
    const std::size_t copied = std::min<std::size_t>(bytes, bufferBytes);
    if (copied) {
        std::memcpy(buffer, data, copied);
    }
    return static_cast<LONG>(copied);
}

}  // namespace

void ImeContext::SetComposition(std::wstring text, int cursor, int targetStart, int targetLength)
{
    m_composing = true;
    m_composition = std::move(text);
    const int length = static_cast<int>(m_composition.size());
    m_cursor = std::clamp(cursor, 0, length);
    m_attributes.assign(m_composition.size(), static_cast<BYTE>(ATTR_INPUT));
    if (targetStart >= 0 && targetLength > 0) {
        const int end = std::min(length, targetStart + targetLength);
        for (int i = std::min(targetStart, length); i < end; ++i) {
            m_attributes[static_cast<std::size_t>(i)] = ATTR_TARGET_CONVERTED;
        }
    }
}

void ImeContext::ClearComposition()
{
    m_composing = false;
    m_composition.clear();
    m_cursor = 0;
    m_attributes.clear();
}

UINT ImeContext::SetCandidates(const SDL_TextEditingCandidatesEvent& event)
{
    if (!event.candidates || event.num_candidates <= 0) {
        return CloseCandidates();
    }
    const bool wasOpen = m_candidatesOpen;
    if (!wasOpen) {
        m_pageSize = 0;
    }
    m_candidates.clear();
    for (int i = 0; i < event.num_candidates; ++i) {
        m_candidates.push_back(event.candidates[i] ? x2::platform::Utf8ToWide(event.candidates[i]) : std::wstring());
    }
    if (HasIndexPrefixes(m_candidates)) {
        for (std::wstring& candidate : m_candidates) {
            candidate.erase(0, 2);
        }
    }
    const int count = static_cast<int>(m_candidates.size());
    m_pageSize = std::max(m_pageSize, count);
    // SDL's Windows backend sends one page but the selection as an index into the whole list (dwSelection). A
    // selection past the page is made relative to it: pages start at multiples of the page size, which is the size of
    // the longest page seen since the list opened (the last page may be shorter).
    int selected = event.selected_candidate;
    if (selected >= count) {
        selected %= m_pageSize;
    }
    m_selected = selected >= 0 && selected < count ? selected : -1;
    m_horizontal = event.horizontal;
    m_candidatesOpen = true;
    return wasOpen ? IMN_CHANGECANDIDATE : IMN_OPENCANDIDATE;
}

UINT ImeContext::CloseCandidates()
{
    if (!m_candidatesOpen) {
        return 0;
    }
    m_candidatesOpen = false;
    m_candidates.clear();
    m_selected = -1;
    m_pageSize = 0;
    return IMN_CLOSECANDIDATE;
}

void ImeContext::CancelComposition()
{
    const bool wasComposing = m_composing;
    ClearComposition();
    if (clearComposition) {
        clearComposition();
    }
    // The IME ends the composition it was told to drop. A candidate list stays open (the Korean IME behaves so; the
    // edit box closes it with NI_CLOSECANDIDATE right after).
    if (wasComposing && send) {
        send(WinMessage{WM_IME_ENDCOMPOSITION, 0, 0});
    }
}

void ImeContext::CloseCandidateWindow()
{
    if (CloseCandidates() == IMN_CLOSECANDIDATE && send) {
        send(WinMessage{WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1});
    }
}

void BindIme(ImeContext* context)
{
    g_bound = context;
}

ImeContext* BoundIme()
{
    return g_bound;
}

namespace imm {

// The private reading-window data of old Chinese IMEs (GetPrivateReadingString) is never there.
INPUTCONTEXT* WINAPI ImmLockIMC(HIMC) { return nullptr; }
BOOL WINAPI ImmUnlockIMC(HIMC) { return FALSE; }
LPVOID WINAPI ImmLockIMCC(HIMCC) { return nullptr; }
BOOL WINAPI ImmUnlockIMCC(HIMCC) { return FALSE; }
BOOL WINAPI ImmDisableTextFrameService(DWORD) { return TRUE; }

LONG WINAPI ImmGetCompositionStringW(HIMC himc, DWORD index, LPVOID buffer, DWORD bufferBytes)
{
    const ImeContext* context = ContextOf(himc);
    if (!context) {
        return IMM_ERROR_GENERAL;
    }
    switch (index) {
    case GCS_COMPSTR: {
        const std::wstring& text = context->CompositionString();
        return CopyOut(text.data(), text.size() * sizeof(WCHAR), buffer, bufferBytes);
    }
    case GCS_RESULTSTR: {
        const std::wstring& text = context->ResultString();
        return CopyOut(text.data(), text.size() * sizeof(WCHAR), buffer, bufferBytes);
    }
    case GCS_COMPATTR: {
        const std::vector<BYTE>& attributes = context->CompositionAttributes();
        return CopyOut(attributes.data(), attributes.size(), buffer, bufferBytes);
    }
    case GCS_COMPCLAUSE: {
        // One clause for the whole string: the offsets of its start and of its end.
        if (context->CompositionString().empty()) {
            return 0;
        }
        const DWORD clauses[2] = {0, static_cast<DWORD>(context->CompositionString().size())};
        return CopyOut(clauses, sizeof(clauses), buffer, bufferBytes);
    }
    case GCS_CURSORPOS:
        return context->CompositionCursor();
    case GCS_DELTASTART:
        return 0;
    default:
        return IMM_ERROR_NODATA;  // reading strings and result clauses: SDL does not report them
    }
}

DWORD WINAPI ImmGetCandidateListW(HIMC himc, DWORD index, LPCANDIDATELIST list, DWORD bufferBytes)
{
    const ImeContext* context = ContextOf(himc);
    if (!context || index != 0 || !context->CandidatesOpen()) {
        return 0;
    }
    const std::vector<std::wstring>& candidates = context->Candidates();
    const DWORD count = static_cast<DWORD>(candidates.size());
    const std::size_t header = offsetof(CANDIDATELIST, dwOffset) + count * sizeof(DWORD);
    std::size_t size = header;
    for (const std::wstring& candidate : candidates) {
        size += (candidate.size() + 1) * sizeof(WCHAR);
    }
    if (bufferBytes == 0 || !list) {
        return static_cast<DWORD>(size);
    }
    if (bufferBytes < size) {
        return 0;
    }
    list->dwSize = static_cast<DWORD>(size);
    list->dwStyle = 0;  // IME_CAND_UNKNOWN
    list->dwCount = count;
    // One page, the one SDL sent. Windows always has a selection; without one SDL reports, the first entry is.
    list->dwSelection = context->SelectedCandidate() < 0 ? 0 : static_cast<DWORD>(context->SelectedCandidate());
    list->dwPageStart = 0;
    list->dwPageSize = count;
    auto* bytes = reinterpret_cast<BYTE*>(list);
    std::size_t offset = header;
    for (DWORD i = 0; i < count; ++i) {
        list->dwOffset[i] = static_cast<DWORD>(offset);
        const std::wstring& candidate = candidates[i];
        std::memcpy(bytes + offset, candidate.c_str(), (candidate.size() + 1) * sizeof(WCHAR));
        offset += (candidate.size() + 1) * sizeof(WCHAR);
    }
    return static_cast<DWORD>(size);
}

HIMC WINAPI ImmGetContext(HWND)
{
    return g_bound && g_bound->Associated() ? reinterpret_cast<HIMC>(g_bound) : nullptr;
}

BOOL WINAPI ImmReleaseContext(HWND, HIMC) { return TRUE; }

HIMC WINAPI ImmAssociateContext(HWND, HIMC himc)
{
    if (!g_bound) {
        return nullptr;
    }
    const HIMC previous = g_bound->Associated() ? reinterpret_cast<HIMC>(g_bound) : nullptr;
    g_bound->SetAssociated(himc != nullptr);
    return previous;
}

BOOL WINAPI ImmGetOpenStatus(HIMC himc)
{
    const ImeContext* context = ContextOf(himc);
    return context && context->Open() ? TRUE : FALSE;
}

BOOL WINAPI ImmSetOpenStatus(HIMC himc, BOOL open)
{
    ImeContext* context = ContextOf(himc);
    if (!context) {
        return FALSE;
    }
    context->SetOpen(open != FALSE);
    return TRUE;
}

BOOL WINAPI ImmGetConversionStatus(HIMC himc, LPDWORD conversion, LPDWORD sentence)
{
    const ImeContext* context = ContextOf(himc);
    if (!context) {
        return FALSE;
    }
    if (conversion) {
        *conversion = context->Conversion();
    }
    if (sentence) {
        *sentence = context->Sentence();
    }
    return TRUE;
}

BOOL WINAPI ImmSetConversionStatus(HIMC himc, DWORD conversion, DWORD sentence)
{
    ImeContext* context = ContextOf(himc);
    if (!context) {
        return FALSE;
    }
    context->SetConversion(conversion, sentence);
    return TRUE;
}

HWND WINAPI ImmGetDefaultIMEWnd(HWND) { return nullptr; }

// No IME file: GetImeId reports no old Chinese IME and SetupImeApi loads no reading-string API.
UINT WINAPI ImmGetIMEFileNameA(HKL, LPSTR buffer, UINT bufferLength)
{
    if (buffer && bufferLength) {
        buffer[0] = '\0';
    }
    return 0;
}

UINT WINAPI ImmGetVirtualKey(HWND) { return 0; }

BOOL WINAPI ImmNotifyIME(HIMC himc, DWORD action, DWORD index, DWORD value)
{
    ImeContext* context = ContextOf(himc);
    if (!context) {
        return FALSE;
    }
    (void)value;
    if (action == NI_COMPOSITIONSTR && index == CPS_CANCEL) {
        context->CancelComposition();
        return TRUE;
    }
    if (action == NI_CLOSECANDIDATE) {
        context->CloseCandidateWindow();
        return TRUE;
    }
    return FALSE;  // completing, converting or reverting through the IME: SDL has no way to ask for it
}

BOOL WINAPI ImmSimulateHotKey(HWND, DWORD) { return FALSE; }

// The keyboard layouts of East Asian languages are the ones with an IME.
BOOL WINAPI ImmIsIME(HKL hkl)
{
    const WORD language = PRIMARYLANGID(LOWORD(reinterpret_cast<std::uintptr_t>(hkl)));
    return language == LANG_CHINESE || language == LANG_JAPANESE || language == LANG_KOREAN ? TRUE : FALSE;
}

}  // namespace imm

}  // namespace x2::dxut
