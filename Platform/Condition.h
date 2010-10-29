#pragma once

#include "API.h"

#include "Types.h"

#ifdef __GNUC__
# include <string.h>
# include <pthread.h>
#endif

namespace Helium
{
    class PLATFORM_API Condition
    {
    public:

#ifdef __GNUC__
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
#elif defined( WIN32 )
        typedef void* Handle;
#else
#  pragma TODO( "Emit an error here..." )
#endif

    private:
        Handle m_Handle;

    public:
        Condition();
        ~Condition();

        const Handle& GetHandle()
        {
            return m_Handle;
        }

        void Signal();
        void Reset();
        bool Wait(uint32_t timeout = 0xffffffff);
    };
}
