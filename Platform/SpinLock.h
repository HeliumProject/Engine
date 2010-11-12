#pragma once

#include "Platform/Atomic.h"
#include "Platform/ScopeLock.h"
#include "Platform/Thread.h"

namespace Helium
{
    /// Spin lock.
    ///
    /// This provides a simple mechanism for synchronizing access to a given resource through the use of a spin lock.
    /// A lock is acquired simply by setting an integer to a non-zero value.  While the counter is non-zero, any other
    /// thread that tries to acquire a lock will block until the counter is available.  Instead of letting the OS handle
    /// putting the thread into a wait state until the lock is available, the thread continuously repeats its attempt to
    /// acquire the lock.  To avoid excess starving of other threads, the thread will yield after each failed attempt.
    ///
    /// Note that recursive locking is not supported.  If a thread attempts to acquire a lock while it already has one,
    /// a deadlock will occur.
    class PLATFORM_API SpinLock
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline SpinLock();
        //@}

        /// @name Synchronization Interface
        //@{
        inline void Lock();
        inline void Unlock();
        inline bool TryLock();

        inline bool IsLocked() const;
        inline void WaitForUnlock();
        //@}

    private:
        /// Spin lock counter.
        volatile int32_t m_counter;
    };

    /// Scope-based locking mechanism for SpinLock objects.
    typedef ScopeLock< SpinLock > ScopeSpinLock;
}

#include "Platform/SpinLock.inl"
