#include "TimerThread.h"

#include "Platform/Platform.h"

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
    HELIUM_ASSERT( !m_Thread.Valid() || !m_Thread.Running() );

    // create thread
    m_Alive = true;

    if ( !m_ThreadArgs )
    {
        m_ThreadArgs = new TimerThreadArgs();
    }

    m_ThreadArgs->m_TimerInstance = this;
    m_ThreadArgs->m_Interval = m_Interval;
    m_Thread.CreateWithArgs( Helium::Thread::EntryHelperWithArgs< TimerThread, TimerThreadArgs, &TimerThread::ThreadEntryPoint >, this, m_ThreadArgs, m_Name.c_str() );

    m_Timer.Reset();
}

void TimerThread::Stop()
{
    HELIUM_ASSERT( m_Alive );

    m_Alive = false;

    // destroy thread
    if ( m_Thread.Valid() && m_Thread.Running() )
    {
        m_Thread.Wait();
        m_Thread.Close();
    }
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
