// Force-included into Crypto++ (MSVC only, cmake/X2Dependencies.cmake). Visual Studio 2026 (MSVC 19.50) removed
// stdext::checked_array_iterator and stdext::unchecked_array_iterator, which Crypto++ 8.9 still calls in
// integer.cpp and zdeflate.cpp. Plain pointers do the same job.
#pragma once

#if defined(_MSC_VER) && _MSC_VER >= 1950
#include <cstddef>

namespace stdext {
template <class T>
T* make_checked_array_iterator(T* p, std::size_t, std::size_t = 0) { return p; }
template <class T>
T* make_unchecked_array_iterator(T* p) { return p; }
}
#endif
