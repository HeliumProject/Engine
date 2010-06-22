#pragma once

#include "Panels.h"

#include "Foundation/Profile.h"
#include "Application/RCS/Providers/Perforce/P4Provider.h"

#include <wx/dialog.h>
#include <wx/timer.h>

namespace PerforceUI
{
  typedef bool (*CancelWaitFunc)();

  class WaitDialog : public wxDialog
  {
  public:
    WaitDialog();

    int ShowModal( Perforce::WaitInterface* waitInterface );
    void Throb();

  private:
    WaitPanel* m_Panel;
  };

  class PerforceWaitTimer : public wxTimer
  {
  public:
    PerforceWaitTimer( class WaitDialog* dialog, Perforce::WaitInterface* waitInterface );

  protected:
    void Notify();

  private:
    WaitDialog*               m_WaitDialog;
    Perforce::WaitInterface*  m_WaitInterface;
    Profile::Timer            m_WaitTimer;
  };
}