// Wide __FILE__/__FUNCTION__ for the logging macros.
//
// The original defines __WFUNCTION__ as L ## __FUNCTION__, which only works on MSVC (elsewhere
// __FUNCTION__ is not a string literal). Here both are runtime `const wchar_t*` with static lifetime,
// so they can be streamed or passed as arguments, but not concatenated with other literals.
#pragma once

namespace x2::platform {

// `name` must have static storage duration (__FILE__, __FUNCTION__); results are cached.
const wchar_t* WidenStatic(const char* name);

} // namespace x2::platform

#define __WFILE__     (::x2::platform::WidenStatic(__FILE__))
#define __WFUNCTION__ (::x2::platform::WidenStatic(__FUNCTION__))
