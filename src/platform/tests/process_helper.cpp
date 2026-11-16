// Child for test_process.cpp:
//   args          print each argument as [arg]
//   cwd           print the current directory
//   echo          for each stdin line, "out:<line>" on stdout and "err:<line>" on stderr; exit 3 at the end of stdin
//   exit <n>      exit with n
//   sleep         print "sleeping" and wait to be killed
//   exit-waiting  print "waiting" once two threads block for good (one on an event, one on its own thread handle, as
//                 the client's KUserProxy does when its session thread destroys it), then return 0 from main
#include "x2/platform/console.h"
#include "x2/platform/process.h"
#include "x2/platform/sync.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

HANDLE g_never = nullptr;              // an event nobody sets
std::atomic<HANDLE> g_self{nullptr};   // the self-waiting thread's own handle
std::atomic<int> g_blocked{0};

unsigned WINAPI WaitForever(void*)
{
    ++g_blocked;
    WaitForSingleObject(g_never, INFINITE);
    return 0;
}

unsigned WINAPI WaitForItself(void*)
{
    HANDLE self = nullptr;
    while ((self = g_self.load()) == nullptr)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ++g_blocked;
    WaitForSingleObject(self, INFINITE);
    return 0;
}

}  // namespace

int main(int argc, char* argv[])
{
    const std::vector<std::string> args = x2::platform::Utf8Arguments(argc, argv);
    if (args.size() < 2)
        return 2;
    const std::string& mode = args[1];
    if (mode == "args") {
        for (std::size_t i = 2; i < args.size(); ++i)
            std::printf("[%s]\n", args[i].c_str());
        return 0;
    }
    if (mode == "ppid") {
        std::printf("%lld\n", x2::platform::ParentProcessId());
        return 0;
    }
    if (mode == "cwd") {
        const std::u8string cwd = std::filesystem::current_path().u8string();
        std::printf("%s\n", std::string(cwd.begin(), cwd.end()).c_str());
        return 0;
    }
    if (mode == "echo") {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            std::printf("out:%s\n", line.c_str());
            std::fflush(stdout);
            std::fprintf(stderr, "err:%s\n", line.c_str());
            std::fflush(stderr);
        }
        return 3;
    }
    if (mode == "exit" && args.size() > 2)
        return std::atoi(args[2].c_str());
    if (mode == "sleep") {
        std::printf("sleeping\n");
        std::fflush(stdout);
        for (;;)
            std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    if (mode == "exit-waiting") {
        g_never = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        unsigned tid = 0;
        if (!_beginthreadex(nullptr, 0, WaitForever, nullptr, 0, &tid))
            return 4;
        HANDLE self = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, WaitForItself, nullptr, 0, &tid));
        if (!self)
            return 4;
        g_self = self;
        while (g_blocked.load() < 2)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));   // both inside their waits
        std::printf("waiting\n");
        std::fflush(stdout);
        return 0;
    }
    return 2;
}
