#include "x2/platform/terminal.h"

#include "x2/platform/unicode.h"

#if defined(_WIN32)
#include "x2/platform/types.h" // <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <csignal>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

namespace x2::platform {

namespace {

std::atomic<bool> g_stop{false};

// UTF-8 <-> UTF-32, replacing invalid input with U+FFFD.
std::u32string ToU32(std::string_view s)
{
    std::u32string out;
    for (std::size_t i = 0; i < s.size();) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        int len = c < 0x80 ? 1 : (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xE ? 3 : (c >> 3) == 0x1E ? 4 : 0;
        if (len == 0 || i + len > s.size()) {
            out += U'�';
            ++i;
            continue;
        }
        char32_t cp = len == 1 ? c : c & (0xFF >> (len + 1));
        for (int k = 1; k < len; ++k)
            cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
        out += cp;
        i += len;
    }
    return out;
}

void AppendUtf8(std::string& out, char32_t cp)
{
    if (cp < 0x80) {
        out += static_cast<char>(cp);
    } else if (cp < 0x800) {
        out += static_cast<char>(0xC0 | (cp >> 6));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        out += static_cast<char>(0xE0 | (cp >> 12));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        out += static_cast<char>(0xF0 | (cp >> 18));
        out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out += static_cast<char>(0x80 | (cp & 0x3F));
    }
}

std::string ToUtf8(std::u32string_view s)
{
    std::string out;
    for (const char32_t cp : s)
        AppendUtf8(out, cp);
    return out;
}

// Length of a complete UTF-8 sequence at the start of `s`; 0 if more bytes are needed.
std::size_t Utf8Length(std::string_view s)
{
    const unsigned char c = static_cast<unsigned char>(s[0]);
    const std::size_t len = c < 0x80 ? 1 : (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xE ? 3 : (c >> 3) == 0x1E ? 4 : 1;
    return len <= s.size() ? len : 0;
}

bool IsWordChar(char32_t ch)
{
    return ch != U' ' && ch != U'\t' && ch != U'.' && ch != U'(' && ch != U')' && ch != U',' && ch != U':';
}

int Width(std::u32string_view s)
{
    int width = 0;
    for (const char32_t ch : s)
        width += DisplayWidth(ch);
    return width;
}

} // namespace

//////////////////////////////////////////////////////////////////////////
// KeyDecoder

void KeyDecoder::Feed(std::string_view bytes, std::vector<Key>& keys)
{
    m_pending.append(bytes);
    std::string_view s = m_pending;
    while (!s.empty()) {
        const unsigned char c = static_cast<unsigned char>(s[0]);
        if (c == 0x1B) {
            if (s.size() < 2)
                break;  // lone ESC or the start of a sequence: wait (Flush decides)
            if (s[1] != '[' && s[1] != 'O') {
                keys.push_back({Key::Escape});
                s.remove_prefix(1);
                continue;
            }
            // CSI/SS3: parameters, then a final byte in 0x40..0x7E
            std::size_t end = 2;
            while (end < s.size() && (static_cast<unsigned char>(s[end]) < 0x40 || static_cast<unsigned char>(s[end]) > 0x7E))
                ++end;
            if (end >= s.size())
                break;
            const char final = s[end];
            const std::string_view params = s.substr(2, end - 2);
            const bool ctrl = params.find(";5") != std::string_view::npos;
            switch (final) {
            case 'A': keys.push_back({Key::Up}); break;
            case 'B': keys.push_back({Key::Down}); break;
            case 'C': keys.push_back({ctrl ? Key::WordRight : Key::Right}); break;
            case 'D': keys.push_back({ctrl ? Key::WordLeft : Key::Left}); break;
            case 'H': keys.push_back({Key::Home}); break;
            case 'F': keys.push_back({Key::End}); break;
            case '~':
                if (params == "1" || params == "7")
                    keys.push_back({Key::Home});
                else if (params == "4" || params == "8")
                    keys.push_back({Key::End});
                else if (params == "3")
                    keys.push_back({Key::Delete});
                break;      // others (Insert, PageUp...) are ignored
            default:
                break;
            }
            s.remove_prefix(end + 1);
            continue;
        }
        if (c < 0x20 || c == 0x7F) {
            switch (c) {
            case '\r': case '\n': keys.push_back({Key::Enter}); break;
            case 0x7F: case 0x08: keys.push_back({Key::Backspace}); break;
            case 0x01: keys.push_back({Key::Home}); break;
            case 0x05: keys.push_back({Key::End}); break;
            case 0x02: keys.push_back({Key::Left}); break;
            case 0x06: keys.push_back({Key::Right}); break;
            case 0x10: keys.push_back({Key::Up}); break;
            case 0x0E: keys.push_back({Key::Down}); break;
            case 0x03: keys.push_back({Key::Interrupt}); break;
            case 0x04: keys.push_back({Key::EndOfInput}); break;
            case 0x0C: keys.push_back({Key::ClearScreen}); break;
            case 0x15: keys.push_back({Key::KillLine}); break;
            case 0x0B: keys.push_back({Key::KillToEnd}); break;
            case 0x17: keys.push_back({Key::KillWord}); break;
            default: break;     // Tab and the other controls are ignored
            }
            // "\r\n" from a paste or a Windows console is one Enter
            if (c == '\r' && s.size() > 1 && s[1] == '\n')
                s.remove_prefix(1);
            s.remove_prefix(1);
            continue;
        }
        const std::size_t len = Utf8Length(s);
        if (len == 0)
            break;
        for (const char32_t ch : ToU32(s.substr(0, len)))
            keys.push_back({Key::Char, ch});
        s.remove_prefix(len);
    }
    m_pending.erase(0, m_pending.size() - s.size());
}

void KeyDecoder::Flush(std::vector<Key>& keys)
{
    if (m_pending == "\x1b")
        keys.push_back({Key::Escape});
    m_pending.clear();
}

int DisplayWidth(char32_t ch)
{
    if (ch < 0x20 || (ch >= 0x7F && ch < 0xA0))
        return 0;
    if ((ch >= 0x0300 && ch <= 0x036F) || (ch >= 0x1160 && ch <= 0x11FF) || ch == 0x200B)
        return 0;   // combining marks, Hangul medial/final jamo, zero width space
    if ((ch >= 0x1100 && ch <= 0x115F) || (ch >= 0x2E80 && ch <= 0x303E) || (ch >= 0x3041 && ch <= 0x33FF) ||
        (ch >= 0x3400 && ch <= 0x4DBF) || (ch >= 0x4E00 && ch <= 0x9FFF) || (ch >= 0xA000 && ch <= 0xA4CF) ||
        (ch >= 0xAC00 && ch <= 0xD7A3) || (ch >= 0xF900 && ch <= 0xFAFF) || (ch >= 0xFE30 && ch <= 0xFE4F) ||
        (ch >= 0xFF00 && ch <= 0xFF60) || (ch >= 0xFFE0 && ch <= 0xFFE6) || (ch >= 0x1F300 && ch <= 0x1F64F) ||
        (ch >= 0x1F900 && ch <= 0x1F9FF) || (ch >= 0x20000 && ch <= 0x3FFFD))
        return 2;
    return 1;
}

//////////////////////////////////////////////////////////////////////////
// LineEditor

LineEditor::Result LineEditor::Apply(const Key& key)
{
    switch (key.kind) {
    case Key::Char:
        m_line.insert(m_cursor, 1, key.ch);
        ++m_cursor;
        break;
    case Key::Enter:
        return Result::Line;
    case Key::Backspace:
        if (m_cursor > 0)
            m_line.erase(--m_cursor, 1);
        break;
    case Key::Delete:
        if (m_cursor < m_line.size())
            m_line.erase(m_cursor, 1);
        break;
    case Key::Left:
        if (m_cursor > 0)
            --m_cursor;
        break;
    case Key::Right:
        if (m_cursor < m_line.size())
            ++m_cursor;
        break;
    case Key::Home:
        m_cursor = 0;
        break;
    case Key::End:
        m_cursor = m_line.size();
        break;
    case Key::Up:
        HistoryMove(-1);
        break;
    case Key::Down:
        HistoryMove(+1);
        break;
    case Key::WordLeft:
        while (m_cursor > 0 && !IsWordChar(m_line[m_cursor - 1]))
            --m_cursor;
        while (m_cursor > 0 && IsWordChar(m_line[m_cursor - 1]))
            --m_cursor;
        break;
    case Key::WordRight:
        while (m_cursor < m_line.size() && !IsWordChar(m_line[m_cursor]))
            ++m_cursor;
        while (m_cursor < m_line.size() && IsWordChar(m_line[m_cursor]))
            ++m_cursor;
        break;
    case Key::Interrupt:
        if (m_line.empty())
            return Result::Interrupt;
        Clear();
        break;
    case Key::EndOfInput:
        if (m_line.empty())
            return Result::EndOfInput;
        if (m_cursor < m_line.size())
            m_line.erase(m_cursor, 1);
        break;
    case Key::KillLine:
        m_line.erase(0, m_cursor);
        m_cursor = 0;
        break;
    case Key::KillToEnd:
        m_line.erase(m_cursor);
        break;
    case Key::KillWord: {
        std::size_t start = m_cursor;
        while (start > 0 && m_line[start - 1] == U' ')
            --start;
        while (start > 0 && m_line[start - 1] != U' ')
            --start;
        m_line.erase(start, m_cursor - start);
        m_cursor = start;
        break;
    }
    case Key::ClearScreen:
    case Key::Escape:
        break;
    }
    return Result::None;
}

void LineEditor::HistoryMove(int delta)
{
    if (m_history.empty())
        return;
    if (m_historyPos == m_history.size())
        m_edited = m_line;
    if (delta < 0 && m_historyPos > 0)
        --m_historyPos;
    else if (delta > 0 && m_historyPos < m_history.size())
        ++m_historyPos;
    else
        return;
    m_line = m_historyPos == m_history.size() ? m_edited : m_history[m_historyPos];
    m_cursor = m_line.size();
}

std::string LineEditor::Take()
{
    std::string line = ToUtf8(m_line);
    // Like the original field: every entered line, without repeating the last one
    if (!m_line.empty() && (m_history.empty() || m_history.back() != m_line)) {
        m_history.push_back(m_line);
        if (m_history.size() > 500)
            m_history.erase(m_history.begin());
    }
    Clear();
    return line;
}

void LineEditor::Clear()
{
    m_line.clear();
    m_edited.clear();
    m_cursor = 0;
    m_historyPos = m_history.size();
}

std::string LineEditor::Text() const
{
    return ToUtf8(m_line);
}

std::string LineEditor::Render(std::string_view prompt, int columns) const
{
    const std::u32string promptU = ToU32(prompt);
    const int available = std::max(columns - Width(promptU) - 1, 8);

    // Window [first, last) of the line around the cursor
    std::size_t first = 0;
    while (Width(std::u32string_view(m_line).substr(first, m_cursor - first)) > available - 1)
        ++first;
    std::size_t last = m_cursor;
    int used = Width(std::u32string_view(m_line).substr(first, m_cursor - first));
    while (last < m_line.size() && used + DisplayWidth(m_line[last]) <= available) {
        used += DisplayWidth(m_line[last]);
        ++last;
    }

    std::string out = "\r";
    out += prompt;
    out += ToUtf8(std::u32string_view(m_line).substr(first, last - first));
    out += "\x1b[K";
    const int back = Width(std::u32string_view(m_line).substr(m_cursor, last - m_cursor));
    if (back > 0)
        out += "\x1b[" + std::to_string(back) + "D";
    return out;
}

//////////////////////////////////////////////////////////////////////////
// Terminal

namespace {

#if !defined(_WIN32)
struct termios g_savedTermios;
std::atomic<bool> g_rawMode{false};

void RestoreTermios()
{
    if (g_rawMode.exchange(false))
        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_savedTermios);
}

void OnStopSignal(int)
{
    g_stop = true;
}

// A crash must not leave the shell in raw mode: restore, then let the default action run.
void OnFatalSignal(int sig)
{
    if (g_rawMode.load())
        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_savedTermios);
    ::signal(sig, SIG_DFL);
    ::raise(sig);
}

void InstallStopSignals()
{
    struct sigaction sa = {};
    sa.sa_handler = OnStopSignal;
    sigemptyset(&sa.sa_mask);
    ::sigaction(SIGINT, &sa, nullptr);
    ::sigaction(SIGTERM, &sa, nullptr);
    ::sigaction(SIGHUP, &sa, nullptr);
}
#else
BOOL WINAPI OnConsoleCtrl(DWORD type)
{
    g_stop = true;
    if (type == CTRL_CLOSE_EVENT || type == CTRL_LOGOFF_EVENT || type == CTRL_SHUTDOWN_EVENT)
        ::Sleep(4500);  // returning ends the process: give the shutdown time (Windows allows ~5 s)
    return TRUE;
}

void InstallStopSignals()
{
    ::SetConsoleCtrlHandler(OnConsoleCtrl, TRUE);
}
#endif

} // namespace

struct Terminal::Impl
{
    Terminal* owner = nullptr;
    bool interactive = false;
    std::mutex mutex;               // output and editor
    LineEditor editor;
    KeyDecoder decoder;
    std::string prompt = "> ";
    bool promptShown = false;
    bool confirming = false;
    std::string title;

    // output hook
    bool hooked = false;
    std::function<void(std::string_view)> outputHandler;    // under mutex
    std::thread pump;

#if defined(_WIN32)
    HANDLE in = INVALID_HANDLE_VALUE;
    HANDLE out = INVALID_HANDLE_VALUE;
    DWORD savedInMode = 0;
    std::wstring savedTitle;
    int savedStdout = -1;
    int savedStderr = -1;
    int pipeRead = -1;
    int pipeWrite = -1;
#else
    int out = STDOUT_FILENO;
    int savedStdout = -1;
    int savedStderr = -1;
    int pipeRead = -1;
    int pipeWrite = -1;
#endif

    void RawWrite(std::string_view text);
    int Columns();
    void StartHook();
    void StopHook();
    void Redraw();
    void Output(std::string_view text);
};

void Terminal::Impl::Output(std::string_view text)
{
    std::function<void(std::string_view)> handler;
    {
        std::lock_guard lock(mutex);
        handler = outputHandler;
    }
    if (handler)
        handler(text);
    else
        owner->Write(text);
}

void Terminal::Impl::RawWrite(std::string_view text)
{
#if defined(_WIN32)
    if (out != INVALID_HANDLE_VALUE) {
        const std::wstring wide = Utf8ToWide(text);
        DWORD written = 0;
        if (!::WriteConsoleW(out, wide.data(), static_cast<DWORD>(wide.size()), &written, nullptr))
            ::WriteFile(out, text.data(), static_cast<DWORD>(text.size()), &written, nullptr);
    }
#else
    while (!text.empty()) {
        const ssize_t n = ::write(out, text.data(), text.size());
        if (n <= 0)
            break;
        text.remove_prefix(static_cast<std::size_t>(n));
    }
#endif
}

int Terminal::Impl::Columns()
{
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (::GetConsoleScreenBufferInfo(out, &info))
        return info.srWindow.Right - info.srWindow.Left + 1;
#else
    struct winsize ws = {};
    if (::ioctl(out, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        return ws.ws_col;
#endif
    return 80;
}

void Terminal::Impl::Redraw()
{
    if (!interactive)
        return;
    RawWrite(confirming ? std::string("\rAre you sure you want to exit? [y/N] \x1b[K") : editor.Render(prompt, Columns()));
    promptShown = true;
}

namespace {

// Input of the process, read by one thread for its whole life: a blocked read cannot be cancelled portably,
// so the thread is never joined, and it must not belong to a Terminal. Raw bytes (interactive) or lines.
// Lines are read from the OS handle, never through std::cin / stdio: a thread blocked in getline holds the stdin
// FILE lock, and exit() waits for that lock when it flushes the streams, so a server in --machine mode that was
// stopped through the protocol only exited once its stdin was closed (found on 2026-11-05).
struct InputQueue
{
    std::mutex mutex;
    std::condition_variable ready;
    std::deque<std::string> chunks;
    bool ended = false;
};

InputQueue& Input(bool raw)
{
    static InputQueue queue;
    static std::once_flag started;
    std::call_once(started, [raw] {
        std::thread([raw] {
            std::string pending;    // line mode: bytes after the last newline
            for (;;) {
                std::string chunk;
#if defined(_WIN32)
                if (raw) {
                    wchar_t buffer[256];
                    DWORD read = 0;
                    if (!::ReadConsoleW(::GetStdHandle(STD_INPUT_HANDLE), buffer, 256, &read, nullptr))
                        break;
                    chunk = WideToUtf8(std::wstring_view(buffer, read));
                } else
#else
                if (raw) {
                    char buffer[256];
                    const ssize_t n = ::read(STDIN_FILENO, buffer, sizeof(buffer));
                    if (n < 0 && errno == EINTR)
                        continue;
                    if (n <= 0)
                        break;
                    chunk.assign(buffer, static_cast<std::size_t>(n));
                } else
#endif
                {
                    char buffer[4096];
#if defined(_WIN32)
                    DWORD n = 0;
                    const bool ok = ::ReadFile(::GetStdHandle(STD_INPUT_HANDLE), buffer, sizeof(buffer), &n, nullptr);
                    if (!ok || n == 0)
                        break;
#else
                    const ssize_t n = ::read(STDIN_FILENO, buffer, sizeof(buffer));
                    if (n < 0 && errno == EINTR)
                        continue;
                    if (n <= 0)
                        break;
#endif
                    pending.append(buffer, static_cast<std::size_t>(n));
                    std::vector<std::string> lines;
                    for (std::size_t end; (end = pending.find('\n')) != std::string::npos;) {
                        std::string line = pending.substr(0, end);
                        if (!line.empty() && line.back() == '\r')
                            line.pop_back();
                        lines.push_back(std::move(line));
                        pending.erase(0, end + 1);
                    }
                    std::lock_guard lock(queue.mutex);
                    for (std::string& line : lines)
                        queue.chunks.push_back(std::move(line));
                    if (!lines.empty())
                        queue.ready.notify_one();
                    continue;
                }
                std::lock_guard lock(queue.mutex);
                queue.chunks.push_back(std::move(chunk));
                queue.ready.notify_one();
            }
            std::lock_guard lock(queue.mutex);
            if (!raw && !pending.empty()) {     // a last line without newline
                if (pending.back() == '\r')
                    pending.pop_back();
                queue.chunks.push_back(std::move(pending));
            }
            queue.ended = true;
            queue.ready.notify_one();
        }).detach();
    });
    return queue;
}

} // namespace

// stdout/stderr (fds 1 and 2) into a pipe whose lines are printed above the prompt (CHookStdio).
void Terminal::Impl::StartHook()
{
    std::fflush(stdout);
    std::fflush(stderr);
#if defined(_WIN32)
    int fds[2];
    if (::_pipe(fds, 65536, _O_BINARY | _O_NOINHERIT) != 0)
        return;
    savedStdout = ::_dup(1);
    savedStderr = ::_dup(2);
    pipeRead = fds[0];
    pipeWrite = fds[1];
    // _dup2 closes the handle fd 1 had, which is the console handle GetStdHandle gave: write through the copy
    out = reinterpret_cast<HANDLE>(::_get_osfhandle(savedStdout));
    ::_dup2(pipeWrite, 1);
    ::_dup2(pipeWrite, 2);
    ::SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(::_get_osfhandle(1)));
    ::SetStdHandle(STD_ERROR_HANDLE, reinterpret_cast<HANDLE>(::_get_osfhandle(2)));
#else
    int fds[2];
    if (::pipe(fds) != 0)
        return;
    ::fcntl(fds[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(fds[1], F_SETFD, FD_CLOEXEC);
    savedStdout = ::dup(STDOUT_FILENO);
    savedStderr = ::dup(STDERR_FILENO);
    out = savedStdout;
    pipeRead = fds[0];
    pipeWrite = fds[1];
    ::dup2(pipeWrite, STDOUT_FILENO);
    ::dup2(pipeWrite, STDERR_FILENO);
#endif
    std::setvbuf(stdout, nullptr, _IOLBF, BUFSIZ);
    hooked = true;

    pump = std::thread([this] {
        std::string pending;
        char buffer[4096];
        for (;;) {
            // Wait up to 100 ms for data; on a pause, an unfinished line is printed as it is.
#if defined(_WIN32)
            const HANDLE handle = reinterpret_cast<HANDLE>(::_get_osfhandle(pipeRead));
            DWORD available = 0;
            bool ready = false;
            for (int i = 0; i < 10 && !ready; ++i) {
                if (!::PeekNamedPipe(handle, nullptr, 0, nullptr, &available, nullptr))
                    break;
                ready = available > 0;
                if (!ready)
                    ::Sleep(10);
            }
            if (!::PeekNamedPipe(handle, nullptr, 0, nullptr, &available, nullptr) && !ready)
                break;  // all write ends closed
            int n = 0;
            if (ready)
                n = ::_read(pipeRead, buffer, sizeof(buffer));
            if (ready && n <= 0)
                break;
#else
            struct pollfd pfd = {pipeRead, POLLIN, 0};
            const int r = ::poll(&pfd, 1, 100);
            if (r < 0 && errno == EINTR)
                continue;
            const bool ready = r > 0;
            ssize_t n = 0;
            if (ready) {
                n = ::read(pipeRead, buffer, sizeof(buffer));
                if (n <= 0)
                    break;
            }
#endif
            if (ready) {
                pending.append(buffer, static_cast<std::size_t>(n));
                const std::size_t end = pending.rfind('\n');
                if (end != std::string::npos) {
                    Output(std::string_view(pending).substr(0, end + 1));
                    pending.erase(0, end + 1);
                }
            } else if (!pending.empty()) {
                Output(pending);
                pending.clear();
            }
        }
        if (!pending.empty())
            Output(pending);
    });
}

void Terminal::Impl::StopHook()
{
    if (!hooked)
        return;
    std::fflush(stdout);
    std::fflush(stderr);
#if defined(_WIN32)
    ::_dup2(savedStdout, 1);
    ::_dup2(savedStderr, 2);
    ::SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(::_get_osfhandle(1)));
    ::SetStdHandle(STD_ERROR_HANDLE, reinterpret_cast<HANDLE>(::_get_osfhandle(2)));
    ::_close(pipeWrite);
#else
    ::dup2(savedStdout, STDOUT_FILENO);
    ::dup2(savedStderr, STDERR_FILENO);
    ::close(pipeWrite);
#endif
    if (pump.joinable())
        pump.join();
#if defined(_WIN32)
    out = ::GetStdHandle(STD_OUTPUT_HANDLE);    // fd 1 has the console again; savedStdout's copy goes away
    ::_close(pipeRead);
    ::_close(savedStdout);
    ::_close(savedStderr);
#else
    out = STDOUT_FILENO;
    ::close(pipeRead);
    ::close(savedStdout);
    ::close(savedStderr);
#endif
    hooked = false;
}

Terminal::Terminal(Options options)
    : m_impl(std::make_unique<Impl>())
{
    Impl& d = *m_impl;
    d.owner = this;
    InstallStopSignals();
#if defined(_WIN32)
    d.in = ::GetStdHandle(STD_INPUT_HANDLE);
    d.out = ::GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD outMode = 0;
    d.interactive = !options.machine && ::GetConsoleMode(d.in, &d.savedInMode) && ::GetConsoleMode(d.out, &outMode);
    if (d.interactive) {
        // Keys arrive as text and VT sequences (the same decoder as elsewhere); Ctrl+C is a key, not a signal.
        const DWORD mode = (d.savedInMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT |
                                              ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) |
                           ENABLE_VIRTUAL_TERMINAL_INPUT;
        ::SetConsoleMode(d.in, mode);
        wchar_t title[512] = {};
        ::GetConsoleTitleW(title, 512);
        d.savedTitle = title;
    }
#else
    d.interactive = !options.machine && ::isatty(STDIN_FILENO) && ::isatty(STDOUT_FILENO);
    if (d.interactive && ::tcgetattr(STDIN_FILENO, &g_savedTermios) == 0) {
        struct termios raw = g_savedTermios;
        // No echo, no line buffering, no signal keys (Ctrl+C asks first); output processing stays ("\n" -> "\r\n").
        raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
        raw.c_iflag &= ~(IXON | ICRNL | INPCK | ISTRIP | BRKINT);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        g_rawMode = true;
        std::atexit(RestoreTermios);
        for (const int sig : {SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL})
            ::signal(sig, OnFatalSignal);
        d.RawWrite("\x1b[22;0t");     // save the title (xterm)
    } else {
        d.interactive = false;
    }
#endif
    if ((d.interactive && options.hookOutput) || options.machine)
        d.StartHook();
}

Terminal::~Terminal()
{
    Impl& d = *m_impl;
    {
        std::lock_guard lock(d.mutex);
        if (d.promptShown)
            d.RawWrite("\r\x1b[K");
        d.promptShown = false;
    }
    d.StopHook();
    if (!d.interactive)
        return;
#if defined(_WIN32)
    ::SetConsoleMode(d.in, d.savedInMode);
    ::SetConsoleTitleW(d.savedTitle.c_str());
#else
    d.RawWrite("\x1b[23;0t");     // restore the title
    RestoreTermios();
#endif
}

bool Terminal::Interactive() const
{
    return m_impl->interactive;
}

void Terminal::Write(std::string_view text)
{
    Impl& d = *m_impl;
    std::lock_guard lock(d.mutex);
    if (!d.interactive && !d.hooked) {
        std::fwrite(text.data(), 1, text.size(), stdout);
        if (!text.empty() && text.back() != '\n')
            std::fputc('\n', stdout);
        std::fflush(stdout);
        return;
    }
    std::string out;
    if (d.promptShown)
        out += "\r\x1b[K";
    out += text;
    if (!text.empty() && text.back() != '\n')
        out += '\n';
    d.RawWrite(out);
    if (d.promptShown)
        d.Redraw();
}

void Terminal::SetTitle(const std::string& title)
{
    Impl& d = *m_impl;
    std::lock_guard lock(d.mutex);
    d.title = title;
    if (!d.interactive)
        return;
#if defined(_WIN32)
    ::SetConsoleTitleW(Utf8ToWide(title).c_str());
#else
    d.RawWrite("\x1b]0;" + title + "\x07");
#endif
}

void Terminal::SetOutputHandler(std::function<void(std::string_view)> handler)
{
    std::lock_guard lock(m_impl->mutex);
    m_impl->outputHandler = std::move(handler);
}

void Terminal::SetPrompt(std::string prompt)
{
    Impl& d = *m_impl;
    std::lock_guard lock(d.mutex);
    d.prompt = std::move(prompt);
    if (d.promptShown)
        d.Redraw();
}

std::optional<std::string> Terminal::ReadLine(const std::function<void()>& idle)
{
    Impl& d = *m_impl;
    InputQueue& input = Input(d.interactive);
    {
        std::lock_guard lock(d.mutex);
        d.Redraw();
    }
    for (;;) {
        if (g_stop)
            return std::nullopt;

        std::deque<std::string> chunks;
        bool ended = false;
        {
            std::unique_lock lock(input.mutex);
            input.ready.wait_for(lock, std::chrono::milliseconds(100),
                                 [&input] { return !input.chunks.empty() || input.ended; });
            chunks.swap(input.chunks);
            ended = input.ended;
        }
        if (idle)
            idle();

        if (!d.interactive) {
            if (!chunks.empty()) {
                // Several lines may be queued: hand back the first, keep the rest
                std::string line = std::move(chunks.front());
                chunks.pop_front();
                std::lock_guard lock(input.mutex);
                for (auto it = chunks.rbegin(); it != chunks.rend(); ++it)
                    input.chunks.push_front(std::move(*it));
                return line;
            }
            if (ended)
                return std::nullopt;
            continue;
        }

        std::vector<Key> keys;
        for (const std::string& chunk : chunks)
            d.decoder.Feed(chunk, keys);
        if (chunks.empty() && d.decoder.Pending())
            d.decoder.Flush(keys);

        std::lock_guard lock(d.mutex);
        for (std::size_t i = 0; i < keys.size(); ++i) {
            const Key& key = keys[i];
            if (d.confirming) {
                d.confirming = false;
                if ((key.kind == Key::Char && (key.ch == U'y' || key.ch == U'Y')) || key.kind == Key::Interrupt) {
                    d.RawWrite("\r\x1b[K");
                    d.promptShown = false;
                    return std::nullopt;
                }
                continue;
            }
            if (key.kind == Key::ClearScreen) {
                d.RawWrite("\x1b[H\x1b[2J");
                continue;
            }
            switch (d.editor.Apply(key)) {
            case LineEditor::Result::Line: {
                // The entered line stays in the scrollback, then the prompt comes back below it
                std::string line = d.editor.Take();
                d.RawWrite("\r" + d.prompt + line + "\x1b[K\n");
                d.promptShown = false;
                // keys typed after Enter wait for the next ReadLine
                if (i + 1 < keys.size()) {
                    std::string rest;
                    for (std::size_t k = i + 1; k < keys.size(); ++k)
                        if (keys[k].kind == Key::Char)
                            AppendUtf8(rest, keys[k].ch);
                        else if (keys[k].kind == Key::Enter)
                            rest += '\r';
                    std::lock_guard inputLock(input.mutex);
                    input.chunks.push_front(std::move(rest));
                }
                return line;
            }
            case LineEditor::Result::Interrupt:
            case LineEditor::Result::EndOfInput:
                d.confirming = true;
                break;
            case LineEditor::Result::None:
                break;
            }
        }
        if (ended && !d.confirming)
            return std::nullopt;
        if (!keys.empty())
            d.Redraw();
    }
}

void Terminal::RequestStop()
{
    g_stop = true;
}

bool Terminal::StopRequested()
{
    return g_stop;
}

void WaitForStop(const std::function<void()>& idle)
{
    InstallStopSignals();
    while (!g_stop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (idle)
            idle();
    }
}

} // namespace x2::platform
