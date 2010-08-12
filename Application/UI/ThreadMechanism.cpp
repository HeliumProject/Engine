#include "ThreadMechanism.h"

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/frame.h>

using namespace Helium;

namespace Helium
{
  /////////////////////////////////////////////////////////////////////////////
  /// DummyWindow
  /////////////////////////////////////////////////////////////////////////////
  static const tchar* s_DummyWindowName = TXT( "DummyWindowThread" );

  // Custom wxEventTypes for the Thread to fire.
  DEFINE_EVENT_TYPE( nocEVT_BEGIN_THREAD )
  DEFINE_EVENT_TYPE( nocEVT_UPDATE_THREAD )
  DEFINE_EVENT_TYPE( nocEVT_END_THREAD )

  class DummyWindow : public wxFrame
  {
  public:
    DummyWindow( const tchar* name = NULL )
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
  class Helium::DummyThread : public wxThread
  {
  private:
    ThreadMechanism* m_ThreadMechanism;
    i32 m_ThreadID;

  public:
    // Detached threads delete themselves once they have completed,
    // and thus must be created on the heap
    DummyThread( ThreadMechanism* threadMechanism, i32 id )
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
}


/////////////////////////////////////////////////////////////////////////////
/// ThreadMechanism
/////////////////////////////////////////////////////////////////////////////
ThreadMechanism::ThreadMechanism( const tstring& evenPrefix )
: m_StopThread( true )
, m_CurrentThreadID( -1 )
, m_DummyWindow( NULL )
{
  tstring eventName;
  eventName = evenPrefix + TXT( "BeginEvent" );
  m_ThreadInitializedEvent = ::CreateEvent( NULL, TRUE, TRUE, eventName.c_str() );
  
  eventName = evenPrefix + TXT( "EndEvent" );
  m_EndThreadEvent = ::CreateEvent( NULL, TRUE, TRUE, eventName.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
ThreadMechanism::~ThreadMechanism()
{
  // wait for thread to complete
  StopThread();
  ::CloseHandle( m_ThreadInitializedEvent );
  ::CloseHandle( m_EndThreadEvent );
}

///////////////////////////////////////////////////////////////////////////////
// Creates and starts the Thread
//
// Detached threads delete themselves once they have completed,
// and thus must be created on the heap
//
bool ThreadMechanism::StartThread()
{
  Helium::TakeMutex beginMutex( m_BeginThreadMutex );

  // kill current thread, if any
  StopThread();

  Helium::TakeMutex resultsMutex( m_UpdateMutex );

  // Initialize thread data here
  InitData();

  // reset event to lockout new threads from starting
  ::ResetEvent( m_ThreadInitializedEvent );
  m_StopThread = false;
  
  // increment the m_CurrentThreadID for the next thread
  ++m_CurrentThreadID;

  HELIUM_ASSERT( !m_DummyWindow );
  m_DummyWindow = new DummyWindow( TXT( "ThreadMechanism" ) );
  m_DummyWindow->Connect( m_DummyWindow->GetId(), nocEVT_BEGIN_THREAD, wxCommandEventHandler( DummyWindow::OnBeginThread ), NULL, m_DummyWindow );
  m_DummyWindow->Connect( m_DummyWindow->GetId(), nocEVT_UPDATE_THREAD, wxCommandEventHandler( DummyWindow::OnThreadUpdate ), NULL, m_DummyWindow );
  m_DummyWindow->Connect( m_DummyWindow->GetId(), nocEVT_END_THREAD, wxCommandEventHandler( DummyWindow::OnEndThread ), NULL, m_DummyWindow );

  m_DummyWindow->AddBeginListener( Helium::ThreadProcSignature::Delegate( this, &ThreadMechanism::OnBeginThread ) );
  m_DummyWindow->AddUpdateListener( Helium::ThreadProcSignature::Delegate( this, &ThreadMechanism::OnUpdateThread ) );
  m_DummyWindow->AddEndListener( Helium::ThreadProcSignature::Delegate( this, &ThreadMechanism::OnEndThread ) );

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
  ::WaitForSingleObject( m_ThreadInitializedEvent, INFINITE );

  // Stop and wait for the current thread
  m_StopThread = true;
  ::WaitForSingleObject( m_EndThreadEvent, INFINITE );

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
void ThreadMechanism::ThreadEnter( i32 threadID )
{
  ::ResetEvent( m_EndThreadEvent );

  wxCommandEvent evt( nocEVT_BEGIN_THREAD, m_DummyWindow->GetId() );
  evt.SetInt( threadID );
  wxPostEvent( m_DummyWindow, evt );

  // Main thread is deadlocked until Thread sets this event
  ::SetEvent( m_ThreadInitializedEvent );
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::ThreadPostResults( i32 threadID )
{
  wxCommandEvent evt( nocEVT_UPDATE_THREAD, m_DummyWindow->GetId() );
  evt.SetInt( threadID );
  wxPostEvent( m_DummyWindow, evt );
}

///////////////////////////////////////////////////////////////////////////////
bool ThreadMechanism::CheckThreadLeave( i32 threadID )
{
  if ( m_StopThread || !IsCurrentThread( threadID ) )
  {
    ThreadLeave( threadID );
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::ThreadLeave( i32 threadID )
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
  ::SetEvent( m_EndThreadEvent );
}


///////////////////////////////////////////////////////////////////////////////
//
// Main Thread Callback Functions
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Main thread callbacks to notify listeners when the thread has started,
// when results are available for use and when the thread has completed
void ThreadMechanism::OnBeginThread( const Helium::ThreadProcArgs& args )
{
  if ( !IsCurrentThread( args.m_ThreadID ) )
    return;
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::OnUpdateThread( const Helium::ThreadProcArgs& args )
{
  if ( !IsCurrentThread( args.m_ThreadID ) )
    return;
}

///////////////////////////////////////////////////////////////////////////////
void ThreadMechanism::OnEndThread( const Helium::ThreadProcArgs& args )
{
  if ( !IsCurrentThread( args.m_ThreadID ) )
    return;
}

