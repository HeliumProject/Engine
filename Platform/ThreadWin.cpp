#include "PlatformPch.h"
#include "Thread.h"

#include "Platform/Assert.h"
#include "Platform/Error.h"
#include "Platform/Print.h"
#include "Platform/Memory.h"
#include "Platform/String.h"

#include <process.h>

using namespace Helium;

// Mapping from Helium::Thread priority values to Win32 thread priority values.
static const int WIN32_THREAD_PRIORITY_MAP[] =
{
    THREAD_PRIORITY_LOWEST,        // PRIORITY_LOWEST
    THREAD_PRIORITY_BELOW_NORMAL,  // PRIORITY_LOW
    THREAD_PRIORITY_NORMAL,        // PRIORITY_NORMAL
    THREAD_PRIORITY_ABOVE_NORMAL,  // PRIORITY_HIGH
    THREAD_PRIORITY_HIGHEST,       // PRIORITY_HIGHEST
};

HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( WIN32_THREAD_PRIORITY_MAP ) == Thread::PRIORITY_MAX );

// Thread name assignment exception information.
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

/// Constructor.
///
/// @param[in] rName  Optional name to assign to the thread (for debugging purposes).
Thread::Thread( const tchar_t* pName )
    : m_Handle( 0 )
    , m_Name( NULL )
{
    SetName( pName );
}

/// Destructor.
Thread::~Thread()
{
    HELIUM_VERIFY( Join() );

    delete [] m_Name;
}

/// Set the thread name (for debugging purposes).
///
/// Note that the thread name can only be set while the thread is not running.
///
/// @param[in] pName  Name to set.
///
/// @see GetName()
void Thread::SetName( const tchar_t* pName )
{
    HELIUM_ASSERT( m_Handle == 0 );

    delete [] m_Name;
    m_Name = NULL;

    if( pName && pName[ 0 ] != TXT( '\0' ) )
    {
        size_t nameArraySize = StringLength( pName ) + 1;
        m_Name = new tchar_t [ nameArraySize ];
        MemoryCopy( m_Name, pName, nameArraySize * sizeof( tchar_t ) );
    }
}

/// Get the name assigned to this thread.
///
/// @return  Thread name.
///
/// @see SetName()
const tchar_t* Thread::GetName() const
{
    return m_Name;
}

/// Begin execution of this thread.
///
/// If a thread has been started and not yet joined, this will fail.
///
/// @param[in] priority  Thread priority.
///
/// @return  True if the thread was started successfully, false if not.
///
/// @see Join(), TryJoin()
bool Thread::Start( EPriority priority )
{
    HELIUM_ASSERT( static_cast< size_t >( priority ) < PRIORITY_MAX );

    // Make sure a thread hasn't already been started.
    HELIUM_ASSERT( m_Handle == 0 );
    if( m_Handle != 0 )
    {
        return false;
    }

    // Create the thread, but don't launch it yet (we want to set the thread name prior to the thread actually
    // starting).
    uint32_t threadId;
    m_Handle = _beginthreadex( NULL, 0, ThreadCallback, this, CREATE_SUSPENDED, &threadId );
    HELIUM_ASSERT( m_Handle != 0 );
    if( m_Handle != 0 )
    {
        // Assign the thread name.
        if( m_Name && m_Name[ 0 ] != TXT( '\0' ) )
        {
            size_t charCount = StringLength( m_Name );

            ThreadNameInfo nameInfo;
            nameInfo.dwType = 0x1000;
            nameInfo.dwThreadId = threadId;
            nameInfo.dwFlags = 0;

#if HELIUM_WCHAR_T
            char name[ 128 ];
            wcstombs_s( NULL, name, m_Name, _TRUNCATE );
            nameInfo.szName = name;
#else
            nameInfo.szName = m_Name;
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

        // Set the thread priority.
        int win32Priority = WIN32_THREAD_PRIORITY_MAP[ priority ];
        BOOL priorityResult = SetThreadPriority( reinterpret_cast< HANDLE >( m_Handle ), win32Priority );
        HELIUM_ASSERT( priorityResult );
        HELIUM_UNREF( priorityResult );

        // Start the thread.
        DWORD resumeResult = ResumeThread( reinterpret_cast< HANDLE >( m_Handle ) );
        HELIUM_ASSERT( resumeResult != static_cast< DWORD >( -1 ) );
        HELIUM_UNREF( resumeResult );
    }

    return( m_Handle != 0 );
}

/// Wait for this thread to finish execution and release any allocated system resources.
///
/// @param[in] timeOutMilliseconds  Maximum time to wait for the thread to finish, or zero to wait indefinitely.
///
/// @return  True if the thread finished or was not running to begin with, false if it is still running.
bool Thread::Join( uint32_t timeOutMilliseconds )
{
    if( m_Handle != 0 )
    {
        DWORD waitResult = WaitForSingleObject(
            reinterpret_cast< HANDLE >( m_Handle ),
            ( timeOutMilliseconds != 0 ? timeOutMilliseconds : INFINITE ) );
        HELIUM_ASSERT( waitResult == WAIT_OBJECT_0 || waitResult == WAIT_TIMEOUT );
        if( waitResult != WAIT_OBJECT_0 )
        {
            return false;
        }

        HELIUM_VERIFY( CloseHandle( reinterpret_cast< HANDLE >( m_Handle ) ) );
        m_Handle = 0;
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
    if( m_Handle != 0 )
    {
        DWORD waitResult = WaitForSingleObject( reinterpret_cast< HANDLE >( m_Handle ), 0 );
        HELIUM_ASSERT( waitResult == WAIT_OBJECT_0 || waitResult == WAIT_TIMEOUT );
        if( waitResult != WAIT_OBJECT_0 )
        {
            return false;
        }

        HELIUM_VERIFY( CloseHandle( reinterpret_cast< HANDLE >( m_Handle ) ) );
        m_Handle = 0;
    }

    return true;
}

/// Get whether this thread is active.
///
/// A thread is active if it has been successfully started using Start() but has not yet been joined using Join() or
/// TryJoin().
///
/// An active thread does not necessarily indicate active execution.  The thread may have completed work, but its
/// resources are not released until the thread is joined.  Manually managed status flags should be used by threads to
/// indicate active execution.
///
/// @return  True if this thread is active (started at some point but not yet joined), false if not.
bool Thread::IsRunning() const
{
    return ( m_Handle != 0 );
}


/// @fn void Helium::Thread::Run()
/// Execute the thread code.

/// Suspend execution of the calling thread for at least a given period of time.
///
/// @param[in] milliseconds  Number of milliseconds to sleep.  Note that the calling thread may not be awaken
///                          immediately upon the timer expiring.
///
/// @see Yield()
void Thread::Sleep( uint32_t milliseconds )
{
    ::Sleep( milliseconds );
}

/// Yield the remainder of the calling thread's time slice for other threads of equal priority.
///
/// If other threads of equal priority are awaiting execution, this will immediately yield execution to those
/// threads.  If no other threads of equal priority are waiting, the thread will continue execution immediately.
///
/// To yield control to lower-priority threads, Sleep() should be called instead with a non-zero amount of time.
///
/// @see Sleep()
void Thread::Yield()
{
    ::Sleep( 0 );
}

/// Get the ID of the thread in which this function is called.
///
/// @return  Current thread ID.
Thread::id_t Thread::GetCurrentId()
{
    return GetCurrentThreadId();
}

/// Thread callback function.
///
/// @param[in] pData  Callback data (in this case, the pointer to the Thread instance being run).
///
/// @return  Thread result code (always zero).
unsigned int __stdcall Thread::ThreadCallback( void* pData )
{
    HELIUM_ASSERT( pData );

    Thread* pThread = static_cast< Thread* >( pData );
    pThread->Run();

    ThreadLocalStackAllocator::ReleaseMemoryHeap();
    DynamicMemoryHeap::UnregisterCurrentThreadCache();

    _endthreadex( 0 );

    return 0;
}

ThreadLocalPointer::ThreadLocalPointer()
{
    m_Key = TlsAlloc(); 
    HELIUM_ASSERT(m_Key != TLS_OUT_OF_INDEXES);
    SetPointer(NULL); 
}

ThreadLocalPointer::~ThreadLocalPointer()
{
    TlsFree(m_Key); 
}

void* ThreadLocalPointer::GetPointer() const
{
    void* value = TlsGetValue(m_Key);
    return value;
}

void ThreadLocalPointer::SetPointer(void* pointer)
{
    TlsSetValue(m_Key, pointer); 
}

uint32_t g_MainThreadID = (uint32_t)::GetCurrentThreadId();

uint32_t Helium::GetMainThreadID()
{
    return g_MainThreadID;
}

uint32_t Helium::GetCurrentThreadID()
{
    return (uint32_t)::GetCurrentThreadId();
}
