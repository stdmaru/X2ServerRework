// The IME edit box's IMM32 on SDL3 (src/client/DXUT/Ime.*): candidate lists from SDL_EVENT_TEXT_EDITING_CANDIDATES as
// WM_IME_NOTIFY and ImmGetCandidateListW, the composition through ImmGetCompositionStringW, and the input context.

#include "Ime.h"
#include "SdlMessages.h"

#include <doctest/doctest.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

using x2::dxut::ImeContext;
using x2::dxut::SdlMessageTranslator;
using x2::dxut::WinMessage;
namespace imm = x2::dxut::imm;

namespace {

std::vector<WinMessage> Translate(SdlMessageTranslator& translator, const SDL_Event& event)
{
    std::vector<WinMessage> out;
    translator.Translate(event, {}, out);
    return out;
}

SDL_Event CandidatesEvent(const std::vector<const char*>& candidates, int selected, bool horizontal = false)
{
    SDL_Event event = {};
    event.type = SDL_EVENT_TEXT_EDITING_CANDIDATES;
    event.edit_candidates.candidates = candidates.empty() ? nullptr : candidates.data();
    event.edit_candidates.num_candidates = static_cast<Sint32>(candidates.size());
    event.edit_candidates.selected_candidate = selected;
    event.edit_candidates.horizontal = horizontal;
    return event;
}

SDL_Event EditingEvent(const char* text, Sint32 start, Sint32 length)
{
    SDL_Event event = {};
    event.type = SDL_EVENT_TEXT_EDITING;
    event.edit.text = text;
    event.edit.start = start;
    event.edit.length = length;
    return event;
}

SDL_Event TextEvent(const char* text)
{
    SDL_Event event = {};
    event.type = SDL_EVENT_TEXT_INPUT;
    event.text.text = text;
    return event;
}

// Reads a candidate list the way KTDGUIIMEEditBox does: ask for the size, then fill a buffer of that size.
std::vector<BYTE> CandidateList(HIMC himc)
{
    const DWORD size = imm::ImmGetCandidateListW(himc, 0, nullptr, 0);
    std::vector<BYTE> buffer(size);
    if (size) {
        CHECK(imm::ImmGetCandidateListW(himc, 0, reinterpret_cast<LPCANDIDATELIST>(buffer.data()), size) == size);
    }
    return buffer;
}

std::wstring CandidateAt(const std::vector<BYTE>& buffer, DWORD i)
{
    const auto* list = reinterpret_cast<const CANDIDATELIST*>(buffer.data());
    return reinterpret_cast<const WCHAR*>(buffer.data() + list->dwOffset[i]);
}

// Binds a translator's input context for one test, as DXUTCreateWindow binds the window's.
struct BoundTranslator {
    SdlMessageTranslator translator;
    std::vector<WinMessage> sent;  // what the IME sent back to the window procedure
    int clears = 0;                // SDL_ClearComposition calls

    BoundTranslator()
    {
        translator.Ime().send = [this](const WinMessage& message) { sent.push_back(message); };
        translator.Ime().clearComposition = [this] { ++clears; };
        x2::dxut::BindIme(&translator.Ime());
    }
    ~BoundTranslator() { x2::dxut::BindIme(nullptr); }
    BoundTranslator(const BoundTranslator&) = delete;
    BoundTranslator& operator=(const BoundTranslator&) = delete;
};

// The edit box stores the emulation in pointers declared like these (KTDGUIIMEEditBox.h): the signatures must match.
static_assert(std::is_same_v<decltype(&imm::ImmLockIMC), INPUTCONTEXT*(WINAPI*)(HIMC)>);
static_assert(std::is_same_v<decltype(&imm::ImmUnlockIMC), BOOL(WINAPI*)(HIMC)>);
static_assert(std::is_same_v<decltype(&imm::ImmLockIMCC), LPVOID(WINAPI*)(HIMCC)>);
static_assert(std::is_same_v<decltype(&imm::ImmUnlockIMCC), BOOL(WINAPI*)(HIMCC)>);
static_assert(std::is_same_v<decltype(&imm::ImmDisableTextFrameService), BOOL(WINAPI*)(DWORD)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetCompositionStringW), LONG(WINAPI*)(HIMC, DWORD, LPVOID, DWORD)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetCandidateListW), DWORD(WINAPI*)(HIMC, DWORD, LPCANDIDATELIST, DWORD)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetContext), HIMC(WINAPI*)(HWND)>);
static_assert(std::is_same_v<decltype(&imm::ImmReleaseContext), BOOL(WINAPI*)(HWND, HIMC)>);
static_assert(std::is_same_v<decltype(&imm::ImmAssociateContext), HIMC(WINAPI*)(HWND, HIMC)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetOpenStatus), BOOL(WINAPI*)(HIMC)>);
static_assert(std::is_same_v<decltype(&imm::ImmSetOpenStatus), BOOL(WINAPI*)(HIMC, BOOL)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetConversionStatus), BOOL(WINAPI*)(HIMC, LPDWORD, LPDWORD)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetDefaultIMEWnd), HWND(WINAPI*)(HWND)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetIMEFileNameA), UINT(WINAPI*)(HKL, LPSTR, UINT)>);
static_assert(std::is_same_v<decltype(&imm::ImmGetVirtualKey), UINT(WINAPI*)(HWND)>);
static_assert(std::is_same_v<decltype(&imm::ImmNotifyIME), BOOL(WINAPI*)(HIMC, DWORD, DWORD, DWORD)>);
static_assert(std::is_same_v<decltype(&imm::ImmSetConversionStatus), BOOL(WINAPI*)(HIMC, DWORD, DWORD)>);
static_assert(std::is_same_v<decltype(&imm::ImmSimulateHotKey), BOOL(WINAPI*)(HWND, DWORD)>);
static_assert(std::is_same_v<decltype(&imm::ImmIsIME), BOOL(WINAPI*)(HKL)>);

// Windows x64 layouts.
static_assert(sizeof(CANDIDATELIST) == 28);
static_assert(offsetof(CANDIDATELIST, dwOffset) == 24);

}  // namespace

TEST_CASE("a candidate list opens, changes and closes as WM_IME_NOTIFY")
{
    SdlMessageTranslator translator;
    const std::vector<const char*> page = {"\xE6\xBC\xA2", "\xE9\x9F\x93", "\xE6\xB1\x97"};  // 漢 韓 汗

    auto open = Translate(translator, CandidatesEvent(page, 0));
    REQUIRE(open.size() == 1);
    CHECK(open[0] == WinMessage{WM_IME_NOTIFY, IMN_OPENCANDIDATE, 1});
    CHECK(translator.Ime().CandidatesOpen());
    REQUIRE(translator.Ime().Candidates().size() == 3);
    CHECK(translator.Ime().Candidates()[1] == L"\x97D3");
    CHECK(translator.Ime().SelectedCandidate() == 0);

    auto change = Translate(translator, CandidatesEvent(page, 2, true));
    REQUIRE(change.size() == 1);
    CHECK(change[0] == WinMessage{WM_IME_NOTIFY, IMN_CHANGECANDIDATE, 1});
    CHECK(translator.Ime().SelectedCandidate() == 2);
    CHECK(translator.Ime().HorizontalCandidates());

    auto close = Translate(translator, CandidatesEvent({}, -1));
    REQUIRE(close.size() == 1);
    CHECK(close[0] == WinMessage{WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1});
    CHECK_FALSE(translator.Ime().CandidatesOpen());
    CHECK(translator.Ime().Candidates().empty());

    // closing what is not open says nothing
    CHECK(Translate(translator, CandidatesEvent({}, -1)).empty());
}

TEST_CASE("SDL's Windows candidates: index prefixes dropped, selection made relative to the page")
{
    ImeContext ime;
    // A full first page of 9 (the Korean IME), as SDL writes it: "<1-based digit> <candidate>".
    const std::vector<const char*> first = {"1 a", "2 b", "3 c", "4 d", "5 e", "6 f", "7 g", "8 h", "9 i"};
    SDL_Event event = CandidatesEvent(first, 4);
    CHECK(ime.SetCandidates(event.edit_candidates) == IMN_OPENCANDIDATE);
    REQUIRE(ime.Candidates().size() == 9);
    CHECK(ime.Candidates()[0] == L"a");
    CHECK(ime.Candidates()[8] == L"i");
    CHECK(ime.SelectedCandidate() == 4);

    // Next page: SDL still reports the selection in the whole list (dwSelection 10 is the second of page two).
    const std::vector<const char*> second = {"1 j", "2 k", "3 l", "4 m", "5 n", "6 o", "7 p", "8 q", "9 r"};
    event = CandidatesEvent(second, 10);
    CHECK(ime.SetCandidates(event.edit_candidates) == IMN_CHANGECANDIDATE);
    CHECK(ime.Candidates()[1] == L"k");
    CHECK(ime.SelectedCandidate() == 1);

    // A short last page keeps the page size of the full ones: 19 is the second of page three.
    const std::vector<const char*> last = {"1 s", "2 t", "3 u"};
    event = CandidatesEvent(last, 19);
    ime.SetCandidates(event.edit_candidates);
    CHECK(ime.Candidates().size() == 3);
    CHECK(ime.SelectedCandidate() == 1);

    // Prefixes that do not count up from the first are part of the candidates.
    const std::vector<const char*> words = {"1 x", "1 y"};
    event = CandidatesEvent(words, -1);
    ime.SetCandidates(event.edit_candidates);
    CHECK(ime.Candidates()[0] == L"1 x");
    CHECK(ime.SelectedCandidate() == -1);
    // Index bases other than 1 (SDL counts from 0 for some IMEs) and the wrap from 9 to 0.
    const std::vector<const char*> zeroBased = {"0 x", "1 y"};
    event = CandidatesEvent(zeroBased, 0);
    ime.SetCandidates(event.edit_candidates);
    CHECK(ime.Candidates()[1] == L"y");
    const std::vector<const char*> wrapped = {"9 x", "0 y"};
    event = CandidatesEvent(wrapped, 0);
    ime.SetCandidates(event.edit_candidates);
    CHECK(ime.Candidates()[1] == L"y");
}

TEST_CASE("ImmGetCandidateListW returns the page as a CANDIDATELIST")
{
    BoundTranslator bound;
    const HIMC himc = imm::ImmGetContext(nullptr);
    REQUIRE(himc != nullptr);
    CHECK(imm::ImmGetCandidateListW(himc, 0, nullptr, 0) == 0);  // no list open

    const std::vector<const char*> page = {"1 \xE6\xBC\xA2", "2 \xE9\x9F\x93", "3 \xE6\xB1\x97"};
    Translate(bound.translator, CandidatesEvent(page, 1));
    const std::vector<BYTE> buffer = CandidateList(himc);
    REQUIRE(buffer.size() >= sizeof(CANDIDATELIST));
    const auto* list = reinterpret_cast<const CANDIDATELIST*>(buffer.data());
    CHECK(list->dwSize == buffer.size());
    CHECK(list->dwCount == 3);
    CHECK(list->dwSelection == 1);
    CHECK(list->dwPageStart == 0);
    CHECK(list->dwPageSize == 3);
    CHECK(list->dwOffset[0] == offsetof(CANDIDATELIST, dwOffset) + 3 * sizeof(DWORD));
    CHECK(CandidateAt(buffer, 0) == L"\x6F22");
    CHECK(CandidateAt(buffer, 1) == L"\x97D3");
    CHECK(CandidateAt(buffer, 2) == L"\x6C57");
    CHECK(buffer.size() == list->dwOffset[0] + 3 * 2 * sizeof(WCHAR));

    // a buffer too small gets nothing; only list 0 exists
    std::vector<BYTE> small(buffer.size() - 1);
    CHECK(imm::ImmGetCandidateListW(himc, 0, reinterpret_cast<LPCANDIDATELIST>(small.data()), static_cast<DWORD>(small.size())) == 0);
    CHECK(imm::ImmGetCandidateListW(himc, 1, nullptr, 0) == 0);

    // no selection reported: the first candidate is selected, as Windows lists always have one
    Translate(bound.translator, CandidatesEvent(page, -1));
    const std::vector<BYTE> unselected = CandidateList(himc);
    CHECK(reinterpret_cast<const CANDIDATELIST*>(unselected.data())->dwSelection == 0);
}

TEST_CASE("ImmGetCompositionStringW reads the composition SDL reported")
{
    BoundTranslator bound;
    const HIMC himc = imm::ImmGetContext(nullptr);
    REQUIRE(himc != nullptr);
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPSTR, nullptr, 0) == 0);

    // "한ㄱ" with the second character as the clause being converted
    Translate(bound.translator, EditingEvent("\xED\x95\x9C\xE3\x84\xB1", 1, 1));
    WCHAR text[16] = {};
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPSTR, nullptr, 0) == 2 * static_cast<LONG>(sizeof(WCHAR)));
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPSTR, text, sizeof(text)) == 2 * static_cast<LONG>(sizeof(WCHAR)));
    CHECK(text[0] == 0xD55C);
    CHECK(text[1] == 0x3131);
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_CURSORPOS, nullptr, 0) == 1);
    BYTE attributes[16] = {};
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPATTR, attributes, sizeof(attributes)) == 2);
    CHECK(attributes[0] == ATTR_INPUT);
    CHECK(attributes[1] == ATTR_TARGET_CONVERTED);
    DWORD clauses[4] = {};
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPCLAUSE, clauses, sizeof(clauses)) == 2 * static_cast<LONG>(sizeof(DWORD)));
    CHECK(clauses[0] == 0);
    CHECK(clauses[1] == 2);
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPREADSTR, text, sizeof(text)) == IMM_ERROR_NODATA);

    // a short buffer gets what fits
    WCHAR one[1] = {};
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPSTR, one, sizeof(one)) == static_cast<LONG>(sizeof(WCHAR)));
    CHECK(one[0] == 0xD55C);

    // without a target clause every character is input
    Translate(bound.translator, EditingEvent("\xED\x95\x9C", 1, 0));
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPATTR, attributes, sizeof(attributes)) == 1);
    CHECK(attributes[0] == ATTR_INPUT);

    // the committed text is the result string until the next composition starts
    Translate(bound.translator, TextEvent("\xED\x95\x9C"));
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_RESULTSTR, text, sizeof(text)) == static_cast<LONG>(sizeof(WCHAR)));
    CHECK(text[0] == 0xD55C);
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_COMPSTR, nullptr, 0) == 0);
    Translate(bound.translator, EditingEvent("\xE3\x84\xB1", 1, 0));
    CHECK(imm::ImmGetCompositionStringW(himc, GCS_RESULTSTR, nullptr, 0) == 0);

    // a handle that is not the bound context
    CHECK(imm::ImmGetCompositionStringW(nullptr, GCS_COMPSTR, nullptr, 0) == IMM_ERROR_GENERAL);
}

TEST_CASE("the candidate window closes before the composition ends")
{
    SdlMessageTranslator translator;
    Translate(translator, EditingEvent("\xED\x95\x9C", 1, 0));
    const std::vector<const char*> page = {"\xE6\xBC\xA2", "\xE9\x9F\x93"};
    Translate(translator, CandidatesEvent(page, 0));

    auto commit = Translate(translator, TextEvent("\xE6\xBC\xA2"));
    REQUIRE(commit.size() == 4);
    CHECK(commit[0] == WinMessage{WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1});
    CHECK(commit[1].message == WM_IME_COMPOSITION);
    CHECK(commit[1].lParam == GCS_RESULTSTR);
    CHECK(commit[2].message == WM_IME_ENDCOMPOSITION);
    CHECK(commit[3].message == WM_CHAR);
    CHECK_FALSE(translator.Ime().CandidatesOpen());

    Translate(translator, EditingEvent("\xED\x95\x9C", 1, 0));
    Translate(translator, CandidatesEvent(page, 0));
    auto cancel = Translate(translator, EditingEvent("", -1, -1));
    REQUIRE(cancel.size() == 3);
    CHECK(cancel[0] == WinMessage{WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1});
    CHECK(cancel[2].message == WM_IME_ENDCOMPOSITION);
}

TEST_CASE("ImmNotifyIME cancels the composition and closes the candidate window")
{
    BoundTranslator bound;
    const HIMC himc = imm::ImmGetContext(nullptr);
    Translate(bound.translator, EditingEvent("\xED\x95\x9C", 1, 0));
    const std::vector<const char*> page = {"\xE6\xBC\xA2"};
    Translate(bound.translator, CandidatesEvent(page, 0));

    // KTDGUIIMEEditBox::FinalizeString
    CHECK(imm::ImmNotifyIME(himc, NI_COMPOSITIONSTR, CPS_CANCEL, 0));
    CHECK(bound.clears == 1);
    CHECK_FALSE(bound.translator.Composing());
    REQUIRE(bound.sent.size() == 1);
    CHECK(bound.sent[0] == WinMessage{WM_IME_ENDCOMPOSITION, 0, 0});
    CHECK(bound.translator.Ime().CandidatesOpen());  // as the Korean IME: the edit box closes it next
    CHECK(imm::ImmNotifyIME(himc, NI_CLOSECANDIDATE, 0, 0));
    REQUIRE(bound.sent.size() == 2);
    CHECK(bound.sent[1] == WinMessage{WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1});
    CHECK_FALSE(bound.translator.Ime().CandidatesOpen());

    // nothing to cancel or close: SDL is still told, but no message comes back
    CHECK(imm::ImmNotifyIME(himc, NI_COMPOSITIONSTR, CPS_CANCEL, 0));
    CHECK(imm::ImmNotifyIME(himc, NI_CLOSECANDIDATE, 0, 0));
    CHECK(bound.clears == 2);
    CHECK(bound.sent.size() == 2);

    // SDL cannot complete a composition on request; the SDL "" editing event after a cancel is a no-op
    CHECK_FALSE(imm::ImmNotifyIME(himc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0));
    CHECK(Translate(bound.translator, EditingEvent("", -1, -1)).empty());
}

TEST_CASE("the input context: association, open and conversion status, IME layouts")
{
    CHECK(imm::ImmGetContext(nullptr) == nullptr);  // nothing bound
    {
        BoundTranslator bound;
        const HIMC himc = imm::ImmGetContext(nullptr);
        REQUIRE(himc != nullptr);
        CHECK(imm::ImmReleaseContext(nullptr, himc));

        // KTDGUIIMEEditBox::OnFocusIn/OnFocusOut
        CHECK(imm::ImmAssociateContext(nullptr, nullptr) == himc);
        CHECK(imm::ImmGetContext(nullptr) == nullptr);
        CHECK(imm::ImmAssociateContext(nullptr, himc) == nullptr);
        CHECK(imm::ImmGetContext(nullptr) == himc);

        CHECK(imm::ImmGetOpenStatus(himc));
        CHECK(imm::ImmSetOpenStatus(himc, FALSE));
        CHECK_FALSE(imm::ImmGetOpenStatus(himc));

        DWORD conversion = 0;
        DWORD sentence = 1;
        CHECK(imm::ImmGetConversionStatus(himc, &conversion, &sentence));
        CHECK(conversion == IME_CMODE_NATIVE);
        CHECK(sentence == 0);
        CHECK(imm::ImmSetConversionStatus(himc, IME_CMODE_ALPHANUMERIC, 0));
        CHECK(imm::ImmGetConversionStatus(himc, &conversion, nullptr));
        CHECK(conversion == IME_CMODE_ALPHANUMERIC);

        char file[8] = "x";
        CHECK(imm::ImmGetIMEFileNameA(nullptr, file, sizeof(file)) == 0);
        CHECK(file[0] == '\0');
        CHECK(imm::ImmLockIMC(himc) == nullptr);
    }
    CHECK(imm::ImmGetContext(nullptr) == nullptr);  // unbound again

    const auto layout = [](unsigned value) { return reinterpret_cast<HKL>(static_cast<std::uintptr_t>(value)); };
    CHECK(imm::ImmIsIME(layout(0x04120412)));   // Korean
    CHECK(imm::ImmIsIME(layout(0x04110411)));   // Japanese
    CHECK(imm::ImmIsIME(layout(0xE00E0804)));   // Chinese (Simplified) MSPY
    CHECK_FALSE(imm::ImmIsIME(layout(0x04090409)));  // English (US)
}
