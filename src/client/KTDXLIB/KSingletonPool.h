#pragma once

// Replacement for boost::singleton_pool<boost::pool_allocator_tag, Size>: one process-wide pool of blocks of Size bytes
// per size, thread safe, which the engine's small objects (matrices, particle sequences, mesh instances) take their
// operator new from. Freed blocks are kept for reuse and never returned to the system, as Boost's pool did.

#include <cstddef>
#include <mutex>
#include <new>
#include <vector>

template <std::size_t Size>
class KSingletonPool
{
public:
    static void* malloc()
    {
        Pool& pool = Instance();
        std::lock_guard<std::mutex> lock( pool.mutex );
        if ( pool.freeBlocks.empty() )
            return ::operator new( Size < sizeof(void*) ? sizeof(void*) : Size );
        void* p = pool.freeBlocks.back();
        pool.freeBlocks.pop_back();
        return p;
    }

    static void free( void* p )
    {
        if ( p == nullptr )
            return;
        Pool& pool = Instance();
        std::lock_guard<std::mutex> lock( pool.mutex );
        pool.freeBlocks.push_back( p );
    }

private:
    struct Pool
    {
        std::mutex          mutex;
        std::vector<void*>  freeBlocks;
    };

    // Never destroyed: objects may be freed during static destruction.
    static Pool& Instance()
    {
        static Pool* s_pPool = new Pool;
        return *s_pPool;
    }
};
