#include "x2/platform/process.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <filesystem>
#include <thread>

using namespace x2::platform;
using namespace std::chrono_literals;

namespace {

std::unique_ptr<Process> Helper(std::vector<std::string> args, bool separateStderr = true)
{
    ProcessOptions options;
    options.program = X2_PROCESS_HELPER;
    options.args = std::move(args);
    options.separateStderr = separateStderr;
    std::string error;
    auto p = Process::Start(options, error);
    REQUIRE_MESSAGE(p, error);
    return p;
}

std::vector<std::string> AllLines(Process& p)
{
    std::vector<std::string> lines;
    std::string line;
    while (p.ReadLine(line))
        lines.push_back(line);
    return lines;
}

}  // namespace

TEST_CASE("process: quoting")
{
    CHECK(QuoteWindowsArgument("plain") == "plain");
    CHECK(QuoteWindowsArgument("") == "\"\"");
    CHECK(QuoteWindowsArgument("a b") == "\"a b\"");
    CHECK(QuoteWindowsArgument("a\"b") == "\"a\\\"b\"");
    CHECK(QuoteWindowsArgument("C:\\dir\\") == "C:\\dir\\");             // no space: as it is
    CHECK(QuoteWindowsArgument("C:\\my dir\\") == "\"C:\\my dir\\\\\"");  // trailing backslash doubled before the quote
    CHECK(QuoteWindowsArgument("a\\\"b") == "\"a\\\\\\\"b\"");

    CHECK(QuoteCmdArgument("GameServer") == "GameServer");
    CHECK(QuoteCmdArgument("") == "^\"^\"");
    CHECK(QuoteCmdArgument("a b") == "^\"a b^\"");
    CHECK(QuoteCmdArgument("%PATH%") == "^%PATH^%");
    CHECK(QuoteCmdArgument("x&y|z<a>b(c)!d^e") == "x^&y^|z^<a^>b^(c^)^!d^^e");
    CHECK(QuoteCmdArgument("q\"uote") == "^\"q\\^\"uote^\"");
    CHECK(QuoteCmdArgument("--dir=C:\\my dir\\") == "^\"--dir=C:\\my dir\\\\^\"");

    CHECK(QuoteShellArgument("GameServer") == "GameServer");
    CHECK(QuoteShellArgument("") == "''");
    CHECK(QuoteShellArgument("a b") == "'a b'");
    CHECK(QuoteShellArgument("it's") == "'it'\\''s'");
    CHECK(QuoteShellArgument("$HOME;rm") == "'$HOME;rm'");
}

TEST_CASE("process: arguments arrive unchanged")
{
    const std::vector<std::string> args = {"plain", "with space", "", "quote\"inside", "back\\slash\\", "한글 인자",
                                           "--control=Game Server", "tab\there"};
    std::vector<std::string> command = {"args"};
    command.insert(command.end(), args.begin(), args.end());
    auto p = Helper(command);
    const auto lines = AllLines(*p);
    REQUIRE(lines.size() == args.size());
    for (std::size_t i = 0; i < args.size(); ++i)
        CHECK(lines[i] == "[" + args[i] + "]");
    CHECK(p->Wait() == 0);
}

TEST_CASE("process: working directory and a program relative to it")
{
    const std::filesystem::path helper(X2_PROCESS_HELPER);
    ProcessOptions options;
    options.workingDirectory = helper.parent_path().string();
    options.program = (std::filesystem::path(".") / helper.filename()).string();
    options.args = {"cwd"};
    std::string error;
    auto p = Process::Start(options, error);
    REQUIRE_MESSAGE(p, error);
    std::string line;
    REQUIRE(p->ReadLine(line));
    CHECK(std::filesystem::equivalent(line, helper.parent_path()));
    CHECK(p->Wait() == 0);
}

TEST_CASE("process: stdin, stdout and stderr")
{
    auto p = Helper({"echo"});
    CHECK(p->Pid() > 0);
    CHECK(p->WriteLine("one"));
    CHECK(p->WriteLine("두울"));
    std::string line;
    REQUIRE(p->ReadLine(line));
    CHECK(line == "out:one");
    REQUIRE(p->ReadLine(line));
    CHECK(line == "out:두울");
    REQUIRE(p->ReadErrorLine(line));
    CHECK(line == "err:one");
    CHECK(p->Running());
    CHECK_FALSE(p->WaitFor(50ms));
    p->CloseStdin();
    CHECK_FALSE(p->WriteLine("closed"));
    CHECK_FALSE(p->ReadLine(line));     // end of stdout
    CHECK(p->Wait() == 3);
    CHECK_FALSE(p->Running());
}

TEST_CASE("process: stderr merged into stdout")
{
    auto p = Helper({"echo"}, false);
    CHECK(p->WriteLine("x"));
    p->CloseStdin();
    std::string line;
    CHECK_FALSE(p->ReadErrorLine(line));
    const auto lines = AllLines(*p);
    CHECK(lines.size() == 2);
    CHECK(std::find(lines.begin(), lines.end(), "err:x") != lines.end());
    CHECK(p->Wait() == 3);
}

TEST_CASE("process: exit codes, kill and terminate")
{
    CHECK(Helper({"exit", "7"})->Wait() == 7);

    auto sleeper = Helper({"sleep"});
    std::string line;
    REQUIRE(sleeper->ReadLine(line));
    sleeper->Kill();
    const int killed = sleeper->Wait();
#if defined(_WIN32)
    CHECK(killed == 1);
#else
    CHECK(killed == 128 + 9);
#endif
    sleeper->Kill();    // after the exit: nothing

    auto echo = Helper({"echo"});
    echo->Terminate();
    const auto code = echo->WaitFor(10s);
    REQUIRE(code);
#if defined(_WIN32)
    CHECK(*code == 3);  // stdin closed
#else
    CHECK(*code == 128 + 15);
#endif

    // the destructor kills and reaps a child still running
    Helper({"sleep"});
}

TEST_CASE("process: a missing program is an error")
{
    ProcessOptions options;
    options.program = "x2-no-such-program-here";
    std::string error;
    CHECK_FALSE(Process::Start(options, error));
    CHECK_FALSE(error.empty());
}
