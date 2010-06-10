#pragma once

#include "API.h"

#include "Common/Types.h"

#ifndef WIN32
# include <semaphore.h>
#endif

namespace Platform
{
  class PLATFORM_API Semaphore
  {
  public:
#ifdef WIN32
    typedef void* Handle;
#else
    typedef sem_t Handle;
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
