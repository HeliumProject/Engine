//----------------------------------------------------------------------------------------------------------------------
// ThreadingWin.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "Platform/Atomic.h"

namespace Lunar
{
    /// Get the name assigned to this thread.
    ///
    /// @return  Thread name.
    ///
    /// @see SetName()
    const String& Thread::GetName() const
    {
        return m_name;
    }

    /// Suspend execution of the calling thread for at least a given period of time.
    ///
    /// @param[in] milliseconds  Number of milliseconds to sleep.  Note that the calling thread may not be awaken
    ///                          immediately upon the timer expiring.
    ///
    /// @see Yield()
    void Thread::Sleep( uint32_t milliseconds )
    {
        ::Sleep( milliseconds );
    }

    /// Yield the remainder of the calling thread's time slice for other threads of equal priority.
    ///
    /// If other threads of equal priority are awaiting execution, this will immediately yield execution to those
    /// threads.  If no other threads of equal priority are waiting, the thread will continue execution immediately.
    ///
    /// To yield control to lower-priority threads, Sleep() should be called instead with a non-zero amount of time.
    ///
    /// @see Sleep()
    void Thread::Yield()
    {
        ::Sleep( 0 );
    }

    /// Get the ID of the thread in which this function is called.
    ///
    /// @return  Current thread ID.
    Thread::id_t Thread::GetCurrentId()
    {
        return GetCurrentThreadId();
    }

    /// Constructor.
    LwMutex::LwMutex()
    {
        InitializeCriticalSection( &m_criticalSection );
    }

    /// Destructor.
    LwMutex::~LwMutex()
    {
        DeleteCriticalSection( &m_criticalSection );
    }

    /// Lock this mutex.
    ///
    /// @see Unlock(), TryLock()
    void LwMutex::Lock()
    {
        EnterCriticalSection( &m_criticalSection );
    }

    /// Unlock this mutex.
    ///
    /// @see Lock(), TryLock()
    void LwMutex::Unlock()
    {
        LeaveCriticalSection( &m_criticalSection );
    }

    /// Try to lock this mutex without blocking.
    ///
    /// @return  True if the mutex was unlocked and this thread managed to acquire the lock, false the mutex is already
    ///          locked by another thread.
    bool LwMutex::TryLock()
    {
        return( TryEnterCriticalSection( &m_criticalSection ) != FALSE );
    }

    /// Constructor.
    ///
    /// @param[in] resetMode      Whether the event should automatically reset after releasing a thread when signaled
    ///                           (RESET_MODE_AUTO) or whether the event will remain signaled until Reset() is called
    ///                           (RESET_MODE_MANUAL).
    /// @param[in] bInitialState  True to initialize this event in the signaled state, false to initialize non-signaled.
    Event::Event( EResetMode resetMode, bool bInitialState )
    {
        HELIUM_ASSERT( static_cast< size_t >( resetMode ) < static_cast< size_t >( RESET_MODE_MAX ) );

        m_hEvent = CreateEvent( NULL, ( resetMode == RESET_MODE_MANUAL ), bInitialState, NULL );
        HELIUM_ASSERT( m_hEvent );
    }

    /// Destructor.
    Event::~Event()
    {
        CloseHandle( m_hEvent );
    }

    /// Signal this event.
    ///
    /// This is used to notify threads waiting on this event to wake up.  The effect differs based on whether this is a
    /// manual-reset or auto-reset event.
    /// - For manual-reset events, all waiting threads will be signaled, and the event will be left in the signaled
    ///   state until Reset() is called.
    /// - For auto-reset events, the event will remain signaled until one waiting thread is released, after which the
    ///   event will automatically be reset.  If no threads are waiting when the event is signaled, it will remain in
    ///   the signaled state until either a thread does attempt to wait on it or Reset() is called.
    ///
    /// @see Wait(), Reset()
    void Event::Signal()
    {
        SetEvent( m_hEvent );
    }

    /// Wait indefinitely on this event.
    ///
    /// This causes the current thread to sleep until the event is signaled.  If the event is currently in the signaled
    /// state, the thread will wake up immediately.  The behavior of the event when signaled is determined by the reset
    /// mode of the event (see Signal() for more information).
    ///
    /// @return  True if the event was signaled, false if waiting was interrupted.
    ///
    /// @see Signal, Reset()
    bool Event::Wait()
    {
        DWORD result = WaitForSingleObject( m_hEvent, INFINITE );

        return( result == WAIT_OBJECT_0 );
    }

    /// Wait on this event for up to a given period of time.
    ///
    /// This causes the current thread to sleep until the event is signaled.  If the event is currently in the signaled
    /// state, the thread will wake up immediately.  The behavior of the event when signaled is determined by the reset
    /// mode of the event (see Signal() for more information).
    ///
    /// @param[in] timeoutMs  Maximum number of milliseconds to wait for the event to be signaled.
    ///
    /// @return  True if the event was signaled, false if waiting was interrupted.
    ///
    /// @see Signal, Reset()
    bool Event::Wait( uint32_t timeoutMs )
    {
        DWORD result = WaitForSingleObject( m_hEvent, timeoutMs );

        return( result == WAIT_OBJECT_0 );
    }

    /// Reset this event to the non-signaled state.
    ///
    /// @see Signal(), Wait()
    void Event::Reset()
    {
        ResetEvent( m_hEvent );
    }

#if L_USE_TBB_READ_WRITE_LOCK
    /// Constructor.
    ReadWriteLock::ReadWriteLock()
    {
    }

    /// Destructor.
    ReadWriteLock::~ReadWriteLock()
    {
    }

    /// Acquire a non-exclusive, read-only lock for the current thread.
    ///
    /// @see UnlockRead(), LockWrite(), UnlockWrite()
    void ReadWriteLock::LockRead()
    {
        m_lock.lock_read();
    }

    /// Release a read-only lock previously acquired using LockRead().
    ///
    /// @see LockRead(), LockWrite(), UnlockWrite()
    void ReadWriteLock::UnlockRead()
    {
        m_lock.unlock();
    }

    /// Acquire an exclusive, read-write lock for the current thread.
    ///
    /// @see UnlockWrite(), LockRead(), UnlockRead()
    void ReadWriteLock::LockWrite()
    {
        m_lock.lock();
    }

    /// Release a read-write lock previously acquired using LockWrite().
    ///
    /// @see LockWrite(), LockRead(), UnlockRead()
    void ReadWriteLock::UnlockWrite()
    {
        m_lock.unlock();
    }
#else  // L_USE_TBB_READ_WRITE_LOCK
    /// Constructor.
    ReadWriteLock::ReadWriteLock()
        : m_readLockCount( 0 )
    {
        m_hReadReleaseEvent = CreateEvent( NULL, TRUE, TRUE, NULL );
        HELIUM_ASSERT( m_hReadReleaseEvent );

        m_hWriteReleaseEvent = CreateEvent( NULL, TRUE, TRUE, NULL );
        HELIUM_ASSERT( m_hWriteReleaseEvent );
    }

    /// Destructor.
    ReadWriteLock::~ReadWriteLock()
    {
        CloseHandle( m_hReadReleaseEvent );
        CloseHandle( m_hWriteReleaseEvent );
    }

    /// Acquire a non-exclusive, read-only lock for the current thread.
    ///
    /// @see UnlockRead(), LockWrite(), UnlockWrite()
    void ReadWriteLock::LockRead()
    {
        int32_t currentLockCount = m_readLockCount;
        int32_t localLockCount;
        do 
        {
            localLockCount = currentLockCount;
            while( localLockCount == -1 )
            {
                while( WaitForSingleObject( m_hWriteReleaseEvent, INFINITE ) != WAIT_OBJECT_0 )
                {
                }

                localLockCount = m_readLockCount;
            }

            HELIUM_ASSERT( localLockCount != -2 );
            currentLockCount = AtomicCompareExchangeAcquire( m_readLockCount, localLockCount + 1, localLockCount );
        } while( currentLockCount != localLockCount );

        if( localLockCount == 0 )
        {
            ResetEvent( m_hReadReleaseEvent );
        }
    }

    /// Release a read-only lock previously acquired using LockRead().
    ///
    /// @see LockRead(), LockWrite(), UnlockWrite()
    void ReadWriteLock::UnlockRead()
    {
        HELIUM_ASSERT( m_readLockCount != -1 );

        int32_t newLockCount = AtomicDecrementRelease( m_readLockCount );
        if( newLockCount == 0 )
        {
            SetEvent( m_hReadReleaseEvent );
        }
    }

    /// Acquire an exclusive, read-write lock for the current thread.
    ///
    /// @see UnlockWrite(), LockRead(), UnlockRead()
    void ReadWriteLock::LockWrite()
    {
        for( ; ; )
        {
            int32_t currentLockCount = AtomicCompareExchangeAcquire( m_readLockCount, -1, 0 );
            if( currentLockCount == 0 )
            {
                break;
            }

            while( WaitForSingleObject( m_hReadReleaseEvent, INFINITE ) != WAIT_OBJECT_0 )
            {
            }

            while( WaitForSingleObject( m_hWriteReleaseEvent, INFINITE ) != WAIT_OBJECT_0 )
            {
            }
        }

        ResetEvent( m_hWriteReleaseEvent );
    }

    /// Release a read-write lock previously acquired using LockWrite().
    ///
    /// @see LockWrite(), LockRead(), UnlockRead()
    void ReadWriteLock::UnlockWrite()
    {
        HELIUM_ASSERT( m_readLockCount == -1 );

        AtomicExchangeRelease( m_readLockCount, 0 );
        SetEvent( m_hWriteReleaseEvent );
    }
#endif  // L_USE_TBB_READ_WRITE_LOCK

    /// Allocate a thread-local storage index.
    ///
    /// When an index is allocated by a thread, any thread can subsequently store and retrieve values in the slot.
    /// Thread-local storage values are always initialized to zero for all threads.
    ///
    /// @return  Allocated index, or an invalid index if storage could not be allocated.
    ///
    /// @see Free(), Get(), Set()
    size_t ThreadLocalStorage::Allocate()
    {
        DWORD index = TlsAlloc();
        return( index != TLS_OUT_OF_INDEXES ? static_cast< size_t >( index ) : Invalid< size_t >() );
    }

    /// Free a previously allocated thread-local storage index.
    ///
    /// @param[in] index  Index of the storage slot to free.
    ///
    /// @see Allocate()
    void ThreadLocalStorage::Free( size_t index )
    {
        HELIUM_ASSERT( IsValid( index ) );
        HELIUM_VERIFY( TlsFree( static_cast< DWORD >( index ) ) );
    }

    /// Get the value stored for this thread in a thread-local storage slot.
    ///
    /// @param[in] index  Thread-local storage slot index.
    ///
    /// @return  Value currently stored in the specified slot.
    ///
    /// @see Allocate(), Free(), Set()
    uintptr_t ThreadLocalStorage::Get( size_t index )
    {
        HELIUM_ASSERT( IsValid( index ) );
        void* pValue = TlsGetValue( static_cast< DWORD >( index ) );

        return reinterpret_cast< uintptr_t >( pValue );
    }

    /// Set the thread-local storage value in a given storage slot for this thread.
    ///
    /// @param[in] index  Thread-local storage slot index.
    /// @param[in] value  Value to set.
    ///
    /// @see Allocate(), Free(), Get()
    void ThreadLocalStorage::Set( size_t index, uintptr_t value )
    {
        HELIUM_ASSERT( IsValid( index ) );
        HELIUM_VERIFY( TlsSetValue( static_cast< DWORD >( index ), reinterpret_cast< void* >( value ) ) );
    }

    /// Constructor.
    ThreadLocalStorage::ThreadLocalStorage()
    {
    }

    /// Destructor.
    ThreadLocalStorage::~ThreadLocalStorage()
    {
    }
}
