/// Constructor.
Helium::SpinLock::SpinLock()
    : m_counter( 0 )
{
}

/// Lock this spin lock.
///
/// @see Unlock(), TryLock(), IsLocked(), WaitForUnlock()
void Helium::SpinLock::Lock()
{
    while( AtomicCompareExchangeAcquire( m_counter, 1, 0 ) != 0 )
    {
        Thread::Yield();
    }
}

/// Unlock this spin lock.
///
/// @see Lock(), TryLock(), IsLocked(), WaitForUnlock()
void Helium::SpinLock::Unlock()
{
    HELIUM_ASSERT( m_counter != 0 );
    AtomicExchangeRelease( m_counter, 0 );
}

/// Try to lock this spin lock without blocking.
///
/// @return  True if a lock was acquired, false if not.
///
/// @see Lock(), Unlock(), IsLocked(), WaitForUnlock()
bool Helium::SpinLock::TryLock()
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
bool Helium::SpinLock::IsLocked() const
{
    return( m_counter != 0 );
}

/// Wait for any threads who have this locked to release their lock.
///
/// Note that this does not modify the lock state.
///
/// @see Lock(), Unlock(), TryLock(), IsLocked()
void Helium::SpinLock::WaitForUnlock()
{
    while( m_counter != 0 )
    {
        Thread::Yield();
    }
}
