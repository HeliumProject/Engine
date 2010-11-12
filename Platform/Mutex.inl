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
