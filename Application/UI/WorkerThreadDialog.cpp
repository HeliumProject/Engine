#include "WorkerThreadDialog.h"
#include "DialogWorkerThread.h"
#include "Foundation/Exception.h"
#include "Platform/Windows/Windows.h"

using namespace Nocturnal;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
WorkerThreadDialog::WorkerThreadDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxDialog( parent, id, title, pos, size, style, name )
, m_Thread( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
WorkerThreadDialog::~WorkerThreadDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
// Pass in the pointer to a new worker thread that will run when this dialog
// is shown.  Call this function only once and you must call this function
// before calling ShowModal.
// 
void WorkerThreadDialog::InitThread( DialogWorkerThread* thread )
{
  if ( m_Thread )
  {
    throw Nocturnal::Exception( "You cannot change threads on a WorkerThreadDialog." );
  }

  m_Thread = thread;
}

///////////////////////////////////////////////////////////////////////////////
// You must call InitThread() before calling this function.  Shows the dialog,
// runs the worker thread, and waits for the worker thread to end the dialog 
// loop.
// 
int WorkerThreadDialog::ShowModal()
{
  if ( !m_Thread )
  {
    throw Nocturnal::Exception( "You must call InitThread before calling WorkerThreadDialog::ShowModal." );
  }

  m_Thread->Create();
  m_Thread->Run();
  SetEvent( m_Thread->GetEventHandle() );

  return __super::ShowModal();
}
