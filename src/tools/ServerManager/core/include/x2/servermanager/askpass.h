#pragma once

// ssh's questions in the window (docs/tools/servermanager.md#ssh-prompts). ssh runs $SSH_ASKPASS (ServerManager itself,
// with SSH_ASKPASS_REQUIRE=force) for a password, a key passphrase or an unknown host key; run that way, ServerManager
// only calls RunAskpass: it connects to the broker's socket, sends the prompt and prints the answer the window gives.
// The variables are set in this process's environment, so every ssh started from then on inherits them.
//
// Helper protocol, one JSON line each way after the server's greeting:
//   {"token":"<X2_ASKPASS_TOKEN>","prompt":"...","kind":"secret|confirm|info","ppid":<ssh's pid>}  helper -> broker
//   {"ok":true,"answer":"..."} | {"ok":false}                                    broker -> helper (exit 0 / 1)

#include "x2/platform/control.h"

#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace x2::servermanager {

// Environment of the helper, set by AskpassBroker::Start.
inline constexpr const char* kAskpassSocketVar = "X2_ASKPASS";        // control socket name (askpass/<...>)
inline constexpr const char* kAskpassTokenVar = "X2_ASKPASS_TOKEN";

enum class AskpassKind {
    Secret,     // a password, passphrase or PIN: hidden input
    Confirm,    // yes or no: an unknown host key, or SSH_ASKPASS_PROMPT=confirm
    Info,       // SSH_ASKPASS_PROMPT=none: only a message
};

// The kind of a prompt from its text and SSH_ASKPASS_PROMPT.
AskpassKind ClassifyPrompt(std::string_view prompt, std::string_view sshPromptVar);
const char* ToString(AskpassKind kind);
std::optional<AskpassKind> ParseAskpassKind(std::string_view text);

// True when this process was started by ssh as its askpass (SSH_ASKPASS and X2_ASKPASS are set): the program then calls
// RunAskpass and exits, before anything else (no window).
bool IsAskpassInvocation();
// The helper: `<program> <prompt>`. Prints the answer and returns 0, or returns 1 when declined or when the window cannot
// be reached (with a message on stderr).
int RunAskpass(int argc, char* argv[]);

struct AskpassRequest {
    int id = 0;
    std::string prompt;
    AskpassKind kind = AskpassKind::Secret;
    long long pid = 0;      // of the ssh that asks (the helper's parent); 0 when unknown
};

class AskpassBroker {
public:
    AskpassBroker() = default;
    ~AskpassBroker();   // Stop
    AskpassBroker(const AskpassBroker&) = delete;
    AskpassBroker& operator=(const AskpassBroker&) = delete;

    // Listens on a private socket and sets SSH_ASKPASS=`helper` (a program that calls RunAskpass: ServerManager itself),
    // SSH_ASKPASS_REQUIRE=force and the X2_ASKPASS variables. False and `error` when the helper does not exist or the
    // socket cannot be opened.
    bool Start(const std::string& helper, std::string& error);
    // Closes the socket and removes the variables (later ssh children run with BatchMode again).
    void Stop();
    bool Running() const { return m_server.Running(); }

    // Called from the socket's threads when a question arrives (WakeEventLoop).
    void SetWake(std::function<void()> wake);
    // Questions waiting for an answer, oldest first. Remembered answers and declined prompts are answered without
    // showing up here.
    std::vector<AskpassRequest> Pending();
    // nullopt declines (the helper exits 1: ssh gives up). `remember` keeps a secret for the same prompt until Stop.
    void Answer(int id, std::optional<std::string> answer, bool remember = false);
    // A declined prompt is declined again at once (discovery asks every few seconds), until this is called (Refresh).
    void ForgetDeclined();

private:
    void OnLine(int client, std::string line);
    void Reply(int client, const std::optional<std::string>& answer);

    x2::platform::ControlServer m_server;
    std::string m_token;
    std::function<void()> m_wake;
    std::mutex m_mutex;
    std::vector<AskpassRequest> m_pending;
    std::map<std::string, std::string> m_remembered;   // prompt -> secret
    std::set<std::string> m_declined;                   // prompts
};

}  // namespace x2::servermanager
