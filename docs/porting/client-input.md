[Home](../README.md) › [Porting](README.md) › Client: input

# Client: input (`KTDXLIB/KTDI*`, in `x2::client_ktdxlib`)

`KTDXLIB`'s input layer, `CKTDIManager` (a stage that reads the devices every frame and turns them into game actions)
and `CKTDIDevice` (keyboard, mouse or joystick), read DirectInput 8 in the original. In the port they keep their API and
read the same state from SDL3 through the [DXUT replacement](client-dxut.md). Overview of the client port:
[Client](client.md); the rest of the engine: [KTDXLIB](client-ktdxlib.md).

Ported on 2026-11-05 (`client/input`).

## What the original did

- `CKTDXApp` creates one `CKTDIManager( hWnd, hInstance )`, then `CreateKeyboard()`, `CreateMouse(TRUE)`,
  `CreateJoystic()`, `SetGamePadDefaultMap()`, `SetDefaultMap()`, and inserts it as a stage, so its `OnFrameMove` runs
  every frame.
- Devices: `DirectInput8Create`, `CreateDevice(GUID_SysKeyboard / GUID_SysMouse)` and the first attached game controller
  (`EnumDevices(DI8DEVCLASS_GAMECTRL)`), data formats `c_dfDIKeyboard`, `c_dfDIMouse`, `c_dfDIJoystick`, cooperative level
  `DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY`; joystick X/Y range -1024..1024 with a 1.28% dead zone
  (`DIPROP_DEADZONE` 128).
- **Immediate data only** (no buffered input): each frame `Poll` + `GetDeviceState` into a 256-byte buffer that also
  held the `DIMOUSESTATE` or `DIJOYSTATE` through casts. Without focus the read failed (`DIERR_NOTACQUIRED`, re-acquire
  failed) and the last state stayed until `DeactivateApp` released everything.
- Mouse: windowed, the position came from `GetCursorPos` + `ScreenToClient`; `lX`/`lY` (relative) and `lZ` (wheel, 120
  per notch) from DirectInput.
- `GetKeyState` reports a press once and **locks** the key until it is released (`GetPureKeyState` does not lock);
  `GetDoubleKeyState` detects press–release–press within 0.4 s. The manager does the same for `GAME_ACTION`s through an
  action map (action → `DIK_*`, or → `DIJOFS_BUTTON(n)` / `JB_*` pseudo-inputs for the joystick).

### What X2Lib calls (survey of 2026-11-05)

69 files of `X2Lib`, `X2` and `KTDXLIB` reach the layer, almost always as `g_pKTDXApp->GetDIManager()->...`:
`Getkeyboard()` (183 calls), `GetMouse()` (88), `GetJoystic()` (24), `SetEnable` (23), and a few each of
`SetKeyChanging`, `SetEnableJoyStick`/`GetEnableJoyStick`, `ListenDeviceInput`, `SetGamePadDefaultMap`, `SetDefaultMap`,
`IsActivated`, and the action getters (also through the `GET_KEY_STATE`… macros). On the devices: keyboard
`GetKeyState` (139), `SetAction` (9), `GetPureKeyState` (9), `SetLock` (7), `GetDoubleKeyState` (6), `GetNumKeyPresses`,
`SetExtraEnter`/`GetExtraEnter`, `GetActionKey`, `Clear`; mouse `GetXPos`/`GetYPos` (32 each), `GetXDelta` (9),
`GetButtonState` (8), `GetPureButtonState` (5), `GetYDelta`; joystick `SetAction`, `GetActionState`, `SetLock`,
`GetActionKeys`/`SetActionKeys`, `GetActionKey`, `SetExtraEnter`. **102 distinct `DIK_*` names** are used, all standard
`dinput.h` codes (7 of them alternate names: `DIK_BACKSPACE`, `DIK_DOWNARROW`, `DIK_LALT`, `DIK_NUMPADPERIOD`, `DIK_PGDN`, `DIK_RALT`, `DIK_UPARROW`). Outside the layer, only
`KTDGUIDialog.cpp` uses DirectInput itself (`DIJOFS_BUTTON`), and `KTDX.h` included `<dinput.h>`.

## The port

| File | Contents |
|---|---|
| `DXUT/DirectInputKeys.h` | What the client used from `dinput.h`: every `DIK_*` (with the alternate names), `DIMOUSESTATE`, `DIJOYSTATE` (80 bytes, checked) and `DIJOFS_*`. `KTDX.h` includes it instead of `<dinput.h>` |
| `DXUT/SdlInput.h/.cpp` | `x2::dxut`: the `DIK_*` ↔ SDL scancode table (`DirectInputKeys()`, `DirectInputKeyFromScancode`, `ScancodeFromDirectInputKey`); `InputState`, DirectInput's view of keyboard and mouse fed by SDL events; `JoystickReader`, the first SDL joystick as a `DIJOYSTATE` |
| `DXUT/DXUT.cpp` | the main loop passes every SDL event to the `InputState` (`DXUTGetInputState()`, a port addition) before translating it to messages; `DXUTInit` also starts SDL's gamepad subsystem (best effort) |
| `KTDXLIB/KTDIGameAction.h` | `GAME_ACTION` and `JOYSTICK_BUTTON`, moved out of `KTDIManager.h` so that `KTDIDevice.h` stands alone |
| `KTDXLIB/KTDIManager.*`, `KTDXLIB/KTDIDevice.*` | the original classes and logic, reading `InputState`/`JoystickReader` instead of DirectInput |

- **Key table.** A `DIK_*` is the set-1 scan code with 0x80 for E0-prefixed keys, except Pause (`0xC5`) and Num Lock
  (`0x45`), whose extended bit is the other way round from `WM_KEYDOWN`. Every code maps to an SDL scancode except 11
  keys SDL does not know (seven NEC PC-98 keys, `DIK_ABNT_C2`, `DIK_CALCULATOR`, `DIK_MYCOMPUTER`, `DIK_MAIL`); none of them
  is used.
  The ISO key left of Enter (`SDL_SCANCODE_NONUSHASH`) reads as `DIK_BACKSLASH`, as Windows reports it.
- **Keyboard and mouse** come from SDL events of the client window, not from polling the OS: keys by scancode (the
  physical key, as DirectInput), buttons left/right/middle/X1 as DirectInput's 0–3, motion and wheel accumulated between
  reads (pixels × pixel density; the wheel in notches × 120, `SDL_MOUSEWHEEL_FLIPPED` honored) with the fractions kept for
  the next read, and the cursor in client pixels for windowed mode and `UpdateMouseDeltaPosition`. Motion deltas are SDL's
  (accelerated like the cursor) rather than DirectInput's raw mickeys.
- **Focus** stands for acquisition: without it `Read` fails and keeps the last state, as before; losing focus also empties
  `InputState`, so nothing is held when focus returns.
- **Joystick.** An SDL gamepad is reported the way DirectInput reported an XInput controller: left stick on X/Y, both
  triggers on Z (left above 32767, right below, which is what `AXIS_THRESHOLD_TRIGGER_LEFT/RIGHT` expect), right stick on
  Rx/Ry, the d-pad as POV 0, buttons A, B, X, Y, LB, RB, Back, Start, left stick, right stick. Other joysticks report
  their axes (X, Y, then Z, Rx, Ry, Rz, sliders), first four hats and 32 buttons in order. The default pad map
  (`SetGamePadDefaultMap`) is unchanged.
- The joystick action map still stores `DIJOFS_*` offsets; `GetActionState` reads that byte of the `DIJOYSTATE`
  (`JoystickByte`), as the original did through the aliased buffer. The three device buffers are now separate members.

### Behavior changes

- **Joystick hot plug**: the original looked for a joystick once, at start-up, and logged `KEM_ERROR54` without one;
  the port opens the first joystick on each `Read` while none is open and drops it when unplugged.
- With no joystick, or before the first read, the joystick state is **neutral** (POVs centered, Z at 32767), where the
  original's cleared buffer had POV 0 ("up") and Z 0 ("right trigger"); the original never read that buffer without a
  device, the port's device always exists.
- `CKTDIManager`'s `m_Locks` were left uninitialized by the original; they start `false`. `Create*` deletes a previous
  device instead of leaking it.
- Removed because nothing calls them: `CKTDIDevice::GetKeypress` (a busy-wait on `ToAsciiEx`), `DeviceCOM`,
  `EnumJoysticks`, the declared but never defined `CKTDIManager::UpdateActionState`, and the `KEY_MAPPING_UPGRADE` and
  `DLL_BUILD` branches (never defined in the client). `Acquire` only reports whether a device exists.
  `KTDIDeviceWinKey.*` were empty after the import and are deleted.
- The `for each` (MSVC extension) in `SetActionKeys` is a range `for`.

## Build and tests

`KTDIDevice.cpp` and `KTDIManager.cpp` first built alone as `x2::client_ktdi` (2026-11-05), with `CKTDXStage` from a test
stub; since 2026-11-06 they are part of `x2::client_ktdxlib` ([KTDXLIB › Whole engine](client-ktdxlib.md#whole-engine-2026-11-06-clientktdxlib-build)).

Tests (`x2_client_tests`, ctest `client`), headless, joysticks through SDL virtual devices:

- `test_input.cpp`: every `DIK_*` spelled in `X2`, `X2Lib` and `KTDXLIB` is in the table and has a scancode (the test
  scans the sources); the table round-trips; `DIK_*` equals the scan code of `WM_KEYDOWN` (`ToVirtualKey`) except Pause
  and Num Lock; keyboard and mouse state, fractional deltas, wheel, focus; a virtual gamepad and a virtual plain joystick
  read as `DIJOYSTATE`, unplugging.
- `test_ktdi.cpp`: `CKTDIManager` set up as `CKTDXApp` does: key locks, double presses (and the 0.4 s limit), actions
  through the default key map, remapping, `GetActionKeys`/`SetActionKeys`, `ListenDeviceInput`, focus loss with
  `DeactivateApp`, `SetEnable`, mouse position/delta/buttons/wheel, and gamepad actions through the default pad map.

## Open

- Exclusive full screen (`CreateMouse(FALSE)`, relative mouse) is not used by the client and was not tried.
