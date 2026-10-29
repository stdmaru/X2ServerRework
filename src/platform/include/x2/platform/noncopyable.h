// Replacement for boost::noncopyable (same semantics: no copy, implicit default constructor kept).
#pragma once

namespace x2 {

class NonCopyable
{
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

} // namespace x2
