// Wide __FILE__/__FUNCTION__ for the logging macros.
//
// The original defines __WFUNCTION__ as L ## __FUNCTION__, which only works on MSVC (elsewhere
// __FUNCTION__ is not a string literal). Here both are runtime `const wchar_t*` with static lifetime,
// so they can be streamed or passed as arguments, but not concatenated with other literals.
#pragma once

#include "x2/platform/types.h"

namespace x2::platform {

// `name` must have static storage duration (__FILE__, __FUNCTION__); results are cached.
const wchar_t* WidenStatic(const char* name);

// True when a debugger is attached to this process (the client's own anti-cheat): IsDebuggerPresent on Windows,
// a tracer in /proc/self/status on Linux.
bool IsDebuggerAttached();

} // namespace x2::platform

#define __WFILE__     (::x2::platform::WidenStatic(__FILE__))
#define __WFUNCTION__ (::x2::platform::WidenStatic(__FUNCTION__))

#if !defined(_WIN32)
// The debugger's output on Windows (native there); stderr elsewhere. The wide text goes out as UTF-8.
void OutputDebugStringW(const wchar_t* text);
void OutputDebugStringA(const char* text);
#define OutputDebugString OutputDebugStringW
#endif
