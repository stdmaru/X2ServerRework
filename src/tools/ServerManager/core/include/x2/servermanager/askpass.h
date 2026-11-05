#pragma once

// ssh's questions in the window (docs/tools/servermanager.md#ssh-prompts). ssh runs $SSH_ASKPASS (x2askpass, with
// SSH_ASKPASS_REQUIRE=force) for a password, a key passphrase or an unknown host key; x2askpass connects to the broker's
// socket, sends the prompt and prints the answer the window gives. The variables are set in this process's
// environment, so every ssh started from then on inherits them.
//
// Helper protocol, one JSON line each way after the server's greeting:
//   {"token":"<X2_ASKPASS_TOKEN>","prompt":"...","kind":"secret|confirm|info"}   x2askpass -> broker
//   {"ok":true,"answer":"..."} | {"ok":false}                                    broker -> x2askpass (exit 0 / 1)

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

// Environment of x2askpass, set by AskpassBroker::Start.
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

struct AskpassRequest {
    int id = 0;
    std::string prompt;
    AskpassKind kind = AskpassKind::Secret;
};

class AskpassBroker {
public:
    AskpassBroker() = default;
    ~AskpassBroker();   // Stop
    AskpassBroker(const AskpassBroker&) = delete;
    AskpassBroker& operator=(const AskpassBroker&) = delete;

    // Listens on a private socket and sets SSH_ASKPASS=`helper`, SSH_ASKPASS_REQUIRE=force and the X2_ASKPASS
    // variables. False and `error` when the helper does not exist or the socket cannot be opened.
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
