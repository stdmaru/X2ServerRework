// Win32 synchronization/thread API subset used by the original code.
// Native on Windows; emulated elsewhere (critical sections, events, threads, waits).
#pragma once

#include "x2/platform/types.h"

#if defined(_WIN32)

#include <process.h> // _beginthreadex

#else

#include <mutex>

struct CRITICAL_SECTION
{
    std::recursive_mutex mutex; // Win32 critical sections are recursive
};
typedef CRITICAL_SECTION* LPCRITICAL_SECTION;

inline void InitializeCriticalSection(CRITICAL_SECTION*) {}
inline BOOL InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION*, DWORD) { return TRUE; }
inline void DeleteCriticalSection(CRITICAL_SECTION*) {}
inline void EnterCriticalSection(CRITICAL_SECTION* cs) { cs->mutex.lock(); }
inline void LeaveCriticalSection(CRITICAL_SECTION* cs) { cs->mutex.unlock(); }
inline BOOL TryEnterCriticalSection(CRITICAL_SECTION* cs) { return cs->mutex.try_lock() ? TRUE : FALSE; }

// Kernel objects (events and threads) are reached through opaque handles.
namespace x2::platform::detail { struct HandleBox; }
typedef x2::platform::detail::HandleBox* HANDLE;
#define INVALID_HANDLE_VALUE (reinterpret_cast<HANDLE>(-1))

struct SECURITY_ATTRIBUTES;
typedef SECURITY_ATTRIBUTES* LPSECURITY_ATTRIBUTES;

#define WAIT_OBJECT_0    0x00000000u
#define WAIT_ABANDONED   0x00000080u
#define WAIT_TIMEOUT     0x00000102u
#define WAIT_FAILED      0xFFFFFFFFu
#define MAXIMUM_WAIT_OBJECTS 64
#define CREATE_SUSPENDED 0x00000004u

HANDLE CreateEvent(const void* attributes, BOOL manualReset, BOOL initialState, const wchar_t* name);
BOOL   SetEvent(HANDLE event);
BOOL   ResetEvent(HANDLE event);
BOOL   CloseHandle(HANDLE handle);
DWORD  WaitForSingleObject(HANDLE handle, DWORD milliseconds);
DWORD  WaitForMultipleObjects(DWORD count, const HANDLE* handles, BOOL waitAll, DWORD milliseconds);

// Same shape as the CRT function: returns the thread handle as an integer, 0 on failure.
// CREATE_SUSPENDED is not supported.
std::uintptr_t _beginthreadex(void* security, unsigned stackSize, unsigned (*start)(void*), void* arg,
                              unsigned initFlag, unsigned* threadId);
BOOL   GetExitCodeThread(HANDLE thread, DWORD* exitCode);
#define STILL_ACTIVE 259u

DWORD  GetCurrentThreadId();
DWORD  GetCurrentProcessId();
void   Sleep(DWORD milliseconds);

#endif // _WIN32
