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
    Texture,    // a picture's SetTex("file.tga", "KEY"), or a control's SetNormalTex, SetOverTex, ... (same arguments)
    AddPicture, // a static's AddPicture(picture)
    AddControl, // g_pUIDialog:AddControl(control)
    PicturePos, // a picture's SetPos(D3DXVECTOR2(x, y)), which places it over its SetPoint's LEFT_TOP
    Placement,  // g_pUIDialog:AddPlacement({ NAME = ..., POS = ... }), a named placement (T52)
};

// Moves what the call of `kind` on `line` (1-based) sets by (dx, dy) layout units: the two arguments of SetPos or a picture's D3DXVECTOR2, every
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

// Structural edits (docs/tools/ui-editor.md#creating-and-deleting): statics and pictures added, copied, moved out of
// their static and removed. An object is a Lua variable; its lines are the statements that start with it (`P:SetTex(`,
// at the start of a line or after a ';') from its creation (`P = g_pUIDialog:CreatePicture()`, the last before the
// line given) to its next assignment. New objects get globals of their own (`g_pStatic_<name>`, a number added when
// the text has it). Each is false with `error`, the text unchanged, when the lines given are not the calls expected.

// Takes the picture `S:AddPicture(P)` on `addPicture` adds out of its static and adds it to a new static named
// `name`, created where the static's `D:AddControl(S)` on `addControl` is: just before it (`before`: the new static is
// drawn first, under the old) or just after (drawn over it). The picture keeps its lines, and the new static gets the
// old one's SetOffsetPos, so it stays where it was (unless code moves the old static).
bool PictureToNewStatic(std::string& text, int addPicture, int addControl, const std::string& name, bool before,
                        std::string& error);

// Adds a new static named `name` at the end of the script, with one picture of `texture` and `key` (empty: the whole
// texture) at (x, y), its size the key's.
bool AddStatic(std::string& text, const std::string& name, const std::string& texture, const std::string& key, float x,
               float y, std::string& error);

// Adds a picture of `texture` and `key` at (x, y) to a static, after the call on `line`: the static's last
// AddPicture (`kind` AddPicture) or its AddControl (AddControl), whose static it names.
bool AddPictureAfter(std::string& text, int line, CallKind kind, const std::string& texture, const std::string& key,
                     float x, float y, std::string& error);

// A new picture: its texture and key, left top, ADD_SIZE (0: the key's size), color.
struct NewPicture {
    std::string texture, key;
    float x = 0.0f, y = 0.0f;
    float addSize[2] = {};
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};
// AddPictureAfter with all of a new picture's fields.
bool AddPictureAfter(std::string& text, int line, CallKind kind, const NewPicture& picture, std::string& error);
// Adds a picture before the one `S:AddPicture(P)` on `addPicture` adds (before P's creation), so it is drawn under
// the static's pictures when that is the first.
bool AddPictureBefore(std::string& text, int addPicture, const NewPicture& picture, std::string& error);

// A named placement (T52): `g_pUIDialog:AddPlacement({ NAME, POS[, ANCHOR][, TARGET[, CONTROL]] })`.
struct NewPlacement {
    std::string name;
    float x = 0.0f, y = 0.0f;
    std::string anchor;     // empty: none
    std::string target;     // a dialog script; empty: a fixed placement
    std::string control;    // the target's control; empty: the target dialog
};
// Adds the placement after the script's last AddPlacement, else after its first lines on g_pUIDialog (SetName,
// SetLayer, ...), else at the top.
bool AddPlacementCall(std::string& text, const NewPlacement& placement, std::string& error);
// Sets a field of the table the `kind` call on `line` takes (`value` as Lua: "\"LEFT\"", "D3DXVECTOR2(1, 2)"); an
// empty value removes the field.
bool SetCallField(std::string& text, int line, CallKind kind, const std::string& field, const std::string& value,
                  std::string& error);
// Removes the `kind` call on `line`, a statement of its own line, with that line.
bool RemoveCallLine(std::string& text, int line, CallKind kind, std::string& error);
// A Lua string literal of `text`.
std::string LuaString(const std::string& text);

// Copies the picture `S:AddPicture(P)` on `addPicture` adds, with its lines, into the same static after its last line,
// moved by (dx, dy).
bool DuplicatePicture(std::string& text, int addPicture, float dx, float dy, std::string& error);

// Copies the static `D:AddControl(S)` on `addControl` adds, with its lines and its pictures', as a new static named
// `name` after its last line, moved by (dx, dy).
bool DuplicateStatic(std::string& text, int addControl, const std::string& name, float dx, float dy, std::string& error);

// Removes the picture `S:AddPicture(P)` on `addPicture` adds: that call, and P's creation and lines when nothing else
// in the text uses P then.
bool RemovePicture(std::string& text, int addPicture, std::string& error);

// Removes the control `D:AddControl(S)` on `addControl` adds: that call, and S's creation and lines and those of the
// pictures it adds, each when nothing else uses it then.
bool RemoveControl(std::string& text, int addControl, std::string& error);

// An expression moved by `delta`: its first number added to (746 + MOVE_BG_Y -> 750 + MOVE_BG_Y, X - 10 -> X - 6),
// or the delta appended when it has none (X -> X + 4). Spaces around it are kept.
std::string ShiftExpression(const std::string& expression, float delta);

// A number as a script would write it: 12, -3.5, 0.125.
std::string FormatNumber(float value);

}  // namespace x2::ui_editor
