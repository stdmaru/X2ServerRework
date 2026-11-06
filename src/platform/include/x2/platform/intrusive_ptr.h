// Replacement for boost::intrusive_ptr: a pointer to an object that counts its own references. Same protocol as
// Boost: the pointee's namespace provides intrusive_ptr_add_ref(T*) and intrusive_ptr_release(T*), found by
// argument-dependent lookup.
#pragma once

#include <cstddef>
#include <functional>
#include <utility>

namespace x2 {

template <class T>
class intrusive_ptr
{
public:
    using element_type = T;

    intrusive_ptr() noexcept = default;
    intrusive_ptr(std::nullptr_t) noexcept {}
    intrusive_ptr(T* p, bool addRef = true) : m_p(p)
    {
        if (m_p && addRef)
            intrusive_ptr_add_ref(m_p);
    }
    intrusive_ptr(const intrusive_ptr& other) : intrusive_ptr(other.m_p) {}
    template <class U>
    intrusive_ptr(const intrusive_ptr<U>& other) : intrusive_ptr(other.get()) {}
    intrusive_ptr(intrusive_ptr&& other) noexcept : m_p(std::exchange(other.m_p, nullptr)) {}
    ~intrusive_ptr()
    {
        if (m_p)
            intrusive_ptr_release(m_p);
    }

    intrusive_ptr& operator=(const intrusive_ptr& other)
    {
        intrusive_ptr(other).swap(*this);
        return *this;
    }
    intrusive_ptr& operator=(intrusive_ptr&& other) noexcept
    {
        intrusive_ptr(std::move(other)).swap(*this);
        return *this;
    }
    intrusive_ptr& operator=(T* p)
    {
        intrusive_ptr(p).swap(*this);
        return *this;
    }

    void reset() noexcept { intrusive_ptr().swap(*this); }
    void reset(T* p, bool addRef = true) { intrusive_ptr(p, addRef).swap(*this); }
    T* get() const noexcept { return m_p; }
    T* detach() noexcept { return std::exchange(m_p, nullptr); }
    T& operator*() const noexcept { return *m_p; }
    T* operator->() const noexcept { return m_p; }
    explicit operator bool() const noexcept { return m_p != nullptr; }
    void swap(intrusive_ptr& other) noexcept { std::swap(m_p, other.m_p); }

private:
    T* m_p = nullptr;
};

template <class T, class U>
bool operator==(const intrusive_ptr<T>& a, const intrusive_ptr<U>& b) noexcept { return a.get() == b.get(); }
template <class T, class U>
bool operator==(const intrusive_ptr<T>& a, U* b) noexcept { return a.get() == b; }
template <class T>
bool operator==(const intrusive_ptr<T>& a, std::nullptr_t) noexcept { return a.get() == nullptr; }
template <class T, class U>
bool operator<(const intrusive_ptr<T>& a, const intrusive_ptr<U>& b) noexcept { return std::less<>()(a.get(), b.get()); }

template <class T, class U>
intrusive_ptr<T> static_pointer_cast(const intrusive_ptr<U>& p) { return static_cast<T*>(p.get()); }
template <class T, class U>
intrusive_ptr<T> dynamic_pointer_cast(const intrusive_ptr<U>& p) { return dynamic_cast<T*>(p.get()); }

template <class T>
void swap(intrusive_ptr<T>& a, intrusive_ptr<T>& b) noexcept { a.swap(b); }

} // namespace x2

template <class T>
struct std::hash<x2::intrusive_ptr<T>>
{
    std::size_t operator()(const x2::intrusive_ptr<T>& p) const noexcept { return std::hash<T*>()(p.get()); }
};
