[Home](../README.md) › [Tools](README.md) › UI editor

# UI editor (`src/tools/UIEditor`, `x2_ui_editor`)

An SDL3 + ImGui tool of the client development suite ([T29](../project/decisions.md), [App foundation](../porting/app.md))
that draws the client's dialogs with the client's own GUI code (KTDXLIB's `CKTDGUIDialog`, `CKTDGUIControl`,
`UIAnchors.lua`), so a dialog looks exactly as in the game, at any resolution and aspect. How it works and where it
saves are decision [T46](../project/decisions.md). Built only with `X2_DEV_TOOLS` (it reads the client_data's loose
files, T35 (3)), when the client and the tools are built.

```bash
x2_ui_editor <game folder> [--open DLG_x.lua]... [--size 1920x1080]   # the client folder: .kom and data/ (client_data)
```

In the build: `build/linux-debug/src/tools/UIEditor/x2_ui_editor`. Like the client, it is static apart from libc, so the
container's build runs on the desktop.

## Window

| Panel | Contents |
|---|---|
| Dialogs (left) | Every `DLG_*.lua` under `data/`, filtered by name; dimmed when the recording never saw it. Double-click opens it alone, Ctrl + double-click adds it to the open ones (a screen made of several dialogs). The open dialogs, to select or close |
| Screen (middle) | The render target at the chosen aspect (4:3, 16:10, 16:9, 21:9) and height (720–2160), fitted to the panel or at 100%. Hovering shows the control under the pointer and the layout coordinates; a click selects it; "all boxes" outlines every drawn control |
| Inspector (right) | The dialog: script, name, anchor (declared or the default), position and where it is drawn, the recorded positions (a click moves the dialog there). Its controls by name (`#<index>` when unnamed) with type and declared anchor. The selected control: anchor, offset, what it drew; for statics, each picture's texture, `.tet` key, corner points and UV in texels |

Slice (a) only inspects; editing the anchor (b) and positions, textures and UV (c) follow ([roadmap](../project/roadmap.md#phase-4--client-graphics)).

## How it works

- **Engine** (`engine.h`, `x2_ui_editor_core`): the real `CKTDXApp` on a window-less `x2::d3d9::Device` over the
  window's SDL_GPU device. Each frame the open dialogs step and draw (what `CX2State` does for the UI: the dialog
  manager's step, the object chain and the front UI) into a D3D9 render target of the chosen size; the D3D9 frame is
  submitted first, then ImGui samples the target's SDL_GPU texture (`x2::render::sdlgpu::Native`) in its own command
  buffer. A size change goes through `CKTDXApp::OnResetDevice`, so the viewport, the UI canvas (T44) and the projection
  follow it as in the game.
- **Start-up**, as `CX2Main`'s: the working folder becomes the game folder, the archives mount (`AddMassFiles`, with
  dev builds preferring the client_data), then `Enum.lua`, `DungeonEnum.lua`, `StringID_def.lua` and
  `Config_INTERNAL.lua` (its fonts) run. X2Lib is not linked: `g_pMain` is a Lua stub whose `InstallFont` installs the
  font and whose other methods do nothing; `g_pCX2SlotManager` and the other game globals a few scripts touch
  (2026-11-14: `g_pData`, `g_pState`, `g_pCouponBox`, `g_pMapSelectWindow`, `g_pPostBoxInventory`) accept any call, so
  item-slot dialogs build everything but their slots.
- **Control boxes**: `CKTDGUIControl` notes what it draws (its quads, transparent ones skipped) when the anchor recorder
  is on or a tool asks for it (`CKTDGUIAnchorRecorder::SetToolTracking`); the editor reads them without writing the
  record file. Boxes are in layout units relative to the dialog and are mapped to pixels with the control's anchor and
  the dialog's stretch (`ControlXToDialogX`), the dialog's drawn position and the canvas scale and origin.
- **Positions of dialogs placed by code** (T46 (3)): until a screen's layout positions move into its scripts, a dialog
  recorded by `X2_RECORD_UI_ANCHORS` ([UI anchors](ui-anchors.md)) opens at its first recorded position
  (`record.h` reads `UIAnchorsRecord.tsv` in the game folder); the others are listed in the inspector. A dialog never
  recorded opens at its script's position.

## Tests (`ctest -R ui_editor`)

`x2_ui_editor_tests`, headless on the null render backend: the script list (sorted, `.kom/` skipped), the record file
(positions by script without folder or case, each once, placed by code), and the engine on a temporary game folder
with loose files only: a missing script refused; a dialog opened, drawn at 1280×720 and at 1024×768, its controls named
(`#1` when unnamed), boxes in layout units and pixels, the slot manager stub; moved and closed.

## Status

2026-11-14: slice (a), the viewer, in progress (`tools/ui-editor`, PR #74).
