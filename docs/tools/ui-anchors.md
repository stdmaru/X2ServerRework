[Home](../README.md) › [Tools](README.md) › UI anchors

# UI anchors (`tools/ui-anchors`)

Proposes the entries of `UIAnchors.lua` ([T44](../project/decisions.md),
[format](../porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors)) from where the dialogs actually sit
in the original 1024×768 layout. Two parts:

1. **Recording, in the client** (`X2_DEV_TOOLS` builds only): run the client with `X2_RECORD_UI_ANCHORS=1`. Every
   dialog drawn from a script notes, for each control, the box of what it drew (layout units, relative to the dialog;
   the quads of `CKTDGUIControl::UpdateVertex`, transparent ones skipped) and every position the dialog was drawn at
   (up to 16), whether code moved the dialog after its script loaded, and the game states it was seen in (`CX2Main`'s
   `XS_*` names, up to 16; a tenth column, `scenes`, since 2026-11-14, absent in older records). Unnamed controls are
   noted as `#<index>`. The file `UIAnchorsRecord.tsv` in the game folder is rewritten
   every 2 seconds when something new was seen, merged with what earlier runs recorded, so several play sessions add up
   (`CKTDGUIAnchorRecorder`, `KTDGUIAnchor.cpp`). Controls that draw only text (edit boxes) are not recorded; they
   follow their dialog.

   With `X2_UI_LIVE=1` the client also writes `UILive.tsv` there, a snapshot of the UI on screen for the
   [UI editor](ui-editor.md#watch-game)'s "Watch game" (format in `CKTDGUIAnchorRecorder::NoteLive`).
2. **Proposal** (`propose.py`): reads one or more record files and writes a proposed `UIAnchors.lua` with the evidence
   as comments, plus a report on stderr.

```bash
X2_RECORD_UI_ANCHORS=1 ./x2                                   # in the client folder; play through the screens
python3 tools/ui-anchors/propose.py <client folder>/UIAnchorsRecord.tsv -o UIAnchors.proposed.lua \
    --scripts <client_data>/ui/scripts                        # also count the dialog scripts never recorded
python3 tools/ui-anchors/tests/test_propose.py                # tests
```

## Rules

In layout units; "held to an edge" is within 24 units of it.

| Recorded | Proposal |
|---|---|
| Dialog listed in `tools/ui-anchors/centered.txt`: floating windows and pieces of screens drawn by several dialogs, kept by review | Center |
| Dialog moved by code after its script loaded (`SetPos` outside the script): the floating windows `CX2UIManager` opens to one side (shop, quests, inventory…), popups next to something | Center, not reviewed: the code's position, in the column (user, 2026-11-13) |
| Dialog drawn at more than one position (dragged, or placed by code next to something) | Center; listed for review (a derived position) |
| Whole screen: at least 600×450 with a control covering 40% of it (a background), or spanning the width with a background and taller than half the screen | Center; listed for review (its pieces may be anchored by hand) |
| Dialog at one edge and at least 600 wide | Center; listed for review |
| Dialog held to the left or right edge | `DIALOG = "LEFT"` / `"RIGHT"` |
| Dialog spanning the width (a HUD) | Center, with control anchors: held to both edges `STRETCH`, to one edge or lying in that third `LEFT`/`RIGHT` |
| Full-screen picture (background), or anything in the middle | Center (the default: nothing written) |
| Dialog in an outer third but not at the edge | Center; listed for review |
| Unnamed control that would be anchored | Not written (anchors go by name); listed for review |

Moved by code catches the windows `CX2UIManager` places at the top left (quests, character, skills…), but not the
inventory (placed by its script) nor a window whose first opening comes before its dialog exists; `centered.txt` holds
those, found by review.

The proposal is reviewed with screen captures before it goes into the client_data's `UIAnchors.lua`
(`client/widescreen-migration`).

## Placement scan (`scan_placement.py`, T52)

Lists the places where X2Lib's code still lays out the UI ([T52](../project/decisions.md)), the checklist of
`client/widescreen-migration` part 3: converted sites stop matching, so the list shrinks as the code leaves the layout
to the content.

```bash
python3 tools/ui-anchors/scan_placement.py -o placement.tsv          # default root: src/client/X2Lib
python3 tools/ui-anchors/tests/test_scan_placement.py                # tests
```

One row per site: file, line, function, kind (T52's A–G, plus `data` for sizes that stay code), call, receiver,
dialog script, arguments and flags. How it finds them:

- `SetPos`, `Move`, `SetOffsetPos`, `SetPoint`, `SetSize*` and `SetAnchor` on UI receivers only: a receiver's type
  comes from its declarations in X2Lib's sources and headers (`CKTDGUI*`, `CPictureData`), else from its name; meshes,
  particles, units and effects also have `SetPos`. Comments and `#if 0` blocks are skipped.
- A dialog variable's script is the `L"DLG_*.lua"` of its `new CKTDGUIDialog`, in the same file or any other.
- A value passed in a local variable is followed to its assignments in the function (`via:<name>`): all literal, the
  site is fixed (A or D).
- A function placing a dialog with its own parameter is followed to its callers, through several levels and inline
  methods in headers (`through:<function>`): `CreateMouseClickUI` → `MouseClickUI` → `SetPos( vPos )` puts the
  tutorial pointer's spots at `X2PlayGuide`, `X2DungeonGame` and `X2StateDungeonRoom`. A name defined in several
  classes and called more than 60 times is not followed (reported on stderr; 2026-11-16: `AddComboPicture`).
- Flags: `screen` (computed with 1024/768/512/384 literals, which do not follow the canvas), `mouse`, `default` (the
  message boxes' `(-999, -999)`, their own default spot from `GetWindowPos`), `param` (placed with the function's
  parameter: the spots are at the callers).

Guesses are a starting point: B (a spot on another dialog's element) is found by review, so it is reported as A.

First scan, 2026-11-16: 2,156 sites in 112 files. A 620, C 1,132, D 143, E 119, F 5, G 31, data 106.

- The message boxes are most of A: `KTDGUIOKMsgBox` 745 calls (443 at fixed spots, the rest at the default),
  `KTDGUIOkAndCancelMsgBox` 163 (80 fixed), `KTDGUIMsgBox` 35. Their spots are few: (250, 300) 384 times,
  (305, 375) 77, (270, 350) 40, and a handful of others. They stand for "in the middle", so one placement per box,
  in the box's script, replaces them.
- The tutorial pointer `DLG_Mouse_Click.lua`: 5 calls through `CreateMouseClickUI`, 13 spots in all, each on another
  dialog's control (B).
- Dialogs placed at several fixed spots (named placements): the dungeon ending event's speech (4) and character (3),
  the local map (3), the random item background (2), the PvP score boards (2 each).
- C: 389 message-box defaults, 313 through functions with computed arguments (305 of them `AddComboPicture`, the
  training game's combo pictures), 40 `param`, 13 at the mouse, 377 others (positions from other elements or
  variables) to review; 16 sites use screen literals.
- Most sites: `X2StateTrainingGame` 307, `X2UIInventory` 239, `X2StateServerSelect` 75, `X2StateMenu` 67,
  `X2UIShop` 67, `X2StateDungeonResult` 66, `X2Main` 64, `X2Community` 63.

## Status

2026-11-13: recording and proposal work; the login screen recorded as expected (buttons and background centered).
Recording the game's screens needs someone to play through them.
