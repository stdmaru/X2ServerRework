// Console of a server: the terminal of decision T15 and the control protocol of decision T30, in place of the
// original's window (Common/ui/ServerUIDlg: log, Lua input field, _Loading/_Ready title, exit confirmation).
//
// Opened before the server loads and closed after it shuts down, like the window. Modes (docs/porting/kncsdk.md):
// - terminal: live log above a Lua prompt with history, status in the title, exit confirmed with y;
// - lines: stdin is not a terminal, each line is a Lua command, its end shuts the server down;
// - machine (--machine): stdin and stdout speak the protocol, JSON lines, for a program driving the server;
// - service (--no-stdin): nothing is read, a stop signal shuts the server down.
// Besides, the control socket (x2ctl attach <name>) speaks the protocol to any number of clients in every mode.
#pragma once

#include "KncLua.h"

#include <functional>
#include <memory>
#include <string>

class KServerConsole
{
public:
    struct Options
    {
        std::string name;           // terminal title ("<name>_Loading" / "_Ready") and protocol greeting
        std::string control;        // name of the control socket; empty: none
        bool        noStdin = false;
        bool        machine = false;
    };

    explicit KServerConsole( Options options );
    ~KServerConsole();
    KServerConsole( const KServerConsole& ) = delete;
    KServerConsole& operator = ( const KServerConsole& ) = delete;

    // Runs the commands (prompt, stdin, control clients) on `L` until the server should shut down. `ready`,
    // polled about every 100 ms, tells when the server finished loading.
    void Run( lua_State* L, std::function<bool()> ready );

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
