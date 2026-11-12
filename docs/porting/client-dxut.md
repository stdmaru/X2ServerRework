[Home](../README.md) › [Porting](README.md) › Client: DXUT

# Client: DXUT replacement (`src/client/DXUT`, target `x2::client_dxut`)

The original client runs on DXUT, the Direct3D 9 sample framework: `X2.cpp` installs callbacks (`DXUTSetCallback*`), calls
`DXUTInit`/`DXUTCreateWindow`/`DXUTCreateDevice` and `DXUTMainLoop`, and the engine asks DXUT for the window, the timer and the
device state. DXUT is not imported (it is a vendored Microsoft library, excluded in `client-ktdxlib`). In its place,
`src/client/DXUT` keeps the **names and signatures** the client calls, so `X2.cpp` and `KTDXLIB` compile against it unchanged,
and implements them on `x2::app` (SDL3, T32) and [`x2::render`](render.md) (T36). Overview of the client port: [Client](client.md).

Added on 2026-11-05 (`client/skeleton`).

## Files

| File | Contents |
|---|---|
| `DXUT.h` / `DXUT.cpp` | The DXUT API the client uses: callback typedefs and setters (`FrameMove`, `FrameRender`, `Keyboard`, `MsgProc`; `DeviceCreated`, `DeviceReset`, `DeviceLost`, `DeviceDestroyed` since 2026-11-12), `DXUTInit`, `DXUTCreateWindow`, `DXUTCreateDevice`, `DXUTMainLoop`, `DXUTStaticWndProc`, `DXUTShutdown`, `DXUTToggleFullScreen`, `DXUTSetCursorSettings`, `DXUTSetMultimonSettings`, getters (`DXUTGetHWND`, `DXUTGetHINSTANCE`, `DXUTGetTime`, `DXUTGetElapsedTime`, `DXUTGetFPS`, `DXUTGetFrameStats`, `DXUTGetDeviceStats`, `DXUTIsWindowed`, `DXUTIsActive`, `DXUTGetExitCode`, `DXUTIsKeyDown`, `DXUTIsMouseButtonDown`), `CDXUTTimer` + `DXUTGetGlobalTimer`, `DXUTTRACE`/`DXUTOutputDebugString`, no-op `DXUT_BeginPerfEvent`/`EndPerfEvent`. Port additions: `DXUTGetSDLWindow`, `DXUTGetRenderDevice` (2026-11-06), `DXUTGetInputState`; `DXUTInit` also starts SDL's gamepad subsystem (best effort) |
| `SdlMessages.h` / `SdlMessages.cpp` | `x2::dxut::SdlMessageTranslator`: SDL3 events → Win32 messages (`WM_*` with Windows `wParam`/`lParam`), and `ToVirtualKey` (SDL scancode/keycode → `VK_*`) |
| `Ime.h` / `Ime.cpp` | `x2::dxut::ImeContext` (the IME state the translator fills) and `x2::dxut::imm`, IMM32 for the IME edit box on it (2026-11-05, [Client: IME](client-ime.md)) |
| `WindowFunctions.h` / `WindowFunctions.cpp` | The Win32 window and keyboard functions the engine's UI calls, on SDL outside Windows (the real ones on Windows): `SetCapture`/`ReleaseCapture`/`GetCapture` (`SDL_CaptureMouse`), `GetKeyState`/`GetAsyncKeyState`, `GetFocus`, `GetClientRect`, `GetCaretBlinkTime` (530 ms), `SystemParametersInfoW(SPI_GETWHEELSCROLLLINES)` (3); the keyboard layout fixed to en-US (`GetKeyboardLayout`, `GetKeyboardLayoutList`, `GetLocaleInfoW` for 0x0409). On every platform, `DXUTGetClipboardText`/`DXUTSetClipboardText` on SDL's clipboard, used by the edit boxes (2026-11-06) |
| `Camera.h` / `Camera.cpp` | `CFirstPersonCamera`, DXUT's free camera for X2Lib's debug free camera: WASD/arrows/numpad move, Q/E and PgUp/PgDn up and down, Z/C roll, X levels, Home resets; the mouse turns it with the rotate button down (`WM_MOUSEMOVE`); `SetViewParams`, `SetProjParams`, `SetScalers`, `SetRotateButtons`, the view and world vectors (2026-11-07) |
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
  The frame is drawn by the callbacks through the D3D9 device; a frame they do not draw is presented cleared to black
  (2026-11-06); after `FrameRender` it presents through KTDXLIB's [D3D9 device](client-d3d9.md), which the callbacks receive (`DXUTGetD3DDevice()`; 2026-11-06, T38).
- **Renderer**: `DXUTCreateWindow` creates the `x2::render` device with the window (`DXUTGetRenderDevice`), claims the window
  and asks for vsync (`D3DPRESENT_INTERVAL_DEFAULT`); the device is destroyed with the window (`WM_CLOSE`, `DXUTShutdown`), never
  by DXUT's static state at `exit()`: there the Vulkan validation layer had already torn itself down and aborted in
  `SDL_WaitForGPUIdle` (a crash on closing the skeleton, fixed 2026-11-06). The backend comes from
  `X2_RENDER_BACKEND`: `sdlgpu` (default) or `null`, which runs the loop headless with no GPU. If the window cannot be claimed,
  creation fails.
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
  `pbNoFurtherProcessing`, the timer, window creation failing cleanly with `sdlgpu` and no GPU, and the main loop on the `null`
  renderer (two frames, each cleared to black and submitted, nothing leaked); the device callbacks (Created, Reset, then
  Lost and Destroyed while the device still exists, once each; `DXUTCreateDevice` fails without a window).
- `test_luacommon.cpp`: [LuaCommon](client-luacommon.md) loads a UTF-8 script and reads its values.

Device callbacks (2026-11-12): `DXUTCreateDevice` calls `DeviceCreated` then `DeviceReset` with the window's device.
If the window is windowed and a size is given, it first sets the client area to that size. When the window closes,
`DeviceLost` then `DeviceDestroyed` run before the device is destroyed (DXUT's `DXUTCleanup3DEnvironment`). The device is
never lost or reset in between (T38), so each one runs once. A failing `DeviceCreated`/`DeviceReset` closes the window
and `DXUTCreateDevice` fails.
