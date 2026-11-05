// Child for test_process.cpp:
//   args          print each argument as [arg]
//   cwd           print the current directory
//   echo          for each stdin line, "out:<line>" on stdout and "err:<line>" on stderr; exit 3 at the end of stdin
//   exit <n>      exit with n
//   sleep         print "sleeping" and wait to be killed
#include "x2/platform/console.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

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
    return 2;
}
