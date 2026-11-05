#include "x2/servermanager/transport.h"

namespace x2::servermanager {

using namespace std::chrono_literals;

void Inbox::Push(TransportEvent event)
{
    std::function<void()> wake;
    {
        std::lock_guard lock(m_mutex);
        m_events.push_back(std::move(event));
        wake = m_wake;
    }
    m_cv.notify_all();
    if (wake)
        wake();
}

std::vector<TransportEvent> Inbox::Drain()
{
    std::lock_guard lock(m_mutex);
    std::vector<TransportEvent> events(std::make_move_iterator(m_events.begin()), std::make_move_iterator(m_events.end()));
    m_events.clear();
    return events;
}

bool Inbox::Wait(std::chrono::milliseconds timeout)
{
    std::unique_lock lock(m_mutex);
    return m_cv.wait_for(lock, timeout, [this] { return !m_events.empty(); });
}

void Inbox::SetWake(std::function<void()> wake)
{
    std::lock_guard lock(m_mutex);
    m_wake = std::move(wake);
}

std::string DescribeCommand(const x2::platform::ProcessOptions& options)
{
    std::string text = x2::platform::QuoteShellArgument(options.program);
    for (const std::string& arg : options.args)
        text += ' ' + x2::platform::QuoteShellArgument(arg);
    if (!options.workingDirectory.empty())
        text += "  (in " + options.workingDirectory + ")";
    return text;
}

ProcessTransport::ProcessTransport(x2::platform::ProcessOptions options, std::string description)
    : m_options(std::move(options)), m_description(std::move(description))
{
    if (m_description.empty())
        m_description = DescribeCommand(m_options);
}

ProcessTransport::~ProcessTransport()
{
    if (m_process) {
        m_process->CloseStdin();
        if (!m_process->WaitFor(0ms))
            m_process->Kill();
    }
    if (m_out.joinable())
        m_out.join();
    if (m_err.joinable())
        m_err.join();
}

bool ProcessTransport::Open(Inbox& inbox, int server, std::uint64_t generation, std::string& error)
{
    m_process = x2::platform::Process::Start(m_options, error);
    if (!m_process)
        return false;
    m_inbox = &inbox;
    m_server = server;
    m_generation = generation;
    m_readers = 2;

    auto line = [this](std::string text, bool fromStderr) {
        TransportEvent event;
        event.server = m_server;
        event.generation = m_generation;
        event.text = std::move(text);
        event.error = fromStderr;
        m_inbox->Push(std::move(event));
    };
    m_out = std::thread([this, line] {
        std::string text;
        while (m_process->ReadLine(text))
            line(std::move(text), false);
        Finished();
    });
    m_err = std::thread([this, line] {
        std::string text;
        while (m_process->ReadErrorLine(text))
            line(std::move(text), true);
        Finished();
    });
    return true;
}

// The last reader to finish reports the end, with the exit code if the child exits soon after closing its streams.
void ProcessTransport::Finished()
{
    if (--m_readers != 0)
        return;
    TransportEvent event;
    event.server = m_server;
    event.generation = m_generation;
    event.kind = TransportEvent::Closed;
    event.exitCode = m_process->WaitFor(5s);
    event.text = event.exitCode ? "exited with code " + std::to_string(*event.exitCode) : "its output was closed";
    m_inbox->Push(std::move(event));
}

bool ProcessTransport::WriteLine(const std::string& line)
{
    return m_process && m_process->WriteLine(line);
}

void ProcessTransport::CloseInput()
{
    if (m_process)
        m_process->CloseStdin();
}

void ProcessTransport::Terminate()
{
    if (m_process)
        m_process->Terminate();
}

void ProcessTransport::Kill()
{
    if (m_process)
        m_process->Kill();
}

long long ProcessTransport::Pid() const
{
    return m_process ? m_process->Pid() : 0;
}

bool ProcessTransport::Running()
{
    return m_process && m_process->Running();
}

}  // namespace x2::servermanager
