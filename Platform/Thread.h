#pragma once

#include "Platform/Utility.h"

#ifndef WIN32
# include <pthread.h>
#endif

// WinBase.h defines Yield() as an empty macro, so we undefine it so it can be used as a function name.
#ifdef Yield
#undef Yield
#endif

namespace Helium
{
    /// Interface for thread execution.
    ///
    /// This class should be implemented to provide the code to be executed by a thread.  When a Thread instance starts,
    /// it will call the Run() method within the context of the running thread.  The Thread instance can be checked to
    /// determine whether or not a thread is still running.
    class HELIUM_PLATFORM_API Runnable
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~Runnable() = 0;
        //@}

        /// @name Runnable Interface
        //@{
        virtual void Run() = 0;
        //@}
    };

    /// Thread interface.
    class HELIUM_PLATFORM_API Thread : NonCopyable
    {
    public:
#if HELIUM_OS_WIN
        /// Platform-specific thread handle type.
        typedef uintptr_t Handle;

        /// Thread ID type.
        typedef unsigned long id_t;

        /// Invalid thread ID value.  Note that this can vary between platforms, so it should not be assumed to be any
        /// value in particular.
        static const id_t INVALID_ID = 0;
#else
# error Implement Thread for this platform.
#endif

        /// Thread priority constants.
        enum EPriority
        {
            PRIORITY_FIRST   =  0,
            PRIORITY_INVALID = -1,

            /// Lowest thread priority.
            PRIORITY_LOWEST,
            /// Low thread priority.
            PRIORITY_LOW,
            /// Normal (default) thread priority.
            PRIORITY_NORMAL,
            /// High thread priority.
            PRIORITY_HIGH,
            /// Highest thread priority.
            PRIORITY_HIGHEST,

            PRIORITY_MAX,
            PRIORITY_LAST = PRIORITY_MAX - 1
        };

    private:
        /// Platform-specific thread handle.
        Handle m_Handle;
        /// Thread name.
        tchar_t* m_Name;

        /// @name Thread Callback
        //@{
        static unsigned int __stdcall ThreadCallback( void* pData );
        //@}

    public:
        /// @name Construction/Destruction
        //@{
        //explicit Thread( const String& rName = String() );
        explicit Thread( const tchar_t* pName = NULL );
        virtual ~Thread();
        //@}

        /// @name Data Access
        //@{
        inline const Handle& GetHandle() const;
        //@}

        /// @name Caller Interface
        //@{
        void SetName( const tchar_t* pName );
        inline const tchar_t* GetName() const;

        bool Start( EPriority priority = PRIORITY_NORMAL );
        bool Join( uint32_t timeOutMilliseconds = 0 );
        bool TryJoin();
        bool IsRunning() const;
        //@}

        /// @name Thread-side Interface
        //@{
        virtual void Run() = 0;
        //@}

        /// @name Static Functions
        //@{
        static void Sleep( uint32_t milliseconds );
        static void Yield();

        static id_t GetCurrentId();
        //@}
    };

    /// Thread interface for creating threads that execute a Runnable object.
    class HELIUM_PLATFORM_API RunnableThread : public Thread
    {
    public:
        /// @name Construction/Destruction
        //@{
        //explicit Thread( Runnable* pRunnable = NULL, const String& rName = String() );
        explicit RunnableThread( Runnable* pRunnable = NULL, const tchar_t* pName = NULL );
        virtual ~RunnableThread();
        //@}

        /// @name Caller Interface
        //@{
        void SetRunnable( Runnable* pRunnable );
        //@}

        /// @name Thread-side Interface
        //@{
        virtual void Run();
        //@}

    private:
        /// Runnable to execute.
        Runnable* m_pRunnable;
    };

    /// Thread interface for easily creating threads based on a C function or C++ member function pointer.
    class HELIUM_PLATFORM_API CallbackThread : public Thread
    {
    public:
        /// Thread callback type.
        typedef void ( *Entry )( void* );

    private:
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

        /// Thread callback.
        Entry m_Entry;
        /// Thread callback parameter.
        void* m_Object;

    public:
        /// @name Construction/Destruction
        //@{
        CallbackThread();
        virtual ~CallbackThread();
        //@}

        /// @name Caller Interface
        //@{
        // create and execute a thread
        bool Create( Entry entry, void* obj, const tchar_t* name, EPriority priority = PRIORITY_NORMAL );

        // C++ helper (remember, it is valid to pass a member function pointer as a template parameter!)
        template< class ObjectT, void (ObjectT::*method)() >
        static void EntryHelper( void* param );

        // create and execute a thread with a separate args object
        inline bool CreateWithArgs( Entry entry, void* obj, void* args, const tchar_t* name, EPriority priority = PRIORITY_NORMAL );

        // C++ helper (remember, it is valid to pass a member function pointer as a template parameter!)
        template< class ObjectT, typename ArgsT, void (ObjectT::*method)( ArgsT& ) >
        static void EntryHelperWithArgs( void* param );
        //@}

        /// @name Thread-side Interface
        //@{
        virtual void Run();
        //@}
    };

    class HELIUM_PLATFORM_API ThreadLocalPointer
    {
    public:
        ThreadLocalPointer();
        ~ThreadLocalPointer();

        void* GetPointer() const;
        void SetPointer(void* value);

    protected:
#if HELIUM_OS_WIN
        unsigned long m_Key;
#else
# error Implement ThreadLocalPointer for this platform.
#endif
    };

    HELIUM_PLATFORM_API uint32_t GetMainThreadID();
    HELIUM_PLATFORM_API uint32_t GetCurrentThreadID();

    inline bool IsMainThread()
    {
        return GetMainThreadID() == GetCurrentThreadID();
    }
}

#include "Platform/Thread.inl"
