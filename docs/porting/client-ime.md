[Home](../README.md) › [Porting](README.md) › Client: IME

# Client: IME (`src/client/DXUT/Ime.*`, `x2/platform/ime.h`)

How the client's IME edit box gets the composition string and the candidate list in the port. The edit box itself is
part of [KTDXLIB](client-ktdxlib.md); the SDL3 events reach it as Win32 messages through the
[DXUT replacement](client-dxut.md). Overview of the client port: [Client](client.md).

Added on 2026-11-05 (`client/ime-candidates`).

## What the original did (survey of 2026-11-05)

`CKTDGUIIMEEditBox` (`KTDXLIB/KTDGUIIMEEditBox.*`, ~3.9k lines) is DXUT's `CDXUTIMEEditBox` with KOG's changes: it hides
the system IME windows (`WM_IME_SETCONTEXT`) and draws the composition string, the candidate list and the reading window
of old Chinese IMEs itself. It reads the IME only through IMM32, loaded at run time: `Initialize()` took 20 functions
from `imm32.dll` (and 3 from `version.dll`) into static `_Imm*` pointers, which point to empty `Dummy_*` versions when
the DLL is missing.

| Message | What the edit box reads |
|---|---|
| `WM_IME_STARTCOMPOSITION` | nothing; resets its composition string and hides its caret (`StaticMsgProc`) |
| `WM_IME_COMPOSITION` | `ImmGetCompositionStringW`: `GCS_RESULTSTR` (sent to itself as `WM_CHAR`s, then the message is trapped), `GCS_COMPSTR`, `GCS_CURSORPOS`, `GCS_COMPATTR` (target clause colors), `GCS_COMPCLAUSE` (Japanese clicks) |
| `WM_IME_ENDCOMPOSITION` | nothing; clears the composition |
| `WM_IME_NOTIFY` `IMN_OPENCANDIDATE` / `IMN_CHANGECANDIDATE` | `ImmGetCandidateListW(himc, 0, ...)`: `dwCount`, `dwSelection`, `dwPageStart`, `dwPageSize` (at most `MAX_CANDLIST` = 10) and the strings of one page, which it numbers itself (`1`..`9`, `0`). Korean and old CHT IMEs show no selection; Korean lists are horizontal, Chinese and Japanese vertical |
| `WM_IME_NOTIFY` `IMN_CLOSECANDIDATE` | nothing; hides the list |
| `WM_IME_NOTIFY` `IMN_SETCONVERSIONMODE` / `IMN_SETOPENSTATUS` | `ImmIsIME`, `ImmGetOpenStatus`, `ImmGetConversionStatus` (`CheckToggleState`, the indicator) |
| `WM_IME_NOTIFY` `IMN_PRIVATE` | old Chinese IMEs' reading string (`GetReadingString` from the IME file, or `ImmLockIMC`/`ImmLockIMCC` private data) |
| `WM_INPUTLANGCHANGE`, `WM_ACTIVATEAPP` | `GetKeyboardLayout`, `GetKeyboardLayoutList`, `GetLocaleInfoW`, `ImmGetIMEFileNameA` + `version.dll` (old Chinese IME versions) |

It also calls `ImmNotifyIME(NI_COMPOSITIONSTR, CPS_CANCEL)` and `ImmNotifyIME(NI_CLOSECANDIDATE)` to drop the
composition (`FinalizeString`, focus loss, clicks in the composition) and `ImmAssociateContext` (the window's context
on focus in, `NULL` on focus out). Outside the edit box, only `CX2Main::ChangeKoreanAndEnglishKey` and
`GetNativeAndEnglisKeyState` (`X2Lib/X2Main.cpp`) call IMM32 directly (`ImmGetContext`, `ImmSet/GetConversionStatus`).

## The port

SDL3 owns the IME on every platform (on Windows it handles the IMM32 messages in its own window procedure), so IMM32 is
**emulated on what SDL reports**, the same on Windows and Linux, and the `MsgProc(HWND, UINT, WPARAM, LPARAM)` model
stays (no decision needed):

| File | Contents |
|---|---|
| `x2/platform/ime.h` | the IMM32 types and constants: `HIMC`, `HIMCC`, `HKL`, `CANDIDATELIST`, `INPUTCONTEXT`, `IMN_*`, `NI_*`, `CPS_*`, `IME_CMODE_*`, `ATTR_*`, `IMM_ERROR_*` and the `LANG_*`/`SUBLANG_*` IDs, with the Windows x64 layouts. `imm.h`/`immdev.h` on Windows; no IMM32 function is declared or linked |
| `DXUT/Ime.h/.cpp` | `x2::dxut::ImeContext`, the input context: composition (string, cursor, attributes, result), the candidate page, association, open and conversion status. `x2::dxut::imm`: the 20 IMM32 functions the edit box loads, same names and signatures, on the bound `ImeContext` (the `HIMC` is the context itself) |
| `DXUT/SdlMessages.*` | the translator owns the `ImeContext` and fills it from `TEXT_EDITING`, `TEXT_INPUT` and `TEXT_EDITING_CANDIDATES` |
| `DXUT/DXUT.cpp` | `DXUTInit` sets `SDL_HINT_IME_IMPLEMENTED_UI` to `composition,candidates`; `DXUTCreateWindow` binds the translator's context, with `SDL_ClearComposition` and the window procedure as what the IME does on request |
| `KTDXLIB/KTDGUIIMEEditBox.cpp` | `Initialize()` points the `_Imm*` pointers at `x2::dxut::imm` (`EMULATEDPROC`, `KTDGUIUniBuffer.h`) instead of loading `imm32.dll`; `version.dll` is not loaded. The rest of the edit box is unchanged |

### SDL events → IME messages

| SDL | Messages (in addition to the composition ones in [DXUT](client-dxut.md#sdl-events--win32-messages)) |
|---|---|
| `TEXT_EDITING_CANDIDATES` with candidates | `WM_IME_NOTIFY(IMN_OPENCANDIDATE, 1)` the first time, then `IMN_CHANGECANDIDATE` |
| `TEXT_EDITING_CANDIDATES` without candidates | `WM_IME_NOTIFY(IMN_CLOSECANDIDATE, 1)` if a list was open |
| `TEXT_INPUT` or empty `TEXT_EDITING` ending a composition with a list open | `IMN_CLOSECANDIDATE` first, then the result/end of the composition, as Windows orders them |

What the functions answer:

- `ImmGetCompositionStringW`: `GCS_COMPSTR`, `GCS_RESULTSTR` (the last committed text, until the next composition
  starts), `GCS_CURSORPOS`, `GCS_COMPATTR` (`ATTR_TARGET_CONVERTED` over SDL's selected range — `start`/`length` of
  the editing event — and `ATTR_INPUT` elsewhere), `GCS_COMPCLAUSE` (one clause), `GCS_DELTASTART` 0, `IMM_ERROR_NODATA`
  for the reading strings. Sizes in bytes of the platform's `WCHAR` (4 on Linux), as the edit box divides by
  `sizeof(WCHAR)`; a zero-length buffer asks for the size, a short one gets what fits.
- `ImmGetCandidateListW(himc, 0, ...)`: the page SDL sent as a whole list (`dwPageStart` 0, `dwPageSize` = `dwCount`),
  strings after the offsets, the size first when asked; 0 for other lists or a buffer too small.
- `ImmNotifyIME`: `NI_COMPOSITIONSTR`+`CPS_CANCEL` clears the composition, calls `SDL_ClearComposition` and, if one was
  open, sends `WM_IME_ENDCOMPOSITION` to the window procedure (synchronously, as the IME did); `NI_CLOSECANDIDATE` closes
  the list and sends `IMN_CLOSECANDIDATE`. Other actions return `FALSE`: SDL cannot complete or convert on request.
- `ImmGetContext` returns the context while associated; `ImmAssociateContext` keeps the flag and returns the previous one.
- `ImmGetOpenStatus`/`ImmSetOpenStatus` and `ImmGet/SetConversionStatus` keep the values (open, `IME_CMODE_NATIVE` at
  start). `ImmIsIME` is true for Chinese, Japanese and Korean layouts.
- No IME file (`ImmGetIMEFileNameA` 0), so no old-Chinese reading-string API and no private data (`ImmLockIMC` `NULL`);
  `ImmDisableTextFrameService` succeeds; the rest do what IMM32 does without an IME.

### SDL behavior handled

- **Only SDL's Windows backend sends candidates** (SDL 3.4.18). On Linux, ibus and fcitx keep drawing their own
  candidate window; the composition still comes as `TEXT_EDITING` (with the hint, ibus and fcitx send the preedit text to the client instead of drawing it).
- SDL's Windows backend writes each candidate as `"<digit> <candidate>"`. The prefix is dropped when every entry has one
  counting up from the first (mod 10), because the edit box numbers the candidates itself.
- It sends **one page but the selection as an index into the whole list** (`dwSelection`). A selection past the page is
  taken modulo the page size, the longest page seen since the list opened (pages start at multiples of it; the last may
  be shorter). Without a selection (`-1`) the list reports the first entry, as Windows lists always have one; the edit
  box drops it for Korean anyway.

## Behavior changes

- The candidate list is always one page: the edit box's Japanese branch (`nPageTopIndex` from the selection) and the
  `dwPageStart` branch both land on it.
- `ImmAssociateContext(NULL)` (no IME edit box focused) only stops the edit box from seeing a context; SDL keeps
  composing, so text typed outside an IME edit box arrives as the IME's committed characters rather than raw keys.
- Old Chinese IMEs' reading window (`IMN_PRIVATE`, `GetReadingString`) is not available.

## Tests

`src/client/tests/test_ime.cpp` (`x2_client_tests`, ctest `client`), headless: candidate lists open/change/close as
`WM_IME_NOTIFY`; SDL's prefixes and whole-list selections, short last pages; the `CANDIDATELIST` layout and sizes;
`ImmGetCompositionStringW` for every index; candidate windows closing before the composition ends; `ImmNotifyIME`
cancel/close with the messages sent back; association, open and conversion status, `ImmIsIME`; and, at compile time,
that every `x2::dxut::imm` function has the type of the edit box's pointer. `test_dxut.cpp` checks the hint.

## Open

- The edit box is not compiled yet (with the rest of KTDXLIB's GUI): `Initialize()` is unverified until then.
- Keyboard-layout language for the edit box (`GetKeyboardLayout`, `GetKeyboardLayoutList`, `GetLocaleInfoW`,
  `WM_INPUTLANGCHANGE`) on Linux, where SDL reports none.
- `CX2Main`'s Korean/English toggle calls IMM32 directly; it has to use `x2::dxut::imm` when X2Lib compiles. SDL cannot
  set the IME's conversion mode, so the toggle only changes the emulated status.
- On Linux, place the IME's own candidate window at the composition (`SDL_SetTextInputArea` from the edit box's
  `s_ptCompString`) once the GUI renders.
- To check with a real Korean/Japanese IME on Windows and with ibus/fcitx on Linux once the edit box draws.
