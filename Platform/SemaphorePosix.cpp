#include "Platform/Semaphore.h"
#include "Platform/Platform.h"

#include "Platform/Assert.h"

using namespace Helium;

Semaphore::Semaphore()
{
    int result = sem_init(&m_Handle, 0, 0);
    HELIUM_ASSERT( result == 0 );
}

Semaphore::~Semaphore()
{
    int result = sem_destroy(&m_Handle);
    HELIUM_ASSERT( result == 0 );
}

void Semaphore::Increment()
{
    int result = sem_post(&m_Handle);
    HELIUM_ASSERT( result == 0 );
}

void Semaphore::Decrement()
{
    int result = sem_wait(&m_Handle);
    HELIUM_ASSERT( result == 0 );
}

void Semaphore::Reset()
{
    int result = sem_destroy(&m_Handle);
    HELIUM_ASSERT( result == 0 );
    result = sem_init(&m_Handle, 0, 0);
    HELIUM_ASSERT( result == 0 );
}
