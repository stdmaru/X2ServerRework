#include "x2/platform/terminal.h"

#include <doctest/doctest.h>

#include <string>
#include <vector>

using x2::platform::Key;
using x2::platform::KeyDecoder;
using x2::platform::LineEditor;

namespace {

std::vector<Key> Decode(KeyDecoder& decoder, std::string_view bytes)
{
    std::vector<Key> keys;
    decoder.Feed(bytes, keys);
    return keys;
}

void Type(LineEditor& editor, std::string_view bytes)
{
    KeyDecoder decoder;
    for (const Key& key : Decode(decoder, bytes))
        editor.Apply(key);
}

} // namespace

TEST_CASE("KeyDecoder: text, controls and VT sequences")
{
    KeyDecoder decoder;
    const std::vector<Key> keys = Decode(decoder, "a\xED\x95\x9C\x1b[A\x1b[B\x1b[C\x1b[D\x1b[H\x1b[F\x1b[3~\x1bOH\x1b[1;5C\x7f\r\x03\x04");
    const std::vector<Key::Kind> kinds = {Key::Char, Key::Char, Key::Up, Key::Down, Key::Right, Key::Left, Key::Home,
                                          Key::End, Key::Delete, Key::Home, Key::WordRight, Key::Backspace, Key::Enter,
                                          Key::Interrupt, Key::EndOfInput};
    REQUIRE(keys.size() == kinds.size());
    for (std::size_t i = 0; i < kinds.size(); ++i)
        CHECK(keys[i].kind == kinds[i]);
    CHECK(keys[0].ch == U'a');
    CHECK(keys[1].ch == U'한');
}

TEST_CASE("KeyDecoder: sequences and UTF-8 split across reads")
{
    KeyDecoder decoder;
    CHECK(Decode(decoder, "\x1b").empty());
    CHECK(Decode(decoder, "[").empty());
    std::vector<Key> keys = Decode(decoder, "A\xED\x95");
    REQUIRE(keys.size() == 1);
    CHECK(keys[0].kind == Key::Up);
    keys = Decode(decoder, "\x9C");
    REQUIRE(keys.size() == 1);
    CHECK(keys[0].ch == U'한');

    // a lone ESC comes out on Flush; "\r\n" is one Enter
    CHECK(Decode(decoder, "\x1b").empty());
    keys.clear();
    decoder.Flush(keys);
    REQUIRE(keys.size() == 1);
    CHECK(keys[0].kind == Key::Escape);
    CHECK(Decode(decoder, "\r\n").size() == 1);
}

TEST_CASE("LineEditor: editing and the cursor")
{
    LineEditor editor;
    Type(editor, "print(1)");
    CHECK(editor.Text() == "print(1)");
    Type(editor, "\x1b[D\x1b[D" "2");          // left twice, insert
    CHECK(editor.Text() == "print(21)");
    Type(editor, "\x01" "x");                  // Ctrl+A, insert at start
    CHECK(editor.Text() == "xprint(21)");
    Type(editor, "\x1b[3~");                   // Delete under the cursor
    CHECK(editor.Text() == "xrint(21)");
    Type(editor, "\x05\x7f\x7f");              // End, two Backspaces
    CHECK(editor.Text() == "xrint(2");
    Type(editor, "\x17");                      // Ctrl+W
    CHECK(editor.Text().empty());
    Type(editor, "a b\x01\x0b");               // Ctrl+K from the start
    CHECK(editor.Text().empty());
}

TEST_CASE("LineEditor: Enter, history and the exit keys")
{
    LineEditor editor;
    KeyDecoder decoder;
    Type(editor, "one");
    CHECK(editor.Apply({Key::Enter}) == LineEditor::Result::Line);
    CHECK(editor.Take() == "one");
    Type(editor, "two");
    editor.Take();
    editor.Take();                             // empty lines are not kept
    Type(editor, "two");
    editor.Take();                             // nor a repeat of the last one
    REQUIRE(editor.History().size() == 2);

    Type(editor, "draft");
    Type(editor, "\x1b[A");
    CHECK(editor.Text() == "two");
    Type(editor, "\x1b[A\x1b[A");
    CHECK(editor.Text() == "one");
    Type(editor, "\x1b[B\x1b[B");
    CHECK(editor.Text() == "draft");           // back to the line being edited

    // Ctrl+C clears a line, and on an empty one asks to exit; Ctrl+D only on an empty line
    CHECK(editor.Apply({Key::Interrupt}) == LineEditor::Result::None);
    CHECK(editor.Text().empty());
    CHECK(editor.Apply({Key::Interrupt}) == LineEditor::Result::Interrupt);
    Type(editor, "x");
    CHECK(editor.Apply({Key::EndOfInput}) == LineEditor::Result::None);
    CHECK(editor.Apply({Key::Backspace}) == LineEditor::Result::None);
    CHECK(editor.Apply({Key::EndOfInput}) == LineEditor::Result::EndOfInput);
}

TEST_CASE("LineEditor: rendering with wide characters and sideways scrolling")
{
    LineEditor editor;
    Type(editor, "\xED\x95\x9C\xEA\xB8\x80" "ab");   // 한글ab: 6 columns
    Type(editor, "\x1b[D\x1b[D");
    CHECK(editor.Render("> ", 80) == "\r> \xED\x95\x9C\xEA\xB8\x80" "ab\x1b[K\x1b[2D");
    CHECK(x2::platform::DisplayWidth(U'한') == 2);
    CHECK(x2::platform::DisplayWidth(U'a') == 1);

    LineEditor longLine;
    Type(longLine, std::string(100, 'x'));
    const std::string rendered = longLine.Render("> ", 40);
    // the last columns are kept free, so the cursor after the text never wraps
    CHECK(rendered == "\r> " + std::string(36, 'x') + "\x1b[K");
}
