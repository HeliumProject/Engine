//----------------------------------------------------------------------------------------------------------------------
// Threading.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    SpinLock::SpinLock()
        : m_counter( 0 )
    {
    }

    /// Lock this spin lock.
    ///
    /// @see Unlock(), TryLock(), IsLocked(), WaitForUnlock()
    void SpinLock::Lock()
    {
        while( AtomicCompareExchangeAcquire( m_counter, 1, 0 ) != 0 )
        {
            Thread::Yield();
        }
    }

    /// Unlock this spin lock.
    ///
    /// @see Lock(), TryLock(), IsLocked(), WaitForUnlock()
    void SpinLock::Unlock()
    {
        HELIUM_ASSERT( m_counter != 0 );
        AtomicExchangeRelease( m_counter, 0 );
    }

    /// Try to lock this spin lock without blocking.
    ///
    /// @return  True if a lock was acquired, false if not.
    ///
    /// @see Lock(), Unlock(), IsLocked(), WaitForUnlock()
    bool SpinLock::TryLock()
    {
        return( AtomicCompareExchangeAcquire( m_counter, 1, 0 ) == 0 );
    }

    /// Check whether a thread has this lock.
    ///
    /// Note that this does not modify the lock state.
    ///
    /// @return  True this is locked, false if not.
    ///
    /// @see Lock(), Unlock(), TryLock(), WaitForUnlock()
    bool SpinLock::IsLocked() const
    {
        return( m_counter != 0 );
    }

    /// Wait for any threads who have this locked to release their lock.
    ///
    /// Note that this does not modify the lock state.
    ///
    /// @see Lock(), Unlock(), TryLock(), IsLocked()
    void SpinLock::WaitForUnlock()
    {
        while( m_counter != 0 )
        {
            Thread::Yield();
        }
    }

    /// Constructor.
    ///
    /// This locks the given synchronization object immediately.  The synchronization object will be later unlocked when
    /// this object is destroyed.
    ///
    /// @param[in] rSyncObject  Synchronization object to lock.
    template< typename T >
    ScopeLock< T >::ScopeLock( T& rSyncObject )
        : m_rSyncObject( rSyncObject )
    {
        rSyncObject.Lock();
    }

    /// Destructor.
    ///
    /// This automatically unlocks the managed synchronization object.
    template< typename T >
    ScopeLock< T >::~ScopeLock()
    {
        m_rSyncObject.Unlock();
    }
}
