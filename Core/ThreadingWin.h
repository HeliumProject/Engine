//----------------------------------------------------------------------------------------------------------------------
// ThreadingWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_THREADING_WIN_H
#define LUNAR_CORE_THREADING_WIN_H

#include "Core/Core.h"
#include "Core/String.h"

/// Non-zero to use the TBB reader-writer lock implementation.
#define L_USE_TBB_READ_WRITE_LOCK 1

#if L_USE_TBB_READ_WRITE_LOCK
#include "tbb/reader_writer_lock.h"
#endif

// WinBase.h defines Yield() as an empty macro, so we undefine it so it can be used as a function name.
#ifdef Yield
#undef Yield
#endif

namespace Lunar
{
    /// Thread instance.
    ///
    ///
    class LUNAR_CORE_API Thread
    {
    public:
        /// Thread ID type.
        typedef DWORD id_t;

        /// Invalid thread ID value.  Note that this can vary between platforms, so it should not be assumed to be any
        /// value in particular.
        static const id_t INVALID_ID = 0;

        /// @name Construction/Destruction
        //@{
        Thread( Runnable* pRunnable = NULL, const String& rName = String() );
        virtual ~Thread();
        //@}

        /// @name Caller Interface
        //@{
        void SetRunnable( Runnable* pRunnable );

        void SetName( const String& rName );
        inline const String& GetName() const;

        bool Start();
        bool Join( uint32_t timeOutMilliseconds = 0 );
        bool TryJoin();
        //@}

        /// @name Thread-side Interface
        //@{
        virtual void Run();
        //@}

        /// @name Static Functions
        //@{
        inline static void Sleep( uint32_t milliseconds );
        inline static void Yield();

        inline static id_t GetCurrentId();
        //@}

    protected:
        /// Thread name.
        String m_name;
        /// Runnable to execute.
        Runnable* m_pRunnable;
        /// Thread resource handle.
        uintptr_t m_thread;

        /// @name Thread Callback
        //@{
        static unsigned int __stdcall ThreadCallback( void* pData );
        //@}
    };

    /// Read-write lock.
    ///
    /// A read-write lock is a synchronization mechanism for allowing multiple threads to acquire read-only access to a
    /// shared resource simultaneously, while allowing exclusive access to a single thread when read-write access is
    /// desired.  This allows for much more minimal blocking of threads, particularly when exclusive write access is
    /// kept to a minimum.
    ///
    /// For efficiency, locking is only guaranteed to be functional within the context of the process in which the
    /// read-write lock was created.
    class LUNAR_CORE_API ReadWriteLock
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline ReadWriteLock();
        inline ~ReadWriteLock();
        //@}

        /// @name Synchronization Interface
        //@{
        inline void LockRead();
        inline void UnlockRead();

        inline void LockWrite();
        inline void UnlockWrite();
        //@}

    private:
#if L_USE_TBB_READ_WRITE_LOCK
        /// Reader-writer mutex instance.
        tbb::interface5::reader_writer_lock m_lock;
#else
        /// Number of threads with read access, or -1 if write access is currently being held.
        volatile int32_t m_readLockCount;

        /// Read-lock release event.
        HANDLE m_hReadReleaseEvent;
        /// Write-lock release event.
        HANDLE m_hWriteReleaseEvent;
#endif
    };

    /// Thread-local storage management.
    class LUNAR_CORE_API ThreadLocalStorage
    {
    public:
        /// @name Storage Access
        //@{
        inline size_t Allocate();
        inline void Free( size_t index );

        inline uintptr_t Get( size_t index );
        inline void Set( size_t index, uintptr_t value );
        //@}

        /// @name Instance Access.
        //@{
        static ThreadLocalStorage& GetInstance();
        //@}

    private:
        /// @name Construction/Destruction
        //@{
        inline ThreadLocalStorage();
        inline ThreadLocalStorage( const ThreadLocalStorage& );  // Not implemented.
        inline ~ThreadLocalStorage();
        //@}

        /// @name Overloaded Operators
        //@{
        inline ThreadLocalStorage& operator=( const ThreadLocalStorage& );  // Not implemented.
        //@}
    };
}

#endif  // LUNAR_CORE_THREADING_WIN_H
