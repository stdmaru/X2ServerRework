// AskpassBroker with the real x2askpass (docs/tools/servermanager.md#ssh-prompts), and with the real ssh when a local
// sshd answers on 127.0.0.1.

#include "x2/servermanager/askpass.h"
#include "x2/servermanager/profile.h"

#include "x2/platform/process.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <thread>

using namespace x2::servermanager;
using namespace std::chrono_literals;

TEST_CASE("askpass: prompt kinds")
{
    CHECK(ClassifyPrompt("x2@vps's password: ", "") == AskpassKind::Secret);
    CHECK(ClassifyPrompt("Enter passphrase for key '/home/x2/.ssh/id_ed25519': ", "") == AskpassKind::Secret);
    CHECK(ClassifyPrompt("The authenticity of host 'vps (10.0.0.5)' can't be established.\nED25519 key fingerprint is "
                         "SHA256:abc.\nAre you sure you want to continue connecting (yes/no/[fingerprint])? ", "")
          == AskpassKind::Confirm);
    CHECK(ClassifyPrompt("Please type 'yes', 'no' or the fingerprint: ", "") == AskpassKind::Confirm);
    CHECK(ClassifyPrompt("Allow use of key?", "confirm") == AskpassKind::Confirm);
    CHECK(ClassifyPrompt("Confirm user presence for key", "none") == AskpassKind::Info);
    for (const AskpassKind kind : {AskpassKind::Secret, AskpassKind::Confirm, AskpassKind::Info})
        CHECK(ParseAskpassKind(ToString(kind)) == kind);
    CHECK_FALSE(ParseAskpassKind("other"));
}

#if defined(X2_ASKPASS_HELPER)
namespace {

std::unique_ptr<x2::platform::Process> Helper(const std::string& prompt)
{
    x2::platform::ProcessOptions options;
    options.program = X2_ASKPASS_HELPER;
    options.args = {prompt};
    std::string error;
    auto p = x2::platform::Process::Start(options, error);
    REQUIRE_MESSAGE(p, error);
    return p;
}

// The first pending request, waiting up to 15 s.
std::optional<AskpassRequest> NextRequest(AskpassBroker& broker)
{
    const auto deadline = std::chrono::steady_clock::now() + 15s;
    while (std::chrono::steady_clock::now() < deadline) {
        const auto pending = broker.Pending();
        if (!pending.empty())
            return pending.front();
        std::this_thread::sleep_for(10ms);
    }
    return std::nullopt;
}

bool HasBatchMode()
{
    const auto command = DefaultSshCommand();
    return std::find(command.begin(), command.end(), "BatchMode=yes") != command.end();
}

}  // namespace

TEST_CASE("askpass: x2askpass asks the broker and prints its answer")
{
    AskpassBroker broker;
    std::string error;
    CHECK_FALSE(broker.Start("/nonexistent/x2askpass", error));
    REQUIRE_MESSAGE(broker.Start(X2_ASKPASS_HELPER, error), error);
    CHECK(std::string(std::getenv("SSH_ASKPASS")) == X2_ASKPASS_HELPER);
    CHECK(std::string(std::getenv("SSH_ASKPASS_REQUIRE")) == "force");
    CHECK_FALSE(HasBatchMode());     // ssh may ask now

    // a secret, remembered
    auto first = Helper("x2@vps's password: ");
    auto request = NextRequest(broker);
    REQUIRE(request);
    CHECK(request->prompt == "x2@vps's password: ");
    CHECK(request->kind == AskpassKind::Secret);
    broker.Answer(request->id, std::string("s3cret \"quoted\""), true);
    std::string line;
    REQUIRE(first->ReadLine(line));
    CHECK(line == "s3cret \"quoted\"");
    CHECK(first->Wait() == 0);
    auto again = Helper("x2@vps's password: ");      // answered without asking
    REQUIRE(again->ReadLine(line));
    CHECK(line == "s3cret \"quoted\"");
    CHECK(again->Wait() == 0);
    CHECK(broker.Pending().empty());

    // a host key, declined: declined again at once until ForgetDeclined
    const std::string hostKey = "The authenticity of host 'far' can't be established.\n"
                                "Are you sure you want to continue connecting (yes/no/[fingerprint])? ";
    auto declined = Helper(hostKey);
    request = NextRequest(broker);
    REQUIRE(request);
    CHECK(request->prompt == hostKey);         // the whole multi-line prompt
    CHECK(request->kind == AskpassKind::Confirm);
    broker.Answer(request->id, std::nullopt);
    CHECK_FALSE(declined->ReadLine(line));
    CHECK(declined->Wait() == 1);
    auto quick = Helper(hostKey);
    CHECK(quick->Wait() == 1);
    CHECK(broker.Pending().empty());
    broker.ForgetDeclined();

    // two helpers with the same question get one answer
    auto a = Helper(hostKey);
    auto b = Helper(hostKey);
    const auto deadline = std::chrono::steady_clock::now() + 15s;
    while (broker.Pending().size() < 2 && std::chrono::steady_clock::now() < deadline)
        std::this_thread::sleep_for(10ms);
    REQUIRE(broker.Pending().size() == 2);
    broker.Answer(broker.Pending().front().id, std::string("yes"));
    for (auto* p : {a.get(), b.get()}) {
        REQUIRE(p->ReadLine(line));
        CHECK(line == "yes");
        CHECK(p->Wait() == 0);
    }

    // a helper without the token is refused
    const std::string token = std::getenv(kAskpassTokenVar);
    x2::platform::SetEnvironment(kAskpassTokenVar, "wrong");
    auto stranger = Helper("x2@vps's password: ");
    x2::platform::SetEnvironment(kAskpassTokenVar, token);
    CHECK(stranger->Wait() == 1);

    broker.Stop();
    CHECK(HasBatchMode());
    const char* gone = std::getenv("SSH_ASKPASS");
    CHECK((gone == nullptr || *gone == '\0'));
    auto alone = Helper("x2@vps's password: ");     // nothing to ask
    CHECK(alone->Wait() == 1);
}

TEST_CASE("askpass: the real ssh asks through it (when an sshd answers on 127.0.0.1)")
{
    AskpassBroker broker;
    std::string error;
    REQUIRE_MESSAGE(broker.Start(X2_ASKPASS_HELPER, error), error);
    const auto knownHosts = std::filesystem::temp_directory_path() / "x2-askpass-known-hosts";
    std::error_code ec;
    std::filesystem::remove(knownHosts, ec);
    x2::platform::ProcessOptions options;
    options.program = "ssh";
    options.args = {"-T", "-o", "ConnectTimeout=5", "-o", "UserKnownHostsFile=" + knownHosts.string(),
                    "-o", "StrictHostKeyChecking=ask", "-o", "PubkeyAuthentication=no", "-o", "NumberOfPasswordPrompts=1",
                    "x2-askpass-nobody@127.0.0.1", "exit"};
    auto ssh = x2::platform::Process::Start(options, error);
    if (!ssh) {
        MESSAGE("no ssh: skipped (" << error << ")");
        return;
    }
    bool hostKey = false, password = false;
    for (;;) {
        if (auto code = ssh->WaitFor(0ms)) {
            CHECK(*code != 0);      // nobody's password is "wrong"
            break;
        }
        const auto pending = broker.Pending();
        if (pending.empty()) {
            std::this_thread::sleep_for(20ms);
            continue;
        }
        const AskpassRequest& r = pending.front();
        MESSAGE("ssh asked (" << ToString(r.kind) << "): " << r.prompt);
        if (r.kind == AskpassKind::Confirm) {
            hostKey = true;
            broker.Answer(r.id, std::string("yes"));
        } else {
            password = password || r.prompt.find("password") != std::string::npos;
            broker.Answer(r.id, std::string("wrong"));
        }
    }
    if (!hostKey)
        MESSAGE("no sshd on 127.0.0.1: skipped");
    else
        CHECK(std::filesystem::exists(knownHosts));    // "yes" was taken
    (void)password;
    std::filesystem::remove(knownHosts, ec);
}
#endif
