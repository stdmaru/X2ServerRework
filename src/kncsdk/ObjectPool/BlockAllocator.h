#ifndef __BlockAllocator
#define __BlockAllocator

// Per-class pooled operator new/delete (thread-safe). Blocks come in batches of `blocksPerBatch`.
// Port: storage moved to KncPool::FixedBlockPool (the original stored 32-bit pointers in free blocks).

#include "FixedBlockPool.h"
#include "x2/platform/sync.h"

template <class T, size_t blocksPerBatch=100, size_t blockAlignment=4>
class BlockAllocator
{
public:
    void* operator new(size_t)
    {
        return Store().AllocateBlock();
    }

    void operator delete(void* pBlock)
    {
        Store().ReleaseBlock((T*)pBlock);
    }

    static T*   AllocateBlock()         { return Store().AllocateBlock(); }
    static void ReleaseBlock(T* pBlock) { Store().ReleaseBlock(pBlock); }

private:
    struct BlockStore;

    // T is incomplete where BlockAllocator<T> is a base class: the store (which needs sizeof(T)) is only
    // instantiated on first use.
    static auto& Store()
    {
        static BlockStore s_Store;
        return s_Store;
    }

    struct BlockStore
    {
        BlockStore()  { InitializeCriticalSection( &m_cs ); }
        ~BlockStore() { DeleteCriticalSection( &m_cs ); }

        T* AllocateBlock()
        {
            EnterCriticalSection( &m_cs );
            void* p = m_pool.Allocate();
            LeaveCriticalSection( &m_cs );
            return static_cast<T*>( p );
        }

        void ReleaseBlock(T* pBlock)
        {
            EnterCriticalSection( &m_cs );
            m_pool.Free( pBlock );
            LeaveCriticalSection( &m_cs );
        }

        KncPool::FixedBlockPool< sizeof(T), (blockAlignment > alignof(T) ? blockAlignment : alignof(T)) > m_pool{ blocksPerBatch, false };
        CRITICAL_SECTION    m_cs;
    };

};

#endif
