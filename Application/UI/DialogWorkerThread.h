#pragma once

#include "Application/API.h"

#include "Platform/Thread.h"

#include <wx/thread.h>
#include <wx/dialog.h>

namespace Nocturnal
{
  /////////////////////////////////////////////////////////////////////////////
  // Abstract class for defining a worker thread that will process while a 
  // modal dialog is being shown.  Derived classes should implement the 
  // DoWork() function to carry out their processing.
  // 
  class APPLICATION_API DialogWorkerThread : public wxThread
  {
  private:
    wxDialog* m_Dialog;
    Platform::Thread::Handle m_EventHandle;

  public:
    DialogWorkerThread( Platform::Thread::Handle evtHandle, wxDialog* dlg );

    Platform::Thread::Handle GetEventHandle() const;

    wxThread::ExitCode Entry();

    virtual void DoWork() = 0;
  };
}
