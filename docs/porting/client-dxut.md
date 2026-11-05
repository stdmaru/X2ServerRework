[Home](../README.md) › [Porting](README.md) › Client: DXUT

# Client: DXUT replacement (`src/client/DXUT`, target `x2::client_dxut`)

The original client runs on DXUT, the Direct3D 9 sample framework: `X2.cpp` installs callbacks (`DXUTSetCallback*`), calls
`DXUTInit`/`DXUTCreateWindow`/`DXUTCreateDevice` and `DXUTMainLoop`, and the engine asks DXUT for the window, the timer and the
device state. DXUT is not imported (it is a vendored Microsoft library, excluded in `client-ktdxlib`). In its place,
`src/client/DXUT` keeps the **names and signatures** the client calls, so `X2.cpp` and `KTDXLIB` compile against it unchanged,
and implements them on `x2::app` (SDL3, T32). Overview of the client port: [Client](client.md).

Added on 2026-11-05 (`client/skeleton`).

## Files

| File | Contents |
|---|---|
| `DXUT.h` / `DXUT.cpp` | The DXUT API the client uses: callback typedefs and setters (`FrameMove`, `FrameRender`, `Keyboard`, `MsgProc`), `DXUTInit`, `DXUTCreateWindow`, `DXUTMainLoop`, `DXUTStaticWndProc`, `DXUTShutdown`, `DXUTToggleFullScreen`, `DXUTSetCursorSettings`, `DXUTSetMultimonSettings`, getters (`DXUTGetHWND`, `DXUTGetHINSTANCE`, `DXUTGetTime`, `DXUTGetElapsedTime`, `DXUTGetFPS`, `DXUTIsWindowed`, `DXUTIsActive`, `DXUTGetExitCode`, `DXUTIsKeyDown`, `DXUTIsMouseButtonDown`), `CDXUTTimer` + `DXUTGetGlobalTimer`, `DXUTTRACE`/`DXUTOutputDebugString`, no-op `DXUT_BeginPerfEvent`/`EndPerfEvent`. Port additions: `DXUTGetSDLWindow`, `DXUTGetInputState`; `DXUTInit` also starts SDL's gamepad subsystem (best effort) |
| `SdlMessages.h` / `SdlMessages.cpp` | `x2::dxut::SdlMessageTranslator`: SDL3 events → Win32 messages (`WM_*` with Windows `wParam`/`lParam`), and `ToVirtualKey` (SDL scancode/keycode → `VK_*`) |
| `Ime.h` / `Ime.cpp` | `x2::dxut::ImeContext` (the IME state the translator fills) and `x2::dxut::imm`, IMM32 for the IME edit box on it (2026-11-05, [Client: IME](client-ime.md)) |
| `DirectInputKeys.h`, `SdlInput.h` / `SdlInput.cpp` | DirectInput's view for KTDXLIB's input layer: `DIK_*` and the device state structs, the `DIK_*` ↔ SDL scancode table, `InputState` (fed by the main loop, `DXUTGetInputState()`) and `JoystickReader` (2026-11-05, [Client: input](client-input.md)) |

The `WM_*`/`VK_*` constants and `WPARAM`/`LPARAM` come from `x2/platform/window_messages.h` ([Platform](platform.md)).
`SAFE_DELETE`/`SAFE_RELEASE`/`SAFE_DELETE_ARRAY` are **not** here: `KTDX.h` defines them (behind `#ifndef`), so DXUT's
`DXUTmisc.h` was never needed for them. `KTDX.h` now includes only `DXUT.h` instead of the six DXUT headers.

## Behavior kept from DXUT

- **Window procedure order**: keyboard callback (for `WM_KEYDOWN`/`WM_SYSKEYDOWN`/`WM_KEYUP`/`WM_SYSKEYUP`), then `MsgProc` with
  `pbNoFurtherProcessing`; if `MsgProc` does not stop the message, DXUT's own handling: `WM_SIZE` minimized/restored (pauses the
  loop), `WM_ACTIVATEAPP` (active flag), Alt+Enter toggles full screen when enabled by `DXUTInit` (on in `X2.cpp`), `WM_CLOSE`
  destroys the window and quits.
- `DXUTCreateWindow` creates a resizable window with a 1024×768 client area (what `X2.cpp` asks of `DXUTCreateDevice`) and starts
  SDL text input; it binds the translator's IME context for the IME edit box's IMM32 calls, and `DXUTInit` sets
  `SDL_HINT_IME_IMPLEMENTED_UI` so the client draws the composition and the candidates. Full screen is borderless desktop full screen.
- Main loop: events → messages → frame (`CDXUTTimer` → `FrameMove` → `FrameRender`); while minimized it waits for events (100 ms).
  For now the frame only clears to black through `x2::app` (SDL_GPU, T19); the callbacks receive a null `IDirect3DDevice9*`.
- `CDXUTTimer` on `steady_clock` with DXUT's semantics (`Stop`/`Start`/`Advance` by 0.1 s, `GetTime`, `GetElapsedTime`).
- `DXUTGetHWND` returns the real `HWND` on Windows (`SDL_PROP_WINDOW_WIN32_HWND_POINTER`, for IME and `MessageBox` calls that stay
  Win32) and the `SDL_Window*` as an opaque handle elsewhere.

## SDL events → Win32 messages

| SDL | Messages |
|---|---|
| `KEY_DOWN`/`KEY_UP` | `WM_KEYDOWN`/`WM_KEYUP`, or `WM_SYSKEY*` when Alt is down without Ctrl, for F10, and for Alt itself; `lParam` = repeat 1, set-1 scan code, extended bit (24), context bit (29), previous state (30), transition (31). Generic `VK_SHIFT`/`VK_CONTROL`/`VK_MENU`; letters and digits follow the keyboard layout; keypad keys depend on Num Lock |
| `TEXT_INPUT` | one `WM_CHAR` per UTF-16 unit; while composing, first `WM_IME_COMPOSITION` (`GCS_RESULTSTR`) and `WM_IME_ENDCOMPOSITION`. The `WM_CHAR`s of a result are dropped if `MsgProc` handled the composition result, as `DefWindowProc` would not generate them |
| `TEXT_EDITING` | `WM_IME_STARTCOMPOSITION`, `WM_IME_COMPOSITION` (`GCS_COMPSTR`, `GCS_COMPATTR`, `GCS_CURSORPOS`); empty text ends the composition. The composition (string, cursor, attributes) is in the translator's `ImeContext`, which IMM32 calls read ([Client: IME](client-ime.md)) |
| `TEXT_EDITING_CANDIDATES` | `WM_IME_NOTIFY` (`IMN_OPENCANDIDATE`, `IMN_CHANGECANDIDATE`, `IMN_CLOSECANDIDATE`); a list still open closes before the composition ends ([Client: IME](client-ime.md)) |
| `MOUSE_MOTION`, `MOUSE_BUTTON_*`, `MOUSE_WHEEL` | `WM_MOUSEMOVE`, `WM_[LRMX]BUTTONDOWN/UP`, `WM_*BUTTONDBLCLK` on even click counts, `WM_MOUSEWHEEL` (delta × 120, flipped for `SDL_MOUSEWHEEL_FLIPPED`, screen coordinates). Coordinates in pixels (× pixel density); `MK_*` in `wParam` |
| `WINDOW_FOCUS_GAINED`/`LOST` | `WM_ACTIVATEAPP` + `WM_SETFOCUS` / mouse buttons released, `WM_KILLFOCUS`, `WM_ACTIVATEAPP(FALSE)` |
| `WINDOW_MINIMIZED`/`MAXIMIZED`/`RESTORED`/`PIXEL_SIZE_CHANGED` | `WM_SIZE` (`SIZE_MINIMIZED`/`SIZE_MAXIMIZED`/`SIZE_RESTORED`, pixel size) |
| `WINDOW_CLOSE_REQUESTED`, `QUIT` | `WM_CLOSE` |

Events of other windows are ignored. The same events also update the DirectInput view that `CKTDIManager` reads
([Client: input](client-input.md)); with `RELEASE_ALL_BUTTONS_AND_KEYS_WHEN_FOCUS_LOSING` true, the client releases its
state on `WM_ACTIVATEAPP`.

## Tests

`src/client/tests` (`x2_client_tests`, ctest `client`), headless with SDL's dummy video driver (no window, no GPU):

- `test_messages.cpp`: key messages and `lParam` bits, SYS keys, virtual keys and layouts, text and IME composition, mouse
  buttons, double clicks, wheel, coordinates with pixel density, focus, size and close.
- `test_dxut.cpp`: callback order (keyboard, then `MsgProc`), key and mouse state, `WM_ACTIVATEAPP` with and without
  `pbNoFurtherProcessing`, the timer, and window creation failing cleanly without a GPU.
- `test_luacommon.cpp`: [LuaCommon](client-luacommon.md) loads a UTF-8 script and reads its values.

To see real events, run the skeleton with `--trace-messages` ([Client: X2](client-x2.md)).
