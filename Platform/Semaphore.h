#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#if !HELIUM_OS_WIN
# include <semaphore.h>
#endif

namespace Helium
{
    class HELIUM_PLATFORM_API Semaphore
    {
    public:
#if HELIUM_OS_WIN
        typedef void* Handle;
#else
        typedef sem_t Handle;
#endif

	public:
        Semaphore();
        ~Semaphore();

        void Increment();
        void Decrement();
        void Reset();

        inline const Handle& GetHandle();

	private:
        Handle m_Handle;
    };
}

#include "Platform/Semaphore.inl"