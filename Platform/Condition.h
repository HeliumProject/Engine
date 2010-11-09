#pragma once

#include "API.h"

#include "Types.h"

#if !HELIUM_OS_WIN
# include <string.h>
# include <pthread.h>
#endif

namespace Helium
{
    /// Condition object.
    ///
    /// A condition is a type of synchronization mechanism that allows one or more threads to sleep until a signal is
    /// triggered by another thread.
    class PLATFORM_API Condition
    {
    public:
#if HELIUM_OS_WIN
        typedef void* Handle;
#else
        struct Handle
        {
            // Protect critical section
            pthread_mutex_t lock;

            // Keeps track of waiters
            pthread_cond_t condition;

            // Specifies if this is an auto- or manual-reset event
            bool manual_reset;

            // "True" if signaled
            bool is_signaled;

            // Number of waiting threads
            unsigned waiting_threads;
        };
#endif

        /// Reset modes.
        enum EResetMode
        {
            RESET_MODE_FIRST   =  0,
            RESET_MODE_INVALID = -1,

            RESET_MODE_MANUAL,  ///< Condition remains signaled until Reset() is explicitly called.
            RESET_MODE_AUTO,    ///< Condition is immediately reset when a single waiting thread is released.

            RESET_MODE_MAX,
            RESET_MODE_LAST = RESET_MODE_MAX - 1
        };

    private:
        /// Platform-specific condition handle.
        Handle m_Handle;

    public:
        /// @name Construction/Destruction
        //@{
        explicit Condition( EResetMode resetMode, bool bInitialState = false );
        ~Condition();
        //@}

        /// @name Synchronization Interface
        //@{
        void Signal();
        void Reset();
        bool Wait();
        bool Wait( uint32_t timeoutMs );
        //@}

        /// @name Data Access
        //@{
        inline const Handle& GetHandle() const;
        //@}
    };
}

#include "Platform/Condition.inl"
