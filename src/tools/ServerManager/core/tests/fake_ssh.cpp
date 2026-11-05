// Plays ssh for the tests: `x2_fake_ssh [ssh options] <destination> <command>` runs the command here, like a remote
// shell would (words split on spaces, single quotes as in sh), relaying stdin, stdout and stderr. The destination must
// be "fake-host"; anything else fails the way ssh does (exit code 255).

#include "x2/platform/process.h"
#include "x2/platform/console.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

// sh-like words: spaces separate, '...' quotes, a backslash escapes (enough for QuoteShellArgument's output)
std::vector<std::string> Words(std::string_view line)
{
    std::vector<std::string> words;
    std::string word;
    bool inWord = false;
    bool quoted = false;
    bool escaped = false;
    for (const char c : line) {
        if (escaped) {
            word += c;
            escaped = false;
        } else if (quoted) {
            if (c == '\'')
                quoted = false;
            else
                word += c;
        } else if (c == '\'') {
            quoted = inWord = true;
        } else if (c == '\\') {
            escaped = inWord = true;
        } else if (c == ' ') {
            if (inWord)
                words.push_back(word);
            word.clear();
            inWord = false;
        } else {
            word += c;
            inWord = true;
        }
    }
    if (inWord)
        words.push_back(word);
    return words;
}

[[noreturn]] void Exit(int code)
{
    std::fflush(stdout);
    std::fflush(stderr);
    std::_Exit(code);   // the stdin thread may still be blocked in a read
}

}  // namespace

int main(int argc, char* argv[])
{
    const std::vector<std::string> args = x2::platform::Utf8Arguments(argc, argv);
    std::size_t i = 1;
    const std::string_view withValue = "bcDEeFIiJLlmOoPpRSWw";
    while (i < args.size() && args[i].size() > 1 && args[i][0] == '-') {
        if (args[i].size() == 2 && withValue.find(args[i][1]) != std::string_view::npos)
            ++i;
        ++i;
    }
    if (i >= args.size()) {
        std::fputs("usage: x2_fake_ssh [options] destination command\n", stderr);
        Exit(255);
    }
    if (args[i] != "fake-host") {
        std::fprintf(stderr, "ssh: Could not resolve hostname %s: Name or service not known\n", args[i].c_str());
        Exit(255);
    }
    std::string command;
    for (++i; i < args.size(); ++i)
        command += (command.empty() ? "" : " ") + args[i];
    const std::vector<std::string> words = Words(command);
    if (words.empty())
        Exit(255);

    x2::platform::ProcessOptions options;
    options.program = words[0];
    options.args.assign(words.begin() + 1, words.end());
    std::string error;
    auto child = x2::platform::Process::Start(options, error);
    if (!child) {
        std::fprintf(stderr, "sh: %s: %s\n", words[0].c_str(), error.c_str());
        Exit(127);
    }

    std::thread([&child] {
        char buffer[4096];
        while (std::fgets(buffer, sizeof(buffer), stdin)) {
            std::string line = buffer;
            while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
                line.pop_back();
            if (!child->WriteLine(line))
                break;
        }
        child->CloseStdin();
    }).detach();
    std::thread errors([&child] {
        std::string line;
        while (child->ReadErrorLine(line)) {
            std::fprintf(stderr, "%s\n", line.c_str());
            std::fflush(stderr);
        }
    });
    std::string line;
    while (child->ReadLine(line)) {
        std::fprintf(stdout, "%s\n", line.c_str());
        std::fflush(stdout);
    }
    errors.join();
    Exit(child->Wait());
}
