#pragma once

#include "Platform/Utility.h"

namespace Helium
{
    /// Scoped locking mechanism.
    ///
    /// This class provides functionality for locking a synchronization object (i.e. mutex, spin lock) and automatically
    /// unlocking the object when the scope in which the scope lock exists ends.
    template< typename T, void ( T::*LockFunction )() = &T::Lock, void ( T::*UnlockFunction )() = &T::Unlock >
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
}

#include "Platform/ScopeLock.inl"
