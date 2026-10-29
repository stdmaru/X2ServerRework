#include "Thread/KCSLockType.h"
#include "Thread/Locker.h"
#include "Thread/Thread.h"

#include <doctest/doctest.h>

#include <atomic>
#include <thread>
#include <vector>

namespace {

class KCounterThread : public KThread
{
public:
    std::atomic<int> m_iTicks{ 0 };

    void Run() override
    {
        // same loop shape as the server threads: wait on the kill event with a timeout
        while( ::WaitForSingleObject( m_hKillEvent, 1 ) == WAIT_TIMEOUT )
            ++m_iTicks;
    }
};

class KTimerOwner
{
public:
    std::atomic<int> m_iRuns{ 0 };
    void OnTick() { ++m_iRuns; }
};

class KLockedOwner
{
public:
    void Add( int v )
    {
        KCSLOCK_BEGIN( m_vecInt )
            m_vecInt.push_back( v );
        KCSLOCK_END()
    }

    size_t Size() const
    {
        KCSLOCK_CBEGIN( m_vecInt )
            return m_vecInt.size();
        KCSLOCK_END()
    }

private:
    KCSLOCK_DECLARE( std::vector<int>, m_vecInt );
};

} // namespace

TEST_CASE("KThread starts, runs and stops through the kill event")
{
    KCounterThread th;
    REQUIRE(th.Begin());
    CHECK_FALSE(th.Begin()); // already running
    while( th.m_iTicks.load() < 3 )
        ::Sleep( 1 );
    th.End();
    const int ticks = th.m_iTicks.load();
    ::Sleep( 10 );
    CHECK(th.m_iTicks.load() == ticks); // really stopped
}

TEST_CASE("KTThread calls the member function on every timeout")
{
    KTimerOwner owner;
    KTThread<KTimerOwner> th( owner, &KTimerOwner::OnTick, 1 );
    REQUIRE(th.Begin());
    while( owner.m_iRuns.load() < 3 )
        ::Sleep( 1 );
    th.End();
    CHECK(owner.m_iRuns.load() >= 3);
}

TEST_CASE("KCSLockType from several threads")
{
    KLockedOwner owner;
    std::vector<std::thread> threads;
    for( int t = 0; t < 4; ++t )
        threads.emplace_back( [&owner] { for( int i = 0; i < 1000; ++i ) owner.Add( i ); } );
    for( auto& t : threads )
        t.join();
    CHECK(owner.Size() == 4000);
}

TEST_CASE("KLocker / KncCriticalSection")
{
    KncCriticalSection cs;
    int counter = 0;
    std::vector<std::thread> threads;
    for( int t = 0; t < 4; ++t )
        threads.emplace_back( [&] { for( int i = 0; i < 1000; ++i ) { KLocker lock( cs ); ++counter; } } );
    for( auto& t : threads )
        t.join();
    CHECK(counter == 4000);
}
