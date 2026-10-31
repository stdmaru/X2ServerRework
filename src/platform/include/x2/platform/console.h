// Process console: every program writes UTF-8 to stdout/stderr (the log and the Lua prompt).
#pragma once

#include <cstdio>

namespace x2::platform {

// Call at the start of main. On Windows the console decodes output with its own code page (the OEM one,
// not UTF-8), so the Korean text of the log came out garbled: this switches the console to UTF-8 for input
// and output (restored at exit, since the console may belong to the parent shell) and turns on ANSI escape
// processing for the log colors. Nothing to do elsewhere.
void InitConsole();

// True when `stream` is a terminal that understands ANSI color escapes.
bool IsColorTerminal(std::FILE* stream);

} // namespace x2::platform
