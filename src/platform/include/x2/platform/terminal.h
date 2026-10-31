// Server terminal (decision T15): the console takes the place of the original MFC window (Common/ui/ServerUIDlg).
//
// When stdin and stdout are a terminal, the keyboard is read raw and edited here (LineEditor), and everything
// written to stdout/stderr goes through a pipe and is printed above the prompt line, as the original's CHookStdio
// did for its log control, so live output never breaks the line being typed. Otherwise (pipes, files: a tool
// driving the server) input is read line by line and nothing is hooked.
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace x2::platform {

struct Key
{
    enum Kind
    {
        Char,           // ch
        Enter,
        Backspace,
        Delete,
        Left,
        Right,
        Home,
        End,
        Up,
        Down,
        WordLeft,
        WordRight,
        Interrupt,      // Ctrl+C
        EndOfInput,     // Ctrl+D
        ClearScreen,    // Ctrl+L
        KillLine,       // Ctrl+U
        KillToEnd,      // Ctrl+K
        KillWord,       // Ctrl+W
        Escape,
    };

    Kind     kind = Char;
    char32_t ch = 0;
};

// Turns terminal input (UTF-8 text and VT escape sequences) into keys.
class KeyDecoder
{
public:
    // Appends the keys in `bytes` to `keys`; an incomplete sequence waits for the next call.
    void Feed(std::string_view bytes, std::vector<Key>& keys);
    // After a pause: what is left pending is a lone ESC (or a broken sequence, dropped).
    void Flush(std::vector<Key>& keys);
    bool Pending() const { return !m_pending.empty(); }

private:
    std::string m_pending;
};

// Columns a character takes in a terminal (2 for Hangul and other East Asian wide characters).
int DisplayWidth(char32_t ch);

// The prompt line: editing, history (like the original's input field, Up/Down) and rendering.
class LineEditor
{
public:
    enum class Result { None, Line, Interrupt, EndOfInput };

    // Interrupt: Ctrl+C on an empty line (on a non-empty one it clears the line). EndOfInput: Ctrl+D on an empty line.
    Result Apply(const Key& key);
    // The finished line (UTF-8), added to the history; the editor is cleared.
    std::string Take();
    void Clear();

    std::string Text() const;
    std::size_t Cursor() const { return m_cursor; }
    const std::vector<std::u32string>& History() const { return m_history; }

    // "\r", the prompt and the visible part of the line, cleared to the end, with the cursor in place. A line
    // wider than `columns` scrolls sideways so the cursor stays visible.
    std::string Render(std::string_view prompt, int columns) const;

private:
    void HistoryMove(int delta);

    std::u32string              m_line;
    std::size_t                 m_cursor = 0;
    std::vector<std::u32string> m_history;
    std::size_t                 m_historyPos = 0;     // == size(): the line being edited
    std::u32string              m_edited;             // that line, while browsing the history
};

class Terminal
{
public:
    struct Options
    {
        bool hookOutput = true;     // interactive only: stdout/stderr through the pipe
        // For a program reading stdout (T30): never interactive, stdout/stderr always through the pipe, and Write
        // goes alone to the real stdout, so that it carries only what the program writes there.
        bool machine = false;
    };

    explicit Terminal(Options options);
    Terminal() : Terminal(Options{}) {}
    ~Terminal();                    // restores the console (mode, title, stdout/stderr)
    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

    // stdin and stdout are a terminal: raw input, prompt, title.
    bool Interactive() const;

    // Thread-safe. Complete lines (a missing final newline is added); printed above the prompt.
    void Write(std::string_view text);
    void SetTitle(const std::string& title);
    // What the program writes to stdout/stderr while it is hooked, by lines; by default it is Write.
    void SetOutputHandler(std::function<void(std::string_view)> handler);
    void SetPrompt(std::string prompt);

    // Waits for a line, calling `idle` about every 100 ms. nullopt: exit confirmed (Ctrl+C / Ctrl+D, then y),
    // end of input or RequestStop.
    std::optional<std::string> ReadLine(const std::function<void()>& idle);

    // From any thread or a signal handler (SIGINT/SIGTERM/SIGHUP, console close on Windows: installed here).
    static void RequestStop();
    static bool StopRequested();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

// Service mode (no stdin): blocks until RequestStop or a stop signal, calling `idle` about every 100 ms.
void WaitForStop(const std::function<void()>& idle);

} // namespace x2::platform
