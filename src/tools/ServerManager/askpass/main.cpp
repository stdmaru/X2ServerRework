// x2askpass: ssh's SSH_ASKPASS for ServerManager (docs/tools/servermanager.md#ssh-prompts).
//   x2askpass <prompt>
// Sends the prompt to the window's AskpassBroker (X2_ASKPASS, X2_ASKPASS_TOKEN) and prints its answer for ssh.
// Exit code 0 with the answer, 1 when declined or when the window cannot be reached.

#include "x2/platform/control.h"
#include "x2/platform/json.h"
#include "x2/servermanager/askpass.h"

#include <cstdio>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[])
{
    using namespace x2::servermanager;
    const char* socket = std::getenv(kAskpassSocketVar);
    const char* token = std::getenv(kAskpassTokenVar);
    const char* sshPrompt = std::getenv("SSH_ASKPASS_PROMPT");
    if (!socket || !*socket || !token) {
        std::fprintf(stderr, "x2askpass: run by ssh from ServerManager only (%s is not set)\n", kAskpassSocketVar);
        return 1;
    }
    const std::string prompt = argc > 1 ? argv[1] : "";

    x2::platform::ControlClient client;
    std::string error;
    std::string greeting;
    if (!client.Connect(socket, error) || !client.ReadLine(greeting)) {
        std::fprintf(stderr, "x2askpass: ServerManager is not reachable: %s\n", error.c_str());
        return 1;
    }
    std::string request = "{\"token\":";
    x2::platform::AppendJsonString(request, token);
    request += ",\"prompt\":";
    x2::platform::AppendJsonString(request, prompt);
    request += ",\"kind\":";
    x2::platform::AppendJsonString(request, ToString(ClassifyPrompt(prompt, sshPrompt ? sshPrompt : "")));
    request += '}';
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
