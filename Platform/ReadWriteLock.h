#pragma once

#include "Platform/ScopeLock.h"

#include "tbb/reader_writer_lock.h"

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
    class HELIUM_PLATFORM_API ReadWriteLock
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
        /// Reader-writer mutex instance.
        tbb::interface5::reader_writer_lock m_lock;
    };

    /// Scope-based read-locking mechanism for ReadWriteLock objects.
    typedef ScopeLock< ReadWriteLock, &ReadWriteLock::LockRead, &ReadWriteLock::UnlockRead > ScopeReadLock;
    /// Scope-based write-locking mechanism for ReadWriteLock objects.
    typedef ScopeLock< ReadWriteLock, &ReadWriteLock::LockWrite, &ReadWriteLock::UnlockWrite > ScopeWriteLock;
}
