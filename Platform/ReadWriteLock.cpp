#include "PlatformPch.h"
#include "Platform/ReadWriteLock.h"

using namespace Helium;

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
