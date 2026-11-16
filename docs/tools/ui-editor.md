[Home](../README.md) › [Tools](README.md) › UI editor

# UI editor (`src/tools/UIEditor`, `x2_ui_editor`)

An SDL3 + ImGui tool of the client development suite ([T29](../project/decisions.md), [App foundation](../porting/app.md))
that draws the client's dialogs with the client's own GUI code (KTDXLIB's `CKTDGUIDialog`, `CKTDGUIControl`,
`UIAnchors.lua`), so a dialog looks exactly as in the game, at any resolution and aspect. How it works and where it
saves are decision [T46](../project/decisions.md). Built only with `X2_DEV_TOOLS` (it reads the client_data's loose
files, T35 (3)), when the client and the tools are built.

```bash
x2_ui_editor <game folder> [--open DLG_x.lua]... [--size 1920x1080]   # the client folder: .kom and data/ (client_data)
X2_UI_LIVE=1 X2_RECORD_UI_ANCHORS=1 ./x2                               # in the game folder: what "Start game" runs
```

In the build: `build/linux-debug/src/tools/UIEditor/x2_ui_editor`. Like the client, it is static apart from libc, so the
container's build runs on the desktop.

## Window

| Panel | Contents |
|---|---|
| Dialogs (left) | "Watch game" and "Start game" ([below](#watch-game)). "Undo", "Redo", "Save" (UIAnchors.lua, the dialog scripts and the `.tet` files edited; Ctrl+S) and whether there are unsaved changes ([below](#editing-anchors)). A scene (a game state, `XS_*`) narrows the list to the scripts the recording saw in it, and "Open scene" opens them all at their recorded positions. Every `DLG_*.lua` under `data/`, filtered by name; dimmed when the recording never saw it. Double-click opens it alone, Ctrl + double-click adds it to the open ones (a screen made of several dialogs). The open dialogs, to select or close |
| Screen (middle) | The render target at the chosen aspect (4:3, 16:10, 16:9, 21:9) and height (720–2160), fitted to the panel or zoomed with the wheel around the pointer (10%–800%, "100%" for one pixel per pixel), panned with the right or middle button. The view never moves the screen's pixels about: magnified, it steps by whole multiples (200%, 300%, ...) with sharp texels on whole panel pixels; reduced, it shows a mipmapped copy of the target, so the UI's 1-pixel lines and edges do not flicker from one step to the next (2026-11-15). Hovering shows the control under the pointer and the layout coordinates; a click selects it; "all boxes" outlines every drawn control |
| Inspector (right) | The dialog: script, name, anchor (declared, the script's own or the default) and its `UIAnchors.lua` entry with its stretch zones (editable), position and where it is drawn, the `SetPos` line that set it with a field that moves it ([below](#editing-positions)), the recorded positions (a click moves the dialog there). Its controls by name (`#<index>` when unnamed) with type and declared anchor. The selected control: anchor and its `UIAnchors.lua` entry (editable), offset, what it drew; for statics, each text (the string, its `STR_ID_*`, position, font, alignment and color; empty when the game's code fills it) and each picture's texture and `.tet` key, with "Make its width a stretch zone"; a picture expanded there is painted green on the screen. The control, each picture and each text can be moved, and show the script line that placed them; a picture's texture and key can be changed and "Edit" opens its key's UV rectangle over the texture ([below](#editing-textures-and-uv)) |

Slice (a) inspects, slice (b) edits the anchors ([below](#editing-anchors)), slice (c) moves what the scripts place
([below](#editing-positions)) and changes the pictures' textures, keys and UV ([below](#editing-textures-and-uv)).

## Editing anchors

Slice (b), 2026-11-14 (`tools/ui-editor-anchors`). The inspector edits the selected dialog's entry in
`UIAnchors.lua` ([KTDXLIB](../porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors)), found under the
client_data (`data/`) by name in any folder; a new one goes with the first dialog script found, never to a fixed
path ([Tools find files by name](client-data.md#managing-client_data)):

- **Dialog anchor:** none (centered), `CENTER`, `LEFT` or `RIGHT`. **Control anchor:** none (the dialog's), `CENTER`,
  `LEFT`, `RIGHT` or `STRETCH`. Unnamed controls cannot be declared (the file names them), and anchors the script sets
  itself (`SetAnchor` in Lua, `IsAnchorFromScript`) cannot be changed here: they win over the file in the game.
- **Stretch zones** (`STRETCH_ZONES`, layout x from the dialog): added, dragged or typed (Ctrl + click), removed; a
  picture's "Make its width a stretch zone" adds the span of that piece (the plain middle of a bar).
- **Backdrops** (`BACKDROPS`, [T51](../project/decisions.md), 2026-11-15): a named static's "full-screen backdrop"
  check box declares it, with its fade width beside it (64 for a new one; dragged or typed). Choose the 21:9 and 4:3
  sizes to see the fade and the crop.
- An edit shows at once in every open dialog of that script (`Engine::ApplyAnchors`, which also clears what the entry
  no longer declares), and is put back every frame over what opening a dialog or watching the game sets. The running
  game reads the file when it starts, so it shows a saved change after a restart; while watching, an anchor removed
  from the file comes back from the game until then.
- **Saving** writes only what changed (`anchors_file.h`): the file is laid out by hand, with comments and controls
  grouped by line, so a value is replaced in place, a field is added after the last of its table (on its line) or
  removed with its line when nothing else is left on it, and an entry left empty is removed. New entries go at the end,
  under a "Set in the UI editor" comment, on one line or with their controls on lines of their own past 120 columns.
  A part of the file it cannot read is shown under the save button (the game reports the same on stderr). Closing with
  unsaved changes asks first.
- **Undo and redo** (user's request after trying it, 2026-11-14): "Undo" / "Redo" beside the save button, or Ctrl+Z
  and Ctrl+Y (Ctrl+Shift+Z) when no text field is being typed in. Each step is a script's entry before and after an
  edit, so it is undone in the open dialogs as well; one drag of a stretch zone is one step. Loading the file again
  clears the history.

## Editing positions

Slice (c), 2026-11-14 (`tools/ui-editor-layout`, [T46](../project/decisions.md) (2)): a move is saved into the dialog
script itself, the loose file in the client_data ([client_data](client-data.md)), with no overrides file.

- **What moves:** the dialog (its `g_pUIDialog:SetPos`), a control (every point table set on it, such as a button's
  normal, over and down points, and those of its pictures and texts), a picture (its `SetPoint`, or its
  `SetPos(D3DXVECTOR2(x, y))` when one comes after it and places it instead) or a text (its `AddString`'s `POS`).
  2026-11-16: moving the character selection's background (`DLG_UI_Character_Selection_Back_New.lua`, the only picture
  `SetPos` in the client_data) edited its `SetPoint`, and its `SetPos(-245, 0)` put it back. Growing such a picture by
  its left or top edge changes the size in the `SetPoint` and moves the `SetPos`. In the inspector each has a field (drag or Ctrl + click to type, whole layout units) and shows
  the line that set it. On the screen, a click selects a control and dragging it moves it (Alt: its dialog); with the
  screen focused, the arrow keys move it by 1 (Shift: 10).
- **A picture's size** (2026-11-15): the inspector shows it with where it comes from, as the script writes it
  (`= key 390 x 1 + ADD_SIZE_Y 480 (the key stretched)`), and a size field; on the screen, the green box of an
  expanded picture grows or shrinks by dragging its edges or corners. The change goes where the size is set
  (`ResizeCall`, each edge moved by its own amount): with `USE_TEXTURE_SIZE` (the default) the size is the key's
  rectangle times `RESIZE_X/Y` plus `ADD_SIZE_X/Y`, so `LEFT_TOP` moves with the left and top edges and
  `ADD_SIZE_X/Y` take the growth (added when missing); with `USE_TEXTURE_SIZE = FALSE` the corners the table gives move
  with their edges (one without `RIGHT_BOTTOM`, or a `USE_TEXTURE_SIZE` that is not a plain true or false, is refused).
  Windows are made of such pieces, a middle of one texel stretched between ends: their size is the `ADD_SIZE`.
- **Where the lines come from:** as a script loads, KTDXLIB notes each such call with the object it set and its Lua line
  ([KTDXLIB](../porting/client-ktdxlib.md#script-calls-for-tools-2026-11-14-toolsui-editor-layout)); the scripts a
  control makes run on its own (another level of loading) are left out. Something code placed, or set by a call the
  engine does not note (a picture's `SetPos`), has no line and its field is disabled.
- **How the text changes** (user's choices, 2026-11-14): only the numbers. Each coordinate keeps its expression and its
  first number is changed (`746 + MOVE_BG_Y` becomes `750 + MOVE_BG_Y`; `X - 10` becomes `X - 6`); one with no number
  gets the move appended (`X + 4`). A control is moved by editing each of its points, not by adding a `SetOffsetPos`.
  A point table moves every corner it gives (`LEFT_TOP`, `RIGHT_TOP`, `LEFT_BOTTOM`, `RIGHT_BOTTOM`) and gets a
  `LEFT_TOP` when it has none (it defaults to 0, 0); a text without `POS` gets one. A line with two calls of the kind, or
  without one (a function defined in another file), is refused with the reason. A line that sets several things (a loop,
  or a function called several times) moves them all, and the inspector says so.
- **Seeing it:** the edited text runs instead of the file (`Engine::SetScriptText`) and each open dialog built from
  that script is built again (`ReloadDialog`), where it was drawn and with the game's state last mirrored into it. The
  game, started before the edit, still draws the old numbers, so its state is moved by what the edit moved (the
  difference between what the scripts placed when first loaded and now).
- **After watching the game** (2026-11-16, user's report): a dialog keeps the game's state it last mirrored, also with
  "Watch game" unchecked, and a reload puts it back with each picture's corners moved by what the edits moved, each
  corner by its own change, and with the texture, key and UV rectangle the edited script and `.tet` give when an edit
  changed them. Until then only the left top was followed and the game's texture and key came back, so a size or key
  edit snapped back to the game's (the key list too, which shows the picture's key). A key the game's code set (Add's
  `name_noclass` in character creation), whose `.tet` rectangle the editor changed, is drawn with the edited rectangle
  and its size scaled from the game's by the rectangle's change (`FollowTet`; the game drew it with the file's).
- **Undo and redo** take back moves as they do anchor edits (one drag is one step); "Save" writes the changed scripts,
  each one as a whole with its BOM (the files are UTF-8 with BOM, T42), and closing asks first.
- **Dialogs placed by code** (T46 (3)): moving one changes its script's `SetPos` only, which the code overrides; the
  inspector says so. Their positions move into the scripts screen by screen, later.
- **Named placements** (2026-11-16, `client/widescreen-migration`, [T52](../project/decisions.md)): a dialog whose
  script declares placements (`AddPlacement`, [KTDXLIB](../porting/client-ktdxlib.md#named-placements-2026-11-16-clientwidescreen-migration-t52))
  lists them in the inspector under **Placements**: a fixed one with its position and anchor, one with a target as
  the offset from its target dialog and control ("not open" while no open dialog has that script). Choosing one puts
  the dialog there as the game's code would (`Engine::ApplyPlacement`); a target placement follows the target dialog
  as it moves. While one is in effect, its `POS` field and dragging the dialog on the screen (Alt) move its
  `AddPlacement`'s `POS`, and the dialog built again after an edit is put at it again. "Watch game" puts a dialog at
  the placement the game has in effect (`placement` in `UILive.tsv`). Not yet: choosing a target control on the
  screen, adding a placement, and its anchor in the inspector (edited in the script meanwhile).

## Resizing a dialog

2026-11-15 (user's request: the editor must be able to change a dialog, not only nudge it). A window is made of
pieces: corners, edges and a middle stretched from a one-texel key ([above](#editing-positions)). "Resize at a line",
in the inspector's dialog section, grows (or, negative, shrinks) the dialog at a line across it:

- **The line:** vertical to change the width, horizontal for the height, magenta on the screen; drag it there or type
  where it is (layout units relative to the dialog, as the scripts' numbers, before anchors). "grow by" is how much;
  a fainter line shows where the far side ends up.
- **What changes** (`layout_edit.h`, `PlanCut`): a picture the line crosses grows (its `SetPoint`, by `ResizeCall`:
  `ADD_SIZE` or its far corners), painted orange; a picture, text or other control wholly past the line moves
  (`MoveCall`), painted blue; what is before it stays. Controls other than statics the line crosses (a button) and
  pieces without a script line are not changed, and the inspector lists them.
- **Apply** makes every edit as one step of undo (`EditCalls`); a line that sets several pieces is edited once, and
  refused when the pieces it sets would change differently (move them one by one).

## Creating and deleting

2026-11-16 (`tools/ui-editor-create`, user's request): anchors and STRETCH apply per control
([KTDXLIB](../porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors)), so a picture that must stretch
while the rest of its static must not (a shade behind a speech balloon) needs a static of its own. The inspector adds,
copies, splits and removes statics and pictures, each an edit of the dialog script's text (`script_edit.h`), one step of
undo, saved with the others:

- **To its own static** (a picture's "To its own static..."): its `S:AddPicture(P)` names a new static instead, created
  (`CreateStatic`, `SetName`) and added (`AddControl`) just before the old static's `AddControl` (drawn under it, the
  default for any but the last picture) or just after (drawn over it). The picture keeps its lines and the new static
  gets the old one's `SetOffsetPos` lines, so it stays where it was (what code moves is not followed); it gets the old static's anchor from `UIAnchors.lua` (a step of undo of its own), so it is also drawn where it
  was, and is selected, to give it another (STRETCH). Refused, with the reason in the tooltip, when the editor did
  not see the picture's `AddPicture` or the static's `AddControl` (code, or a control's own script), when either runs in
  a loop or a function, or when they are in different scripts.
- **New static** (the dialog's "Controls" section): a named static with one picture (texture and key, `SetTex2` for a
  texture without a `.tet`) at a left top, its size the key's, at the end of the dialog's script: drawn over the rest.
  **New picture** (a static's): the same picture added after the static's last picture and its lines.
- **Duplicate**: a picture is copied with its lines into its static, moved by 10, 10; a static (named, with a "moved
  by") with its lines, its pictures and their lines, and its texts, after its last line, and with its anchor.
- **Delete**: a picture's `AddPicture`, a control's `AddControl`, with the object's creation and lines (and, for a
  control, its pictures'), each only when no other line uses its variable; otherwise only the call that adds it goes, so
  the script still runs and the object is not drawn. The game's code may look a control up by name, or a static's
  pictures and texts by index (`GetPicture(n)`): check it first ([client_data](client-data.md) records every such edit).
- **An object's lines** are the statements that start with its variable (at the start of a line or after a `;`) from
  its creation (the last `V = ...` before the call) to the next assignment of the variable, so scripts that reuse one
  `g_pPicture` for every picture work. New objects get globals of their own (`g_pStatic_<name>`, `<picture>_Copy`, with a
  number when the script already has the name). The scripts' `AddPicture` and `AddControl` lines come from KTDXLIB, as
  the setters' ([KTDXLIB](../porting/client-ktdxlib.md#script-calls-for-tools-2026-11-14-toolsui-editor-layout)).
- **Watching the game:** the game mirrors controls by index, so after one of these edits its state goes into the wrong
  controls until it restarts with the saved script.

## Editing textures and UV

Slice (c), 2026-11-14 ([T46](../project/decisions.md) (2)): a picture's texture and key are saved into its script's
`SetTex("file.tga", "KEY")`, and a key's UV rectangle into the texture's `.tet`, both loose files in the client_data.

- **Texture and key:** the inspector shows the picture's `SetTex` line, a field with the texture's file name (Enter
  changes it; the key stays when the new texture has it, else its first key) and a list of the texture's keys, each
  with its rectangle. Both are string literals in the call (`SetTexCall`); a call naming them otherwise, through a
  variable, is refused. Pictures set by `SetTex2` (a whole texture) or by code have no line here.
- **UV rectangle**, edited only in **Interactive Select** (user's design, 2026-11-14; the inspector's number fields and
  corners table went, 2026-11-14: numbers without the texture were edited blind): "Edit" opens a dialog with the texture on
  the left and the rectangle's numbers on the right, Apply / Cancel below. The texture is drawn as the game loaded it
  (`Engine::LoadTextureImage`, over a checkerboard, texels sharp when magnified and a pixel grid from 800%), dimmed
  around the rectangle, which has a clear border and its key's name; the texture's other keys can be outlined faintly.
  The mouse only drags the rectangle (whole texels); the wheel zooms around the pointer and the right or middle button
  pans; the arrows move it by 1 (Shift: 10) while the pointer is over the texture. The size is what
  `USE_TEXTURE_SIZE` gives the picture; a rectangle the `.tet` rotates (its script's `RotateRect`) can be moved only.
  The numbers (left, top, width,
  height; right and bottom shown) are typed or stepped from the keyboard, with the pointer's texel, the rectangle
  before ("Back to it") and the rectangle drawn at its size. While it is open the screen previews the rectangle in the
  dialogs (it can be turned off). Apply gives the key the rectangle as one step of undo, asking first, over the select,
  when others use the key; Cancel puts back what the edits held.
  **Upscaled textures** (2026-11-16, `client/ui-hd-assets`): the rectangle stays in the `.tet`'s units, the original
  texture's texels ([D3D9](../porting/client-d3d9.md#upscaled-textures-2026-11-16-clientui-hd-assets)), and the
  texture is shown at that size in its own detail; drags, arrows, fields and the pixel grid follow the file's texels
  (quarter units on a 4× file: `TextureImage::scale`), and the panel says "file 4x". The `.tet` keeps floats.
- **A key others use** (user's choice, 2026-11-14): before the first UV change of a key, the editor searches every
  `.lua` under `data/` for lines that name the texture and the key in quotes (the game's code is not searched). The
  client_data's are 177 MB in 4,565 files among 105,000 (2026-11-16): read for every key on the UI thread, the editor
  froze for about 10 s at each Apply, so an index of the lines that name files in quotes (`KeyUseIndex`) is built in
  the background when the editor starts and again after saving, and each key is looked up in it. When lines other than the picture's own `SetTex` name it, or that line sets several pictures, it asks:
  change it in place (for every picture that names it, remembered for that key until the editor closes), or make a
  new key, a copy of the rectangle under a name the user may edit (suggested: the key and the dialog's name, as
  `BG_LEFT_TOP_UI_LOCAL_MAP_NEW`), which the picture's `SetTex` then names, as one step of undo.
- **The `.tet` file** (`tet_file.h`), as the client reads it (`CKTDXDeviceTET_Preprocessing`): header (magic `TET `,
  version 0x100, count), each key's offset, the rectangles (10 floats in texels: left top, right bottom, left bottom,
  right top, size), then the keys (a WORD length, UTF-16LE, a null). The client finds a key by binary search, so the
  keys stay sorted (uppercase, by code unit); a new key is made uppercase and is letters, digits and underscores.
  Every `.tet` of the user's client_data writes back unchanged (2026-11-14).
- **Seeing it:** the edited bytes replace the texture's rectangles in the engine (`Engine::SetTet`, KTDXLIB's
  `CKTDXDeviceTexture::ReplaceTET`; the engine holds the texture so it stays loaded with them), and the open dialogs
  with pictures of it are built again. Undo, redo and Save cover the `.tet` files as they do the scripts.

## Watch game

A script alone is not a screen: the game's code opens several dialogs, moves them, hides the statics of the modes not
in use, fills texts and swaps textures. "Watch game" shows what a running client shows:

- The client, run in the game folder with `X2_UI_LIVE=1` (a dev build), writes `UILive.tsv` there a few times a second
  ([UI anchors](ui-anchors.md), `CKTDGUIAnchorRecorder::NoteLive`): the game state, its resolution, and the shown
  dialogs drawn from scripts, back to front, each with its position, layer, anchor in effect (code anchors some at run time, such as the quick slot items after the slot frames; 2026-11-14), the scripts code loaded into it after the
  first (`OpenScriptFile`: a local map's regions, 2026-11-14), and every control (shown, type, the anchor it declares, the offset code moved it by: the menu buttons close the gaps of hidden ones); for statics, every
  picture (texture, key, corners, texture coordinates, color, shown) and every text (position, font, alignment, color).
- The editor reads it when it changes (`live.h`) and mirrors it: a dialog stands for the game's when it was built from
  the same scripts (the n-th of them for the game's n-th), the others close, the missing ones open with their scripts,
  and each takes the game's state (`Engine::ApplyLive`: anchors first, then the position, which is in layout units under the anchor): statics are made what the game holds, so pictures code removed
  (the wake orbs of other classes) go, those it added or resized (gauges, quick slot items, quest marks) come, and so do
  statics code added. The screen takes the game's exact viewport size ("(game)" next to the size; until 2026-11-14 the nearest of the editor's aspects and heights, so with the game at another aspect the anchors placed the script controls for a different aspect than the code had positioned items and icons for). Choosing an aspect or a height returns to the editor's sizes. Unchecking "Watch
  game" leaves the screen as it was last, to work on that state; dialogs opened by hand while watching close at the
  next change.
- "Start game" runs this platform's client from the game folder (`x2`; `x2.exe` on Windows, so a folder holding both works; before 2026-11-14 it tried `x2.exe` first and failed on Linux) with `X2_UI_LIVE=1` and `X2_RECORD_UI_ANCHORS=1`, its output in the
  editor's terminal; closing the editor leaves it running.
- Not mirrored: dialogs built in code without a script, controls of other types than statics that code adds (buttons,
  edit boxes), and what code changes in them besides being shown.

## What the game sets

2026-11-16 (user's request: it was not clear what the editor can change and what it only mirrors). Each value of a
screen has one owner:

| Owner | What it sets | In the editor |
|---|---|---|
| The dialog script (`DLG_*.lua`) | What the script places: positions, sizes, the textures and keys of its `SetTex`, the statics and pictures | Edited and saved; the game reads it when it opens the dialog |
| `UIAnchors.lua` | Anchors, STRETCH, stretch zones, backdrops | Edited and saved |
| A texture's `.tet` | Each key's rectangle | Edited and saved; the game draws it for every picture of that key, whether the script or the code chose the key |
| The game's code (C++) | What changes at run time: which texture and key (Add's `name_noclass` in character creation, by class), shown or hidden, texts, gauge sizes, pictures and controls it makes, dialogs it places | Mirrored only; changing it needs code (layout positions move into the scripts, [T46](../project/decisions.md) (3)) |
| `UILive.tsv`, `UIAnchorsRecord.tsv` | Copies of what the game showed (watch game) and where it placed its dialogs (the record) | Read to rebuild the screen; never saved |

The inspector marks in orange, with `[game]` in the control list and on the pictures and texts, what the game's code
set over the script in a dialog that holds the game's state (`GameSets`; also after unchecking "Watch game"): a control,
picture or text it made, a control or picture it shows or hides, a texture and key it chose (the tooltip gives the
script's `SetTex`), a text it filled. Editing the script line there changes the script, which the game's code then
overrides; a key's rectangle still applies. Positions are not marked (user's choice, 2026-11-16): the layout positions
move into the scripts with the widescreen migration ([T46](../project/decisions.md) (3)), while these stay the code's.

## How it works

- **Engine** (`engine.h`, `x2_ui_editor_core`): the real `CKTDXApp` on a window-less `x2::d3d9::Device` over the
  window's SDL_GPU device. Each frame the open dialogs step and draw (what `CX2State` does for the UI: the dialog
  manager's step, the object chain and the front UI) into a D3D9 render target of the chosen size; the D3D9 frame is
  submitted first, then ImGui samples the target's SDL_GPU texture (`x2::render::sdlgpu::Native`) in its own command
  buffer. A size change goes through `CKTDXApp::OnResetDevice`, so the viewport, the UI canvas (T44) and the projection
  follow it as in the game.
- **Start-up**, as `CX2Main`'s: the working folder becomes the game folder, the archives mount (`AddMassFiles`, with
  dev builds preferring the client_data), then `Enum.lua`, `DungeonEnum.lua` and `StringID_def.lua` run, the default
  font (`Gulim.ttc`, which every other font falls back to for Hangul) is installed and `Config_INTERNAL.lua` (its fonts)
  runs. The device manager's loading thread starts suspended and is resumed last, as `CX2Main` does; a size change
  (`OnResetDevice`) waits for it. X2Lib is not linked: `g_pMain` is a Lua stub whose `InstallFont` installs the
  font and whose other methods do nothing; `g_pCX2SlotManager` and the other game globals a few scripts touch
  (2026-11-14: `g_pData`, `g_pState`, `g_pCouponBox`, `g_pMapSelectWindow`, `g_pPostBoxInventory`) accept any call, so
  item-slot dialogs build everything but their slots.
- **ImGui and SDL_GPU**: the screen is drawn with `ImGuiLayer::TextureId`, which turns an `x2::render` texture into
  ImGui's texture identifier, so the tool stays off the SDL_GPU API (T36's boundary, checked by `ctest -R render`).
- **Control boxes**: `CKTDGUIControl` notes what it draws (its quads, transparent ones skipped) when the anchor recorder
  is on or a tool asks for it (`CKTDGUIAnchorRecorder::SetToolTracking`); the editor reads them without writing the
  record file. Boxes are in layout units relative to the dialog and are mapped to pixels with the control's anchor and
  the dialog's stretch (`ControlXToDialogX`), the dialog's drawn position and the canvas scale and origin.
- **Positions of dialogs placed by code** (T46 (3)): until a screen's layout positions move into its scripts, a dialog
  recorded by `X2_RECORD_UI_ANCHORS` ([UI anchors](ui-anchors.md)) opens at its first recorded position
  (`record.h` reads `UIAnchorsRecord.tsv` in the game folder); the others are listed in the inspector. A dialog never
  recorded opens at its script's position. The record also gives the scenes; while watching, it is read again when the
  game rewrites it.

## Tests (`ctest -R ui_editor`)

`UIAnchors.lua` (`test_anchors_file.cpp`): entries read by script without folder or case; each kind of edit changes
only its own text (a value, a control removed from the middle or the end of a line and with its line, added after a
trailing comma or on one line, a name that needs quotes, the dialog's anchor, the zones and the backdrops added, changed and removed,
an entry emptied and removed, a comment after it kept); new entries under one section, long ones wrapped; a missing
file written with its header; text that cannot be read reported. The client_data's own file (2026-11-14) reads
without problems and is unchanged by setting its entries to what they are.
The engine: an entry put into an open dialog and cleared, the script's own anchors kept, a backdrop declared and
drawn at 21:9 (2026-11-15).
Named placements (2026-11-16): a fixed one and its anchor (given back by the next), one following a centered and a
right-anchored control at 16:9, the target moved, hidden and shown again, and an edited `POS` after a reload;
`MoveCall` moves an `AddPlacement`'s `POS` (`test_script_edit.cpp`).

`x2_ui_editor_tests`, headless on the null render backend: the script list (sorted, `.kom/` skipped), the record file
(positions by script without folder or case, each once, placed by code; scenes and their scripts, older records without
them), the live snapshot (records under their dialog, escaped texts, empty fields, the scripts code loaded, pictures and
texts in full), and the engine on a temporary game folder
with loose files only: a missing script refused; a dialog opened, drawn at 1280×720 and at 1024×768, its controls named
(`#1` when unnamed), boxes in layout units and pixels, the slot manager stub; moved and closed; a static's text
inspected (not drawn: the tests have no fonts); the game's live state put into a dialog (position, a control hidden,
a text); a script loaded into an open dialog; statics as the game holds them (a picture code removed, a static code
added with its picture and text, a filler keeping the indexes); the line of each `SetPos`, `SetPoint` and `AddString`;
an edited script reloaded into its dialog, moved where it was, the game's state put into it moved by the edit, and the
file's text again (2026-11-14); a picture's key read from its texture's `.tet`, its `SetTex` line, and a changed `.tet`
growing it after a reload, and the texture loaded for Interactive Select at its size in texels (2026-11-14); a dialog
grown at a line (a crossed picture grows, one past the line moves, one before stays) and its extent after (2026-11-15);
the `AddPicture` and `AddControl` lines, a picture moved to a static of its own drawn at the same pixels and the static
removed, a picture resized after the game's state went in, a key the script changed to and a key the game's code set
drawn with their edited rectangles after it, what the game's code set marked, and a picture moved by its `SetPos`
after its `SetPoint` (2026-11-16).

Position edits (`test_script_edit.cpp`): numbers written as a script would; expressions keep their constants (the
number term changed, removed at 0, a move appended to one without); `SetPos`, a picture's `SetPos(D3DXVECTOR2)`, point tables on one line or several (a
commented-out corner and a string that looks like one left alone), the four corners, a `LEFT_TOP` added, `AddString`'s
`POS`; lines with two calls, a corner that is not a `D3DXVECTOR2` and a commented-out call refused, unchanged; a
point table grown by its edges (`ADD_SIZE` changed or added and `LEFT_TOP` moved, or the corners given, two or four;
one without `RIGHT_BOTTOM` or an unknown `USE_TEXTURE_SIZE` refused); a
`SetTex`'s texture and key changed (either quotes), one naming them through a variable, two on a line and a quote in
the new name refused. Structural edits (2026-11-16): a picture moved to a new static before or after its old one (also when it is added
before the static is), refused for other calls, another static or a quoted name, a taken global numbered; statics and
pictures added (`SetTex2` without a key); a picture and a static duplicated and moved; a picture and a control removed,
kept where another line uses them, and a reused variable's other picture left alone.

`.tet` files (`test_tet_file.cpp`): read and written back as the client writes them; files cut short or not `.tet`
refused; rectangles moved and grown, a rotated one not grown; keys added in order, a duplicate or invalid name
refused; the name of a texture's `.tet`; the lines of the scripts that name a key (any case, either quotes, `.kom/`
skipped). With `X2_UI_EDITOR_GAME`, every `.tet` of that client_data writes back unchanged.

With `X2_UI_EDITOR_GAME=<game folder>` (not in CI; `X2_UI_EDITOR_FILTER=<prefix>` narrows it), every dialog script of
that client_data is opened in turn and every call the engine noted is moved in its text. On the user's (2026-11-14):
29,571 calls, all movable, 2,465 of them on lines that set several things. Also (2026-11-16) every picture is moved to a static of its own
in the text and the first shown one of each dialog is drawn after the reload: of 14,342 pictures, 1,485 are not added
by a call the engine saw and 928 in loops or functions; none of the rest was refused, and all 837 were drawn at the
same pixels once the new static got the old one's anchor and `SetOffsetPos` (without them, 26 were not: anchored
statics such as the Speech shade and the menu's EXP bar, and statics the script offsets, such as the message boxes).

Closing a dialog clears the Lua globals its scripts set to objects (2026-11-14): `DLG_Event_Clock_Talk_Box.lua` names
its static through `g_pStaticEVENT_CLOCK`, a global of `DLG_Event_Clock.lua`, and opening the two in turn crashed the
editor on the freed control; it now gets nil, a script error.

## Status

2026-11-14: slice (a), the viewer, works on the desktop (`tools/ui-editor`, PR #74): the login screen and the menu bar
drawn at 1920×1080 and 2560×1080 as in the game (login centered, menu buttons right, EXP and vigor bars stretched),
a click selects the control under the pointer (the Quest button, `RIGHT`) and the inspector shows it.

Texts: a static's text comes from its script (`AddString`: `MSG = STR_ID_*`, an index into `general.ess`, or a
literal `STRING`), with its position, font, alignment and color; 1,642 `MSG = STR_ID_*` in 474 scripts (2026-11-14).
Texts the game's code fills at run time (names, numbers, counts) are empty in the script and so in the editor.

Known: the game's own input still reads the system mouse, so a button under the pointer shows its hover picture.

Duplicated texts in a script alone (`DLG_UI_PVP_SETTINGS_MAP.lua`, 2026-11-14) are the statics of every mode, which
the game's code hides but the one in use: "Watch game" shows them as the game does.

2026-11-14: slice (b), anchor editing ([above](#editing-anchors)), checked by the user on the desktop ("worked perfectly"); undo and redo added after.

2026-11-14: slice (c) (`tools/ui-editor-layout`): positions, then textures, keys and UV
([above](#editing-positions)); to be tried on the desktop with the local map (`DLG_UI_Local_Map_NEW.lua`).

2026-11-16: creating and deleting (`tools/ui-editor-create`, [above](#creating-and-deleting)); to be tried on the
desktop. The first real case, the speech balloon's shade of `DLG_Dungeon_Ending_Event_Speech.lua`, was split by hand in
the client_data the same day (`client/speech-shadow-stretch`).

Next ([T46](../project/decisions.md)): the C++ layout positions moved into the scripts, screen by screen.
