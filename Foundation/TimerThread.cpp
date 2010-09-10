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

    TimerThreadArgs* args = new TimerThreadArgs( this, m_Interval );
    Helium::Thread::Entry entry = Helium::Thread::EntryHelperWithArgs< TimerThread, TimerThreadArgs, &TimerThread::ThreadEntryPoint >;
    m_Thread.CreateWithArgs( entry, this, args, m_Name.c_str() );
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
