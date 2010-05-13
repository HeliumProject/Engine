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
  m_Panel->m_ButtonRunGame->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "buildserver_64.png" ) );
  m_Panel->m_ButtonAssetEditor->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "asset_editor_64.png" ) );
  m_Panel->m_ButtonSceneEditor->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "scene_editor_64.png" ) );

  SetSize( m_Panel->GetSize() );
  SetMinSize( m_Panel->GetMinSize() );
  SetMaxSize( m_Panel->GetMaxSize() );

  SetSizer( sizer );
  Layout();

  m_Panel->m_ButtonAssetEditor->Connect( m_Panel->m_ButtonAssetEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonSceneEditor->Connect( m_Panel->m_ButtonSceneEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonRunGame->Connect( m_Panel->m_ButtonRunGame->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
EditorChooser::~EditorChooser()
{
  m_Panel->m_ButtonAssetEditor->Disconnect( m_Panel->m_ButtonAssetEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonSceneEditor->Disconnect( m_Panel->m_ButtonSceneEditor->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );
  m_Panel->m_ButtonRunGame->Disconnect( m_Panel->m_ButtonRunGame->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorChooser::OnButton ), NULL, this );

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
