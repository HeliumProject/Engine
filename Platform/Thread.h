#pragma once

#include "API.h"

#include "Common/Types.h"

#ifndef WIN32
# include <pthread.h>
# include <sys/ppu_thread.h>
#endif

namespace Platform
{
  class PLATFORM_API Thread
  {
  public:
#ifdef WIN32
    typedef HANDLE              Handle;
    typedef DWORD               Return;
    typedef void*               Param;
    typedef Return              (*Entry)(Param);
#else
    typedef sys_ppu_thread_t    Handle;
    typedef void                Return;
    typedef u64                 Param;
    typedef Return              (*Entry)(Param);
#endif

  private:
    Handle m_Handle;

    struct ThreadHelperArgs
    {
      ThreadHelperArgs( void* object, void* args )
      : m_Object (object)
      , m_Args (args)
      {
      }
      
      void* m_Object;
      void* m_Args;
    };

  public:
    Thread();
    ~Thread();

    const Handle& GetHandle()
    {
      return m_Handle;
    }

    // create and execute a thread
    bool Create(Entry entry, void* obj, const char* name, int priority = 0);
    bool CreateWithArgs(Entry entry, void* obj, void* args, const char* name, int priority = 0)
    {
      ThreadHelperArgs* threadHelperArgs = new ThreadHelperArgs( obj, args );
      return Create(entry, threadHelperArgs, name);
    }

    // exit the calling thread
    static Return Exit();

    // close the thread handle
    void Close();

    // wait for thread completion
    Return Wait(u32 timeout = 0);

    // are we running?
    bool Running();

    // are we valid?
    bool Valid();

    template<class T, void (T::*F)()>
    static Thread::Return EntryHelper(Thread::Param param)
    {
      T* object = (T*)param;
      (object->*F)();
      return Thread::Exit();
    }

    template<class T, typename A, void (T::*F)( A& )>
    static Thread::Return EntryHelperWithArgs(Thread::Param param)
    {
      ThreadHelperArgs* args = (ThreadHelperArgs*) param;
      T* object = (T*) ( args->m_Object );
      A* context = (A*) ( args->m_Args );
      
      (object->*F)( *context );
      
      delete context;
      delete args;

      return Thread::Exit();
    }
  };
}
