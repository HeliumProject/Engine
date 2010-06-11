#pragma once

#include "API.h"

#include "Types.h"

#ifndef WIN32
# include <pthread.h>
#endif

namespace Platform
{
  class PLATFORM_API Event
  {
  public:
#ifdef WIN32
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

  private:
    Handle m_Handle;

  public:
    Event();
    ~Event();

    const Handle& GetHandle()
    {
      return m_Handle;
    }

    void Signal();
    void Reset();
    bool Wait(u32 timeout = 0xffffffff);
  };
}
