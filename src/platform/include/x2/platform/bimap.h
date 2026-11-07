// Replacement for the subset of boost::bimap the client uses: a one-to-one map viewed from either side. `left` maps
// L to R and `right` R to L, both ordered; their iterators' first is the view's key and second the other side.
// insert() adds a pair only when neither side's key is present, as boost::bimap's default set_of views do.
#pragma once

#include <map>
#include <utility>

namespace x2 {

template <class L, class R>
class bimap
{
public:
    using left_map = std::map<L, R>;
    using right_map = std::map<R, L>;
    using left_const_iterator = typename left_map::const_iterator;
    using right_const_iterator = typename right_map::const_iterator;

    struct value_type {
        value_type(L l, R r) : left(std::move(l)), right(std::move(r)) {}
        L left;
        R right;
    };

    bool insert(const value_type& value)
    {
        if (m_left.contains(value.left) || m_right.contains(value.right))
            return false;
        m_left.emplace(value.left, value.right);
        m_right.emplace(value.right, value.left);
        return true;
    }
    void clear()
    {
        m_left.clear();
        m_right.clear();
    }
    bool empty() const { return m_left.empty(); }
    std::size_t size() const { return m_left.size(); }

    // The views are read-only: the pairs change only through the bimap, so both sides stay in step.
    const left_map& left = m_left;
    const right_map& right = m_right;

    bimap() = default;
    bimap(const bimap& other) : m_left(other.m_left), m_right(other.m_right) {}
    bimap& operator=(const bimap& other)
    {
        m_left = other.m_left;
        m_right = other.m_right;
        return *this;
    }

private:
    left_map m_left;
    right_map m_right;
};

}  // namespace x2
