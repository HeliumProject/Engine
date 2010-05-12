#include "Precompile.h"
#include "EditorChooser.h"

#include "Editor.h"
#include "EditorChooserInnerPanel.h"
#include "RunGameFrame.h"
#include "SessionManager.h"
#include "Browser/Browser.h"

#include "Asset/AssetClass.h"
#include "Asset/Tracker.h"
#include "Common/Assert.h"
#include "Console/Console.h"
#include "File/Manager.h"
#include "Finder/Finder.h"
#include "Live/LiveManager.h"
#include "Symbol/SymbolBuilder.h"
#include "UIToolKit/ImageManager.h"
#include "Windows/Process.h"

// Using
using namespace Luna;

AssetsUpdatedSignature::Event EditorChooser::s_AssetsUpdated;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
EditorChooser::EditorChooser( wxWindow* parent )
: wxPanel( parent )
, m_Panel( new EditorChooserInnerPanel( this ) )
, m_RunGame( NULL )
{
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_Panel, 1, wxEXPAND );

  // Set up icons
  m_Panel->m_ButtonGetAssets->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "get_assets_64.png" ) );
  m_Panel->m_ButtonRunGame->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "buildserver_64.png" ) );
  m_Panel->m_ButtonLive->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "live_64.png" ) );
  m_Panel->m_ButtonAssetEditor->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "asset_editor_64.png" ) );
  m_Panel->m_ButtonSceneEditor->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "scene_editor_64.png" ) );
  m_Panel->m_ButtonAnimationEventsEditor->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "events_editor_64.png" ) );
  m_Panel->m_ButtonCharacterEditor->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "character_editor_64.png" ) );
  m_Panel->m_ButtonCinematicEventsEditor->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "cinematic_events_editor_64.png" ) );

  SetSize( m_Panel->GetSize() );
  SetMinSize( m_Panel->GetMinSize() );
  SetMaxSize( m_Panel->GetMaxSize() );

  SetSizer( sizer );
  Layout();

  m_Panel->m_ButtonAssetEditor->Connect( m_Panel->m_ButtonAssetEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonSceneEditor->Connect( m_Panel->m_ButtonSceneEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonAnimationEventsEditor->Connect( m_Panel->m_ButtonAnimationEventsEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonCinematicEventsEditor->Connect( m_Panel->m_ButtonCinematicEventsEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonGetAssets->Connect( m_Panel->m_ButtonGetAssets->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonRunGame->Connect( m_Panel->m_ButtonRunGame->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonLive->Connect( m_Panel->m_ButtonLive->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonCharacterEditor->Connect( m_Panel->m_ButtonCharacterEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
EditorChooser::~EditorChooser()
{
  m_Panel->m_ButtonAssetEditor->Disconnect( m_Panel->m_ButtonAssetEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonSceneEditor->Disconnect( m_Panel->m_ButtonSceneEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonAnimationEventsEditor->Disconnect( m_Panel->m_ButtonAnimationEventsEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonCinematicEventsEditor->Disconnect( m_Panel->m_ButtonCinematicEventsEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonGetAssets->Disconnect( m_Panel->m_ButtonGetAssets->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonRunGame->Disconnect( m_Panel->m_ButtonRunGame->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonLive->Disconnect( m_Panel->m_ButtonLive->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonCharacterEditor->Disconnect( m_Panel->m_ButtonCharacterEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );

  if ( m_RunGame )
  {
    m_RunGame->Disconnect( m_RunGame->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( EditorChooser::OnCloseRunGame ), NULL, this );
    m_RunGame = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a button is pressed.  Dismisses the dialog, returning the
// id of the button that was pressed.
// 
void EditorChooser::OnButton( wxCommandEvent& event )
{
  if ( event.GetEventObject() == m_Panel->m_ButtonAssetEditor )
  {
    Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Asset );
  }
  else if ( event.GetEventObject() == m_Panel->m_ButtonSceneEditor )
  {
    Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene );
  }
  else if ( event.GetEventObject() == m_Panel->m_ButtonAnimationEventsEditor )
  {
    Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::AnimationEvents );
  }
  else if ( event.GetEventObject() == m_Panel->m_ButtonCinematicEventsEditor )
  {
    Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::CinematicEvents );
  }
  else if ( event.GetEventObject() == m_Panel->m_ButtonGetAssets )
  {
    wxMessageBox( "Get Assets is temporarily disabled within Luna.  Please close all your tools and run 'getassets' from a prompt.", "Unavailable", wxOK | wxCENTER | wxICON_ERROR, this );
    return;

    if ( ( SessionManager::GetInstance()->GetRunningEditorCount() > 0 )
      || GlobalBrowser().HasFrame() )
    {
      wxMessageBox( "You must close all your open editor windows and asset vault before getting assets.", "Error", wxOK | wxCENTER | wxICON_ERROR, this );
    }
    else
    {
      if ( SessionManager::GetInstance()->UseTracker() )
      {
        // Stop the thread before we start getassets
        Asset::GlobalTracker()->StopThread();
      }
      
      Windows::Execute( std::string ("perl.exe ") + Finder::ProjectTools() + "scripts/getassets.pl", true, true );

      s_AssetsUpdated.Raise( Nocturnal::Void() );

      // unload cached assets classes
      Asset::AssetClass::InvalidateCache();

      // release symbols and signal objects to convert
      Symbol::SymbolBuilder::GetInstance()->Reset();

      if ( SessionManager::GetInstance()->UseTracker() )
      {
        // Re-start the tracker
        Asset::GlobalTracker()->StartThread();
      }
    }
  }
  else if ( event.GetEventObject() == m_Panel->m_ButtonRunGame ) 
  {
    if ( !m_RunGame )
    {
      m_RunGame = new RunGameFrame( NULL );
      m_RunGame->Connect( m_RunGame->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( EditorChooser::OnCloseRunGame ), NULL, this );
    }
    m_RunGame->Show();
    m_RunGame->Raise();
  }
  else if ( event.GetEventObject() == m_Panel->m_ButtonLive ) 
  {
    Luna::OpenLiveFrame();
  }
  else if ( event.GetEventObject() == m_Panel->m_ButtonCharacterEditor )
  {
    Editor* editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Character );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the Run Game window is closed.  Gets rid of our reference to
// that window.
// 
void EditorChooser::OnCloseRunGame( wxCloseEvent& event )
{
  event.Skip();
  m_RunGame->Disconnect( m_RunGame->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( EditorChooser::OnCloseRunGame ), NULL, this );
  m_RunGame = NULL;
}
