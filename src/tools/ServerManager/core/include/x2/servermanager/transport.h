#pragma once

// How ServerManager reaches a server: always a child process speaking protocol lines on stdin/stdout.
// - a server it starts: `<server> <args> --machine` (the server exits at the end of its stdin);
// - a server already running here: `x2ctl attach <name>` (its control socket);
// - a remote server: `ssh host x2ctl attach <name>`.
// Ending the child always unblocks its readers, which a socket closed from another thread does not on Windows
// (docs/porting/console.md). Reader threads hand lines to an Inbox that the UI thread drains.

#include "x2/platform/process.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace x2::servermanager {

struct TransportEvent {
    enum Kind { Line, Closed };
    int server = -1;                // index given to Open
    std::uint64_t generation = 0;   // tells an old connection's events from the current one's
    long long pid = 0;              // the child's
    Kind kind = Line;
    std::string text;               // Line: the line; Closed: why
    bool error = false;             // Line: from stderr
    std::optional<int> exitCode;    // Closed: the child's exit code, when it exited
};

// Thread-safe queue from the reader threads to the thread that owns the sessions.
class Inbox {
public:
    void Push(TransportEvent event);
    std::vector<TransportEvent> Drain();
    // Waits up to `timeout` for an event (tests; the UI uses SetWake to wake its event loop).
    bool Wait(std::chrono::milliseconds timeout);
    // Called (from the reader threads) after each Push.
    void SetWake(std::function<void()> wake);

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<TransportEvent> m_events;
    std::function<void()> m_wake;
};

class ProcessTransport {
public:
    ProcessTransport(x2::platform::ProcessOptions options, std::string description);
    // Kills the child if it still runs, then joins the readers.
    ~ProcessTransport();
    ProcessTransport(const ProcessTransport&) = delete;
    ProcessTransport& operator=(const ProcessTransport&) = delete;

    // Starts the child and its readers: every stdout and stderr line, then one Closed, go to `inbox`.
    bool Open(Inbox& inbox, int server, std::uint64_t generation, std::string& error);

    bool WriteLine(const std::string& line);
    void CloseInput();      // end of the child's stdin
    void Terminate();
    void Kill();
    long long Pid() const;
    bool Running();
    const std::string& Description() const { return m_description; }

private:
    void Finished();

    x2::platform::ProcessOptions m_options;
    std::string m_description;
    std::unique_ptr<x2::platform::Process> m_process;
    Inbox* m_inbox = nullptr;
    int m_server = -1;
    std::uint64_t m_generation = 0;
    std::atomic<int> m_readers{0};
    std::thread m_out;
    std::thread m_err;
};

// The command line of a ProcessOptions, for display.
std::string DescribeCommand(const x2::platform::ProcessOptions& options);

}  // namespace x2::servermanager
