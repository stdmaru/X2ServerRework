[Home](../README.md) › [Tools](README.md) › UI editor

# UI editor (`src/tools/UIEditor`, `x2_ui_editor`)

An SDL3 + ImGui tool of the client development suite ([T29](../project/decisions.md), [App foundation](../porting/app.md))
that draws the client's dialogs with the client's own GUI code (KTDXLIB's `CKTDGUIDialog`, `CKTDGUIControl`,
`UIAnchors.lua`), so a dialog looks exactly as in the game, at any resolution and aspect. How it works and where it
saves are decision [T46](../project/decisions.md).

## Status

2026-11-14: slice (a), the viewer, in progress (`tools/ui-editor`, PR #74).
