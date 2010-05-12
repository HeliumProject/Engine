#include "Precompile.h"
#include "LiveLinkPanel.h"
#include "RemoteScene.h"

#include "Entity.h"
#include "EntityAssetSet.h"
#include "SceneLiveGenerated.h"
#include "Scene.h"
#include "SceneEditor.h"
#include "Editor/SessionManager.h"
#include "Live/RuntimeConnection.h"
#include "Live/LiveManager.h"
#include "RemoteView.h"

#include "FileSystem/FileSystem.h"
#include <wx/msgdlg.h>

using namespace Luna;

LiveLinkPanel::LiveLinkPanel( SceneEditor* sceneEditor )
: wxPanel( sceneEditor, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, "LiveLinkPanel" )
, m_SceneEditor( sceneEditor )
, m_Panel( new InnerLiveLinkPanel( this ) )
, m_RemoteScene( m_SceneEditor->GetRemoteScene() )
{
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_Panel, 1, wxEXPAND );

  Init();

  SetSizer( sizer );
  Layout();

  Luna::SceneManager* sceneManager = m_SceneEditor->GetSceneManager();
  sceneManager->AddCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &LiveLinkPanel::SceneChanging ) );
  sceneManager->AddCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &LiveLinkPanel::SceneChanged ) );

  SessionManager::GetInstance()->AddViewerControlChangedListener( ViewerControlChangeSignature::Delegate( this, &LiveLinkPanel::ViewerControlChanged ) );

  RuntimeConnection::AddRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &LiveLinkPanel::ConnectionStatusChanged ) );

  ConnectionStatusChanged( RuntimeConnectionStatusArgs( RuntimeConnection::IsConnected() ) );

  // General tab
  m_Panel->m_ToggleLiveSync->Connect( m_Panel->m_ToggleLiveSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnLiveSyncToggled), NULL, this );
  m_Panel->m_ToggleLiveGameSync->Connect( m_Panel->m_ToggleLiveGameSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnLiveGameSyncToggled), NULL, this );
  m_Panel->m_ToggleCameraSync->Connect( m_Panel->m_ToggleCameraSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnCameraSyncToggled), NULL, this );
  m_Panel->m_ToggleLightingSync->Connect( m_Panel->m_ToggleLightingSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnLightingSyncToggled), NULL, this );
  m_Panel->m_ButtonBuildCollisionDb->Connect( m_Panel->m_ButtonBuildCollisionDb->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildCollisionDb ), NULL, this );
  m_Panel->m_ButtonBuildStaticDb->Connect( m_Panel->m_ButtonBuildStaticDb->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildStaticDb ), NULL, this );
  m_Panel->m_ButtonBuildAll->Connect( m_Panel->m_ButtonBuildAll->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildAll ), NULL, this );
  m_Panel->m_ButtonBuildLightingData->Connect( m_Panel->m_ButtonBuildLightingData->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildLightingData ), NULL, this );
  m_Panel->m_ButtonScreenshot->Connect( m_Panel->m_ButtonScreenshot->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonTakeScreenshot ), NULL, this );

  // Physics Sim tab
  m_Panel->m_ButtonAdd->Connect( m_Panel->m_ButtonAdd->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonAdd ), NULL, this );
  m_Panel->m_ButtonRemove->Connect( m_Panel->m_ButtonRemove->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonRemove ), NULL, this );
  m_Panel->m_ButtonSync->Connect( m_Panel->m_ButtonSync->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonSync ), NULL, this );
  m_Panel->m_ButtonReset->Connect( m_Panel->m_ButtonReset->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonReset), NULL, this );
  m_Panel->m_ToggleSimulate->Connect( m_Panel->m_ToggleSimulate->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonSimulate), NULL, this );
  m_Panel->m_SimulatedMobys->Connect( m_Panel->m_SimulatedMobys->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LiveLinkPanel::OnMobySelected ), NULL, this );

  // default "Selected" to on
  m_Panel->m_RadioSelected->SetValue( true );

  // Default camera sync to disabled
  m_Panel->m_ToggleCameraSync->Enable( false );
}

LiveLinkPanel::~LiveLinkPanel()
{
  Luna::SceneManager* sceneManager = m_SceneEditor->GetSceneManager();
  sceneManager->RemoveCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &LiveLinkPanel::SceneChanging ) );
  sceneManager->RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &LiveLinkPanel::SceneChanged ) );

  SessionManager::GetInstance()->RemoveViewerControlChangedListener( ViewerControlChangeSignature::Delegate( this, &LiveLinkPanel::ViewerControlChanged ) );

  RuntimeConnection::RemoveRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &LiveLinkPanel::ConnectionStatusChanged ) );

  // General tab
  m_Panel->m_ToggleLiveSync->Disconnect( m_Panel->m_ToggleLiveSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnLiveSyncToggled), NULL, this );
  m_Panel->m_ToggleLiveGameSync->Disconnect( m_Panel->m_ToggleLiveGameSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnLiveGameSyncToggled), NULL, this );
  m_Panel->m_ToggleCameraSync->Disconnect( m_Panel->m_ToggleCameraSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnCameraSyncToggled), NULL, this );
  m_Panel->m_ToggleLightingSync->Disconnect( m_Panel->m_ToggleLightingSync->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnLightingSyncToggled), NULL, this );
  m_Panel->m_ButtonBuildStaticDb->Disconnect( m_Panel->m_ButtonBuildStaticDb->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildStaticDb ), NULL, this );
  m_Panel->m_ButtonBuildCollisionDb->Disconnect( m_Panel->m_ButtonBuildCollisionDb->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildCollisionDb ), NULL, this );
  m_Panel->m_ButtonBuildAll->Disconnect( m_Panel->m_ButtonBuildAll->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildAll ), NULL, this );
  m_Panel->m_ButtonBuildLightingData->Disconnect( m_Panel->m_ButtonBuildLightingData->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonBuildLightingData ), NULL, this );

  // Physics Sim tab
  m_Panel->m_ButtonAdd->Disconnect( m_Panel->m_ButtonAdd->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonAdd ), NULL, this );
  m_Panel->m_ButtonRemove->Disconnect( m_Panel->m_ButtonRemove->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonRemove ), NULL, this );
  m_Panel->m_ButtonSync->Disconnect( m_Panel->m_ButtonSync->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonSync ), NULL, this );
  m_Panel->m_ButtonReset->Disconnect( m_Panel->m_ButtonReset->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonReset), NULL, this );  m_Panel->m_ToggleSimulate->Disconnect( m_Panel->m_ToggleSimulate->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( LiveLinkPanel::OnButtonSimulate), NULL, this );
  m_Panel->m_SimulatedMobys->Disconnect( m_Panel->m_SimulatedMobys->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( LiveLinkPanel::OnMobySelected ), NULL, this );
}

void LiveLinkPanel::SelectionChanged( const OS_SelectableDumbPtr& selection )
{
  u32 count = m_Panel->m_SimulatedMobys->GetCount();
  for ( u32 i = 0; i < count; ++i )
  {
    //do a quick rename of all entries to their givenname in case they were renamed in the outliner
    //i also added a SelectionChanged call to renaming in the outliner to make this call happen
    //probably not the best way, but it works without adding a new listener
    Luna::Entity* entity = (Luna::Entity*)m_Panel->m_SimulatedMobys->GetClientData( i );
    std::string listName = GetListNameForEntity( entity );
    m_Panel->m_SimulatedMobys->SetString( i, listName );

    m_Panel->m_SimulatedMobys->Deselect( i );
  }

  OS_SelectableDumbPtr::Iterator it = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End();
  for ( ; it != end; ++it )
  {
    const Selectable* sel = *it;

    const Luna::Entity* entity = dynamic_cast< const Luna::Entity* >( sel );
    if ( entity )
    {
      std::string listName = GetListNameForEntity( entity );
      u32 index = m_Panel->m_SimulatedMobys->FindString( listName );
      if ( index != wxNOT_FOUND )
      {
        m_Panel->m_SimulatedMobys->Select( index );
      }
    }
  }
}

void LiveLinkPanel::SceneChanging( const SceneChangeArgs& args )
{
  if ( args.m_Scene )
  {
    // Connect listeners
    args.m_Scene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LiveLinkPanel::SelectionChanged ) );
    args.m_Scene->RemoveNodeRemovingListener( NodeChangeSignature::Delegate ( this, &LiveLinkPanel::SceneNodeDeleting ) );
  }
}

void LiveLinkPanel::SceneChanged( const SceneChangeArgs& args )
{
  if ( args.m_Scene )
  {
    // Connect listeners
    args.m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LiveLinkPanel::SelectionChanged ) );
    args.m_Scene->AddNodeRemovingListener( NodeChangeSignature::Delegate ( this, &LiveLinkPanel::SceneNodeDeleting ) );
  }

  V_EntityDumbPtr mobys;
  GetAllMobys( mobys );
  m_RemoteScene->RemoveMobysFromSim( mobys );

  m_Panel->m_SimulatedMobys->Clear();
}

void LiveLinkPanel::SceneNodeDeleting( const NodeChangeArgs& args )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( args.m_Node );

  if ( entity )
  {
    V_EntityDumbPtr mobys;
    mobys.push_back( entity );
    m_RemoteScene->RemoveMobysFromSim( mobys );

    std::string listName = GetListNameForEntity( entity );
    u32 index = m_Panel->m_SimulatedMobys->FindString( listName );
    if ( index != wxNOT_FOUND )
    {
      m_Panel->m_SimulatedMobys->Delete( index );
    }
  }
}

void LiveLinkPanel::ViewerControlChanged( const ViewerControlChangeArgs& args )
{
  if ( args.m_Editor != m_SceneEditor )
  {
    m_Panel->m_ToggleLiveSync->SetValue( false );
    m_Panel->m_ToggleCameraSync->Enable( false );
    m_Panel->m_ToggleSimulate->SetValue( false );
  }
}

void LiveLinkPanel::ConnectionStatusChanged( const RuntimeConnectionStatusArgs& args )
{
  m_Panel->m_TextLinkStatus->Clear();
  if ( args.m_Connected )
  {
    m_Panel->m_TextLinkStatus->AppendText( "Connected" );
  }
  else
  {
    m_Panel->m_TextLinkStatus->AppendText( "Not Connected" );
  }
}

std::string LiveLinkPanel::GetListNameForEntity( const Luna::Entity* entity )
{
  return entity->GetName();
}

void LiveLinkPanel::GetSelectedMobys( V_EntityDumbPtr& mobys )
{
  // get the current selection
  Luna::SceneManager* manager = m_SceneEditor->GetSceneManager();
  Luna::Scene* scene = manager->GetCurrentScene();
  OS_SelectableDumbPtr selectedItems;
  scene->GetFlattenedSelection( selectedItems );

  // iterate over the selection and find all the mobys
  OS_SelectableDumbPtr::Iterator it = selectedItems.Begin();
  OS_SelectableDumbPtr::Iterator end = selectedItems.End();
  for ( ; it != end; ++it )
  {
    const LSelectablePtr& selectable = *it;

    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( selectable );
    if( entity && !entity->IsTransient() )
    {
      Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

      if ( entityClass && entityClass->GetEngineType() == Asset::EngineTypes::Moby )
      {
        mobys.push_back( entity );
      }
    }
  }
}

void LiveLinkPanel::GetHighlightedMobys( V_EntityDumbPtr& mobys )
{
  wxArrayInt highlighted;
  u32 count = m_Panel->m_SimulatedMobys->GetSelections( highlighted );
  for ( u32 i = 0; i < count; ++i )
  {
    mobys.push_back( static_cast< Luna::Entity* >( m_Panel->m_SimulatedMobys->GetClientData( highlighted[ i ] ) ) );
  }
}

void LiveLinkPanel::GetAllMobys( V_EntityDumbPtr& mobys )
{
  u32 count = m_Panel->m_SimulatedMobys->GetCount();
  for ( u32 i = 0; i < count; ++i )
  {
    Luna::Entity* entity = (Luna::Entity*)m_Panel->m_SimulatedMobys->GetClientData( i );
    mobys.push_back( entity );
  }
}

void LiveLinkPanel::OnButtonAdd( wxCommandEvent& args )
{
  V_EntityDumbPtr mobys;
  GetSelectedMobys( mobys );

  m_RemoteScene->AddMobysToSim( mobys );

  for each ( const Luna::Entity* entity in mobys )
  {
    std::string itemName = GetListNameForEntity( entity );

    if ( m_Panel->m_SimulatedMobys->FindString( itemName ) == wxNOT_FOUND )
    {
      m_Panel->m_SimulatedMobys->Select( m_Panel->m_SimulatedMobys->Append( itemName, (void*)entity ) );
    }
  }
}

void LiveLinkPanel::OnButtonRemove( wxCommandEvent& args )
{
  V_EntityDumbPtr mobys;

  if ( m_Panel->m_RadioAll->GetValue() )
  {
    GetAllMobys( mobys );
  }
  else if ( m_Panel->m_RadioHighlighted->GetValue() )
  {
    GetHighlightedMobys( mobys );
  }
  else if ( m_Panel->m_RadioSelected->GetValue() )
  {
    GetSelectedMobys( mobys );
  }

  m_RemoteScene->RemoveMobysFromSim( mobys );

  wxArrayInt highlighted;
  u32 count = m_Panel->m_SimulatedMobys->GetSelections( highlighted );
  for ( i32 i = count - 1; i >= 0; --i )
  {
    m_Panel->m_SimulatedMobys->Delete( highlighted[ i ] );
  }
}

void LiveLinkPanel::OnButtonSync( wxCommandEvent& args )
{
  V_EntityDumbPtr mobys;

  if ( m_Panel->m_RadioAll->GetValue() )
  {
    GetAllMobys( mobys );
  }
  else if ( m_Panel->m_RadioHighlighted->GetValue() )
  {
    GetHighlightedMobys( mobys );
  }
  else if ( m_Panel->m_RadioSelected->GetValue() )
  {
    GetSelectedMobys( mobys );
  }

  EnableSim( false );

  m_RemoteScene->SyncMobys( mobys );
}

void LiveLinkPanel::EnableSim( bool enable )
{
  m_RemoteScene->EnablePhysics( enable );
  m_Panel->m_ToggleSimulate->SetValue( enable );

  if ( enable )
  {
    m_Panel->m_ToggleSimulate->SetLabel( "Stop Simulation" );
  }
  else
  {
    m_Panel->m_ToggleSimulate->SetLabel( "Start Simulation" );
  }
}

void LiveLinkPanel::OnButtonReset( wxCommandEvent& args )
{
  V_EntityDumbPtr mobys;

  if ( m_Panel->m_RadioAll->GetValue() )
  {
    GetAllMobys( mobys );
  }
  else if ( m_Panel->m_RadioHighlighted->GetValue() )
  {
    GetHighlightedMobys( mobys );
  }
  else if ( m_Panel->m_RadioSelected->GetValue() )
  {
    GetSelectedMobys( mobys );
  }

  EnableSim( false );

  for each ( const Luna::Entity* entity in mobys )
  {
    m_RemoteScene->TransformInstance( (Luna::Transform*)entity );
    m_RemoteScene->SendMobyAnimJointMats( (Asset::Entity*)entity->GetPackage<Asset::Entity>() );
  }
}

void LiveLinkPanel::OnButtonSimulate( wxCommandEvent& args )
{
  EnableSim( args.IsChecked() );
}

void LiveLinkPanel::OnMobySelected( wxCommandEvent& args )
{
  wxArrayInt highlighted;
  u32 count = m_Panel->m_SimulatedMobys->GetSelections( highlighted );

  OS_SelectableDumbPtr newSelection;
  for ( u32 i = 0; i < count; ++i )
  {
    newSelection.Append( static_cast< Selectable* >( m_Panel->m_SimulatedMobys->GetClientData( highlighted[ i ] ) ) );
  }

  Luna::Scene* scene = m_SceneEditor->GetSceneManager()->GetCurrentScene();
  Selection& selection = scene->GetSelection();

  selection.Clear();
  selection.AddItems( newSelection );
}

void LiveLinkPanel::OnLiveSyncToggled( wxCommandEvent& args )
{
  SceneEditor* editor = static_cast< SceneEditor* >( SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ) );

  if ( args.IsChecked() )
  {
    Manager::GetInstance()->SetViewerMode( ViewerModes::Uberview );
    SessionManager::GetInstance()->GiveViewerControl( NULL );
    editor->GetRemoteScene()->EnableCamera( m_Panel->m_ToggleCameraSync->GetValue() ); 
    editor->GetRemoteScene()->Enable( true, true );
   
    SessionManager::GetInstance()->GiveViewerControl( m_SceneEditor );

    m_Panel->m_ToggleLiveSync->SetValue( true );
    m_Panel->m_ToggleCameraSync->Enable( true );
  }
  else
  {
    SessionManager::GetInstance()->GiveViewerControl( NULL );

    m_Panel->m_ToggleCameraSync->Enable( false );
    m_Panel->m_ToggleSimulate->SetValue( false );
  }
}

void LiveLinkPanel::OnLiveGameSyncToggled( wxCommandEvent& args )
{
  if ( args.IsChecked() )
  {
    Manager::GetInstance()->SetViewerMode( ViewerModes::Game );
    RemoteView::Enable( args.IsChecked() );
    SessionManager::GetInstance()->GiveViewerControl( m_SceneEditor );
    m_Panel->m_ToggleLiveSync->SetValue( false );
    m_Panel->m_ToggleLiveSync->Enable( false );
    m_Panel->m_ToggleCameraSync->SetValue( false );
    m_Panel->m_ToggleCameraSync->Enable( true );
  }
  else
  {
    SessionManager::GetInstance()->GiveViewerControl( NULL );
    m_Panel->m_ToggleCameraSync->SetValue( false );
    m_Panel->m_ToggleLiveSync->Enable( true );
  }
}

void LiveLinkPanel::OnCameraSyncToggled( wxCommandEvent& args )
{
  m_RemoteScene->EnableCamera( args.IsChecked() );
}

void LiveLinkPanel::OnLightingSyncToggled( wxCommandEvent& args )
{
  m_RemoteScene->EnableLighting( args.IsChecked() );
}

void LiveLinkPanel::OnButtonBuildAll( wxCommandEvent& args )
{
  m_SceneEditor->BuildAllLoadedAssets(); 
}

void LiveLinkPanel::OnButtonBuildCollisionDb( wxCommandEvent& args )
{
  m_RemoteScene->BuildCollisionDb();
}

void LiveLinkPanel::OnButtonBuildStaticDb( wxCommandEvent& args )
{
}

void LiveLinkPanel::OnButtonBuildLightingData( wxCommandEvent& args )
{
  m_RemoteScene->BuildLightingData(); 
}

void LiveLinkPanel::OnButtonTakeScreenshot( wxCommandEvent& args )
{
  // Do we know whats displaying on the console
  if( !m_Panel->m_ToggleLiveSync->GetValue())
  {
    wxMessageDialog requireSync( this, wxT("The scene is not currently synchronized. Scene on the console may not match what is in Luna. Continue with console screenshot?"), wxT("Console display warning"), wxYES|wxNO|wxCENTRE );
    if( requireSync.ShowModal() == wxID_NO )
    {
      return;
    }
  }
  m_RemoteScene->Screenshot( ); 
}