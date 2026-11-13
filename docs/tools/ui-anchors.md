[Home](../README.md) › [Tools](README.md) › UI anchors

# UI anchors (`tools/ui-anchors`)

Proposes the entries of `UIAnchors.lua` ([T44](../project/decisions.md),
[format](../porting/client-ktdxlib.md#anchors-2026-11-13-clientwidescreen-anchors)) from where the dialogs actually sit
in the original 1024×768 layout. Two parts:

1. **Recording, in the client** (`X2_DEV_TOOLS` builds only): run the client with `X2_RECORD_UI_ANCHORS=1`. Every
   dialog drawn from a script notes, for each control, the box of what it drew (layout units, relative to the dialog;
   the quads of `CKTDGUIControl::UpdateVertex`, transparent ones skipped) and every position the dialog was drawn at
   (up to 16). Unnamed controls are noted as `#<index>`. The file `UIAnchorsRecord.tsv` in the game folder is rewritten
   every 2 seconds when something new was seen, merged with what earlier runs recorded, so several play sessions add up
   (`CKTDGUIAnchorRecorder`, `KTDGUIAnchor.cpp`). Controls that draw only text (edit boxes) are not recorded; they
   follow their dialog.
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
| Dialog drawn at more than one position (dragged, or placed by code next to something) | Center; listed for review (a derived position) |
| Dialog held to the left or right edge | `DIALOG = "LEFT"` / `"RIGHT"` |
| Dialog spanning the width (a HUD) | Center, with control anchors: held to both edges `STRETCH`, to one edge or lying in that third `LEFT`/`RIGHT` |
| Full-screen picture (background), or anything in the middle | Center (the default: nothing written) |
| Dialog in an outer third but not at the edge | Center; listed for review |
| Unnamed control that would be anchored | Not written (anchors go by name); listed for review |

The proposal is reviewed with screen captures before it goes into the client_data's `UIAnchors.lua`
(`client/widescreen-migration`).

## Status

2026-11-13: recording and proposal work; the login screen recorded as expected (buttons and background centered).
Recording the game's screens needs someone to play through them.
