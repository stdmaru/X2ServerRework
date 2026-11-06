#pragma once

// The arguments of the engine's GetReplacedString( id, "params", ... ), made fit for C varargs. Callers pass
// std::wstring/std::string by value for the 'L'/'l' parameters, which MSVC copied bitwise into the variadic call and
// other compilers refuse (a non-trivially-copyable type): the strings go as their c_str() pointers instead, and the
// rest as they are (floats are promoted to double by the call, as before).

#include <string>

namespace KStringVarArg
{
    inline const wchar_t*   Arg( const std::wstring& wstr ) { return wstr.c_str(); }
    inline const char*      Arg( const std::string& str )   { return str.c_str(); }
    template <class T> T    Arg( T value )                  { return value; }
}
