// Replacement for boost::timer (elapsed()/restart(), seconds as double).
// boost::timer measured process CPU time (std::clock); the original uses it to time packet handling and queries,
// where wall time is what matters, so this one is a steady (monotonic) wall clock.
#pragma once

#include <chrono>

namespace x2 {

class Timer
{
public:
    Timer() : m_start(std::chrono::steady_clock::now()) {}

    void restart() { m_start = std::chrono::steady_clock::now(); }
    double elapsed() const { return std::chrono::duration<double>(std::chrono::steady_clock::now() - m_start).count(); }

private:
    std::chrono::steady_clock::time_point m_start;
};

} // namespace x2
