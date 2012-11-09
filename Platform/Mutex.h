#pragma once

#include "Platform/ScopeLock.h"

#if !HELIUM_OS_WIN
# include <pthread.h>
#endif

namespace Helium
{
    /// Mutex.
    ///
    /// On supported platforms, this is implemented using a lightweight mutex that can only be used within the context
    /// of the process in which it is created (i.e. critical sections on Windows).  This typically yields better
    /// performance than a full-scale mutex, making it a more desirable alternative for general use.
    ///
    /// If a mutex is needed for synchronization across processes, ProcessMutex should be used instead.
    class HELIUM_PLATFORM_API Mutex : NonCopyable
    {
    public:
#if HELIUM_OS_WIN
        struct Handle
        {
            struct DebugInfoStruct
            {
                uint16_t Type;
                uint16_t CreatorBackTraceIndex;
                struct Handle *CriticalSection;
                struct ListEntryStruct
                {
                    struct ListEntryStruct *Flink;
                    struct ListEntryStruct *Blink;
                } ProcessLocksList;
                uint32_t EntryCount;
                uint32_t ContentionCount;
                uint32_t Spare[ 2 ];
            } *DebugInfo;

            int32_t LockCount;
            int32_t RecursionCount;
            void* OwningThread;
            void* LockSemaphore;
            uint32_t* SpinCount;
        };
#else
        typedef pthread_mutex_t Handle;
#endif

    public:
        /// @name Construction/Destruction
        //@{
        Mutex();
        ~Mutex();
        //@}

        /// @name Synchronization Interface
        //@{
        void Lock();
        void Unlock();
        bool TryLock();
        //@}

        /// @name Data Access
        //@{
        inline const Handle& GetHandle() const;
        //@}

    private:
        /// Platform-specific mutex handle.
        Handle m_Handle;
    };

    /// Scope-based locking mechanism for Mutex objects.
    typedef ScopeLock< Mutex > MutexScopeLock;

    /// Simple template to make some data only accessible to one thread at a time.
    template< typename T, typename LockType = Mutex >
    class Locker : NonCopyable
    {
    public:
        /// Handle for accessing the protected data.
        class Handle : NonCopyable
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline explicit Handle( Locker& locker );
            inline ~Handle();
            //@}

            /// @name Overloaded Operators
            //@{
            inline T* operator->();
            //@}

        private:
            /// Protected data locker.
            Locker& m_Locker;
        };

    private:
        /// Protected data.
        T m_Data;
        /// Synchronization object.
        LockType m_LockObject;
    };
}

#include "Platform/Mutex.inl"
