#pragma once

#include "API.h"

#include "Types.h"

#ifndef WIN32
# include <pthread.h>
#endif

namespace Helium
{
    class PLATFORM_API Thread
    {
    public:
#ifdef WIN32
        typedef void*               Handle;
        typedef uint32_t                 Return;
        typedef void*               Param;
        typedef Return              (*Entry)(Param);
#else
        typedef sys_ppu_thread_t    Handle;
        typedef void                Return;
        typedef uint64_t                 Param;
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
        bool Create( Entry entry, void* obj, const char* name, int priority = 0 );

        // C++ helper (remember, it is valid to pass a member function pointer as a template parameter!)
        template< class ObjectT, void (ObjectT::*method)() >
        static Thread::Return EntryHelper( Thread::Param param )
        {
            ObjectT* object = (ObjectT*)param;
            (object->*method)();
            return Thread::Exit();
        }

        // create and execute a thread with a separate args object
        bool CreateWithArgs( Entry entry, void* obj, void* args, const char* name, int priority = 0 )
        {
            ThreadHelperArgs* threadHelperArgs = new ThreadHelperArgs( obj, args );
            return Create(entry, threadHelperArgs, name);
        }

        // C++ helper (remember, it is valid to pass a member function pointer as a template parameter!)
        template< class ObjectT, typename ArgsT, void (ObjectT::*method)( ArgsT& ) >
        static Thread::Return EntryHelperWithArgs( Thread::Param param )
        {
            ThreadHelperArgs*   helperArgs = (ThreadHelperArgs*)param;
            ObjectT*            object = (ObjectT*)helperArgs->m_Object;
            ArgsT*              args = (ArgsT*)helperArgs->m_Args;

            (object->*method)( *args );

            delete helperArgs;
            delete args;

            return Thread::Exit();
        }

        // exit the calling thread
        static Return Exit();

        // close the thread handle
        void Close();

        // wait for thread completion
        Return Wait(uint32_t timeout = 0xffffffff);

        // are we running?
        bool Running();

        // are we valid?
        bool Valid();
    };

    class PLATFORM_API ThreadLocalPointer
    {
    public:
        ThreadLocalPointer();
        ~ThreadLocalPointer();

        void* GetPointer();
        void SetPointer(void* value);

    protected:
        uint32_t m_Key;
    };

    PLATFORM_API uint32_t GetMainThreadID();
    PLATFORM_API uint32_t GetCurrentThreadID();

    inline bool IsMainThread()
    {
        return GetMainThreadID() == GetCurrentThreadID();
    }
}
