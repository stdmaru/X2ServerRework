#include "ServerPacket.h"
#include <Thread/Locker.h>
#include "UserList.h"

#include <doctest/doctest.h>

static_assert(alignof(KUserList) >= alignof(KncCriticalSection),
    "KUserList must preserve the natural alignment required by its critical section");

TEST_CASE("KUserList preserves critical section alignment")
{
    CHECK(alignof(KUserList) >= alignof(KncCriticalSection));
}
