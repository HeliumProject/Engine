#include "DialogWorkerThread.h"

#include "Platform/Windows/Windows.h"

using namespace Nocturnal;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// Pass in the dialog that will be shown while this thread is working, and an
// event handle that will be signaled when the caller is ready to continue.
// 
DialogWorkerThread::DialogWorkerThread( HANDLE evtHandle, wxDialog* dlg )
: m_EventHandle( evtHandle )
, m_Dialog( dlg )
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the event handle that this thread uses to synchronize.
// 
HANDLE DialogWorkerThread::GetEventHandle() const
{
  return m_EventHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Callback to enter the thread.  Uses virtual DoWork function to allow
// derived classes to carry out their processing.
// 
wxThread::ExitCode DialogWorkerThread::Entry()
{
  // Derived classes will do their work here.
  DoWork();

  // Wait for the caller to synchronize.
  WaitForSingleObject( m_EventHandle, INFINITE );
  CloseHandle( m_EventHandle );

  // Notify our dialog that it needs to close.
  wxCloseEvent close( wxEVT_CLOSE_WINDOW );
  m_Dialog->GetEventHandler()->AddPendingEvent( close );

  return NULL;
}
