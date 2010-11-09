#include "TimerThread.h"

#include "Platform/PlatformUtility.h"

using namespace Helium;

TimerThread::~TimerThread()
{
    // m_TimerThreadArgs are cleaned up when the thread stops

    if ( m_Alive )
    {
        Stop();
    }
}


void TimerThread::ThreadEntryPoint( TimerThreadArgs& args )
{
    HELIUM_ASSERT( args.m_TimerInstance != NULL );
    HELIUM_ASSERT( args.m_Interval != -1 );

    while( args.m_TimerInstance->IsAlive() )
    {
        Helium::Sleep( args.m_Interval );
        args.m_TimerInstance->Fire();
    }
}

void TimerThread::Start()
{
    HELIUM_ASSERT( !m_Thread.IsRunning() );

    // create thread
    m_Alive = true;

    TimerThreadArgs* args = new TimerThreadArgs( this, m_Interval );
    Helium::CallbackThread::Entry entry = Helium::CallbackThread::EntryHelperWithArgs< TimerThread, TimerThreadArgs, &TimerThread::ThreadEntryPoint >;
    m_Thread.CreateWithArgs( entry, this, args, m_Name.c_str() );
    m_Timer.Reset();
}

void TimerThread::Stop()
{
    HELIUM_ASSERT( m_Alive );

    m_Alive = false;

    // destroy thread
    m_Thread.Join();
}

void TimerThread::Fire()
{
    TimerTickArgs args( m_Timer.Elapsed() );
    m_Timer.Reset();

    m_TimerTickEvent.Raise( args );

    if ( m_SingleShot )
    {
        m_Alive = false;
    }
}
