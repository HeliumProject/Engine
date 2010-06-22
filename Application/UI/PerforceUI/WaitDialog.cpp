#include "WaitDialog.h"

#include "Application/RCS/Providers/Perforce/Perforce.h"
#include "Application/Application.h"

using namespace PerforceUI;

WaitDialog::WaitDialog()
: wxDialog( NULL, wxID_ANY, wxT("Waiting for Perforce..."), wxDefaultPosition, wxSize( 260,80 ), wxCAPTION )
{
  m_Panel = new WaitPanel( this );
  m_Panel->m_Gauge->SetIndeterminateMode();
}

int WaitDialog::ShowModal( Perforce::WaitInterface* waitInterface )
{
  PerforceWaitTimer timer ( this, waitInterface );

  timer.Start( 100 );
  int result = __super::ShowModal();
  timer.Stop();

  return result;
}

void WaitDialog::Throb()
{
  m_Panel->m_Gauge->Pulse();
}

PerforceWaitTimer::PerforceWaitTimer( WaitDialog* dialog, Perforce::WaitInterface* waitInterface )
: wxTimer ( dialog )
, m_WaitDialog( dialog )
, m_WaitInterface( waitInterface )
{

}

void PerforceWaitTimer::Notify()
{
  if ( m_WaitInterface->StopWaiting() )
  {
    wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK );
    wxPostEvent( m_WaitDialog, event );
  }
  else
  {
    m_WaitDialog->Throb();

    int time = (int)m_WaitTimer.Elapsed();
    int milli = time % 1000; time /= 1000;
    int sec = time % 60; time /= 60;
    int min = time % 60; time /= 60;
    int hour = time;

    char buf[256];
    if (hour > 0)
    {
      sprintf(buf, "Waiting for Perforce... %d:%02d:%02d hours", hour, min, sec);
    }
    else
    {
      if (min > 0)
      {
        sprintf(buf, "Waiting for Perforce... %d:%02d minutes", min, sec);
      }
      else
      {
        if (sec > 0)
        {
          sprintf(buf, "Waiting for Perforce... %d seconds", sec);
        }
        else
        {
          sprintf(buf, "Waiting for Perforce...", sec);
        }
      }
    }

    m_WaitDialog->SetTitle(buf);
  }
}