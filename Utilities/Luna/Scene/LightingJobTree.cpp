#include "Precompile.h"
#include "LightingJobTree.h"

#include "JobItem.h"
#include "Light.h"
#include "LightingJob.h"
#include "Scene.h"
#include "SceneEditor.h"
#include "SceneManager.h"

#include "Console/Console.h"
#include "Asset/LevelAsset.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingJobTree::LightingJobTree( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: UIToolKit::SortTreeCtrl( parent, id, pos, size, style, validator, name )
, m_SceneEditor( NULL )
, m_SceneManager( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingJobTree::~LightingJobTree()
{
  Disconnect( GetId(), wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( LightingJobTree::OnSelectionChanging ), NULL, this );
  Disconnect( GetId(), wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( LightingJobTree::OnDeleteItem ), NULL, this );
  Disconnect( GetId(), wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( LightingJobTree::OnBeginLabelEdit ), NULL, this );
  Disconnect( GetId(), wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( LightingJobTree::OnEndLabelEdit ), NULL, this );
  Disconnect( GetId(), wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( LightingJobTree::OnContextMenu ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Call after constructing the tree item to prepare it for first use.  Only
// call this function once.
// 
void LightingJobTree::Init( SceneEditor* sceneEditor )
{
  NOC_ASSERT( !m_SceneEditor );

  AddRoot( "INVISIBLE_ROOT" );

  // Hook up application event callbacks.
  m_SceneEditor = sceneEditor;
  m_SceneManager = m_SceneEditor->GetSceneManager();
  m_SceneManager->AddSceneAddedListener( SceneChangeSignature::Delegate ( this, &LightingJobTree::SceneAdded ) );
  m_SceneManager->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LightingJobTree::SceneRemoving ) );

  // Hook up UI callbacks.
  Connect( GetId(), wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( LightingJobTree::OnSelectionChanging ), NULL, this );
  Connect( GetId(), wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( LightingJobTree::OnDeleteItem ), NULL, this );
  Connect( GetId(), wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( LightingJobTree::OnBeginLabelEdit ), NULL, this );
  Connect( GetId(), wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( LightingJobTree::OnEndLabelEdit ), NULL, this );
  Connect( GetId(), wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( LightingJobTree::OnContextMenu ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the selected items in the tree and adds commands to the 
// specified batch for removing those items from the lighting job.
// 
Undo::CommandPtr LightingJobTree::RemoveJobMembers( const OS_SelectableDumbPtr& itemsToRemove )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();
  OS_TreeItemIds::Iterator itr = m_SelectedItems.Begin();
  OS_TreeItemIds::Iterator end = m_SelectedItems.End();
  for ( ; itr != end; ++itr )
  {
    LightingTreeItem* data = static_cast< LightingTreeItem* >( GetItemData( *itr ) );
    if ( data )
    {
      batch->Push( data->GetRemoveCommand( itemsToRemove ) );
    }
  }

  if ( batch->IsEmpty() )
  {
    batch = NULL;
  }
  else
  {
    batch->Redo();
  }

  return batch;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a root tree node to represent the specified lighting job.
// 
void LightingJobTree::AddJob( Luna::LightingJob* job )
{
  JobItem* itemData = new JobItem( this, job );
  wxTreeItemId jobItem = AppendItem( GetRootItem(), job->GetName().c_str(), -1, -1, itemData );
  itemData->Load();
  job->AddNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &LightingJobTree::NodeNameChanged ) );
  m_JobTreeItems.insert( M_JobsToItems::value_type( job, jobItem ) );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the tree node representing the specified lighting job.
// 
void LightingJobTree::RemoveJob( Luna::LightingJob* job )
{
  M_JobsToItems::iterator found = m_JobTreeItems.find( job );
  if ( found != m_JobTreeItems.end() )
  {
    job->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &LightingJobTree::NodeNameChanged ) );
    wxTreeItemId itemToRemove = found->second;
    Delete( itemToRemove );
    m_JobTreeItems.erase( found );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes all lighting jobs that belong to the specified scene.
// 
void LightingJobTree::RemoveJobs( Luna::Scene* scene )
{
  Freeze();
  HMS_TypeToSceneNodeTypeDumbPtr::const_iterator found = scene->GetNodeTypesByType().find( Reflect::GetType<Luna::LightingJob>() );
  if ( found != scene->GetNodeTypesByType().end() )
  {
    S_SceneNodeTypeDumbPtr::const_iterator typeItr = found->second.begin();
    S_SceneNodeTypeDumbPtr::const_iterator typeEnd = found->second.end();
    for ( ; typeItr != typeEnd; ++typeItr )
    {
      Luna::SceneNodeType* nodeType = *typeItr;
      HM_SceneNodeSmartPtr::const_iterator instItr = nodeType->GetInstances().begin();
      HM_SceneNodeSmartPtr::const_iterator instEnd = nodeType->GetInstances().end();
      for ( ; instItr != instEnd; ++instItr )
      {
        RemoveJob( Reflect::AssertCast< Luna::LightingJob >( instItr->second ) );
      }
    }
  }
  Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Recursively removes the specified item and any of its children from the list
// of selected items.
// 
void LightingJobTree::RemoveChildrenFromSelection( const wxTreeItemId& item )
{
  if ( m_SelectedItems.Size() > 0 )
  {
    wxTreeItemIdValue cookie;
    wxTreeItemId child = GetFirstChild( item, cookie );
    while ( child.IsOk() )
    {
      RemoveChildrenFromSelection( child );
      m_SelectedItems.Remove( child.m_pItem );
      child = GetNextChild( item, cookie );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene has been added to the scene manager.
// 
void LightingJobTree::SceneAdded( const SceneChangeArgs& args )
{
  args.m_Scene->AddLoadFinishedListener( LoadSignature::Delegate ( this, &LightingJobTree::SceneLoadFinished ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene has been removed from the scene manager.
// 
void LightingJobTree::SceneRemoving( const SceneChangeArgs& args )
{
  args.m_Scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingJobTree::NodeCreated ) );
  args.m_Scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LightingJobTree::NodeDeleted ) );
  args.m_Scene->RemoveLoadFinishedListener( LoadSignature::Delegate ( this, &LightingJobTree::SceneLoadFinished ) );
  
 Asset::LevelAsset* level = m_SceneManager->GetCurrentLevel();

 if( level )
 {
   // Remove lighing jobs if the this is the root scene.
   if ( args.m_Scene->GetFileID() == level->m_LightingZone )
   {
     RemoveJobs( args.m_Scene );
   }
   else
   {
     // The scene is just some other zone, unload its UI.
     tuid zoneID = args.m_Scene->GetFileID();
     if ( zoneID != TUID::Null )
     {
       M_JobsToItems::const_iterator jobItr = m_JobTreeItems.begin();
       M_JobsToItems::const_iterator jobEnd = m_JobTreeItems.end();
       for ( ; jobItr != jobEnd; ++jobItr )
       {
         wxTreeItemId jobID = jobItr->second;
         LightingTreeItem* treeItem = static_cast< LightingTreeItem* >( GetItemData( jobID ) );
         JobItem* jobItem = treeItem->Cast< JobItem >( treeItem, LightingTreeItem::TypeJob );
         jobItem->UnloadZone( zoneID );
       }
     }
   }
 }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a scene is done loading.
// 
void LightingJobTree::SceneLoadFinished( const LoadArgs& args )
{
  Asset::LevelAsset* level = m_SceneManager->GetCurrentLevel();

  if( level )
  {
    Freeze();

    if ( args.m_Scene->GetFileID() == level->m_LightingZone )
    {
      args.m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingJobTree::NodeCreated ) );
      args.m_Scene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LightingJobTree::NodeDeleted ) );

      // If this is the lighting scene, we need to add all the lighting jobs.
      HM_SceneNodeDumbPtr::const_iterator nodeItr = args.m_Scene->GetNodes().begin();
      HM_SceneNodeDumbPtr::const_iterator nodeEnd = args.m_Scene->GetNodes().end();
      for ( ; nodeItr != nodeEnd; ++nodeItr )
      {
        Luna::SceneNode* sceneNode = nodeItr->second;
        if ( sceneNode->HasType( Reflect::GetType<Luna::LightingJob>() ) )
        {
          AddJob( Reflect::DangerousCast< Luna::LightingJob >( sceneNode ) );
        }
      }
    }
    else
    {
      // Update the UI with all the lighting jobs that belong to this scene.
      tuid zoneID = args.m_Scene->GetFileID();
      if ( zoneID != TUID::Null )
      {
        M_JobsToItems::const_iterator jobItr = m_JobTreeItems.begin();
        M_JobsToItems::const_iterator jobEnd = m_JobTreeItems.end();
        for ( ; jobItr != jobEnd; ++jobItr )
        {
          wxTreeItemId jobID = jobItr->second;
          LightingTreeItem* treeItem = static_cast< LightingTreeItem* >( GetItemData( jobID ) );
          JobItem* jobItem = treeItem->Cast< JobItem >( treeItem, LightingTreeItem::TypeJob );
          jobItem->LoadZone( zoneID );
        }
      }
    }

    Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is added to a scene.  Handles adding lighting jobs
// to the tree.
// 
void LightingJobTree::NodeCreated( const NodeChangeArgs& args )
{
  Asset::LevelAsset* level = m_SceneManager->GetCurrentLevel();

  if( level )
  {
    if ( args.m_Node->HasType( Reflect::GetType<Luna::LightingJob>() ) )
    {
      if ( args.m_Node->GetScene()->GetFileID() != level->m_LightingZone )
      {
        // The UI should prevent this from happening, otherwise there's a bug
        Console::Error( "Lighting jobs can only be added to the lighting zone!\n" );
        NOC_BREAK();
      }
      else
      {
        AddJob( Reflect::DangerousCast< Luna::LightingJob >( args.m_Node ) );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when a node is deleted from a scene.  Handles removing lighting jobs
// from the tree.
// 
void LightingJobTree::NodeDeleted( const NodeChangeArgs& args )
{
  if ( args.m_Node->HasType( Reflect::GetType<Luna::LightingJob>() ) )
  {
    RemoveJob( Reflect::DangerousCast< Luna::LightingJob >( args.m_Node ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the name of a node has changed.  Updates the corresponding
// tree item.
// 
void LightingJobTree::NodeNameChanged( const SceneNodeChangeArgs& args )
{
  if ( args.m_Node->HasType( Reflect::GetType<Luna::LightingJob>() ) )
  {
    Luna::LightingJob* job = Reflect::DangerousCast< Luna::LightingJob >( args.m_Node );
    M_JobsToItems::const_iterator found = m_JobTreeItems.find( job );
    if ( found != m_JobTreeItems.end() )
    {
      wxTreeItemId id = found->second;
      SetItemText( id, job->GetName().c_str() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user is trying to change selection within the tree 
// control.  Verifies that the selection is valid, otherwise vetos the event.
// 
void LightingJobTree::OnSelectionChanging( wxTreeEvent& args )
{
  if ( !IsSelected( args.GetItem() ) )
  {
    // Selecting the item

    LightingTreeItem* data = static_cast< LightingTreeItem* >( GetItemData( args.GetItem() ) );
    if ( data )
    {
      if ( !data->IsSelectable( m_SelectedItems ) )
      {
        args.Veto();
      }
      else
      {
        m_SelectedItems.Append( args.GetItem().m_pItem );
      }
    }
  }
  else
  {
    // Deselecting the item
    m_SelectedItems.Remove( args.GetItem().m_pItem );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item is deleted from the tree.  Removes it from the
// list of selected items that this control caches.
// 
void LightingJobTree::OnDeleteItem( wxTreeEvent& args )
{
  m_SelectedItems.Remove( args.GetItem().m_pItem );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user begins to edit an item in the tree.  Vetoes the
// event if the item is not allowed to be renamed.
// 
void LightingJobTree::OnBeginLabelEdit( wxTreeEvent& args )
{
  LightingTreeItem* data = static_cast< LightingTreeItem* >( GetItemData( args.GetItem() ) );
  if ( data )
  {
    if ( !data->CanRename() )
    {
      args.Veto();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user is done editing the name of a tree item.  Updates
// the proper scene node.
// 
void LightingJobTree::OnEndLabelEdit( wxTreeEvent& args )
{
  if ( args.IsAllowed() && !args.IsEditCancelled() )
  {
    LightingTreeItem* data = static_cast< LightingTreeItem* >( GetItemData( args.GetItem() ) );
    if ( data )
    {
      std::string newName( args.GetLabel().c_str() );
      data->Rename( newName );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the context menu should be shown.  Builds a context menu based
// upon what items are selected in the tree.
// 
void LightingJobTree::OnContextMenu( wxTreeEvent& args )
{
  Luna::Scene* lightingScene = m_SceneManager->GetLightingScene();
  if ( lightingScene )
  {
    if( lightingScene->IsEditable() )
    {
      bool foundFirst = false;
      ContextMenuItemSet items;
      if ( m_SelectedItems.Size() > 0 )
      {
        // Build the context menu items
        OS_TreeItemIds::Iterator itr = m_SelectedItems.Begin();
        OS_TreeItemIds::Iterator end = m_SelectedItems.End();
        for ( ; itr != end; ++itr )
        {
          LightingTreeItem* data = static_cast< LightingTreeItem* >( GetItemData( *itr ) );
          if ( data )
          {
            if ( !foundFirst )
            {
              foundFirst = true;
              items = data->GetContextMenuItems();
            }
            else
            {
              items = ContextMenuGenerator::Merge( items, data->GetContextMenuItems() );
            }
          }
          else
          {
            items.Clear();
          }

          if ( items.IsEmpty() )
          {
            break;
          }
        }
      }

      // Actually display the context menu
      if ( !items.IsEmpty() )
      {
        ContextMenuPtr contextMenu = new ContextMenu( m_SceneEditor );
        ContextMenuGenerator::Build( items, contextMenu.Ptr() );
        if ( !contextMenu->IsEmpty() )
        {
          ContextMenuArgsPtr contextMenuArgs = new ContextMenuArgs();
          contextMenu->Popup( m_SceneEditor, contextMenuArgs );
          if ( !contextMenuArgs->GetBatch()->IsEmpty() )
          {
            contextMenuArgs->GetBatch()->Redo(); // REQUIRED FOR REMOVE AT THE MOMENT
            lightingScene->Push( contextMenuArgs->GetBatch() );
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// Select the tree item corresponding to the specified Luna::LightingJob
// 
void LightingJobTree::SelectLightingJob( Luna::LightingJob* job )
{
  M_JobsToItems::const_iterator findItor = m_JobTreeItems.find( job );
  if( findItor != m_JobTreeItems.end() )
  {
    SelectItem( findItor->second );
  }
}

void LightingJobTree::SelectItems( const OS_SelectableDumbPtr& sceneSelection )
{
  OS_SelectableDumbPtr::Iterator selectItr = sceneSelection.Begin();
  OS_SelectableDumbPtr::Iterator selectEnd = sceneSelection.End();
  for ( ; selectItr != selectEnd; ++selectItr )
  {
    Luna::SceneNode* sceneNode = Reflect::ObjectCast< Luna::SceneNode >( *selectItr );
    
    if( sceneNode )
    {
      M_JobsToItems::iterator itor = m_JobTreeItems.begin();
      M_JobsToItems::iterator end  = m_JobTreeItems.end();

      for( ; itor != end; ++itor )
      {
        LightingTreeItem* item = static_cast< LightingTreeItem* >( GetItemData( itor->second ) );
        item->SelectItem( sceneNode );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// This function is faster than GetSelections().  It returns the cached list
// of selected items instead of traversing the entire tree.
// 
const OS_TreeItemIds& LightingJobTree::GetSelectedItems() const
{
  return m_SelectedItems;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to update our cached selection.
// 
void LightingJobTree::DeleteChildren( const wxTreeItemId& item )
{
  RemoveChildrenFromSelection( item );
  __super::DeleteChildren( item );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to update our cached selection.
// 
void LightingJobTree::DeleteAllItems()
{
  m_SelectedItems.Clear();
  __super::DeleteAllItems();
}
