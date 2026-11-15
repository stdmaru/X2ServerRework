#pragma once

// Position edits of a dialog script (DLG_*.lua; decision T46 (2), docs/tools/ui-editor.md#editing-positions). The
// engine notes the line of each setter call a script made (CKTDGUIDialog::GetScriptCalls); an edit finds that call in
// the script's text and changes only the numbers it must, so the script keeps its layout, comments and constants.

#include <string>

namespace x2::ui_editor {

enum class CallKind {
    DialogPos,  // g_pUIDialog:SetPos(x, y)
    Point,      // a point table: SetPoint, SetNormalPoint, SetBGPoint_LUA, ... (a method named *Point, or SetRect)
    String,     // AddString's POS
    Texture,    // a picture's SetTex("file.tga", "KEY")
};

// Moves what the call of `kind` on `line` (1-based) sets by (dx, dy) layout units: the two arguments of SetPos, every
// corner a point table gives (LEFT_TOP, RIGHT_TOP, LEFT_BOTTOM, RIGHT_BOTTOM; a LEFT_TOP, which defaults to 0, 0, is
// added when it has none), or an AddString's POS (added when missing). In each coordinate the number is changed and
// the rest of the expression kept (ShiftExpression). False with `error`, the text unchanged, when the line has no
// such call or more than one.
bool MoveCall(std::string& text, int line, CallKind kind, float dx, float dy, std::string& error);

// Moves the edges of what the point table on `line` places by layout units (left, top, right, bottom: + is right or
// down), so it grows or shrinks. With USE_TEXTURE_SIZE (the default) the size is the texture key's plus ADD_SIZE_X/Y:
// LEFT_TOP moves and ADD_SIZE_X/Y change (added when missing). Without, the corners it gives move (each by its two
// edges). False with `error`, the text unchanged, when the line has no such call, more than one, or USE_TEXTURE_SIZE is
// not a plain true or false.
bool ResizeCall(std::string& text, int line, float left, float top, float right, float bottom, std::string& error);

// Makes the SetTex call on `line` name another texture and key, both written as string literals there. False with
// `error`, the text unchanged, when the line has no such call, more than one, or it names them otherwise.
bool SetTexCall(std::string& text, int line, const std::string& texture, const std::string& key, std::string& error);

// An expression moved by `delta`: its first number added to (746 + MOVE_BG_Y -> 750 + MOVE_BG_Y, X - 10 -> X - 6),
// or the delta appended when it has none (X -> X + 4). Spaces around it are kept.
std::string ShiftExpression(const std::string& expression, float delta);

// A number as a script would write it: 12, -3.5, 0.125.
std::string FormatNumber(float value);

}  // namespace x2::ui_editor
