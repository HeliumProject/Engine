#include "Precompile.h"

#include "LiveFrame.h"
#include "LiveGenerated.h"
#include "LiveManager.h"

#include "Common/Exception.h"
#include "Console/Console.h"
#include "TargetManager/TargetManager.h"
#include "UIToolKit/ImageManager.h"
#include "Windows/Process.h"
#include "File/Manager.h"
#include "Common/Boost/Regex.h"
#include "boost/tokenizer.hpp"
#include "AssetEditor/RemoteAsset.h"
#include "AssetEditor/AssetEditor.h"
#include "Editor/SessionManager.h"
#include "FileSystem/FileSystem.h"
#include <algorithm>

using namespace Luna;

BEGIN_EVENT_TABLE(LiveFrame, wxFrame)
EVT_CLOSE( LiveFrame::OnCloseWindow )
END_EVENT_TABLE()

LiveFrame::LiveFrame(wxWindow *parent, wxWindowID id, const wxString& title, 
                       const wxPoint& pos, const wxSize& size,
                       long style, const wxString& name)
                       : wxFrame( parent, id, title, pos, size, style, name )
                       
{
  wxIconBundle iconBundle;
  wxIcon tempIcon;
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "live_64.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "live_32.png" ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "live_16.png" ) );
  iconBundle.AddIcon( tempIcon );
  SetIcons( iconBundle );

  m_LivePanel = new LivePanel( this );
  m_FrameManager.AddPane( m_LivePanel, wxAuiPaneInfo().Name( wxT( "main" ) ).DestroyOnClose( false ).Caption( wxT( "Live" ) ).CenterPane() );

  PopulateTargetList( Manager::GetInstance()->GetDefaultTarget() );

  Connect( m_LivePanel->m_ButtonRefreshTargetList->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveFrame::OnRefreshTargetList ) );
  Connect( m_LivePanel->m_ButtonStartUberview->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveFrame::OnStartViewer ) );
  Connect( m_LivePanel->m_ButtonResetConnection->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveFrame::OnResetConnection ) );
  Connect( m_LivePanel->m_AutoStartUberviewCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LiveFrame::OnAutoStartViewer ) );
  Connect( m_LivePanel->m_ForceAutoStartUberviewCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LiveFrame::OnForceAutoStartViewer ) );
  Connect( m_LivePanel->m_AutoDisplayWindowCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LiveFrame::OnAutoOpenFrame ) );
  Connect( m_LivePanel->m_TargetList->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LiveFrame::OnTargetSelection ) );
  Connect( m_LivePanel->m_listbook1->GetId(),  wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler( LiveFrame::OnLightingSelected ) );
  Connect( m_LivePanel->m_LightingEnvironmentList->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LiveFrame::OnLightingEnvironmentSelected ) );
  Connect( m_LivePanel->m_FilterText->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( LiveFrame::OnFilterLightingList ) );

  ConnectionStatusChanged( RuntimeConnectionStatusArgs( RuntimeConnection::IsConnected() ) );

  SetAutoOpenFrame( Manager::GetInstance()->GetAutoOpenFrame() );
  SetAutoStartViewer( Manager::GetInstance()->GetAutoStartViewer() );
  SetForceAutoStartViewer( Manager::GetInstance()->GetForceAutoStartViewer() );

  RuntimeConnection::AddRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &LiveFrame::ConnectionStatusChanged ) );
}

LiveFrame::~LiveFrame()
{
  RuntimeConnection::RemoveRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &LiveFrame::ConnectionStatusChanged ) );

  Disconnect( m_LivePanel->m_ButtonRefreshTargetList->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveFrame::OnRefreshTargetList ) );
  Disconnect( m_LivePanel->m_ButtonStartUberview->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveFrame::OnStartViewer ) );
  Disconnect( m_LivePanel->m_ButtonResetConnection->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveFrame::OnResetConnection ) );
  Disconnect( m_LivePanel->m_AutoStartUberviewCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LiveFrame::OnAutoStartViewer ) );
  Disconnect( m_LivePanel->m_ForceAutoStartUberviewCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LiveFrame::OnForceAutoStartViewer ) );
  Disconnect( m_LivePanel->m_AutoDisplayWindowCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LiveFrame::OnAutoOpenFrame ) );
  Disconnect( m_LivePanel->m_TargetList->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LiveFrame::OnTargetSelection ) );
  Disconnect( m_LivePanel->m_listbook1->GetId(),  wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED,  wxListbookEventHandler( LiveFrame::OnLightingSelected ) );
  Disconnect( m_LivePanel->m_LightingEnvironmentList->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LiveFrame::OnLightingEnvironmentSelected ) );
  Disconnect( m_LivePanel->m_FilterText->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( LiveFrame::OnFilterLightingList ) );

  delete m_LivePanel;
}

void LiveFrame::PopulateTargetList( const std::string& toSelect )
{
  Manager::GetInstance()->RefreshTargets();

  const TargetManager::V_Target& targets = Manager::GetInstance()->GetTargets();

  m_LivePanel->m_TargetList->Clear();

  if ( targets.empty() )
  {
    return;
  }

  for each ( const TargetManager::TargetPtr& target in targets )
  {
    std::string name = target->GetName();
    i32 index = m_LivePanel->m_TargetList->Append( target->GetName() );

    if ( name == toSelect )
    {
      m_LivePanel->m_TargetList->Select( index );
    }
  }
}



void LiveFrame::SetAutoStartViewer( bool enable )
{
  m_LivePanel->m_AutoStartUberviewCheckbox->SetValue( enable );
}

void LiveFrame::SetForceAutoStartViewer( bool enable )
{
  m_LivePanel->m_ForceAutoStartUberviewCheckbox->SetValue( enable );
}

void LiveFrame::SetAutoOpenFrame( bool enable )
{
  m_LivePanel->m_AutoDisplayWindowCheckbox->SetValue( enable );
}

void LiveFrame::SetLastBackTrace( const std::string& backtrace )
{
  m_LivePanel->m_BackTraceText->SetValue( backtrace );
}

void LiveFrame::ConnectionStatusChanged( const RuntimeConnectionStatusArgs& args )
{
  m_LivePanel->m_StatusTextControl->Clear();
  if ( args.m_Connected )
  {
    m_LivePanel->m_StatusTextControl->AppendText( "Connected" );
  }
  else
  {
    m_LivePanel->m_StatusTextControl->AppendText( "Not Connected" );
  }
}

void LiveFrame::OnCloseWindow( wxCloseEvent& evt )
{
  Manager::GetInstance()->LiveFrameClosed();
  Destroy();
}

void LiveFrame::OnRefreshTargetList( wxCommandEvent& evt )
{
  std::string selection = m_LivePanel->m_TargetList->GetStringSelection();
  PopulateTargetList( selection );
}

void LiveFrame::OnStartViewer( wxCommandEvent& evt )
{
  Manager::GetInstance()->StartViewer();
}

void LiveFrame::OnResetConnection( wxCommandEvent& evt )
{
  RuntimeConnection::ResetConnection();
}

void LiveFrame::OnAutoStartViewer( wxCommandEvent& evt )
{
  Manager::GetInstance()->SetAutoStartViewer( evt.IsChecked() );
}

void LiveFrame::OnForceAutoStartViewer( wxCommandEvent& evt )
{
  Manager::GetInstance()->SetForceAutoStartViewer( evt.IsChecked() );
}

void LiveFrame::OnAutoOpenFrame( wxCommandEvent& evt )
{
  Manager::GetInstance()->SetAutoOpenFrame( evt.IsChecked() );
}

void LiveFrame::OnTargetSelection( wxCommandEvent& evt )
{
  std::string name = m_LivePanel->m_TargetList->GetStringSelection();

  Manager::GetInstance()->SetDefaultTarget( name );
}

void LiveFrame::OnLightingSelected( wxListbookEvent& evt )
{
  m_LivePanel->m_LightingEnvironmentList->Clear();

  V_string levelFiles;

  std::string searchString = Finder::ProjectAssets() + "levels/";
  File::GlobalManager().Find( searchString + "*.level.irb", levelFiles );

  V_string::iterator itor = levelFiles.begin();
  V_string::iterator end  = levelFiles.end();

  sort( levelFiles.begin(), levelFiles.end() );

  static const boost::regex ignoreTestLevelPattern ( searchString + "test/" ); 
  static const boost::regex ignoreLegacyLevelPattern ( searchString + "legacy/" ); 
  boost::smatch matchResult;
  for( ; itor != end; ++itor )
  {
    if ( boost::regex_search( *itor, matchResult, ignoreTestLevelPattern ) || boost::regex_search( *itor, matchResult, ignoreLegacyLevelPattern ) )
    {
      continue;
    }
    std::string leaf = FileSystem::StripPrefix( searchString, itor->c_str() );
    wxString dummy( leaf.c_str() );
    m_LivePanel->m_LightingEnvironmentList->InsertItems( 1, &dummy, m_LivePanel->m_LightingEnvironmentList->GetCount() );
  }
}

void LiveFrame::OnLightingEnvironmentSelected( wxCommandEvent& evt )
{
  std::string selectedLevel = m_LivePanel->m_LightingEnvironmentList->GetStringSelection();
  selectedLevel = Finder::ProjectAssets() + "levels/" + selectedLevel;

  AssetEditor*  assetEditor = (AssetEditor*)(SessionManager::GetInstance()->LaunchEditor( EditorTypes::Asset ) );


  
  RemoteAsset* remoteAsset = assetEditor->GetRemoteInterface();

  remoteAsset->SetLightingEnvironment( selectedLevel );

}

void LiveFrame::OnFilterLightingList( wxCommandEvent& evt )
{
  m_LivePanel->m_LightingEnvironmentList->Clear();

  std::string searchString = Finder::ProjectAssets() + "levels/";

  V_string levelFiles;

  File::GlobalManager().Find( searchString + m_LivePanel->m_FilterText->GetValue().c_str() + "*.level.irb" , levelFiles );

  V_string::iterator itor = levelFiles.begin();
  V_string::iterator end  = levelFiles.end();

  sort( levelFiles.begin(), levelFiles.end() );

 

  for( ; itor != end; ++itor )
  {
    std::string stripped = FileSystem::StripPrefix( searchString, itor->c_str() );
    wxString dummy( stripped.c_str() );
    m_LivePanel->m_LightingEnvironmentList->InsertItems( 1, &dummy, m_LivePanel->m_LightingEnvironmentList->GetCount() );
  }

}