#pragma once

#include "API.h"

#include "Types.h"

#ifdef __GNUC__
# include <pthread.h>
#endif

namespace Helium
{
    class PLATFORM_API Mutex
    {
    public:
#ifdef __GNUC__
        typedef pthread_mutex_t Handle;
#elif defined( WIN32 )
        struct Handle
        {
            struct DebugInfoStruct
            {
                uint16_t Type;
                uint16_t CreatorBackTraceIndex;
                struct Handle *CriticalSection;
                struct ListEntryStruct
                {
                    struct ListEntryStruct *Flink;
                    struct ListEntryStruct *Blink;
                } ProcessLocksList;
                uint32_t EntryCount;
                uint32_t ContentionCount;
                uint32_t Spare[ 2 ];
            } *DebugInfo;

            int32_t LockCount;
            int32_t RecursionCount;
            void* OwningThread;
            void* LockSemaphore;
            uint32_t* SpinCount;
        };
#else
#  pragma TODO( "Emit an error here..." )
#endif

    private:
        Handle m_Handle;

    public:
        Mutex();

    private:
        Mutex( const Mutex& mutex )
        {

        }

    public:
        ~Mutex();

        const Handle& GetHandle()
        {
            return m_Handle;
        }

        void Lock();
        void Unlock();
    };

    //
    // TakeMutex - Allocate one of these on the stack to have it hold a mutex while in a function
    //  This technique is preferred in c++ exception throwing APIs
    //

    class TakeMutex
    {
    private:
        Mutex& m_Mutex;

    public:
        TakeMutex(Mutex& mutex)
            : m_Mutex (mutex)
        {
            m_Mutex.Lock();
        }

    private:
        TakeMutex(const TakeMutex& rhs)
            : m_Mutex (rhs.m_Mutex)
        {

        }

    public:
        ~TakeMutex()
        {
            m_Mutex.Unlock();
        }
    };

    //
    // Locker - Simple template to make some data only accessible to one thread at a time
    //

    template<class T>
    class Locker
    {
    public:
        friend class Handle;
        class Handle
        {
        public:
            Handle( Locker* locker )
                : m_Locker( locker )
            {
                m_Locker->m_Mutex.Lock();
            }

        public:
            ~Handle()
            {
                m_Locker->m_Mutex.Unlock();
            }

            inline T* operator->()
            {
                return &m_Locker->m_Data;
            }

        private:
            Locker* m_Locker;
        };

        inline Handle Lock()
        {
            return Handle( this );
        }

    private:
        T               m_Data;
        Helium::Mutex m_Mutex;
    };
}
