#if !defined(_WIN32)

#include "x2/platform/sync.h"

#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

#include <sys/syscall.h>
#include <unistd.h>

namespace x2::platform::detail {

// One lock/condition for every kernel object keeps multi-object waits simple and correct.
// Signals are rare compared to the work done between them, so contention is not a concern.
std::mutex              g_mutex;
std::condition_variable g_cond;

struct KernelObject
{
    virtual ~KernelObject() = default;
    virtual bool IsSignaled() const = 0;   // called with g_mutex held
    virtual void OnWaitSatisfied() {}      // called with g_mutex held
};

struct Event final : KernelObject
{
    bool manualReset = false;
    bool signaled = false;
    bool IsSignaled() const override { return signaled; }
    void OnWaitSatisfied() override
    {
        if (!manualReset)
            signaled = false;
    }
};

struct Thread final : KernelObject
{
    bool  finished = false;
    DWORD exitCode = STILL_ACTIVE;
    bool IsSignaled() const override { return finished; }
};

struct HandleBox
{
    std::shared_ptr<KernelObject> object;
};

template <typename T>
T* As(HANDLE h)
{
    if (!h || h == INVALID_HANDLE_VALUE)
        return nullptr;
    return dynamic_cast<T*>(h->object.get());
}

} // namespace x2::platform::detail

using namespace x2::platform::detail;

HANDLE CreateEvent(const void*, BOOL manualReset, BOOL initialState, const wchar_t*)
{
    auto ev = std::make_shared<Event>();
    ev->manualReset = manualReset != FALSE;
    ev->signaled = initialState != FALSE;
    return new HandleBox{ std::move(ev) };
}

BOOL SetEvent(HANDLE h)
{
    std::lock_guard lock(g_mutex);
    Event* ev = As<Event>(h);
    if (!ev)
        return FALSE;
    ev->signaled = true;
    g_cond.notify_all();
    return TRUE;
}

BOOL ResetEvent(HANDLE h)
{
    std::lock_guard lock(g_mutex);
    Event* ev = As<Event>(h);
    if (!ev)
        return FALSE;
    ev->signaled = false;
    return TRUE;
}

BOOL CloseHandle(HANDLE h)
{
    if (!h || h == INVALID_HANDLE_VALUE)
        return FALSE;
    // the object itself lives on while a running thread still references it
    std::lock_guard lock(g_mutex);
    delete h;
    return TRUE;
}

DWORD WaitForMultipleObjects(DWORD count, const HANDLE* handles, BOOL waitAll, DWORD milliseconds)
{
    if (count == 0 || count > MAXIMUM_WAIT_OBJECTS || !handles)
        return WAIT_FAILED;

    std::unique_lock lock(g_mutex);
    std::vector<KernelObject*> objects(count);
    for (DWORD i = 0; i < count; ++i) {
        if (!handles[i] || handles[i] == INVALID_HANDLE_VALUE || !handles[i]->object)
            return WAIT_FAILED;
        objects[i] = handles[i]->object.get();
    }

    DWORD result = WAIT_TIMEOUT;
    auto ready = [&] {
        if (waitAll) {
            for (auto* o : objects)
                if (!o->IsSignaled())
                    return false;
            for (auto* o : objects)
                o->OnWaitSatisfied();
            result = WAIT_OBJECT_0;
            return true;
        }
        for (DWORD i = 0; i < count; ++i) {
            if (objects[i]->IsSignaled()) {
                objects[i]->OnWaitSatisfied();
                result = WAIT_OBJECT_0 + i;
                return true;
            }
        }
        return false;
    };

    if (milliseconds == INFINITE)
        g_cond.wait(lock, ready);
    else
        g_cond.wait_for(lock, std::chrono::milliseconds(milliseconds), ready);
    return result;
}

DWORD WaitForSingleObject(HANDLE h, DWORD milliseconds)
{
    return WaitForMultipleObjects(1, &h, FALSE, milliseconds);
}

std::uintptr_t _beginthreadex(void*, unsigned, unsigned (*start)(void*), void* arg, unsigned initFlag,
                              unsigned* threadId)
{
    if (!start || (initFlag & CREATE_SUSPENDED))
        return 0;
    auto th = std::make_shared<Thread>();
    auto* box = new HandleBox{ th };
    std::mutex idMutex;
    std::condition_variable idReady;
    unsigned tid = 0;
    try {
        std::thread([th, start, arg, &idMutex, &idReady, &tid] {
            {
                std::lock_guard l(idMutex);
                tid = GetCurrentThreadId();
            }
            idReady.notify_one();
            const unsigned code = start(arg);
            std::lock_guard lock(g_mutex);
            th->exitCode = code;
            th->finished = true;
            g_cond.notify_all();
        }).detach();
    } catch (...) {
        delete box;
        return 0;
    }
    std::unique_lock l(idMutex);
    idReady.wait(l, [&] { return tid != 0; });
    if (threadId)
        *threadId = tid;
    return reinterpret_cast<std::uintptr_t>(box);
}

BOOL GetExitCodeThread(HANDLE h, DWORD* exitCode)
{
    std::lock_guard lock(g_mutex);
    Thread* th = As<Thread>(h);
    if (!th || !exitCode)
        return FALSE;
    *exitCode = th->exitCode;
    return TRUE;
}

DWORD GetCurrentThreadId()
{
    return static_cast<DWORD>(::syscall(SYS_gettid));
}

DWORD GetCurrentProcessId()
{
    return static_cast<DWORD>(::getpid());
}

void Sleep(DWORD milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

#endif // !_WIN32
