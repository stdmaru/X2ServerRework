#include "ObjectPool/BlockAllocator.h"
#include "ObjectPool/BoostAllocator.h"

#include <doctest/doctest.h>

#include <cstdint>
#include <set>
#include <vector>

namespace {

struct KSmallBlock : public BlockAllocator<KSmallBlock, 4>
{
    char c = 'a'; // smaller than a pointer: the free list must still fit
};

BOOST_ALOC_CLASS( KPooled )
{
public:
    KPooled( int v ) : value( v ) {}
    int value;
    double align;
};

BOOST_ALOC_SYNC_CLASS( KPooledSync )
{
public:
    std::int64_t value = 0;
};

} // namespace

TEST_CASE("BlockAllocator reuses blocks and survives more than one batch")
{
    std::vector<KSmallBlock*> blocks;
    for( int i = 0; i < 10; ++i ) // batches of 4
        blocks.push_back( new KSmallBlock );
    std::set<KSmallBlock*> unique( blocks.begin(), blocks.end() );
    CHECK(unique.size() == blocks.size());
    for( auto* b : blocks )
        CHECK(b->c == 'a');
    KSmallBlock* last = blocks.back();
    delete last;
    blocks.pop_back();
    KSmallBlock* again = new KSmallBlock;
    CHECK(again == last); // LIFO free list
    blocks.push_back( again );
    for( auto* b : blocks )
        delete b;
}

TEST_CASE("BoostAllocator objects are aligned and distinct")
{
    std::vector<KPooled*> objs;
    for( int i = 0; i < 100; ++i )
        objs.push_back( new KPooled( i ) );
    for( int i = 0; i < 100; ++i )
    {
        CHECK(objs[i]->value == i);
        CHECK(reinterpret_cast<std::uintptr_t>( objs[i] ) % alignof( KPooled ) == 0);
    }
    CHECK(objs[0]->GetNextAllocSize() > 64); // chunks grow by doubling
    for( auto* o : objs )
        delete o;

    KPooledSync* s = new KPooledSync;
    s->value = 5;
    CHECK(s->value == 5);
    delete s;
}
