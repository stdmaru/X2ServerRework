/** @file   KJohnThread.h
    @brief  interface of KJohnThread
    @author seojt@kogstudios.com
    @since  2006-1-11, 10:26:49

    Port: on std::thread on every platform (the original created a Win32 thread with _beginthreadex). A thread begun
    suspended waits for ResumeThread() before it runs, as CREATE_SUSPENDED did. EndThread() always waits for the thread
    to finish: a thread object cannot be dropped while it runs (the original closed the handle after the timeout).
*/

#ifndef _KJOHNTHREAD_H
#define _KJOHNTHREAD_H

#include "x2/platform/win32.h"

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>


///-----------------------------------------------------------------------------
/// @class  KJohnThread
/// @brief  base class for a thread job
/// @note   override below 3 virtual functions
/** @code
    virtual bool BeginThread();
    virtual void EndThread(DWORD dwTimeoutMilliseconds_ = INFINITE);
    virtual DWORD RunThread() = 0;
    @endcode
*/
///
class KJohnThread
{
public:
    /** The type of the thread callback the user gives when RunThread() is not overridden. */
    typedef DWORD (WINAPI *THREAD_CALLBACK)(LPVOID);

public:
    KJohnThread() = default;

    /// Ends the thread safely, so it may be called even when BeginThread() never was.
    virtual ~KJohnThread()
    {
        EndThread();
    }//~KJohnThread

    /** Objects derived from KJohnThread must not be copied. - jintaeks on 2008-06-13, 10:00 */
    KJohnThread(const KJohnThread&) = delete;
    KJohnThread& operator=(const KJohnThread&) = delete;

public:
    /// begin thread.
    virtual bool BeginThread(THREAD_CALLBACK pThreadCallback_=NULL, LPVOID pParam_=NULL, bool bStartSuspended = false )
    {
        ASSERT( !m_thread.joinable() );
        if ( m_thread.joinable() )
            return false;

        m_bResumed = !bStartSuspended;
        m_bFinished = false;
        std::promise<void> started;
        std::future<void> startedFuture = started.get_future();
        m_thread = std::thread( [this, pThreadCallback_, pParam_, &started]()
        {
            m_dwThreadID = ::GetCurrentThreadId();
            started.set_value();
            {
                std::unique_lock<std::mutex> lock( m_mutex );
                m_resume.wait( lock, [this]() { return m_bResumed; } );
            }
            if ( pThreadCallback_ != NULL )
                pThreadCallback_( pParam_ );
            else
                RunThread();
            m_bFinished = true;
        } );
        // GetThreadID() is right as soon as BeginThread() returns, as with _beginthreadex.
        startedFuture.wait();
        return true;
    }//BeginThread()

    /// Lets a thread begun suspended run (no effect otherwise).
    void ResumeThread()
    {
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            m_bResumed = true;
        }
        m_resume.notify_all();
    }//ResumeThread()

    /// Waits for the thread to end and clears it. A thread still suspended is resumed first, so it can end.
    /// @param  dwTimeoutMilliseconds_: not supported (see the file comment); the wait has no limit.
    virtual void EndThread(DWORD /*dwTimeoutMilliseconds_*/ = INFINITE)
    {
        if ( m_thread.joinable() )
        {
            ResumeThread();
            m_thread.join();
            m_dwThreadID = 0;
        }//if
    }//EndThread()

    /// threading pure virtual function.
    virtual DWORD RunThread() = 0;

public:
    /// get thread ID (0 without a thread).
    DWORD GetThreadID() const { return m_dwThreadID; }

    /// Whether a thread was begun and not ended yet (the original compared the thread handle with NULL).
    bool HasThread() const { return m_thread.joinable(); }

    /// Whether the thread's function has returned.
    bool IsThreadFinished() const { return m_bFinished; }

private:
    std::thread             m_thread;
    std::mutex              m_mutex;
    std::condition_variable m_resume;
    bool                    m_bResumed = false;
    std::atomic<DWORD>      m_dwThreadID{ 0 };
    std::atomic<bool>       m_bFinished{ false };
};//class KJohnThread

#endif // _KJOHNTHREAD_H
