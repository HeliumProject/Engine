#pragma once

#include "API.h"

namespace UIToolKit
{
  /////////////////////////////////////////////////////////////////////////////
  // Abstract class for defining a worker thread that will process while a 
  // modal dialog is being shown.  Derived classes should implement the 
  // DoWork() function to carry out their processing.
  // 
  class UITOOLKIT_API DialogWorkerThread NOC_ABSTRACT : public wxThread
  {
  private:
    wxDialog* m_Dialog;
    HANDLE m_EventHandle;

  public:
    DialogWorkerThread( HANDLE evtHandle, wxDialog* dlg );

    HANDLE GetEventHandle() const;

    wxThread::ExitCode Entry();

    virtual void DoWork() = 0;
  };
}
