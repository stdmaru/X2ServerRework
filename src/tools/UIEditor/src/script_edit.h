#pragma once

// Position edits of a dialog script (DLG_*.lua; decision T46 (2), docs/tools/ui-editor.md#editing-positions). The
// engine notes the line of each setter call a script made (CKTDGUIDialog::GetScriptCalls); an edit finds that call in
// the script's text and changes only the numbers it must, so the script keeps its layout, comments and constants.

#include <string>

namespace x2::ui_editor {

enum class CallKind {
    DialogPos,  // g_pUIDialog:SetPos(x, y)
    Point,      // a point table: SetPoint, SetNormalPoint, SetBGPoint, ... (a method named *Point, or SetRect)
    String,     // AddString's POS
};

// Moves what the call of `kind` on `line` (1-based) sets by (dx, dy) layout units: the two arguments of SetPos, every
// corner a point table gives (LEFT_TOP, RIGHT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM; a LEFT_TOP, which defaults to 0, 0, is
// added when it has none), or an AddString's POS (added when missing). In each coordinate the number is changed and
// the rest of the expression kept (ShiftExpression). False with `error`, the text unchanged, when the line has no
// such call or more than one.
bool MoveCall(std::string& text, int line, CallKind kind, float dx, float dy, std::string& error);

// An expression moved by `delta`: its first number added to (746 + MOVE_BG_Y -> 750 + MOVE_BG_Y, X - 10 -> X - 6),
// or the delta appended when it has none (X -> X + 4). Spaces around it are kept.
std::string ShiftExpression(const std::string& expression, float delta);

// A number as a script would write it: 12, -3.5, 0.125.
std::string FormatNumber(float value);

}  // namespace x2::ui_editor
