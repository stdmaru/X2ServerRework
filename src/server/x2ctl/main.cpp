// x2ctl: talks to the control socket of a running server (decision T30, protocol in docs/porting/console.md).
//   x2ctl [--dir=<folder>] <command>   sockets in <folder> instead of the default (X2_CONTROL_DIR, XDG_RUNTIME_DIR/x2)
//   x2ctl list                    servers listening (their control socket names)
//   x2ctl attach <name>           stdin -> server, server -> stdout, in protocol lines (ssh host x2ctl attach GameServer)
//   x2ctl run <name> <lua...>     runs one Lua command, prints what it printed; exit code 1 on a Lua error
//   x2ctl stop <name>             shuts the server down and waits for it to finish
#include "x2/platform/console.h"
#include "x2/platform/control.h"
#include "x2/platform/json.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace x2::platform;

namespace {

int Usage()
{
    std::fputs("usage: x2ctl [--dir=<folder>] list\n"
               "       x2ctl [--dir=<folder>] attach <name>\n"
               "       x2ctl [--dir=<folder>] run <name> <lua...>\n"
               "       x2ctl [--dir=<folder>] stop <name>\n"
               "Control sockets live in ", stderr);
    std::fputs(ControlSocketDir().c_str(), stderr);
    std::fputs("\n", stderr);
    return 2;
}

bool Open(ControlClient& client, const std::string& name)
{
    std::string error;
    if (client.Connect(name, error))
        return true;
    std::fprintf(stderr, "x2ctl: %s\n", error.c_str());
    return false;
}

int Attach(const std::string& name)
{
    ControlClient client;
    if (!Open(client, name))
        return 1;
    // stdin's end closes our side; the server then ends the connection, which ends the loop below
    std::thread([&client] {
        std::string line;
        while (std::getline(std::cin, line))
            if (!client.WriteLine(line))
                break;
        client.CloseWrite();
    }).detach();
    std::string line;
    while (client.ReadLine(line)) {
        line += '\n';
        std::fwrite(line.data(), 1, line.size(), stdout);
        std::fflush(stdout);
    }
    std::fflush(stdout);
    std::_Exit(0);  // the stdin thread may still be blocked in its read
}

// Sends one command and waits for its result: 0 ok, 1 Lua error, 3 connection lost.
int Request(const std::string& name, const std::string& command, bool waitForClose)
{
    ControlClient client;
    if (!Open(client, name) || !client.WriteLine(command))
        return 3;
    std::string line;
    while (client.ReadLine(line)) {
        const auto object = ParseJsonObject(line);     // log lines carry arrays and are skipped
        if (!object)
            continue;
        const auto type = object->find("type");
        const auto id = object->find("id");
        if (type == object->end() || type->second.text != "result" || id == object->end() || id->second.text != "1")
            continue;
        const std::string& output = object->at("output").text;
        const std::string& error = object->at("error").text;
        std::fwrite(output.data(), 1, output.size(), stdout);
        if (!error.empty())
            std::fprintf(stderr, "%s\n", error.c_str());
        const bool ok = object->at("ok").text == "true";
        if (waitForClose)
            while (client.ReadLine(line)) {
            }
        return ok ? 0 : 1;
    }
    std::fputs("x2ctl: the server closed the connection\n", stderr);
    return 3;
}

} // namespace

int main(int argc, char* argv[])
{
    InitConsole();
    std::vector<std::string> args = Utf8Arguments(argc, argv);
    while (args.size() > 1 && args[1].rfind("--dir", 0) == 0) {
        if (args[1].rfind("--dir=", 0) == 0) {
            SetControlSocketDir(args[1].substr(6));
            args.erase(args.begin() + 1);
        } else if (args[1] == "--dir" && args.size() > 2) {
            SetControlSocketDir(args[2]);
            args.erase(args.begin() + 1, args.begin() + 3);
        } else {
            return Usage();
        }
    }
    if (args.size() < 2)
        return Usage();
    const std::string& verb = args[1];
    if (verb == "list" && args.size() == 2) {
        for (const std::string& name : ListControlSockets())
            std::printf("%s\n", name.c_str());
        return 0;
    }
    if (args.size() < 3)
        return Usage();
    const std::string& name = args[2];
    if (verb == "attach" && args.size() == 3)
        return Attach(name);
    if (verb == "stop" && args.size() == 3)
        return Request(name, "{\"id\":1,\"stop\":true}", true);
    if (verb == "run" && args.size() >= 4) {
        std::string lua = args[3];
        for (std::size_t i = 4; i < args.size(); ++i)
            lua += " " + args[i];
        return Request(name, "{\"id\":1,\"lua\":" + JsonString(lua) + "}", false);
    }
    return Usage();
}
