#include "PlatformPch.h"
#include "Platform/Thread.h"

using namespace Helium;

/// Destructor.
Runnable::~Runnable()
{
}

/// Constructor.
///
/// @param[in] pRunnable  Runnable instance to execute.
/// @param[in] rName      Optional name to assign to the thread (for debugging purposes).
RunnableThread::RunnableThread( Runnable* pRunnable, const tchar_t* pName )
    : Thread( pName )
    , m_pRunnable( pRunnable )
{
}

/// Destructor.
RunnableThread::~RunnableThread()
{
    HELIUM_VERIFY( Join() );
}

/// Set the runnable instance that this thread is set to execute.
///
/// Note that the runnable instance can only be set while the thread is not running.
///
/// @param[in] pRunnable  Runnable instance to execute.
void RunnableThread::SetRunnable( Runnable* pRunnable )
{
    HELIUM_ASSERT( !IsRunning() );
    m_pRunnable = pRunnable;
}

/// @copydoc Thread::Run()
void RunnableThread::Run()
{
    if( m_pRunnable )
    {
        m_pRunnable->Run();
    }
}

/// Constructor.
CallbackThread::CallbackThread()
    : m_Entry( NULL )
    , m_Object( NULL )
{
}

/// Destructor.
CallbackThread::~CallbackThread()
{
    HELIUM_VERIFY( Join() );
}

bool CallbackThread::Create(Entry entry, void* obj, const tchar_t* name, EPriority priority)
{
    HELIUM_ASSERT( entry );

    Join();

    m_Entry = entry;
    m_Object = obj;

    SetName( name );

    bool bResult = Start( priority );

    return bResult;
}

/// @copydoc Thread::Run()
void CallbackThread::Run()
{
    HELIUM_ASSERT( m_Entry );

    m_Entry( m_Object );
}
