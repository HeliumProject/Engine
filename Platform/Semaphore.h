#pragma once

#include "API.h"

#include "Types.h"

#ifdef __GNUC__
# include <semaphore.h>
#endif

namespace Helium
{
    class HELIUM_PLATFORM_API Semaphore
    {
    public:

#ifdef __GNUC__
        typedef sem_t Handle;
#elif defined( WIN32 )
        typedef void* Handle;
#else
#  pragma TODO( "Emit an error here..." )
#endif

    private:
        Handle m_Handle;

    public:
        Semaphore();
        ~Semaphore();

        const Handle& GetHandle()
        {
            return m_Handle;
        }

        void Increment();
        void Decrement();
        void Reset();
    };
}
