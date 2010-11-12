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
