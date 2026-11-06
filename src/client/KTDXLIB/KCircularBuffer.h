// Replacement for the part of boost::circular_buffer the engine uses (KTDNUDPStatisticData): a deque with a capacity,
// where push_back on a full buffer drops the front element.
#pragma once

#include <cstddef>
#include <deque>

template <class T>
class KCircularBuffer : public std::deque<T>
{
public:
    using std::deque<T>::deque;

    std::size_t capacity() const { return m_capacity; }
    void set_capacity(std::size_t capacity)
    {
        m_capacity = capacity;
        while (this->size() > m_capacity)
            this->pop_back();
    }
    bool full() const { return this->size() >= m_capacity; }
    void push_back(const T& value)
    {
        if (m_capacity == 0)
            return;
        if (full())
            this->pop_front();
        std::deque<T>::push_back(value);
    }

private:
    std::size_t m_capacity = 0;
};
