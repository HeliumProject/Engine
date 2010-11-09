#pragma once

#include "Platform/ScopeLock.h"

/// Non-zero to use the TBB reader-writer lock implementation.
#define HELIUM_USE_TBB_READ_WRITE_LOCK 1

#if HELIUM_USE_TBB_READ_WRITE_LOCK
#include "tbb/reader_writer_lock.h"
#endif

namespace Helium
{
    /// Read-write lock.
    ///
    /// A read-write lock is a synchronization mechanism for allowing multiple threads to acquire read-only access to a
    /// shared resource simultaneously, while allowing exclusive access to a single thread when read-write access is
    /// desired.  This allows for much more minimal blocking of threads, particularly when exclusive write access is
    /// kept to a minimum.
    ///
    /// For efficiency, locking is only guaranteed to be functional within the context of the process in which the
    /// read-write lock was created.
    class PLATFORM_API ReadWriteLock
    {
    public:
        /// @name Construction/Destruction
        //@{
        ReadWriteLock();
        ~ReadWriteLock();
        //@}

        /// @name Synchronization Interface
        //@{
        void LockRead();
        void UnlockRead();

        void LockWrite();
        void UnlockWrite();
        //@}

    private:
#if HELIUM_USE_TBB_READ_WRITE_LOCK
        /// Reader-writer mutex instance.
        tbb::interface5::reader_writer_lock m_lock;
#elif HELIUM_OS_WIN
        /// Number of threads with read access, or -1 if write access is currently being held.
        volatile int32_t m_readLockCount;

        /// Read-lock release event.
        void* m_hReadReleaseEvent;
        /// Write-lock release event.
        void* m_hWriteReleaseEvent;
#else
# error Implement for this platform.
#endif
    };

    /// Scope-based read-locking mechanism for ReadWriteLock objects.
    typedef ScopeLock< ReadWriteLock, &ReadWriteLock::LockRead, &ReadWriteLock::UnlockRead > ScopeReadLock;
    /// Scope-based write-locking mechanism for ReadWriteLock objects.
    typedef ScopeLock< ReadWriteLock, &ReadWriteLock::LockWrite, &ReadWriteLock::UnlockWrite > ScopeWriteLock;
}
