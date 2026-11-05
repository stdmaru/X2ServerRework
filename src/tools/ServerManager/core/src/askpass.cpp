#include "x2/servermanager/askpass.h"

#include "x2/platform/json.h"
#include "x2/platform/process.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <random>

namespace x2::servermanager {

namespace {

bool Contains(std::string_view text, std::string_view part)
{
    return text.find(part) != std::string_view::npos;
}

std::string RandomHex(std::size_t bytes)
{
    std::random_device random;
    std::string out;
    for (std::size_t i = 0; i < bytes; ++i) {
        char hex[3];
        std::snprintf(hex, sizeof(hex), "%02x", static_cast<unsigned>(random() & 0xff));
        out += hex;
    }
    return out;
}

}  // namespace

AskpassKind ClassifyPrompt(std::string_view prompt, std::string_view sshPromptVar)
{
    if (sshPromptVar == "confirm")
        return AskpassKind::Confirm;
    if (sshPromptVar == "none")
        return AskpassKind::Info;
    // an unknown host key: "Are you sure you want to continue connecting (yes/no/[fingerprint])?", and again
    // "Please type 'yes', 'no' or the fingerprint:" after any other answer
    if (Contains(prompt, "(yes/no") || Contains(prompt, "'yes', 'no'"))
        return AskpassKind::Confirm;
    return AskpassKind::Secret;
}

const char* ToString(AskpassKind kind)
{
    switch (kind) {
    case AskpassKind::Confirm: return "confirm";
    case AskpassKind::Info: return "info";
    case AskpassKind::Secret: break;
    }
    return "secret";
}

std::optional<AskpassKind> ParseAskpassKind(std::string_view text)
{
    for (const AskpassKind kind : {AskpassKind::Secret, AskpassKind::Confirm, AskpassKind::Info})
        if (text == ToString(kind))
            return kind;
    return std::nullopt;
}

bool IsAskpassInvocation()
{
    const char* socket = std::getenv(kAskpassSocketVar);
    const char* askpass = std::getenv("SSH_ASKPASS");
    return socket && *socket && askpass && *askpass;
}

int RunAskpass(int argc, char* argv[])
{
    const char* socket = std::getenv(kAskpassSocketVar);
    const char* token = std::getenv(kAskpassTokenVar);
    const char* sshPrompt = std::getenv("SSH_ASKPASS_PROMPT");
    if (!socket || !*socket || !token) {
        std::fprintf(stderr, "askpass: run by ssh from ServerManager only (%s is not set)\n", kAskpassSocketVar);
        return 1;
    }
    const std::string prompt = argc > 1 ? argv[1] : "";

    x2::platform::ControlClient client;
    std::string error;
    std::string greeting;
    if (!client.Connect(socket, error) || !client.ReadLine(greeting)) {
        std::fprintf(stderr, "askpass: ServerManager is not reachable: %s\n", error.c_str());
        return 1;
    }
    std::string request = "{\"token\":";
    x2::platform::AppendJsonString(request, token);
    request += ",\"prompt\":";
    x2::platform::AppendJsonString(request, prompt);
    request += ",\"kind\":";
    x2::platform::AppendJsonString(request, ToString(ClassifyPrompt(prompt, sshPrompt ? sshPrompt : "")));
    request += ",\"ppid\":" + std::to_string(x2::platform::ParentProcessId()) + '}';
    std::string reply;
    if (!client.WriteLine(request) || !client.ReadLine(reply))
        return 1;
    const auto answer = x2::platform::ParseJsonObject(reply);
    if (!answer)
        return 1;
    const auto ok = answer->find("ok");
    if (ok == answer->end() || ok->second.text != "true")
        return 1;
    if (const auto text = answer->find("answer"); text != answer->end()) {
        std::fwrite(text->second.text.data(), 1, text->second.text.size(), stdout);
        std::fputc('\n', stdout);
    }
    return std::fflush(stdout) == 0 ? 0 : 1;
}

AskpassBroker::~AskpassBroker()
{
    Stop();
}

bool AskpassBroker::Start(const std::string& helper, std::string& error)
{
    std::error_code ec;
    if (!std::filesystem::is_regular_file(
            std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(helper.data()), helper.size())), ec)) {
        error = helper + ": not found";
        return false;
    }
    // a folder of their own, so x2ctl list does not take the broker for a server
    std::filesystem::create_directories(std::filesystem::path(x2::platform::ControlSocketDir()) / "askpass", ec);
    m_token = RandomHex(16);
    const std::string name = "askpass/ServerManager-" + RandomHex(4);
    if (!m_server.Start(name, [] { return std::string(R"({"type":"askpass","protocol":1})"); },
                        [this](int client, std::string line) { OnLine(client, std::move(line)); }, error))
        return false;
    x2::platform::SetEnvironment("SSH_ASKPASS", helper);
    x2::platform::SetEnvironment("SSH_ASKPASS_REQUIRE", "force");
    x2::platform::SetEnvironment(kAskpassSocketVar, name);
    x2::platform::SetEnvironment(kAskpassTokenVar, m_token);
    return true;
}

void AskpassBroker::Stop()
{
    if (!m_server.Running())
        return;
    for (const char* name : {"SSH_ASKPASS", "SSH_ASKPASS_REQUIRE", kAskpassSocketVar, kAskpassTokenVar})
        x2::platform::SetEnvironment(name, "");
    m_server.Stop();
    const std::lock_guard lock(m_mutex);
    m_pending.clear();
    m_remembered.clear();
    m_declined.clear();
}

void AskpassBroker::SetWake(std::function<void()> wake)
{
    const std::lock_guard lock(m_mutex);
    m_wake = std::move(wake);
}

void AskpassBroker::Reply(int client, const std::optional<std::string>& answer)
{
    std::string line = answer ? R"({"ok":true,"answer":)" : R"({"ok":false)";
    if (answer)
        x2::platform::AppendJsonString(line, *answer);
    line += '}';
    m_server.Send(client, line);
}

void AskpassBroker::OnLine(int client, std::string line)
{
    const auto message = x2::platform::ParseJsonObject(line);
    const auto field = [&](const char* key) -> std::string {
        if (!message)
            return {};
        const auto it = message->find(key);
        return it != message->end() && it->second.kind == x2::platform::JsonValue::String ? it->second.text : std::string();
    };
    if (!message || field("token") != m_token) {
        Reply(client, std::nullopt);    // not ours: another user cannot reach the folder, but a stale helper could
        return;
    }
    AskpassRequest request;
    request.id = client;
    request.prompt = field("prompt");
    request.kind = ParseAskpassKind(field("kind")).value_or(AskpassKind::Secret);
    if (const auto ppid = message->find("ppid"); ppid != message->end() && ppid->second.kind == x2::platform::JsonValue::Number)
        request.pid = std::strtoll(ppid->second.text.c_str(), nullptr, 10);

    std::function<void()> wake;
    {
        const std::lock_guard lock(m_mutex);
        if (const auto it = m_remembered.find(request.prompt); it != m_remembered.end()) {
            Reply(client, it->second);
            return;
        }
        if (m_declined.contains(request.prompt)) {
            Reply(client, std::nullopt);
            return;
        }
        m_pending.push_back(std::move(request));
        wake = m_wake;
    }
    if (wake)
        wake();
}

std::vector<AskpassRequest> AskpassBroker::Pending()
{
    const std::lock_guard lock(m_mutex);
    return m_pending;
}

void AskpassBroker::Answer(int id, std::optional<std::string> answer, bool remember)
{
    // the same question from another ssh (a host's discovery and an attach at once) gets the same answer
    std::vector<int> clients;
    {
        const std::lock_guard lock(m_mutex);
        const auto it = std::find_if(m_pending.begin(), m_pending.end(), [&](const AskpassRequest& r) { return r.id == id; });
        if (it == m_pending.end())
            return;
        const AskpassRequest request = *it;
        if (!answer)
            m_declined.insert(request.prompt);
        else if (remember && request.kind == AskpassKind::Secret)
            m_remembered[request.prompt] = *answer;
        std::erase_if(m_pending, [&](const AskpassRequest& r) {
            if (r.prompt != request.prompt || r.kind != request.kind)
                return false;
            clients.push_back(r.id);
            return true;
        });
    }
    for (const int client : clients)
        Reply(client, answer);
}

void AskpassBroker::ForgetDeclined()
{
    const std::lock_guard lock(m_mutex);
    m_declined.clear();
}

}  // namespace x2::servermanager
