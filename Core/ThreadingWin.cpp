//----------------------------------------------------------------------------------------------------------------------
// ThreadingWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/Platform.h"

#if L_OS_WIN

#include "Core/Threading.h"

#include <process.h>

/// Thread name assignment exception information.
struct ThreadNameInfo
{
    /// Info type (must be set to 0x1000).
    ULONG_PTR dwType;
    /// Thread name.
    LPCSTR szName;
    /// Thread ID (-1 = caller thread).
    ULONG_PTR dwThreadId;
    /// Reserved (must be set to zero).
    ULONG_PTR dwFlags;
};

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] pRunnable  Runnable instance to execute.
    /// @param[in] rName      Optional name to assign to the thread (for debugging purposes).
    Thread::Thread( Runnable* pRunnable, const String& rName )
        : m_name( rName )
        , m_pRunnable( pRunnable )
        , m_thread( 0 )
    {
    }

    /// Destructor.
    Thread::~Thread()
    {
        L_VERIFY( Join() );
    }

    /// Set the runnable instance that this thread is set to execute.
    ///
    /// Note that the runnable instance can only be set while the thread is not running.
    ///
    /// @param[in] pRunnable  Runnable instance to execute.
    void Thread::SetRunnable( Runnable* pRunnable )
    {
        L_ASSERT( m_thread == 0 );
        m_pRunnable = pRunnable;
    }

    /// Set the thread name (for debugging purposes).
    ///
    /// Note that the thread name can only be set while the thread is not running.
    ///
    /// @param[in] rName  Name to set.
    ///
    /// @see GetName()
    void Thread::SetName( const String& rName )
    {
        L_ASSERT( m_thread == 0 );
        m_name = rName;
    }

    /// Begin execution of this thread.
    ///
    /// If a thread has been started and not yet joined, this will fail.
    ///
    /// @return  True if the thread was started successfully, false if not.
    ///
    /// @see Join(), TryJoin()
    bool Thread::Start()
    {
        // Make sure a thread hasn't already been started.
        L_ASSERT( m_thread == 0 );
        if( m_thread != 0 )
        {
            return false;
        }

        // Create the thread, but don't launch it yet (we want to set the thread name prior to the thread actually
        // starting).
        uint32_t threadId;
        m_thread = _beginthreadex( NULL, 0, ThreadCallback, this, CREATE_SUSPENDED, &threadId );
        L_ASSERT( m_thread != 0 );
        if( m_thread != 0 )
        {
            // Assign the thread name.
            size_t charCount = m_name.GetSize();
            if( charCount != 0 )
            {
                ThreadNameInfo nameInfo;
                nameInfo.dwType = 0x1000;
                nameInfo.dwThreadId = threadId;
                nameInfo.dwFlags = 0;

#if L_UNICODE
                // Perform a direct conversion from Unicode to single-byte characters by casting.
                char name[ 128 ];
                charCount = Min( charCount, L_ARRAY_COUNT( name ) - 1 );

                size_t charIndex;
                for( charIndex = 0; charIndex < charCount; ++charIndex )
                {
                    wchar_t character = m_name[ charIndex ];
                    name[ charIndex ] = ( character > 0xff ? 0xff : static_cast< char >( character ) );
                }

                name[ charIndex ] = '\0';

                nameInfo.szName = name;
#else
                nameInfo.szName = m_name.GetData();
                L_ASSERT( nameInfo.szName );
#endif

                __try
                {
                    RaiseException(
                        0x406D1388,
                        0,
                        sizeof( nameInfo ) / sizeof( ULONG_PTR ),
                        reinterpret_cast< ULONG_PTR* >( &nameInfo ) );
                }
                __except( EXCEPTION_CONTINUE_EXECUTION )
                {
                }
            }

            // Start the thread.
            DWORD resumeResult = ResumeThread( reinterpret_cast< HANDLE >( m_thread ) );
            L_ASSERT( resumeResult != static_cast< DWORD >( -1 ) );
            L_UNREF( resumeResult );
        }

        return( m_thread != 0 );
    }

    /// Wait for this thread to finish execution and release any allocated system resources.
    ///
    /// @param[in] timeOutMilliseconds  Maximum time to wait for the thread to finish, or zero to wait indefinitely.
    ///
    /// @return  True if the thread finished or was not running to begin with, false if it is still running.
    bool Thread::Join( uint32_t timeOutMilliseconds )
    {
        if( m_thread != 0 )
        {
            DWORD waitResult = WaitForSingleObject(
                reinterpret_cast< HANDLE >( m_thread ),
                ( timeOutMilliseconds != 0 ? timeOutMilliseconds : INFINITE ) );
            L_ASSERT( waitResult == WAIT_OBJECT_0 || waitResult == WAIT_TIMEOUT );
            if( waitResult != WAIT_OBJECT_0 )
            {
                return false;
            }

            L_VERIFY( CloseHandle( reinterpret_cast< HANDLE >( m_thread ) ) );
            m_thread = 0;
        }

        return true;
    }

    /// Check if this thread has finished execution without blocking the calling thread.
    ///
    /// If the thread has finished execution, any allocated system resources will be released.
    ///
    /// @return  True if the thread finished or was not running to begin with, false if it is still running.
    bool Thread::TryJoin()
    {
        if( m_thread != 0 )
        {
            DWORD waitResult = WaitForSingleObject( reinterpret_cast< HANDLE >( m_thread ), 0 );
            L_ASSERT( waitResult == WAIT_OBJECT_0 || waitResult == WAIT_TIMEOUT );
            if( waitResult != WAIT_OBJECT_0 )
            {
                return false;
            }

            L_VERIFY( CloseHandle( reinterpret_cast< HANDLE >( m_thread ) ) );
            m_thread = 0;
        }

        return true;
    }

    /// Execute the thread code.
    ///
    /// This is called by the running thread once it has started.  By default, this calls the Runnable::Run() method of
    /// the Runnable instance if one exists.
    void Thread::Run()
    {
        if( m_pRunnable )
        {
            m_pRunnable->Run();
        }
    }

    /// Thread callback function.
    ///
    /// @param[in] pData  Callback data (in this case, the pointer to the Thread instance being run).
    ///
    /// @return  Thread result code (always zero).
    unsigned int __stdcall Thread::ThreadCallback( void* pData )
    {
        L_ASSERT( pData );

        Thread* pThread = static_cast< Thread* >( pData );
        pThread->Run();

        ThreadLocalStackAllocator::ReleaseMemoryHeap();
        DynamicMemoryHeap::UnregisterCurrentThreadCache();

        _endthreadex( 0 );

        return 0;
    }
}

#endif  // L_OS_WIN
