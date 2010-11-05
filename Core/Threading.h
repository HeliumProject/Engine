//----------------------------------------------------------------------------------------------------------------------
// Threading.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_THREADING_H
#define LUNAR_CORE_THREADING_H

#include "Core/Core.h"
#include "Core/Platform.h"

namespace Lunar
{
    class ReadWriteLock;

    /// Interface for thread execution.
    ///
    /// This class should be implemented to provide the code to be executed by a thread.  When a Thread instance starts,
    /// it will call the Run() method within the context of the running thread.  The Thread instance can be checked to
    /// determine whether or not a thread is still running.
    class LUNAR_CORE_API Runnable
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~Runnable() = 0;
        //@}

        /// @name Runnable Interface
        //@{
        virtual void Run() = 0;
        //@}
    };

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
    class LUNAR_CORE_API SpinLock
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

    /// Scoped locking mechanism.
    ///
    /// This class provides functionality for locking a synchronization object (i.e. mutex, lightweight mutex) and
    /// automatically unlocking the object when the scope in which the scope lock exists ends.
    template< typename T >
    class ScopeLock : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        explicit ScopeLock( T& rSyncObject );
        ~ScopeLock();
        //@}

    private:
        /// Reference to the synchronization object being locked.
        T& m_rSyncObject;
    };

    /// Scoped locking mechanism for non-exclusive (read-only) locks using ReadWriteLock.
    ///
    /// This class provides functionality for acquiring a read lock on a ReadWriteLock object and automatically
    /// unlocking the object when the scope in which the scope lock exists ends.
    class LUNAR_CORE_API ScopeReadLock : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline explicit ScopeReadLock( ReadWriteLock& rSyncObject );
        inline ~ScopeReadLock();
        //@}

    private:
        /// Reference to the ReadWriteLock object being locked.
        ReadWriteLock& m_rSyncObject;
    };

    /// Scoped locking mechanism for exclusive (read-write) locks using ReadWriteLock.
    ///
    /// This class provides functionality for locking a write lock on a ReadWriteLock object and automatically unlocking
    /// the object when the scope in which the scope lock exists ends.
    class LUNAR_CORE_API ScopeWriteLock : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline explicit ScopeWriteLock( ReadWriteLock& rSyncObject );
        inline ~ScopeWriteLock();
        //@}

    private:
        /// Reference to the ReadWriteLock object being locked.
        ReadWriteLock& m_rSyncObject;
    };
}

#if L_OS_WIN
#include "ThreadingWin.h"
#include "ThreadingWin.inl"
#endif

#include "Threading.inl"

#endif  // LUNAR_CORE_THREADING_H
