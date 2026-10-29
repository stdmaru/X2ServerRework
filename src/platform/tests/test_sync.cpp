#include "x2/platform/sync.h"

#include <doctest/doctest.h>

#include <atomic>

namespace {

std::atomic<int> g_runs{0};

unsigned WINAPI ThreadBody(void* arg)
{
    HANDLE kill = static_cast<HANDLE>(arg);
    while (WaitForSingleObject(kill, 1) == WAIT_TIMEOUT)
        ++g_runs;
    return 7;
}

} // namespace

TEST_CASE("critical section is recursive")
{
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    EnterCriticalSection(&cs);
    EnterCriticalSection(&cs);
    LeaveCriticalSection(&cs);
    LeaveCriticalSection(&cs);
    DeleteCriticalSection(&cs);
}

TEST_CASE("auto-reset and manual-reset events")
{
    HANDLE autoEv = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    HANDLE manualEv = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    CHECK(WaitForSingleObject(autoEv, 0) == WAIT_TIMEOUT);
    SetEvent(autoEv);
    CHECK(WaitForSingleObject(autoEv, 0) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(autoEv, 0) == WAIT_TIMEOUT); // auto reset

    SetEvent(manualEv);
    CHECK(WaitForSingleObject(manualEv, 0) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(manualEv, 0) == WAIT_OBJECT_0); // stays signaled
    ResetEvent(manualEv);
    CHECK(WaitForSingleObject(manualEv, 0) == WAIT_TIMEOUT);

    HANDLE both[2] = { autoEv, manualEv };
    SetEvent(manualEv);
    CHECK(WaitForMultipleObjects(2, both, FALSE, 0) == WAIT_OBJECT_0 + 1);
    CHECK(WaitForMultipleObjects(2, both, TRUE, 0) == WAIT_TIMEOUT);
    SetEvent(autoEv);
    CHECK(WaitForMultipleObjects(2, both, TRUE, 0) == WAIT_OBJECT_0);

    CloseHandle(autoEv);
    CloseHandle(manualEv);
}

TEST_CASE("thread handle is signaled on exit (the KThread pattern)")
{
    HANDLE kill = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    unsigned tid = 0;
    HANDLE th = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadBody, kill, 0, &tid));
    REQUIRE(th != nullptr);
    CHECK(tid != 0);
    CHECK(WaitForSingleObject(th, 20) == WAIT_TIMEOUT);
    SetEvent(kill);
    CHECK(WaitForSingleObject(th, 3000) == WAIT_OBJECT_0);
    DWORD code = 0;
    CHECK(GetExitCodeThread(th, &code));
    CHECK(code == 7);
    CHECK(g_runs.load() > 0);
    CloseHandle(th);
    CloseHandle(kill);
}
