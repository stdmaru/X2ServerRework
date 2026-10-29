// Free-list pool of fixed-size blocks, shared by BlockAllocator and BoostAllocator (port of the original
// allocators, which stored 32-bit links in the blocks and used boost::object_pool). Not thread-safe.
#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <new>
#include <vector>

namespace KncPool {

template <std::size_t BlockSize, std::size_t Alignment>
class FixedBlockPool
{
    static constexpr std::size_t kAlign = std::max(Alignment, alignof(void*));
    static constexpr std::size_t kBlock = (std::max(BlockSize, sizeof(void*)) + kAlign - 1) / kAlign * kAlign;

public:
    // `firstBatch`: blocks in the first chunk; `grow`: double the chunk size each time (boost::pool behaviour)
    explicit FixedBlockPool(std::size_t firstBatch, bool grow) : m_nextSize(std::max<std::size_t>(firstBatch, 1)), m_grow(grow) {}
    FixedBlockPool(const FixedBlockPool&) = delete;
    FixedBlockPool& operator=(const FixedBlockPool&) = delete;

    ~FixedBlockPool()
    {
        for (void* chunk : m_chunks)
            ::operator delete(chunk, std::align_val_t(kAlign));
    }

    void* Allocate()
    {
        if (!m_free)
            AddChunk();
        Node* n = m_free;
        m_free = n->next;
        return n;
    }

    void Free(void* p)
    {
        if (!p)
            return;
        Node* n = static_cast<Node*>(p);
        n->next = m_free;
        m_free = n;
    }

    std::size_t NextSize() const { return m_nextSize; }

private:
    struct Node { Node* next; };

    void AddChunk()
    {
        const std::size_t count = m_nextSize;
        auto* chunk = static_cast<unsigned char*>(::operator new(count * kBlock, std::align_val_t(kAlign)));
        m_chunks.push_back(chunk);
        for (std::size_t i = count; i-- > 0;)
            Free(chunk + i * kBlock);
        if (m_grow)
            m_nextSize *= 2;
    }

    Node*               m_free = nullptr;
    std::size_t         m_nextSize;
    bool                m_grow;
    std::vector<void*>  m_chunks;
};

} // namespace KncPool
