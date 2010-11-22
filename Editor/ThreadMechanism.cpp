#include "Precompile.h"
#include "ThreadMechanism.h"

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/frame.h>

using namespace Helium;
using namespace Helium::Editor;

/////////////////////////////////////////////////////////////////////////////
/// DummyWindow
/////////////////////////////////////////////////////////////////////////////
static const tchar_t* s_DummyWindowName = TXT( "DummyWindowThread" );

// Custom wxEventTypes for the Thread to fire.
DEFINE_EVENT_TYPE( nocEVT_BEGIN_THREAD )
DEFINE_EVENT_TYPE( nocEVT_UPDATE_THREAD )
DEFINE_EVENT_TYPE( nocEVT_END_THREAD )

class DummyWindow : public wxFrame
{
public:
    DummyWindow( const tchar_t* name = NULL )
        : wxFrame( NULL, wxID_ANY, s_DummyWindowName, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, s_DummyWindowName )
    {
        Hide();

        if ( name )
        {
            wxString newName( s_DummyWindowName );
            newName += TXT( "-" );
            newName += name;
            SetName( newName );
            SetTitle( newName );
        }
    }

    virtual ~DummyWindow()
    {

    }

    void OnBeginThread( wxCommandEvent& evt )
    {
        m_BeginListeners.Raise( ThreadProcArgs( evt.GetInt() ) );
    }

    void OnThreadUpdate( wxCommandEvent& evt )
    {
        m_UpdateListeners.Raise( ThreadProcArgs( evt.GetInt() ) );
    }

    void OnEndThread( wxCommandEvent& evt )
    {
        m_EndListeners.Raise( ThreadProcArgs( evt.GetInt() ) ); 
    }

    // 
    // Events
    //
private:
    ThreadProcSignature::Event m_BeginListeners;
public:
    void AddBeginListener( const ThreadProcSignature::Delegate& listener )
    {
        m_BeginListeners.Add( listener );
    }
    void RemoveBeginListener( const ThreadProcSignature::Delegate& listener )
    {
        m_BeginListeners.Remove( listener );
    }

private:
    ThreadProcSignature::Event m_UpdateListeners;
public:
    void AddUpdateListener( const ThreadProcSignature::Delegate& listener )
    {
        m_UpdateListeners.Add( listener );
    }
    void RemoveUpdateListener( const ThreadProcSignature::Delegate& listener )
    {
        m_UpdateListeners.Remove( listener );
    }

private:
    ThreadProcSignature::Event m_EndListeners;
public:
    void AddEndListener( const ThreadProcSignature::Delegate& listener )
    {
        m_EndListeners.Add( listener );
    }
    void RemoveEndListener( const ThreadProcSignature::Delegate& listener )
    {
        m_EndListeners.Remove( listener );
    }

};


/////////////////////////////////////////////////////////////////////////////
/// DummyThread
/////////////////////////////////////////////////////////////////////////////
class DummyThread : public wxThread
{
private:
    ThreadMechanism* m_ThreadMechanism;
    int32_t m_ThreadID;

public:
    // Detached threads delete themselves once they have completed,
    // and thus must be created on the heap
    DummyThread( ThreadMechanism* threadMechanism, int32_t id )
        : wxThread( wxTHREAD_DETACHED )
        , m_ThreadMechanism( threadMechanism )
        , m_ThreadID( id )
    {
    }

    virtual ~DummyThread()
    {
    }

    virtual wxThread::ExitCode Entry()
    {
        m_ThreadMechanism->ThreadProc( m_ThreadID );

        return NULL;
    }
};


/////////////////////////////////////////////////////////////////////////////
/// ThreadMechanism
/////////////////////////////////////////////////////////////////////////////
ThreadMechanism::ThreadMechanism( const tstring& evenPrefix )
: m_StopThread( true )
, m_CurrentThreadID( -1 )
, m_DummyWindow( NULL )
, m_ThreadInitializedEvent( Condition::RESET_MODE_MANUAL, true )
, m_EndThreadEvent( Condition::RESET_MODE_MANUAL, true )
{
    //tstring eventName;
    //eventName = evenPrefix + TXT( "BeginEvent" );
    //m_ThreadInitializedEvent = ::CreateEvent( NULL, TRUE, TRUE, eventName.c_str() );

    //eventName = evenPrefix + TXT( "EndEvent" );
    //m_EndThreadEvent = ::CreateEvent( NULL, TRUE, TRUE, eventName.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
ThreadMechanism::~ThreadMechanism()
{
    // wait for thread to complete
    StopThread();
}

///////////////////////////////////////////////////////////////////////////////
// Creates and starts the Thread
//
// Detached threads delete themselves once they have completed,
// and thus must be created on the heap
//
bool ThreadMechanism::StartThread()
{
    Helium::MutexScopeLock beginMutex( m_BeginThreadMutex );

    // kill current thread, if any
    StopThread();

    Helium::MutexScopeLock resultsMutex( m_UpdateMutex );

    // Initialize thread data here
    InitData();

    // reset event to lockout new threads from starting
    m_ThreadInitializedEvent.Reset();
    m_StopThread = false;

    // increment the m_CurrentThreadID for the next thread
    ++m_CurrentThreadID;

    HELIUM_ASSERT( !m_DummyWindow );
    m_DummyWindow = new DummyWindow( TXT( "ThreadMechanism" ) );
    m_DummyWindow->Connect( m_DummyWindow->GetId(), nocEVT_BEGIN_THREAD, wxCommandEventHandler( DummyWindow::OnBeginThread ), NULL, m_DummyWindow );
    m_DummyWindow->Connect( m_DummyWindow->GetId(), nocEVT_UPDATE_THREAD, wxCommandEventHandler( DummyWindow::OnThreadUpdate ), NULL, m_DummyWindow );
    m_DummyWindow->Connect( m_DummyWindow->GetId(), nocEVT_END_THREAD, wxCommandEventHandler( DummyWindow::OnEndThread ), NULL, m_DummyWindow );

    m_DummyWindow->AddBeginListener( ThreadProcSignature::Delegate( this, &ThreadMechanism::OnBeginThread ) );
    m_DummyWindow->AddUpdateListener( ThreadProcSignature::Delegate( this, &ThreadMechanism::OnUpdateThread ) );
    m_DummyWindow->AddEndListener( ThreadProcSignature::Delegate( this, &ThreadMechanism::OnEndThread ) );

    // start the thread
    DummyThread* dummyThread = new DummyThread( this, GetCurrentThreadID() );
    dummyThread->Create();
    dummyThread->Run();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Something has requested the thread to stop; possibly
// a new thread is ready to be run, stop the old and start
// the new.
void ThreadMechanism::StopThread()
{
    // can't cancel a thread until this is initialized and m_ThreadInitializedEvent is set
    m_ThreadInitializedEvent.Wait();

    // Stop and wait for the current thread
    m_StopThread = true;
    m_EndThreadEvent.Wait();

    if ( m_DummyWindow )
    {
        m_DummyWindow->Destroy();
        m_DummyWindow = NULL;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// ThreadProc Helper Functions - Called from the Thread
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// ThreadProc Helper Functions
// Used in ThreadProc() to create begin and end events
//
void ThreadMechanism::ThreadEnter( int32_t threadID )
{
    m_EndThreadEvent.Reset();

    wxCommandEvent evt( nocEVT_BEGIN_THREAD, m_DummyWindow->GetId() );
    evt.SetInt( threadID );
    wxPostEvent( m_DummyWindow, evt );

    // Main thread is deadlocked until Thread sets this event
    m_ThreadInitializedEvent.Signal();
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::ThreadPostResults( int32_t threadID )
{
    wxCommandEvent evt( nocEVT_UPDATE_THREAD, m_DummyWindow->GetId() );
    evt.SetInt( threadID );
    wxPostEvent( m_DummyWindow, evt );
}

///////////////////////////////////////////////////////////////////////////////
bool ThreadMechanism::CheckThreadLeave( int32_t threadID )
{
    if ( m_StopThread || !IsCurrentThread( threadID ) )
    {
        ThreadLeave( threadID );
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::ThreadLeave( int32_t threadID )
{
    ThreadPostResults( threadID );

    m_StopThread = true;

    wxCommandEvent evt( nocEVT_END_THREAD, m_DummyWindow->GetId() );
    evt.SetInt( threadID );
    wxPostEvent( m_DummyWindow, evt );
    if ( m_DummyWindow )
    {
        m_DummyWindow->Destroy();
        m_DummyWindow = NULL;
    }

    // Main thread is deadlocked until Thread sets this event
    m_EndThreadEvent.Signal();
}


///////////////////////////////////////////////////////////////////////////////
//
// Main Thread Callback Functions
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Main thread callbacks to notify listeners when the thread has started,
// when results are available for use and when the thread has completed
void ThreadMechanism::OnBeginThread( const ThreadProcArgs& args )
{
    if ( !IsCurrentThread( args.m_ThreadID ) )
        return;
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::OnUpdateThread( const ThreadProcArgs& args )
{
    if ( !IsCurrentThread( args.m_ThreadID ) )
        return;
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::OnEndThread( const ThreadProcArgs& args )
{
    if ( !IsCurrentThread( args.m_ThreadID ) )
        return;
}

