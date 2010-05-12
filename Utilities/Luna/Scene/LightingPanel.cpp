#include "Precompile.h"
#include "LightingPanel.h"

#include "Entity.h"
#include "EntityAssetSet.h"
#include "InstanceCodeSet.h"
#include "EntityType.h"
#include "LightingGenerated.h"
#include "DirectionalLight.h"
#include "ShadowDirection.h"
#include "AmbientLight.h"
#include "SunLight.h"
#include "Light.h"
#include "LightingJob.h"
#include "LightingJobTree.h"
#include "LightingListLightingEnvironment.h"
#include "LightingListLight.h"
#include "LightingListObject.h"
#include "LightingListProbe.h"
#include "LightingListVolume.h"
#include "LightingListZone.h"
#include "LightingVolume.h"
#include "AmbientLight.h"
#include "LightingVolumeType.h"
#include "SceneEditor.h"
#include "LightingTreeItem.h"
#include "Zone.h"
#include "Curve.h" 
#include "SceneNodeItem.h"
#include "Layer.h"

#include "Content/LightingVolume.h"
#include "Content/DirectionalLight.h"
#include "Content/ShadowDirection.h"
#include "Content/AmbientLight.h"
#include "Content/Zone.h"
#include "Content/Scene.h"
#include "Content/Camera.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/BakedLightingAttribute.h"
#include "Asset/SpatialLightingAttribute.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/CubeMapAsset.h"

#include "Finder/ContentSpecs.h"
#include "Finder/LevelSpecs.h"

#include "LightingJob/LightingJob.h"

#include "Console/Console.h"
#include "Windows/Process.h"

#include "Undo/PropertyCommand.h"
#include "ParentCommand.h"

#include "UIToolKit/SortableListView.h"

#include "LightingCondenser/LightingCondenser.h"
#include "FileSystem/FileSystem.h"
#include "File/Manager.h"

#include "LightingSetupChecker.h"
#include "LightmapUVViewer.h"

#include "RCS/rcs.h"

#include "Task/Export.h"
#include <float.h>

using namespace Luna;
using namespace Asset;
using namespace Attribute;


///////////////////////////////////////////////////////////////////////////////
// Templated helper function to build an undoable command which adds all the
// selected items in the specified list to all the selected jobs in the specified
// tree.
// 
template< class T >
Undo::CommandPtr AddJobMembers( LightingList< T >* list, LightingJobTree* tree )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();
  const OS_TreeItemIds& treeItems = tree->GetSelectedItems();
  if ( treeItems.Size() > 0 )
  {
    long id = list->GetFirstSelected();
    while ( id >= 0 )
    {
      T* item = list->FindListItem( id );
      if ( item )
      {
        OS_TreeItemIds::Iterator itr = treeItems.Begin();
        OS_TreeItemIds::Iterator end = treeItems.End();
        for ( ; itr != end; ++itr )
        {
          LightingTreeItem* data = static_cast< LightingTreeItem* >( tree->GetItemData( *itr ) );
          if ( data && data->CanAddChild( item ) )
          {
            batch->Push( data->AddChild( item ) );
          }
        }
      }
      id = list->GetNextSelected( id );
    }
  }

  if ( batch->IsEmpty() )
  {
    batch = NULL;
  }

  return batch;
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingPanel::LightingPanel( SceneEditor* sceneEditor )
: wxPanel( sceneEditor, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, "LightingPanel" )
, m_SceneEditor( sceneEditor )
, m_Panel( new InnerLightingPanel( this ) )
{
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_Panel, 1, wxEXPAND );

  Init();

  SetSizer( sizer );
  Layout();

  // Add scene manager listeners
  Luna::SceneManager* sceneManager = m_SceneEditor->GetSceneManager();
  sceneManager->AddSceneAddedListener( SceneChangeSignature::Delegate ( this, &LightingPanel::SceneAdded ) );
  sceneManager->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LightingPanel::SceneRemoved ) );

  // Add UI listeners
  m_Panel->m_ButtonNewJob->Connect( m_Panel->m_ButtonNewJob->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonNewJob ), NULL, this );
  m_Panel->m_ButtonDeleteJob->Connect( m_Panel->m_ButtonDeleteJob->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonDeleteJob ), NULL, this );
  m_Panel->m_ButtonAddJobMembers->Connect( m_Panel->m_ButtonAddJobMembers->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonAddJobMembers ), NULL, this );
  m_Panel->m_ButtonRemoveJobMembers->Connect( m_Panel->m_ButtonRemoveJobMembers->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonRemoveJobMembers ), NULL, this );
  m_Panel->m_ButtonRender->Connect( m_Panel->m_ButtonRender->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonRender ), NULL, this );
  m_Panel->m_ButtonQuickSetup->Connect( m_Panel->m_ButtonQuickSetup->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonQuickSetup ), NULL, this );
  m_Panel->m_ButtonQuickRender->Connect( m_Panel->m_ButtonQuickRender->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonQuickRender ), NULL, this );
  m_Panel->m_ButtonSelect->Connect( m_Panel->m_ButtonSelect->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSelect ), NULL, this );
  m_Panel->m_ButtonSort->Connect( m_Panel->m_ButtonSort->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSort ), NULL, this );
  m_Panel->m_ButtonView->Connect( m_Panel->m_ButtonView->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonView ), NULL, this );
  m_Panel->m_ButtonCopyJob->Connect( m_Panel->m_ButtonCopyJob->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonCopyJob ), NULL, this );
  m_Panel->m_ButtonCheckSetup->Connect( m_Panel->m_ButtonCheckSetup->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonCheckSetup ), NULL, this );
  m_Panel->m_ButtonUpdateCasters->Connect( m_Panel->m_ButtonUpdateCasters->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonUpdateShadowCasters ), NULL, this );
  m_Panel->m_ButtonUpdateLit->Connect( m_Panel->m_ButtonUpdateLit->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonUpdateLitColumn ), NULL, this );
  m_Panel->m_ButtonViewUVs->Connect( m_Panel->m_ButtonViewUVs->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonViewUVs ), NULL, this );
  m_Panel->m_ButtonSetTPM->Connect( m_Panel->m_ButtonSetTPM->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSetTPM ), NULL, this );
  m_Panel->m_ChoiceRenderType->Connect( m_Panel->m_ChoiceRenderType->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( LightingPanel::OnChoiceSelectRenderType ), NULL, this );
  m_Panel->m_Tabs->Connect( m_Panel->m_Tabs->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( LightingPanel::OnTabChanged ), NULL, this );
  m_Panel->m_ButtonSync->Connect( m_Panel->m_ButtonSync->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSyncLighting ), NULL, this );
  //m_Panel->m_ChoiceQuickRender->Connect( m_Panel->m_ChoiceQuickRender->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( LightingPanel::OnChoiceSelectQuickRenderType ), NULL, this );

  m_Panel->m_JobTree->Connect( m_Panel->m_JobTree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( LightingPanel::OnJobTreeSelectionChanged ), NULL, this );
  

  UpdateButtons();

  m_SelectionFuncs[0] = &LightingPanel::SelectInGui;
  m_SelectionFuncs[1] = &LightingPanel::SelectInScene;
  m_SelectionFuncs[2] = &LightingPanel::SelectNonRenderTargets;
  m_SelectionFuncs[3] = &LightingPanel::SelectAllRenderTargets;

}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingPanel::~LightingPanel()
{
  // Remove scene manager listeners
  Luna::SceneManager* sceneManager = m_SceneEditor->GetSceneManager();
  sceneManager->RemoveSceneAddedListener( SceneChangeSignature::Delegate ( this, &LightingPanel::SceneAdded ) );
  sceneManager->RemoveSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LightingPanel::SceneRemoved ) );

  // Remove UI listeners
  m_Panel->m_ButtonNewJob->Disconnect( m_Panel->m_ButtonNewJob->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonNewJob ), NULL, this );
  m_Panel->m_ButtonDeleteJob->Disconnect( m_Panel->m_ButtonDeleteJob->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonDeleteJob ), NULL, this );
  m_Panel->m_ButtonAddJobMembers->Disconnect( m_Panel->m_ButtonAddJobMembers->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonAddJobMembers ), NULL, this );
  m_Panel->m_ButtonRemoveJobMembers->Disconnect( m_Panel->m_ButtonRemoveJobMembers->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonRemoveJobMembers ), NULL, this );
  m_Panel->m_ButtonRender->Disconnect( m_Panel->m_ButtonRender->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonRender ), NULL, this );
  m_Panel->m_ButtonQuickSetup->Disconnect( m_Panel->m_ButtonQuickSetup->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonQuickSetup ), NULL, this );  
  m_Panel->m_ButtonQuickRender->Disconnect( m_Panel->m_ButtonQuickRender->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonQuickRender ), NULL, this );  
  m_Panel->m_ButtonSelect->Disconnect( m_Panel->m_ButtonSelect->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSelect ), NULL, this );
  m_Panel->m_ButtonSort->Disconnect( m_Panel->m_ButtonSort->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSort ), NULL, this );
  m_Panel->m_ButtonCopyJob->Disconnect( m_Panel->m_ButtonCopyJob->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonCopyJob ), NULL, this );
  m_Panel->m_ButtonCheckSetup->Disconnect( m_Panel->m_ButtonCheckSetup->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonCheckSetup ), NULL, this );
  m_Panel->m_ButtonUpdateCasters->Disconnect( m_Panel->m_ButtonUpdateCasters->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonUpdateShadowCasters ), NULL, this );
  m_Panel->m_ButtonUpdateLit->Disconnect( m_Panel->m_ButtonUpdateLit->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonUpdateLitColumn ), NULL, this ); 
  m_Panel->m_ButtonViewUVs->Disconnect( m_Panel->m_ButtonViewUVs->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonViewUVs ), NULL, this );
  m_Panel->m_ButtonSetTPM->Disconnect( m_Panel->m_ButtonSetTPM->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSetTPM ), NULL, this );
  m_Panel->m_ChoiceRenderType->Disconnect( m_Panel->m_ChoiceRenderType->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( LightingPanel::OnChoiceSelectRenderType ), NULL, this );
  m_Panel->m_Tabs->Disconnect( m_Panel->m_Tabs->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( LightingPanel::OnTabChanged ), NULL, this );
  m_Panel->m_ButtonSync->Disconnect( m_Panel->m_ButtonSync->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LightingPanel::OnButtonSyncLighting ), NULL, this );
  m_Panel->m_JobTree->Disconnect( m_Panel->m_JobTree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( LightingPanel::OnJobTreeSelectionChanged ), NULL, this );
  //m_Panel->m_ChoiceQuickRender->Disconnect( m_Panel->m_ChoiceQuickRender->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( LightingPanel::OnChoiceSelectQuickRenderType ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// One-time set up of UI elements.
// 
void LightingPanel::Init()
{
  // Set up the list on each tab
  m_Panel->m_LightableInstanceList->Init( m_SceneEditor );
  m_Panel->m_LightList->Init( m_SceneEditor );
  m_Panel->m_VolumeList->Init( m_SceneEditor );
  m_Panel->m_ProbeList->Init( m_SceneEditor );
  m_Panel->m_EnvironmentList->Init( m_SceneEditor );
  m_Panel->m_ZoneList->Init( m_SceneEditor );

  // Set up lighting jobs tree
  m_Panel->m_JobTree->Init( m_SceneEditor );

  // set the default choice
  m_Panel->m_ChoiceRenderType->SetSelection( 0 );
  m_Panel->m_ChoiceRenderLocation->SetSelection( 0 );
//  m_Panel->m_ChoiceQuickRender->SetSelection( 0 );
}

///////////////////////////////////////////////////////////////////////////////
// Enables/disables all the buttons as appropriate.
// 
void LightingPanel::UpdateButtons( wxWindow* selectedTab )
{
  const size_t numSelectedTreeItems = m_Panel->m_JobTree->GetSelectedItems().Size();
  bool hasRootScene = m_SceneEditor->GetSceneManager()->GetRootScene() != NULL;
  Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();
  const bool selectionInCurrentScene = currentScene ? currentScene->HasSelection() : false;

  bool isLightTabCurrent = false;
  bool isInstanceTabCurrent = false;
  bool isVolumeTabCurrent = false;
  bool hasListItemsSelected = false;
  bool isEnvironmentTabCurrent = false;
  bool isZoneTabCurrent = false;
  bool isProbeTabCurrent = false;

  if ( selectedTab == NULL )
  {
    selectedTab = m_Panel->m_Tabs->GetCurrentPage();
  }

  if ( selectedTab == m_Panel->m_TabLights )
  {
    isLightTabCurrent = true;
    hasListItemsSelected = m_Panel->m_LightList->GetSelectedItemCount() > 0;
  }
  else if ( selectedTab == m_Panel->m_TabObjects )
  {
    isInstanceTabCurrent = true;
    hasListItemsSelected = m_Panel->m_LightableInstanceList->GetSelectedItemCount() > 0;
  }
  else if ( selectedTab == m_Panel->m_TabVolumes )
  {
    isVolumeTabCurrent = true;
    hasListItemsSelected = m_Panel->m_VolumeList->GetSelectedItemCount() > 0;
  }
  else if ( selectedTab == m_Panel->m_TabLightingEnvironments )
  {
    isEnvironmentTabCurrent = true;
    hasListItemsSelected = m_Panel->m_EnvironmentList->GetSelectedItemCount() > 0;
  }
  else if ( selectedTab == m_Panel->m_TabZones )
  {
    isZoneTabCurrent = true;
    hasListItemsSelected = m_Panel->m_ZoneList->GetSelectedItemCount() > 0;
  }
  else if ( selectedTab == m_Panel->m_TabProbes )
  {
    isProbeTabCurrent = true;
    hasListItemsSelected = m_Panel->m_ProbeList->GetSelectedItemCount() > 0;
  }

  m_Panel->m_ButtonRender->Enable( numSelectedTreeItems > 0 );
  m_Panel->m_ButtonView->Enable( numSelectedTreeItems > 0 );
  m_Panel->m_ButtonNewJob->Enable( hasRootScene );
  m_Panel->m_ButtonCopyJob->Enable( hasRootScene );
  m_Panel->m_ButtonDeleteJob->Enable( numSelectedTreeItems > 0 );
  m_Panel->m_ButtonAddJobMembers->Enable( hasListItemsSelected );
  m_Panel->m_ButtonRemoveJobMembers->Enable( numSelectedTreeItems > 0 && selectionInCurrentScene );
  m_Panel->m_ButtonSort->Enable( m_Panel->m_JobTree->GetChildrenCount( m_Panel->m_JobTree->GetRootItem(), false ) > 0 );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene has been added to the scene manager.
// 
void LightingPanel::SceneAdded( const SceneChangeArgs& args )
{
  // Connect listeners
  args.m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LightingPanel::SelectionChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene has been removed from the scene manager.
// 
void LightingPanel::SceneRemoved( const SceneChangeArgs& args )
{
  // Disconnect listeners
  args.m_Scene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LightingPanel::SelectionChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the selection has changed (not originating from this
// lighting UI).  Selects the appropriate items in the lighting UI.
// 
void LightingPanel::SelectionChanged( const OS_SelectableDumbPtr& selection )
{
  UpdateButtons();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user clicks the "new job" button.  Creates a new 
// lighting job.
// 
void LightingPanel::OnButtonNewJob( wxCommandEvent& args )
{
  Luna::Scene* lightingZone = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
  if ( lightingZone )
  {
    lightingZone->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, lightingZone, new Luna::LightingJob( lightingZone ) ) );
  }
  else
  {
    wxMessageBox( "Could not Load the Lighting Zone!\nYou may need to associate a Lighting Zone  with your level in the Asset Editor.", "Error", wxCENTER | wxICON_ERROR | wxOK );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user clicks the "delete job" button.  Deletes all the
// jobs that are currently selected in the job tree.  This operation is undoable.
// 
void LightingPanel::OnButtonDeleteJob( wxCommandEvent& args )
{
  Luna::Scene* lightingZone = m_SceneEditor->GetSceneManager()->GetLightingScene();
  if ( lightingZone )
  {
    const OS_TreeItemIds& selection = m_Panel->m_JobTree->GetSelectedItems();
    if ( selection.Size() > 0 )
    {
      Undo::BatchCommandPtr batch = new Undo::BatchCommand();
      std::set< Luna::LightingJob* > jobs;
      OS_TreeItemIds::Iterator itr = selection.Begin();
      OS_TreeItemIds::Iterator end = selection.End();
      for ( ; itr != end; ++itr )
      {
        LightingTreeItem* data = static_cast< LightingTreeItem* >( m_Panel->m_JobTree->GetItemData( *itr ) );
        if ( data && data->GetLightingJob() )
        {
          jobs.insert( data->GetLightingJob() );
        }
      }

      for each ( Luna::LightingJob* job in jobs )
      {
        batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, lightingZone, job ) );
      }

      if ( !batch->IsEmpty() )
      {
        lightingZone->Push( batch );
      }
    }
  }

}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user clicks the "add" button to add members to a lighting
// job.
// 
void LightingPanel::OnButtonAddJobMembers( wxCommandEvent& args )
{
  Luna::Scene* lightingZone = m_SceneEditor->GetSceneManager()->GetLightingScene();
  if ( lightingZone )
  {
    m_Panel->m_JobTree->Freeze();
    Undo::CommandPtr command;

    if ( m_Panel->m_Tabs->GetCurrentPage() == m_Panel->m_TabObjects )
    {
      command = AddJobMembers< Luna::Entity >( m_Panel->m_LightableInstanceList, m_Panel->m_JobTree );
    }
    else if ( m_Panel->m_Tabs->GetCurrentPage() == m_Panel->m_TabLights )
    {
      command = AddJobMembers< Luna::Light >( m_Panel->m_LightList, m_Panel->m_JobTree );
    }
    else if ( m_Panel->m_Tabs->GetCurrentPage() == m_Panel->m_TabVolumes )
    {
      command = AddJobMembers< Luna::LightingVolume >( m_Panel->m_VolumeList, m_Panel->m_JobTree );
    }
    else if ( m_Panel->m_Tabs->GetCurrentPage() == m_Panel->m_TabProbes )
    {
      command = AddJobMembers< Luna::CubeMapProbe >( m_Panel->m_ProbeList, m_Panel->m_JobTree );
    }
    else if ( m_Panel->m_Tabs->GetCurrentPage() == m_Panel->m_TabLightingEnvironments )
    {
      command = AddJobMembers< Luna::LightingEnvironment >( m_Panel->m_EnvironmentList, m_Panel->m_JobTree );
    }
    else if ( m_Panel->m_Tabs->GetCurrentPage() == m_Panel->m_TabZones )
    {
      command = AddJobMembers< Zone >( m_Panel->m_ZoneList, m_Panel->m_JobTree );
    }
    else
    {
      // A new tab was added, but this function was not updated!
      NOC_BREAK();
    }

    if ( command.ReferencesObject() )
    {
      lightingZone->Push( command );
    }

    m_Panel->m_JobTree->Thaw();
  }

}

///////////////////////////////////////////////////////////////////////////////
// Called when the user clicks the "remove members" button on the jobs panel.
// Gets the list of items selected in the current scene and attempts to remove
// them from all the highlighted items in the job tree.
// 
void LightingPanel::OnButtonRemoveJobMembers( wxCommandEvent& args )
{

  Luna::Scene* lightingZone = m_SceneEditor->GetSceneManager()->GetLightingScene();
  if ( lightingZone )
  {
    Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();
    if ( currentScene )
    {
      const OS_SelectableDumbPtr& selection = currentScene->GetSelection().GetItems();
      if ( !selection.Empty() )
      {
        m_Panel->m_JobTree->Freeze();
        Undo::CommandPtr command = m_Panel->m_JobTree->RemoveJobMembers( selection );
        if ( command.ReferencesObject() )
        {
          lightingZone->Push( command );
        }
        m_Panel->m_JobTree->Thaw();
      }
    }
  }
}


void LightingPanel::OnButtonRender( wxCommandEvent& args )
{
  try
  {
    int renderMode = m_Panel->m_ChoiceRenderType->GetSelection();
    int renderLocation = m_Panel->m_ChoiceRenderLocation->GetSelection();

    Luna::Scene* lightingZone = m_SceneEditor->GetSceneManager()->GetLightingScene();

    if( !lightingZone )
    {
      wxMessageBox( "Could not Load the Lighting Zone!\nYou may need to associate a Lighting Zone  with your level in the Asset Editor.", "Error", wxCENTER | wxICON_ERROR | wxOK );
      return;
    }

    Luna::SceneManager* sceneManager = m_SceneEditor->GetSceneManager();
    Luna::Scene* worldScene = sceneManager->GetRootScene();

    // this would be weird...
    NOC_ASSERT(worldScene);

    std::vector<Content::Zone*> contentZones;
    worldScene->GetAllPackages< Content::Zone >( contentZones );

    const OS_TreeItemIds selection = m_Panel->m_JobTree->GetSelectedItems();

    Content::S_LightingJob jobSet;  

    // make a Content camera from current Luna camera
    Content::CameraPtr contentCamera = new Content::Camera();
    if( renderMode == Content::RenderModes::Preview || renderMode == Content::RenderModes::RenderCubemap )
    {
      Luna::Camera* camera = m_SceneEditor->GetSceneManager()->GetCurrentScene()->GetView()->GetCamera();
      contentCamera->m_View = camera->GetView();
      contentCamera->m_FOV = Luna::Camera::FieldOfView;
      contentCamera->m_Width  = camera->GetWidth();
      contentCamera->m_Height = camera->GetHeight();
    }

    // for each selected item in the lighting job panel

    {
      OS_TreeItemIds::Iterator itr = selection.Begin();
      OS_TreeItemIds::Iterator end = selection.End();
      for ( ; itr != end; ++itr )
      {
        LightingTreeItem* item = (LightingTreeItem*)m_Panel->m_JobTree->GetItemData( *itr );
        Luna::LightingJob* job = item->GetLightingJob();

        Content::LightingJobPtr contentLightingJob = job->GetPackage< Content::LightingJob >();
        jobSet.insert( contentLightingJob );

        if( item->GetType() == LightingTreeItem::TypeInstance || item->GetType() == LightingTreeItem::TypeProbe )
        {
          SceneNodeItem* sceneItem = static_cast< SceneNodeItem* >( item );
          contentLightingJob->m_InstanceRenders.insert( sceneItem->GetSceneNode()->GetID() );

        }
      }
    }

    Content::S_LightingJob::iterator itor = jobSet.begin();
    Content::S_LightingJob::iterator end  = jobSet.end();

    for( ; itor != end; ++itor )
    {
      Content::LightingJob* contentLightingJob = *itor;
      contentLightingJob->m_RenderMode = (Content::RenderMode)renderMode;
      contentLightingJob->m_RenderLocation = (Content::RenderLocation)renderLocation;

      // set up the lighting job to render
      Content::V_SceneNode loadedRenderItems;

      // put all zoneID's in the set
      S_tuid contentZoneIDs;
      std::vector<Content::Zone*>::iterator zoneItor = contentZones.begin();
      std::vector<Content::Zone*>::iterator zoneEnd  = contentZones.end();
      for( ; zoneItor != zoneEnd; ++zoneItor  )
      {
        contentZoneIDs.insert( (*zoneItor)->m_FileID );
      }

      // add the camera
      loadedRenderItems.push_back( contentCamera );

      // assign the default lighting environment for now
      Luna::LightingEnvironment* env = Luna::LightingEnvironment::GetDefaultLightingEnvironment( sceneManager->GetCurrentScene() );
      if( env )
        contentLightingJob->m_LightingEnvironment = env->GetPackage< Content::LightingEnvironment >() ;

      // for each group of items per zone in the lighting job
      Content::M_LightingJobZoneItems::iterator itemsItor = contentLightingJob->m_JobElements.begin();
      Content::M_LightingJobZoneItems::iterator itemsEnd  = contentLightingJob->m_JobElements.end();
      for( ; itemsItor != itemsEnd; ++itemsItor )
      {
        // see if the zone is currently loaded
        Luna::Scene* scene = m_SceneEditor->GetSceneManager()->GetScene( itemsItor->first );

        if( scene )
        {
          contentZoneIDs.erase( itemsItor->first );

          Content::LightingJobZoneItemsPtr& zoneItemIDs = itemsItor->second;

          UniqueID::S_TUID renderTargetIDs;

          if( !contentLightingJob->m_InstanceRenders.empty() )
          {
            renderTargetIDs = contentLightingJob->m_InstanceRenders;
          }
          else
          {
            renderTargetIDs = zoneItemIDs->m_RenderTargets;
            renderTargetIDs.insert( zoneItemIDs->m_CubeMapProbes.begin(), zoneItemIDs->m_CubeMapProbes.end() );
          }

          //doing this makes "renderTargetIDs" kind of a misnomer, but it makes sense, as far as the render pipeline/logic is concerned
          renderTargetIDs.insert( zoneItemIDs->m_Lights.begin(), zoneItemIDs->m_Lights.end() );

          if( !zoneItemIDs->m_LightingEnvironments.empty() )
          {
            Luna::LightingEnvironment* env = Reflect::ObjectCast<Luna::LightingEnvironment>( scene->FindNode( *zoneItemIDs->m_LightingEnvironments.begin() ) );
            if( env )
              contentLightingJob->m_LightingEnvironment = env->GetPackage< Content::LightingEnvironment >();             
          }

          bool excludedZone = contentLightingJob->IsExcludedZone( itemsItor->first );
          if( !excludedZone )
          {
            std::vector<Asset::Entity*> entities;
            scene->GetAllPackages< Asset::Entity >( entities, Reflect::GetType<Asset::BakedLightingAttribute>(), true );

            std::vector<Asset::Entity*>::iterator entityItor = entities.begin();
            std::vector<Asset::Entity*>::iterator entityEnd  = entities.end();
            for( ; entityItor != entityEnd; ++entityItor )
            {
              // if it's not in the ignore list, add it
              if( !contentLightingJob->IsExcluded( (*entityItor)->m_ID ) )
              {
                (*entityItor)->m_Selected = false;
                loadedRenderItems.push_back( *entityItor );
              }
            }
          }

          for each( const UniqueID::TUID& id in renderTargetIDs )
          {
            Luna::SceneNode* sceneNode = scene->FindNode( id );
            if( sceneNode )
            {
              Content::SceneNode* node = sceneNode->GetPackage< Content::SceneNode >();
              if( node )
              {
                Content::Light* light = Reflect::ObjectCast< Content::Light >( node );
                if( !light || ( light && ( light->m_RenderType != Content::LightRenderTypes::RealTime ) ) )
                {
                  sceneNode->Pack();
                  node->m_Selected = true;
                  loadedRenderItems.push_back( node );
                }               
              }
            }
          }
        }
      }
      S_tuid::iterator idItor = contentZoneIDs.begin();
      S_tuid::iterator endItor = contentZoneIDs.end();

      for( ; idItor != endItor; )
      {
        // see if the zone is currently loaded
        Luna::Scene* scene = m_SceneEditor->GetSceneManager()->GetScene( *idItor );

        if( scene )
        {
          bool excludedZone = contentLightingJob->IsExcludedZone( *idItor );
          if( !excludedZone )
          {
            std::string filePath;
            File::GlobalManager().GetPath( *idItor, filePath );

            std::vector<Asset::Entity*> entities;
            scene->GetAllPackages< Asset::Entity >( entities, Reflect::GetType<Asset::BakedLightingAttribute>(), true );   

            std::vector<Asset::Entity*>::iterator entityItor = entities.begin();
            std::vector<Asset::Entity*>::iterator entityEnd  = entities.end();
            for( ; entityItor != entityEnd; ++entityItor )
            {
              (*entityItor)->m_Selected = false;
              loadedRenderItems.push_back( *entityItor );
            }
          }
          idItor = contentZoneIDs.erase( idItor );
        }
        else
          ++idItor;
      }
      ::LightingJob::RenderLightingJob( *contentLightingJob, loadedRenderItems, contentZoneIDs );
    }
  }

  catch( Nocturnal::Exception& e )
  {
    Console::Error( "Unable to render lighting job(s) \n\nReason: %s\n\n", e.what() );
  }
}

void LightingPanel::SelectInScene()
{

  OS_SelectableDumbPtr selectionAcrossAllScenes;

  OS_TreeItemIds::Iterator treeItr = m_Panel->m_JobTree->GetSelectedItems().Begin();
  OS_TreeItemIds::Iterator treeEnd = m_Panel->m_JobTree->GetSelectedItems().End();
  for ( ; treeItr != treeEnd; ++treeItr )
  {
    LightingTreeItem* item = static_cast< LightingTreeItem* >( m_Panel->m_JobTree->GetItemData( *treeItr ) );
    if ( item )
    {
      item->GetSelectableItems( selectionAcrossAllScenes );
    }
  }



  Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();
  OS_SelectableDumbPtr sceneSelection;
  OS_SelectableDumbPtr::Iterator selectItr = selectionAcrossAllScenes.Begin();
  OS_SelectableDumbPtr::Iterator selectEnd = selectionAcrossAllScenes.End();
  for ( ; selectItr != selectEnd; ++selectItr )
  {
    Luna::SceneNode* sceneNode = Reflect::ObjectCast< Luna::SceneNode >( *selectItr );
    if ( sceneNode && sceneNode->GetScene() == currentScene )
    {
      sceneSelection.Append( sceneNode );
    }
  }
  currentScene->Push( currentScene->GetSelection().SetItems( sceneSelection ) );
}

void LightingPanel::SelectInGui( )
{
  Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();

  NOC_ASSERT( currentScene );

  // load up the lighting scene, if it's not loaded
  m_SceneEditor->GetSceneManager()->GetLightingScene( true ); 

  m_Panel->Freeze();
  m_Panel->m_JobTree->Freeze();
  //clear the selection
  m_Panel->m_JobTree->UnselectAll(); 

  Selection& selection = currentScene->GetSelection();

  const OS_SelectableDumbPtr& selectedItems = selection.GetItems();

  m_Panel->m_JobTree->SelectItems( selectedItems );
  m_Panel->m_JobTree->Thaw();
  m_Panel->Thaw();


}

void LightingPanel::SelectNonRenderTargets(  )
{
  Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();

  NOC_ASSERT( currentScene );

  std::vector< Luna::SceneNode* > items;
  currentScene->GetAll< Luna::SceneNode >( items );

    // load up the lighting scene, if it's not loaded
  Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true ); 

  if( !lightingScene )
    return;

  std::vector< Content::LightingJob* > lightingJobs;

  lightingScene->GetAllPackages<  Content::LightingJob >( lightingJobs );

  OS_SelectableDumbPtr nonMembers;

  std::vector< Luna::SceneNode* >::iterator itor = items.begin();
  std::vector< Luna::SceneNode* >::iterator end = items.end();

  for( ; itor != end; ++itor )
  {
    Content::SceneNode* contentNode = Reflect::ObjectCast<Content::SceneNode>( (*itor)->GetPackage() );

    if (contentNode)
    {
      if( contentNode->GetAttribute( Reflect::GetType< Asset::BakedLightingAttribute >() ).ReferencesObject()
       || contentNode->GetAttribute( Reflect::GetType< Asset::SpatialLightingAttribute >() ).ReferencesObject() )
      {
        std::vector< Content::LightingJob* >::iterator jobItor = lightingJobs.begin();
        std::vector< Content::LightingJob* >::iterator jobEnd  = lightingJobs.end();
        
        bool isRenderTarget = false;
        for( ; jobItor != jobEnd; ++jobItor )
        {
          isRenderTarget = (*jobItor)->IsRenderTarget( currentScene->GetFileID(), (*itor)->GetID() );
          if( isRenderTarget )
          {
            break;
          }
        }
        if( !isRenderTarget )
          nonMembers.Append( *itor );
      }
    }
  }
  currentScene->Push( currentScene->GetSelection().SetItems( nonMembers ) ); 

}

void LightingPanel::SelectAllRenderTargets( )
{
  Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();

  NOC_ASSERT( currentScene );

  std::vector< Luna::SceneNode* > items;
  currentScene->GetAll< Luna::SceneNode >( items );

    // load up the lighting scene, if it's not loaded
  Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true ); 

  if( !lightingScene )
    return;

  std::vector< Content::LightingJob* > lightingJobs;

  lightingScene->GetAllPackages<  Content::LightingJob >( lightingJobs );

  OS_SelectableDumbPtr renderTargets;

  std::vector< Luna::SceneNode* >::iterator itor = items.begin();
  std::vector< Luna::SceneNode* >::iterator end = items.end();

  for( ; itor != end; ++itor )
  {
    Content::SceneNode* contentNode = Reflect::ObjectCast<Content::SceneNode>( (*itor)->GetPackage() );

    if (contentNode)
    {
      if( contentNode->GetAttribute( Reflect::GetType< Asset::BakedLightingAttribute >() ).ReferencesObject()
       || contentNode->GetAttribute( Reflect::GetType< Asset::SpatialLightingAttribute >() ).ReferencesObject() )
      {
        std::vector< Content::LightingJob* >::iterator jobItor = lightingJobs.begin();
        std::vector< Content::LightingJob* >::iterator jobEnd  = lightingJobs.end();
        
        for( ; jobItor != jobEnd; ++jobItor )
        {
          if( (*jobItor)->IsRenderTarget( currentScene->GetFileID(), (*itor)->GetID() ) )
          {
            renderTargets.Append( *itor );
            break;
          }
        }
      }

    }
  }
  currentScene->Push( currentScene->GetSelection().SetItems( renderTargets ) ); 

}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user presses the select button on the lighting panel.
// Selects the lighting job items in the scene (if loaded)
// 
void LightingPanel::OnButtonSelect( wxCommandEvent& args )
{
  int selectType = m_Panel->m_ChoiceSelectType->GetSelection();

  if( selectType < NumSelectionFuncs && m_SelectionFuncs[selectType] )
  {
    ((*this).*m_SelectionFuncs[selectType])();   
  }
}


///////////////////////////////////////////////////////////////////////////////
// Callback for when the user presses the view button on the lighting panel.
// for now just uncondenses the relevant lighting job file
// 
void LightingPanel::OnButtonView( wxCommandEvent& args )
{

  LightingSetupChecker* checker = new LightingSetupChecker( this, m_SceneEditor, "Lighting Data Viewer", 100, 100, 800, 500  );
  checker->CentreOnScreen();

  checker->Show();

  wxTextAttr whiteFont(*wxWHITE);
  whiteFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  whiteFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

  wxTextAttr redFont(*wxRED);
  redFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  redFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxTextAttr greenFont(*wxGREEN);
  greenFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  greenFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));


  std::map< Luna::LightingJob*, OS_SelectableDumbPtr > lightingJobSelections;

  OS_TreeItemIds::Iterator treeItr = m_Panel->m_JobTree->GetSelectedItems().Begin();
  OS_TreeItemIds::Iterator treeEnd = m_Panel->m_JobTree->GetSelectedItems().End();
  for ( ; treeItr != treeEnd; ++treeItr )
  {
    LightingTreeItem* item = static_cast< LightingTreeItem* >( m_Panel->m_JobTree->GetItemData( *treeItr ) );
    if ( item )
    {
      item->GetSelectableItems( lightingJobSelections[item->GetLightingJob()] );
    }
  }

  std::map< Luna::LightingJob*, OS_SelectableDumbPtr >::iterator itor = lightingJobSelections.begin();
  std::map< Luna::LightingJob*, OS_SelectableDumbPtr >::iterator end = lightingJobSelections.end();
  for( ; itor != end; ++itor )
  {
    Content::LightingJob* contentJob = itor->first->GetPackage< Content::LightingJob >();
    
    //clear and load/cache the condensed data if it exists
    contentJob->ClearCondensedData();
    LightingCondenser::CondensedDataPtr condensedData = contentJob->GetCondensedData();

    std::string condensedFile = contentJob->GetLightingDirectory() + contentJob->GetDataFileName();
    std::string condensedDir = condensedFile;
    FileSystem::StripExtension( condensedDir );
    condensedDir += "/";

    std::stringstream fileColumns;
    struct _stati64 fileStats;
    if( FileSystem::GetStats64( condensedFile, fileStats ) )
    {
      char modifiedPrint[32];

      // try to get a printer friendly version of the dates
      if ( _ctime64_s( modifiedPrint, 32, &fileStats.st_mtime ) == 0 ) 
      {
        fileColumns <<  modifiedPrint;
      }
      else
      {
        fileColumns <<  fileStats.st_mtime << "\n";
      }
    }

    OS_SelectableDumbPtr::Iterator selectItr = itor->second.Begin();
    OS_SelectableDumbPtr::Iterator selectEnd = itor->second.End();
    for ( ; selectItr != selectEnd; ++selectItr )
    {
      Luna::SceneNode* sceneNode = Reflect::ObjectCast< Luna::SceneNode >( *selectItr );
      if ( sceneNode )
      {
        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( sceneNode );
        if( entity )
        {
          Asset::Entity* contentEntity = entity->GetPackage< Asset::Entity >();
          Attribute::AttributeViewer< Asset::BakedLightingAttribute > bakedLighting( contentEntity, true );
          bakedLighting->CacheLightingPaths( contentEntity->m_ID, condensedDir );

          std::string entityName = contentEntity->GetName();
          entityName += ": ";

          checker->GetTextCtrl()->SetDefaultStyle( whiteFont );
          checker->GetTextCtrl()->AppendText( entityName.c_str() );
        
          if( !FileSystem::Exists( bakedLighting->m_LightmapFilePath[0]) || FileSystem::CompareModified( bakedLighting->m_LightmapFilePath[0], condensedFile ) == -1 )
          {
            if( condensedData.ReferencesObject() )
            {
              LightingCondenser::UnCondense( contentEntity->m_ID, condensedData, condensedDir );
            }
          }

          bool hasData = false;
          if( FileSystem::Exists( bakedLighting->m_LightmapFilePath[0]) && FileSystem::Exists( bakedLighting->m_LightmapFilePath[1] ) )
          {
            Windows::Execute( (std::string("imf_disp ") + bakedLighting->m_LightmapFilePath[0]).c_str() );
            Windows::Execute( (std::string("imf_disp ") + bakedLighting->m_LightmapFilePath[1]).c_str() ); 
            hasData = true;
          }
          if( FileSystem::Exists( bakedLighting->m_VertexFilePath ) )
          {           
            hasData = true;
          }
          if( hasData )
          {
            checker->GetTextCtrl()->SetDefaultStyle( greenFont );
            checker->GetTextCtrl()->AppendText( fileColumns.str().c_str() );
          }
          else
          {
             checker->GetTextCtrl()->SetDefaultStyle( redFont );
             checker->GetTextCtrl()->AppendText( "No rendered data!\n" );
          }
        }
        else if( sceneNode->HasType( Reflect::GetType<CubeMapProbe>() ) )
        {
          CubeMapProbe* lunaProbe = Reflect::DangerousCast< CubeMapProbe >( sceneNode );
          Content::CubeMapProbe* probe = lunaProbe->GetPackage< Content::CubeMapProbe >();

          try
          {
            Asset::CubeMapAsset* cubeMap = Asset::AssetClass::GetAssetClass< Asset::CubeMapAsset >( probe->m_CubeMapID );
            std::string imagePath;
            File::GlobalManager().GetPath( cubeMap->m_FileID, imagePath );
            Windows::Execute( std::string("imf_disp \"") + imagePath + "\"" );
          }
          catch (Nocturnal::Exception& e)
          {
            checker->GetTextCtrl()->SetDefaultStyle( redFont );
            checker->GetTextCtrl()->AppendText( e.what() );
          }
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// Callback for when the sort button is pressed.  Sorts the entire lighting
// job tree.
// 
void LightingPanel::OnButtonSort( wxCommandEvent& args )
{
  m_Panel->m_JobTree->Sort();
}

//////////////////////////////////////////////////////////////////////////////
// Callback for when the tab page is changed.
// 
void LightingPanel::OnTabChanged( wxNotebookEvent& args )
{
  args.Skip();

  // We have to pass in the newly selected tab page because the event handler hasn't
  // run to update the tab selection yet.
  UpdateButtons( m_Panel->m_Tabs->GetPage( args.GetSelection() ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when selection has changed in the job tree.
// 
void LightingPanel::OnJobTreeSelectionChanged( wxTreeEvent& args )
{
  args.Skip();
  UpdateButtons();
}

void LightingPanel::OnButtonCopyJob( wxCommandEvent& args )
{
  Luna::Scene* lightingZone = m_SceneEditor->GetSceneManager()->GetLightingScene();
  if ( lightingZone )
  {
    const OS_TreeItemIds selection = m_Panel->m_JobTree->GetSelectedItems();

    std::set< void* > jobSet;  

    Undo::BatchCommandPtr batch = new Undo::BatchCommand();
    OS_TreeItemIds::Iterator itr = selection.Begin();
    OS_TreeItemIds::Iterator end = selection.End();
    for ( ; itr != end; ++itr )
    {
      LightingTreeItem* item = (LightingTreeItem*)m_Panel->m_JobTree->GetItemData( *itr );
      Luna::LightingJob* job = item->GetLightingJob();
      if ( jobSet.insert( (void*)job ).second )
      {
        // make a cloned Luna::LightingJob
        Content::LightingJobPtr package = job->GetPackage< Content::LightingJob >();
        NOC_ASSERT( package.ReferencesObject() );      
        LightingJobPtr copy = new Luna::LightingJob( lightingZone, Reflect::ObjectCast<Content::LightingJob>( package->Clone() ) );

        Content::LightingJobPtr copyPackage = copy->GetPackage< Content::LightingJob >();
        NOC_ASSERT( copyPackage.ReferencesObject() );      

        // give it a fancy name and a new UniqueID::TUID
        copyPackage->m_DefaultName = "Copy_Of_" + copyPackage->GetName();
        UniqueID::TUID::Generate( copyPackage->m_ID );

        batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, lightingZone, copy ) );
      }
    }

    lightingZone->Push( batch );
  }
}

void LightingPanel::OnButtonUpdateShadowCasters( wxCommandEvent& args )
{
 
  Luna::Scene* lightingZone = m_SceneEditor->GetSceneManager()->GetLightingScene();

  if( !lightingZone )
  {
    wxMessageBox( "Could not Load the Lighting Zone!\nYou may need to associate a Lighting Zone  with your level in the Asset Editor.", "Error", wxCENTER | wxICON_ERROR | wxOK );
    return;
  }

  Luna::SceneManager* sceneManager = m_SceneEditor->GetSceneManager();
  Luna::Scene* worldScene = sceneManager->GetRootScene();

  // this would be weird...
  NOC_ASSERT(worldScene);

  std::vector<Content::Zone*> contentZones;
  worldScene->GetAllPackages< Content::Zone >( contentZones );

  typedef std::multimap< tuid, tuid > M_ShadowCasters;

  M_ShadowCasters potentialShadowCastersTemp;
  std::vector<Content::Zone*>::iterator zoneItor = contentZones.begin();
  std::vector<Content::Zone*>::iterator zoneEnd  = contentZones.end();

  Console::Print( "Loading potential shadow casters from...\n" );
  for( ; zoneItor != zoneEnd; ++zoneItor )
  {

    std::vector<Asset::Entity*> entities;
    // see if the zone is currently loaded
    Luna::Scene* scene = m_SceneEditor->GetSceneManager()->GetScene( (*zoneItor)->m_FileID );
    if( scene )
    {
      scene->GetAllPackages< Asset::Entity >( entities, Reflect::GetType< Asset::BakedLightingAttribute >(), true );
      std::vector<Asset::Entity*>::iterator entityItor = entities.begin();
      std::vector<Asset::Entity*>::iterator entityEnd  = entities.end();
      for( ; entityItor != entityEnd; ++entityItor )
      {
        potentialShadowCastersTemp.insert( M_ShadowCasters::value_type( (*zoneItor)->m_FileID, (*entityItor)->m_ID ) );
      }
    }
    else
    {
      Reflect::V_Element zoneElements;

      std::string zonePath = File::GlobalManager().GetPath( (*zoneItor)->m_FileID );

      Console::Print( "\to %s\n", zonePath.c_str() );

      if( FileSystem::Exists( zonePath ) )
      {
        Reflect::Archive::FromFile( zonePath, zoneElements );

        Reflect::V_Element::iterator elemItor = zoneElements.begin();
        Reflect::V_Element::iterator elemEnd  = zoneElements.end();
        for( ; elemItor != elemEnd; ++elemItor )
        {
          if( (*elemItor)->HasType( Reflect::GetType< Asset::Entity >() ) )
          {
            Asset::Entity* node = Reflect::DangerousCast< Asset::Entity >( *elemItor );
            if( node->GetAttribute( Reflect::GetType< Asset::BakedLightingAttribute >() ) )
            {
              potentialShadowCastersTemp.insert( M_ShadowCasters::value_type( (*zoneItor)->m_FileID, node->m_ID ) );
            }
          }
        }
      }
    }
  }


  const OS_TreeItemIds selection = m_Panel->m_JobTree->GetSelectedItems();

  Content::S_LightingJob jobSet;  

  // for each selected item in the lighting job panel find the parent lighting job
  OS_TreeItemIds::Iterator selectItor = selection.Begin();
  OS_TreeItemIds::Iterator selectEnd  = selection.End();
  for ( ; selectItor != selectEnd; ++selectItor )
  {
    LightingTreeItem* item = (LightingTreeItem*)m_Panel->m_JobTree->GetItemData( *selectItor );
    Luna::LightingJob* job = item->GetLightingJob();

    Content::LightingJobPtr contentLightingJob = job->GetPackage< Content::LightingJob >();
    jobSet.insert( contentLightingJob );
  }

  Content::S_LightingJob::iterator itor = jobSet.begin();
  Content::S_LightingJob::iterator end  = jobSet.end();

  bool isEditable = true;
  for( ; itor != end; ++itor )
  {
    if( isEditable )
    {
      isEditable = lightingZone->IsEditable();
      if( isEditable )
      {
        M_ShadowCasters potentialShadowCasters = potentialShadowCastersTemp;

        std::string shadowCasterFilePath = (*itor)->GetLightingDirectory() + (*itor)->GetShadowCastersFileName();

        if( !FileSystem::Exists( shadowCasterFilePath ) )
          continue;

        Console::Print( "Updating shadow casters for: %s\n", (*itor)->GetName().c_str() );

        std::ifstream shadowCastersFile( shadowCasterFilePath.c_str() );

        S_tuid seenIDs;
        std::string line;
        while( !shadowCastersFile.eof() )
        {
          shadowCastersFile >> line;

          tuid id = _strtoui64( line.c_str(), NULL, 16 );

          if( seenIDs.insert( id ).second )
          {
            M_ShadowCasters::iterator shadowItor = potentialShadowCasters.begin();
            M_ShadowCasters::iterator shadowEnd  = potentialShadowCasters.end();

            for( ; shadowItor != shadowEnd; ++shadowItor )
            {
              // see if this zone is excluded (except for rendertargets and environments)
              if( (*itor)->IsExcludedZone( shadowItor->first ) )
              {
                potentialShadowCasters.erase( shadowItor->first );
                break;
              }
              else if( shadowItor->second == id )
              {
                //std::cout << line << std::endl;
                potentialShadowCasters.erase( shadowItor );
                break;             
              }
            }
          }       
        }
        shadowCastersFile.close();

        // clear out existing excluded shadowcasters
        Content::M_LightingJobZoneItems::iterator itemItor = (*itor)->m_JobElements.begin();
        Content::M_LightingJobZoneItems::iterator itemEnd  = (*itor)->m_JobElements.end();
        for( ; itemItor != itemEnd; ++itemItor )
        {
          itemItor->second->m_ExcludedShadowCasters.clear();
        }

        M_ShadowCasters::iterator shadowItor = potentialShadowCasters.begin();
        M_ShadowCasters::iterator shadowEnd  = potentialShadowCasters.end();
        for( ; shadowItor != shadowEnd; ++shadowItor )
        {
          Content::M_LightingJobZoneItems::iterator findItor = (*itor)->m_JobElements.find( shadowItor->first );
          if( findItor == (*itor)->m_JobElements.end() )
          {
            (*itor)->m_JobElements.insert(Content::M_LightingJobZoneItems::value_type( shadowItor->first, new Content::LightingJobZoneItems( shadowItor->first ) ) );
          }

          (*itor)->m_JobElements[ shadowItor->first ]->m_ExcludedShadowCasters.insert( shadowItor->second );
        }
      }
    }
  }
}


void LightingPanel::OnButtonCheckSetup( wxCommandEvent& args )
{

  Luna::Scene* worldScene = m_SceneEditor->GetSceneManager()->GetRootScene();

  // this would be weird...
  NOC_ASSERT(worldScene);

  

  LightingSetupChecker* checker = new LightingSetupChecker( this, m_SceneEditor, "Lighting Setup Checker", 100, 100, 800, 500  );
  checker->CentreOnScreen();

  checker->Show();

  wxTextAttr whiteFont(*wxWHITE);
  whiteFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  whiteFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

  wxTextAttr redFont(*wxRED);
  redFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  redFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  wxTextAttr greenFont(*wxGREEN);
  greenFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  greenFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

  // wtf, no yellow?
  static const wxColor yellow ( 0xff, 0xff, 0x00 );
  static const wxColor* wxYELLOW (&yellow);

  static const wxColor orange( 0xff, 0x80, 0x00 );
  static const wxColor* wxORANGE (&orange);


  wxTextAttr yellowFont(*wxYELLOW);
  greenFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  greenFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

  wxTextAttr orangeFont(*wxORANGE);
  greenFont.SetFlags(wxTEXT_ATTR_FONT_FACE);
  greenFont.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

  V_ZoneDumbPtr zones;
  worldScene->GetAll< Zone >( zones );

  S_tuid uniqueClassIds;

  V_ZoneDumbPtr::iterator zoneItor = zones.begin();
  V_ZoneDumbPtr::iterator zoneEnd  = zones.end();
  for ( ; zoneItor != zoneEnd; ++zoneItor )
  {
    Luna::Scene* scene = m_SceneEditor->GetSceneManager()->GetScene( (*zoneItor)->GetFileID() );
    if( scene )
    {
      std::vector<Asset::Entity*> entities;
      scene->GetAllPackages< Asset::Entity >( entities );

      std::vector<Asset::Entity*>::iterator entityItor = entities.begin();
      std::vector<Asset::Entity*>::iterator entityEnd  = entities.end();
      for( ; entityItor != entityEnd; ++entityItor )
      {
        std::string entityName;
        std::string artName;
        std::string errorMsg;

        bool fine = true;
        bool warningOnly = true;

        try
        {
          //if this is the first time we've seen this entity class, do a bunch of checks on it
          if( uniqueClassIds.insert( (*entityItor)->GetEntityAssetID() ).second )
          {
            Asset::EntityAssetPtr entityClass = (*entityItor)->GetEntityAsset();
            if( !entityClass.ReferencesObject() )
            {
              Console::Warning( "Unable to load entity class with UID: "TUID_HEX_FORMAT"", (*entityItor)->GetEntityAssetID() );
              continue;
            }
            Asset::EngineType type = entityClass->GetEngineType();

            entityName = entityClass->GetFullName();
            entityName += "\n";    
            checker->GetTextCtrl()->SetDefaultStyle(whiteFont);

            if( type == Asset::EngineTypes::Tie || type == Asset::EngineTypes::Shrub || type == Asset::EngineTypes::Ufrag )
            {
              if( !entityClass->ContainsAttribute( Reflect::GetType< Asset::BakedLightingAttribute >() ) )
              {
                checker->GetTextCtrl()->AppendText( entityName.c_str() );
                checker->GetTextCtrl()->SetDefaultStyle(redFont);
                checker->GetTextCtrl()->AppendText( "\to Does NOT contain the Asset::BakedLightingAttribute (aborting checks for this entity class)!\n" );
                fine = false;
                warningOnly = false;
                continue;
              }
            }
            else
            {
              continue;
            }
            BitArray errorFlags( SetupCheckerStates::NumSetupCheckerStates );

            if( entityClass->ContainsAttribute( Reflect::GetType< Asset::BakedLightingAttribute >() ) )
            {
              Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( entityClass );

              if( artFile.Valid() )
              {
                artName = artFile->GetFilePath();
                Content::Scene scene( FinderSpecs::Content::STATIC_DECORATION.GetExportFile( artName, artFile->m_FragmentNode ) );

                FileSystem::Win32Name( artName );
                artName += "\n";

                if( !scene.MeshesAreClassifiedForLighting() )
                {
                  errorMsg += "\to Some or all meshes are not classified as Lightmapped or VertexLit!\n";
                  fine = false;
                  warningOnly = false;
                  checker->m_ErrorEntityClasses.insert( entityClass->m_AssetClassID );
                  errorFlags[SetupCheckerStates::UnclassifiedMeshes] = true;
                }
                if( !scene.LightmapUVsExist() )
                {
                  errorMsg += "\to Lightmap UV's do NOT exist for some or all lightmapped meshes!\n";
                  fine = false;
                  warningOnly = false;
                  checker->m_ErrorEntityClasses.insert( entityClass->m_AssetClassID );
                  errorFlags[SetupCheckerStates::LightmapUVsMissing] = true;
                }
                if( !scene.LightmapUVsInRange() )
                {
                  errorMsg += "\to Lightmap UV's are out of range for some or all lightmapped meshes!\n";
                  fine = false;
                  checker->m_ErrorEntityClasses.insert( entityClass->m_AssetClassID );
                  errorFlags[SetupCheckerStates::LightmapUVsOutOfRange] = true;
                }   
                if( scene.LightmapUVsOverlap() )
                {
                  errorMsg += "\to Lightmap UV's overlap for some or all lightmapped meshes!\n";
                  if( scene.HasSharedLightmapOverlays() )
                    errorMsg += "\to This may be because the object has shared lightmap overlays.\n";
                  else
                    checker->m_ErrorEntityClasses.insert( entityClass->m_AssetClassID );
                  fine = false;
                  errorFlags[SetupCheckerStates::LightmapUVsOverlap] = true;
                } 
                if( scene.MeshesAreClassifiedForLighting() 
                  && scene.LightmapUVsExist()
                  && scene.UVSurfaceArea( Content::UVSetTypes::Lightmap ) <= 0.3f )
                {
                  std::string manifestFile = FinderSpecs::Content::MANIFEST_DECORATION.GetExportFile( artFile->GetFilePath(), artFile->m_FragmentNode );
                  Asset::EntityManifestPtr assetManifest = Reflect::Archive::FromFile< Asset::EntityManifest >(manifestFile);
                  if (!assetManifest.ReferencesObject())
                  {
                    return;
                  }

                  if( assetManifest->m_LightMapped )
                  {
                    errorMsg += "\to Using less than 30% of available lightmap UV space!\n";
                    fine = false;
                    errorFlags[SetupCheckerStates::LowUVSurfaceArea] = true;
                  }                  
                }                  
              }
              else
              {    
                checker->GetTextCtrl()->AppendText( entityName.c_str() );
                checker->GetTextCtrl()->SetDefaultStyle(redFont);
                checker->GetTextCtrl()->AppendText( "\to Does NOT have an associated maya art file (aborting checks for this entity class)!\n" );
                fine = false;
                continue;
              }
            }

            if( !fine )
            {
              
              wxListItem item;
              item.SetId( checker->GetListCtrl()->GetItemCount() );
              item.SetMask( wxLIST_MASK_TEXT|wxLIST_MASK_DATA );
              item.SetText( entityClass->GetShortName().c_str() );
              
              SetupCheckerItem* data = new SetupCheckerItem;
              data->m_AssetID = entityClass->m_AssetClassID;
              data->m_AssetName = entityClass->GetShortName();
              data->m_ErrorFlags = errorFlags;
              checker->m_SetupCheckerItems.push_back( data );

              item.SetData( (void*)data );
              long index = checker->GetListCtrl()->InsertItem( item );

              checker->GetTextCtrl()->AppendText( entityName.c_str() );
              checker->GetTextCtrl()->SetDefaultStyle(yellowFont);
              checker->GetTextCtrl()->AppendText( artName.c_str() );

              if( warningOnly )
              {
                checker->GetListCtrl()->SetItemBackgroundColour( index, *wxYELLOW );
                checker->GetTextCtrl()->SetDefaultStyle(orangeFont);
              }
              else
              {
                checker->GetListCtrl()->SetItemBackgroundColour( index, *wxRED );  
                checker->GetTextCtrl()->SetDefaultStyle(redFont);
              }
              checker->GetTextCtrl()->AppendText( errorMsg.c_str() );
              checker->GetTextCtrl()->AppendText( "\n" );         

              wxListItem item2;
              item2.SetId( index );
              item2.SetMask( wxLIST_MASK_TEXT );
              item2.SetText( errorMsg.c_str() );
              item2.SetColumn(1);
              checker->GetListCtrl()->SetItem( item2 );
              
              
            } 
            else
            {
              wxListItem item;
              item.SetId( checker->GetListCtrl()->GetItemCount() );
              item.SetMask( wxLIST_MASK_TEXT|wxLIST_MASK_DATA );
              item.SetText( entityClass->GetShortName().c_str() );
              
              SetupCheckerItem* data = new SetupCheckerItem;
              data->m_AssetID = entityClass->m_AssetClassID;
              data->m_AssetName = entityClass->GetShortName();
              data->m_ErrorFlags = errorFlags;
              checker->m_SetupCheckerItems.push_back( data );
              
              item.SetData( (void*)data );
              checker->GetListCtrl()->InsertItem( item );
              
            }    
          }    
        }
        catch( Nocturnal::Exception& e )
        {
          checker->GetTextCtrl()->AppendText( entityName.c_str() );
          //stream << "\to Unable to perform checks for this entity class: " << e.what() << std::endl;
          checker->GetTextCtrl()->SetDefaultStyle(redFont);

          std::string error = "\to Unable to perform checks for this entity class: \n";
          error += e.what();
          checker->GetTextCtrl()->AppendText( error.c_str() );
          fine = false;
        }
      }
    }
  }
}


void LightingPanel::OnChoiceSelectRenderType( wxCommandEvent& args )
{
  Content::RenderMode renderType = (Content::RenderMode)m_Panel->m_ChoiceRenderType->GetSelection();

  // if Preview or RenderCubemap
  if( renderType == Content::RenderModes::Preview || renderType == Content::RenderModes::RenderCubemap )
  {
    //set render location to local
    m_Panel->m_ChoiceRenderLocation->SetSelection( 1 );
    m_Panel->m_ChoiceRenderLocation->Disable();
  }
  else
  {
    m_Panel->m_ChoiceRenderLocation->Enable();
  }
}


void LightingPanel::UncondenseAllLightingJobs()
{
  // get all the lighting jobs
  std::vector<Content::LightingJob*> lightingJobs;
  Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
  if( lightingScene )
    lightingScene->GetAllPackages< Content::LightingJob >( lightingJobs );


  std::vector<Content::LightingJob*>::iterator itor = lightingJobs.begin();
  std::vector<Content::LightingJob*>::iterator end  = lightingJobs.end();

  for( ; itor != end; ++itor )
  {
    (*itor)->ClearCondensedData();
    (*itor)->Uncondense();
  }

}

void LightingPanel::OnButtonSetTPM( wxCommandEvent& args )
{
  Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();
  if( !currentScene )
    return;

  std::vector< Content::Curve* > curves;
  Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
  if( !lightingScene )
  {
    wxMessageBox( "Could not Load the Lighting Zone!\nYou may need to associate a Lighting Zone  with your level in the Asset Editor.", "Error", wxCENTER | wxICON_ERROR | wxOK );
    return;
  }

  lightingScene->GetAllPackages< Content::Curve >( curves );

  if( curves.empty() )
  {
    wxMessageBox( "Could not find a player-path curve in Lighting Zone!", "Error", wxCENTER | wxICON_ERROR | wxOK );
    return;
  }

  std::vector< Luna::Entity* > entities;
  currentScene->GetAll< Luna::Entity >( entities );

  std::vector< Luna::Entity* >::iterator itor = entities.begin();
  std::vector< Luna::Entity* >::iterator end  = entities.end();

  double tpmPerMeter;
  double tpmMax;

  wxString value = m_Panel->m_ValueTPMMax->GetValue();
  value.ToDouble( &tpmMax );

  value = m_Panel->m_ValueTPMStep->GetValue();
  value.ToDouble( &tpmPerMeter );

  for( ; itor != end; ++itor )
  {
    Asset::Entity* assetEntity = (*itor)->GetPackage< Asset::Entity >();

    if( !assetEntity )
      continue;

    Attribute::AttributeViewer< Asset::BakedLightingAttribute > bakedLighting( assetEntity );

    if( !bakedLighting.Valid() )
      continue;

    Math::Vector3 transform; Math::EulerAngles dummy2; Math::Scale dummy;
    (*itor)->GetGlobalTransform().Decompose( dummy, dummy2, transform );

    std::vector< Content::Curve* >::iterator curveItor = curves.begin();
    std::vector< Content::Curve* >::iterator curveEnd  = curves.end();
    
    f32 minDist = (*curveItor++)->DistanceToCurve( transform );

    for( ; curveItor != curveEnd; ++curveItor )
    {
      f32 dist = (*curveItor)->DistanceToCurve( transform );
      if( dist < minDist )
        minDist = dist;
    }

    f32 tpm = tpmMax - (minDist*tpmPerMeter);

    if( tpm < 0.0f )
      tpm = 1.0f;

    (*itor)->SetSetRuntimeLightmap( true );
    (*itor)->SetTexelsPerMeter( tpm );
  }
}

void LightingPanel::OnButtonViewUVs( wxCommandEvent& args )
{
  LightmapUVViewer* viewer = new LightmapUVViewer( this, "Lightmap UV Viewer", 100, 100, 600, 650 );
  viewer->Enable();
  viewer->CentreOnScreen();
  viewer->Show();  
}



void LightingPanel::OnButtonUpdateLitColumn( wxCommandEvent& args )
{
  Luna::Scene* worldScene = m_SceneEditor->GetSceneManager()->GetRootScene();

  // this would be weird...
  NOC_ASSERT(worldScene);

  std::vector<Content::LightingJob*> lightingJobs;
  Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
  if( lightingScene )
    lightingScene->GetAllPackages< Content::LightingJob >( lightingJobs );

  // clear any currently loaded condensed data
  std::vector<Content::LightingJob*>::iterator itor = lightingJobs.begin();
  std::vector<Content::LightingJob*>::iterator end  = lightingJobs.end();

  for( ; itor != end; ++itor )
  {
    (*itor)->ClearCondensedData();
  }

  V_ZoneDumbPtr zones;
  worldScene->GetAll< Zone >( zones );

  m_Panel->m_LightableInstanceList->Freeze();
 
  V_ZoneDumbPtr::iterator zoneItor = zones.begin();
  V_ZoneDumbPtr::iterator zoneEnd  = zones.end();
  for ( ; zoneItor != zoneEnd; ++zoneItor )
  {
    Luna::Scene* scene = m_SceneEditor->GetSceneManager()->GetScene( (*zoneItor)->GetFileID() );
    if( scene )
    {
      std::vector<Luna::Entity*> entities;
      scene->GetAll< Luna::Entity >( entities );

      std::vector<Luna::Entity*>::iterator entityItor = entities.begin();
      std::vector<Luna::Entity*>::iterator entityEnd  = entities.end();
      for( ; entityItor != entityEnd; ++entityItor )
      {
        Asset::Entity* contentEntity = (*entityItor)->GetPackage< Asset::Entity >();
        Attribute::AttributeViewer< Asset::BakedLightingAttribute > bakedLighting( contentEntity, true );

        if( bakedLighting.Valid() )
        {
          std::vector<Content::LightingJob*>::iterator jobItor = lightingJobs.begin();
          std::vector<Content::LightingJob*>::iterator jobEnd  = lightingJobs.end();

          bool hasData = false;
          for ( ; jobItor != jobEnd; ++jobItor )
          {
            if( (*jobItor)->ContainsRenderTarget( (*entityItor)->GetID() ) )
            {
              LightingCondenser::CondensedDataPtr& condensedData = (*jobItor)->GetCondensedData();

             
              LightingCondenser::LightmapData data;
              if( condensedData->GetLightmapData( (*entityItor)->GetID(), data ) )
              {
                 if( data.m_TGAFile1Size && data.m_TGAFile2Size )
                   hasData = true;
                 if( data.m_VLDFileSize )
                   hasData = true;
                 else
                   hasData = false;                 
                 if( hasData )
                   break;
              } 
            }
          }
          m_Panel->m_LightableInstanceList->UpdateLit( *entityItor, hasData );  
        }
      }
    }
  }
  m_Panel->m_LightableInstanceList->Thaw();
}



void LightingPanel::OnButtonSyncLighting( wxCommandEvent& args )
{
  const OS_TreeItemIds selection = m_Panel->m_JobTree->GetSelectedItems();
  Content::S_LightingJob jobSet;  

  // for each selected item in the lighting job panel, find it's lighting job

  OS_TreeItemIds::Iterator itr = selection.Begin();
  OS_TreeItemIds::Iterator end = selection.End();
  for ( ; itr != end; ++itr )
  {
    LightingTreeItem* item = (LightingTreeItem*)m_Panel->m_JobTree->GetItemData( *itr );
    LightingJob* job = item->GetLightingJob();

    Content::LightingJobPtr contentLightingJob = job->GetPackage< Content::LightingJob >();
    jobSet.insert( contentLightingJob );
  }

  Content::S_LightingJob::iterator jobItor   = jobSet.begin();
  Content::S_LightingJob::iterator jobEnd  = jobSet.end();

  for( ; jobItor != jobEnd; ++jobItor )
  {
    std::string jobDataFile = (*jobItor)->GetLightingDirectory() + (*jobItor)->GetDataFileName();

    try
    {
      RCS::File rcsFile( jobDataFile );
      rcsFile.Sync();
    }
    catch( RCS::Exception& e )
    {
      Console::Warning("Unable to sync file: %s. Reason: %s", jobDataFile.c_str(), e.what() );
    }
  }
}

void CheckLightmapUVAndExport( const V_EntityDumbPtr& entities )
{
  S_tuid uniqueClassIds;
  S_tuid entityClassesToExport;

  S_tuid uniqueMayaFileIds;

  V_EntityDumbPtr::const_iterator entityItor = entities.begin();
  V_EntityDumbPtr::const_iterator end  = entities.end();
  
  for( ; entityItor != end; ++entityItor )
  {
    Asset::Entity* entity = (*entityItor)->GetPackage< Asset::Entity >();

    // if we haven't seen this entity class before
    if( uniqueClassIds.insert( entity->GetEntityAssetID() ).second )
    {
      Asset::EntityAssetPtr entityClass = entity->GetEntityAsset();
      if( !entityClass.ReferencesObject() )
      {
        Console::Warning( "Unable to load entity class with UID: "TUID_HEX_FORMAT"", entity->GetEntityAssetID() );
        continue;
      }
      EngineType type = entityClass->GetEngineType();

      if( type == EngineTypes::Tie || type == EngineTypes::Shrub || type == EngineTypes::Ufrag )
      {
        if( !entityClass->ContainsAttribute( Reflect::GetType< BakedLightingAttribute >() ) )
        {
          continue;
        }

        AttributeViewer<ArtFileAttribute> artFile( entityClass );
        if( artFile.Valid() )
        {
          // if we haven't seen this maya file before
          if( uniqueMayaFileIds.insert( artFile->GetFileID() ).second )
          {
            std::string staticFilePath = FinderSpecs::Content::STATIC_DECORATION.GetExportFile( artFile->GetFilePath(), artFile->m_FragmentNode );
            if( FileSystem::Exists( staticFilePath ) )
            {
              Content::Scene scene( staticFilePath );

              if( !scene.MeshesAreClassifiedForLighting() )
              {
                entityClassesToExport.insert( entityClass->m_AssetClassID );
              }
              else if( !scene.LightmapUVsExist() )
              {
                entityClassesToExport.insert( entityClass->m_AssetClassID );
              }
              else if( !scene.LightmapUVsInRange() )
              {
                entityClassesToExport.insert( entityClass->m_AssetClassID );
              }   
              else if( scene.LightmapUVsOverlap() )
              {
                entityClassesToExport.insert( entityClass->m_AssetClassID );             
              } 
            }            
          }
        }
      }
    }
  }

  size_t numToExport = entityClassesToExport.size();
  
  if( numToExport )
  {
    std::stringstream stream;
    stream << numToExport << " art files need lighting setup and export.";

    if( numToExport > 10 )
      stream << " This may take a few minutes, depending on the complexity of the art being setup.";
    else if( numToExport > 20 )
      stream << " This may take awhile, depending on the complexity of the art being setup";
    else if( numToExport > 30 )
      stream << " This will take awhile.";

    stream << " Proceed?";
    if( IDYES ==  ::MessageBox( NULL, stream.str().c_str(), "Lighting Setup", MB_YESNO ) )
    {
      Luna::ExportAssets( entityClassesToExport, NULL, false, false, true );
    }
  }  
}

void LightingPanel::OnButtonQuickSetup( wxCommandEvent& args )
{
  SceneManager* sceneManager = m_SceneEditor->GetSceneManager();

  if( sceneManager )
  {
    Scene* rootScene = sceneManager->GetRootScene();
    Scene* currentScene = sceneManager->GetCurrentScene();

    if( rootScene && currentScene )
    {
      std::vector< Luna::Entity* > entities;
      currentScene->GetAll< Luna::Entity >( entities, &LightingJob::IsLightable );

      if( entities.empty() )
      {
        ::MessageBox( NULL, "There are no renderable instances in the currently selected Zone!", "No Instances!", MB_OK );
        return;
      }

      if( IDYES ==  ::MessageBox( NULL, "Check Art for Lighting Setup? This may take a few minutes.\nIf you have recently done this, you can probably skip.", "Lighting Setup", MB_YESNO ) )
      {
        CheckLightmapUVAndExport(entities);            
      }
    }
  }
}

void LightingPanel::OnButtonQuickRender( wxCommandEvent& args )
{
  SceneManager* sceneManager = m_SceneEditor->GetSceneManager();

  if( sceneManager )
  {
    //Content::QuickRenderMode renderMode = (Content::QuickRenderMode)m_Panel->m_ChoiceQuickRenderType->GetSelection();

    Scene* rootScene = sceneManager->GetRootScene();
    Scene* currentScene = sceneManager->GetCurrentScene();
  
    if( rootScene && currentScene )
    {
      std::vector< Luna::Entity* > entities;
      currentScene->GetAll< Luna::Entity >( entities, &LightingJob::IsLightable );

      OS_SelectableDumbPtr        lightsLinkedToVolume;
      std::vector< LightingVolume* > volumes;
      rootScene->GetAll< LightingVolume >( volumes );

      Selection& selection = currentScene->GetSelection();
      const OS_SelectableDumbPtr& selectedItems = selection.GetItems();
      bool renderSelected = !selectedItems.Empty();

      if( !renderSelected && entities.empty() )
      {
        ::MessageBox( NULL, "There are no renderable instances in the currently selected Zone!", "No Instances!", MB_OK );
        return;
      }

      V_LightDumbPtr lights;
      if( !volumes.empty() )
      {    
        volumes[0]->GetLinkedLights( lights );
  
        V_LightingJobDumbPtr lightingJobs;
        currentScene->GetAll< LightingJob >( lightingJobs );

        Content::LightingJobPtr lightingJob;
        LightingJob* lunaLightingJob;

        // try to find an existing 'quick render' job
        if( !lightingJobs.empty() )
        {
          V_LightingJobDumbPtr::iterator itor = lightingJobs.begin();
          V_LightingJobDumbPtr::iterator end  = lightingJobs.end();

          for( ; itor != end; ++itor )
          {
            Content::LightingJob* package = (*itor)->GetPackage< Content::LightingJob >();
            if( package->m_QuickRender )
            {
              lightingJob = package;
              lunaLightingJob = *itor;
              break;
            }
          }
        }

        // if not, create one
        if( !lightingJob.ReferencesObject() )
        {
          if( currentScene->IsEditable() )
          {
            lightingJob = new Content::LightingJob();
            lightingJob->m_QuickRender = true;
            lunaLightingJob = new LightingJob( currentScene, lightingJob );
            lunaLightingJob->SetName( "Quick_Render_" );
            currentScene->AddObject( lunaLightingJob );
          }          
        }

        if( lightingJob.ReferencesObject() )
        {          
          Content::V_SceneNode loadedRenderItems;
   
          f32 desiredTpm = 10.0f;
          bool setTPM = true;
      
          std::vector< Luna::Entity* >::iterator itor = entities.begin();
          std::vector< Luna::Entity* >::iterator end  = entities.end ();

          for( ; itor != end; ++itor )
          {
            (*itor)->GetPackage<Asset::Entity>()->m_Selected = false;

            if( renderSelected )
            {
              if( selectedItems.Contains( *itor ) )
              {
                if( setTPM )
                {
                  (*itor)->SetTexelsPerMeterAndRuntime( desiredTpm, true );
                  (*itor)->Pack();
                }
                (*itor)->GetPackage<Asset::Entity>()->m_Selected = true;
                lightingJob->m_InstanceRenders.insert( (*itor)->GetID() );
              }
            }
            else
            {
              if( setTPM )
              {
                (*itor)->SetTexelsPerMeterAndRuntime( desiredTpm, true );
                (*itor)->Pack();
              }
              (*itor)->GetPackage<Asset::Entity>()->m_Selected = true;              
            }
            lunaLightingJob->AddRenderTarget( currentScene->GetFileID(), (*itor) );
            loadedRenderItems.push_back( (*itor)->GetPackage<Asset::Entity>() );     
          }

          loadedRenderItems.push_back( new Content::Camera() );

          V_LightDumbPtr::iterator lightItor = lights.begin();
          V_LightDumbPtr::iterator lightEnd  = lights.end();
          for( ; lightItor != lightEnd; ++lightItor )
            loadedRenderItems.push_back( (*lightItor)->GetPackage< Content::Light >() );

          lightingJob->m_RenderLocation = Content::RenderLocations::Local;
          S_tuid dummyParameter;
          ::LightingJob::RenderLightingJob( *lightingJob, loadedRenderItems, dummyParameter );
        }
        else
        {
          ::MessageBox( NULL, "Could not create 'quick render' lighting job, probably because the zone could not be checked out to you!", "Quick Render Failure", MB_OK | MB_ICONEXCLAMATION);
        }
        
      }      
    }
  }
}


