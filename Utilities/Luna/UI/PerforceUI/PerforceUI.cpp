#include "Precompile.h"
#include "PerforceUI.h"
#include "WaitDialog.h"

#include "Perforce/Perforce.h"

bool ShowWaitDialog(Perforce::WaitInterface* waitInterface)
{
  int result = PerforceUI::WaitDialog().ShowModal( waitInterface );

  return result != wxID_OK;
}

void ShowWarningDialog(const Perforce::MessageArgs& args )
{
  wxMessageBox( args.m_Message.c_str(), args.m_Title.c_str(), wxOK | wxICON_EXCLAMATION );
}

static u32 g_InitCount = 0;

void PerforceUI::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    Perforce::g_ShowWaitDialog.Set( &ShowWaitDialog );
    Perforce::g_ShowWarningDialog.Set( &ShowWarningDialog );
  }
}

void PerforceUI::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    Perforce::g_ShowWaitDialog.Clear();
    Perforce::g_ShowWarningDialog.Clear();
  }
}