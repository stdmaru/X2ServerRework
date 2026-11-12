#pragma once

// x2::scope_exit: runs a callable when the scope ends, in place of BOOST_SCOPE_EXIT.
//
//     x2::scope_exit endTable( [&] { luaManager.EndTable(); } );

#include <utility>

namespace x2 {

template <class F>
class [[nodiscard]] scope_exit
{
public:
    explicit scope_exit(F f) : m_f(std::move(f)) {}
    ~scope_exit() { m_f(); }

    scope_exit(const scope_exit&) = delete;
    scope_exit& operator=(const scope_exit&) = delete;

private:
    F m_f;
};

} // namespace x2
