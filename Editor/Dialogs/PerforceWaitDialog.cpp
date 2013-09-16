#include "EditorPch.h"
#include "PerforceWaitDialog.h"

#include "Editor/Perforce/Perforce.h"
#include "Application/Startup.h"

#include <wx/msgdlg.h>

using namespace Helium;
using namespace Helium::Editor;

static void ShowWaitDialog( Perforce::WaitArgs& args )
{
    args.m_Cancel = Editor::PerforceWaitDialog().ShowModal( args.m_WaitInterface ) != wxID_OK;
}

static void ShowWarningDialog(const Perforce::MessageArgs& args )
{
    wxMessageBox( args.m_Message.c_str(), args.m_Title.c_str(), wxOK | wxICON_EXCLAMATION );
}

void PerforceWaitDialog::EnableWaitDialog( bool enable )
{
    if ( enable )
    {
        Perforce::g_ShowWaitDialog.Set( &ShowWaitDialog );
        Perforce::g_ShowWarningDialog.Set( &ShowWarningDialog );
    }
    else
    {
        Perforce::g_ShowWaitDialog.Clear();
        Perforce::g_ShowWarningDialog.Clear();
    }
}

PerforceWaitDialog::PerforceWaitDialog()
: wxDialog( NULL, wxID_ANY, wxT("Waiting for Perforce..."), wxDefaultPosition, wxSize( 260,80 ), wxCAPTION )
{
    m_Panel = new PerforceWaitPanelGenerated( this );
}

int PerforceWaitDialog::ShowModal( Perforce::WaitInterface* waitInterface )
{
    PerforceWaitTimer timer ( this, waitInterface );

    timer.Start( 100 );
    int result = wxDialog::ShowModal();
    timer.Stop();

    return result;
}

void PerforceWaitDialog::Throb()
{
    m_Panel->m_Gauge->Pulse();
}

PerforceWaitTimer::PerforceWaitTimer( PerforceWaitDialog* dialog, Perforce::WaitInterface* waitInterface )
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
            StringPrint(buf, TXT( "Waiting for Perforce... %d:%02d:%02d hours" ), hour, min, sec);
        }
        else
        {
            if (min > 0)
            {
                StringPrint(buf, TXT( "Waiting for Perforce... %d:%02d minutes" ), min, sec);
            }
            else
            {
                if (sec > 0)
                {
                    StringPrint(buf, TXT( "Waiting for Perforce... %d seconds" ), sec);
                }
                else
                {
                    StringPrint(buf, TXT( "Waiting for Perforce..." ), sec);
                }
            }
        }

        m_WaitDialog->SetTitle(buf);
    }
}