/// Constructor.
///
/// This locks the given synchronization object immediately.  The synchronization object will be later unlocked when
/// this object is destroyed.
///
/// @param[in] rSyncObject  Synchronization object to lock.
template< typename T, void ( T::*LockFunction )(), void ( T::*UnlockFunction )() >
Helium::ScopeLock< T, LockFunction, UnlockFunction >::ScopeLock( T& rSyncObject )
    : m_rSyncObject( rSyncObject )
{
    ( ( &rSyncObject )->*LockFunction )();
}

/// Destructor.
///
/// This automatically unlocks the managed synchronization object.
template< typename T, void ( T::*LockFunction )(), void ( T::*UnlockFunction )() >
Helium::ScopeLock< T, LockFunction, UnlockFunction >::~ScopeLock()
{
    ( ( &m_rSyncObject )->*UnlockFunction )();
}

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

/// Get the platform-specific mutex handle.
///
/// @return  Reference to the mutex handle.
const Helium::Mutex::Handle& Helium::Mutex::GetHandle() const
{
    return m_Handle;
}

/// Constructor.
///
/// @param[in] locker  Protected data locker instance.
template< typename T, typename LockType >
Helium::Locker< T, LockType >::Handle::Handle( Locker& locker )
    : m_Locker( locker )
{
    m_Locker.m_LockObject.Lock();
}

/// Destructor.
template< typename T, typename LockType >
Helium::Locker< T, LockType >::Handle::~Handle()
{
    m_Locker.m_LockObject.Unlock();
}

/// Access the data locked via this handle.
///
/// @return  Pointer to the locked data.
template< typename T, typename LockType >
T* Helium::Locker< T, LockType >::Handle::operator->()
{
    return &m_Locker.m_Data;
}
