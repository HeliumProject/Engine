#include "Platform/ReadWriteLock.h"

#if HELIUM_OS_WIN && !HELIUM_USE_TBB_READ_WRITE_LOCK

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

#endif  // HELIUM_OS_WIN && !HELIUM_USE_TBB_READ_WRITE_LOCK
