#include "Precompile.h"
#include "HierarchyOutliner.h"
#include "HierarchyOutlinerItemData.h"

#include "Application/UI/ArtProvider.h"
#include "Editor/UI/Controls/Tree/SortTreeCtrl.h"
#include "Editor/Scene/ParentCommand.h"
#include "Editor/Scene/Scene.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
HierarchyOutliner::HierarchyOutliner( Editor::SceneManager* sceneManager )
: SceneOutliner( sceneManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
HierarchyOutliner::~HierarchyOutliner()
{
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to fetch the client data off a given tree item.  The data
// for each item in the tree is always assumed to be an HierarchyOutlinerItemData,
// so if that assumption changes, this function will no longer be valid.  This 
// function replaces the helper on the base class to return data of the proper
// type in this derived class.
// 
HierarchyOutlinerItemData* HierarchyOutliner::GetTreeItemData( const wxTreeItemId& item )
{
  HELIUM_ASSERT( item.IsOk() );
  HELIUM_ASSERT( m_TreeCtrl->GetItemData( item ) );
  return static_cast< HierarchyOutlinerItemData* >( m_TreeCtrl->GetItemData( item ) );
}

///////////////////////////////////////////////////////////////////////////////
// Recurses over all the hierarchy nodes in the scene and adds each one to
// the tree.
// 
void HierarchyOutliner::AddHierarchyNodes()
{
  EDITOR_SCENE_SCOPE_TIMER( ("") );

  if ( m_CurrentScene )
  {
    // Freeze and disable sorting
    m_TreeCtrl->Freeze();
    bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
    m_TreeCtrl->DisableSorting();

    RecurseAddHierarchyNode( m_CurrentScene->GetRoot() );

    // The root item gets a special icon
    i32 image = Helium::GlobalFileIconsTable().GetIconID( TXT( "world" ) );
    m_TreeCtrl->SetItemImage( m_TreeCtrl->GetRootItem(), image, wxTreeItemIcon_Normal );
    m_TreeCtrl->SetItemImage( m_TreeCtrl->GetRootItem(), image, wxTreeItemIcon_Expanded );
    m_TreeCtrl->SetItemImage( m_TreeCtrl->GetRootItem(), image, wxTreeItemIcon_Selected );
    m_TreeCtrl->SetItemImage( m_TreeCtrl->GetRootItem(), image, wxTreeItemIcon_SelectedExpanded );

    // Sort tree if necessary
    m_TreeCtrl->EnableSorting( isSortingEnabled );
    Sort( m_TreeCtrl->GetRootItem() );
    m_TreeCtrl->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Recursively adds the specified hierarchy node, and all of it's children, as
// items in this tree.
// 
void HierarchyOutliner::RecurseAddHierarchyNode( Editor::HierarchyNode* node )
{
  EDITOR_SCENE_SCOPE_TIMER( ("") );

  m_TreeCtrl->Freeze();

  AddHierarchyNode( node );

  // Recursively add all the children of node
  OS_HierarchyNodeDumbPtr::Iterator childItr = node->GetChildren().Begin();
  OS_HierarchyNodeDumbPtr::Iterator childEnd = node->GetChildren().End();
  for ( ; childItr != childEnd; ++childItr )
  {
    RecurseAddHierarchyNode( *childItr );
  }

  m_TreeCtrl->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Adds a single hierarchy node to the tree.
// 
void HierarchyOutliner::AddHierarchyNode( Editor::HierarchyNode* node )
{
  EDITOR_SCENE_SCOPE_TIMER( ("") );

  wxTreeItemId parentItem;
  M_TreeItems::const_iterator foundParent = m_Items.find( node->GetParent() );
  if ( foundParent != m_Items.end() )
  {
    parentItem = foundParent->second;
  }

  node->AddParentChangedListener( ParentChangedSignature::Delegate ( this, &HierarchyOutliner::ParentChanged ) );

  AddItem( parentItem, node->GetName(), node->GetImageIndex(), new HierarchyOutlinerItemData( node ), node->IsSelected() );
}

///////////////////////////////////////////////////////////////////////////////
// Required by the base class.  Creates the tree control that this class wraps,
// and connects GUI event handlers.
// 
SortTreeCtrl* HierarchyOutliner::CreateTreeCtrl( wxWindow* parent, wxWindowID id )
{
  SortTreeCtrl* tree = new SortTreeCtrl( parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_MULTIPLE | wxTR_EDIT_LABELS, wxDefaultValidator, wxT( "HierarchyOutliner" ) );

  // Override dynamic GUI event handlers here
  tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( HierarchyOutliner::OnBeginDrag ), NULL, this );
  tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( HierarchyOutliner::OnEndDrag ), NULL, this );

  return tree;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the base class has finished changing the current scene.  Loads
// the hierarchy nodes into the tree control.
// 
void HierarchyOutliner::CurrentSceneChanged( Editor::Scene* oldScene )
{
  AddHierarchyNodes();
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from the base class to add additional listeners to the current 
// scene.
// 
void HierarchyOutliner::ConnectSceneListeners()
{
  __super::ConnectSceneListeners();

  if ( m_CurrentScene )
  {
    m_CurrentScene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &HierarchyOutliner::NodeAdded ) );
    m_CurrentScene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &HierarchyOutliner::NodeRemoved ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Since we HELIUM_OVERRIDE ConnectSceneListeners, we also HELIUM_OVERRIDE this function to
// disconnect our additional listeners from the current scene.
// 
void HierarchyOutliner::DisconnectSceneListeners()
{
  __super::DisconnectSceneListeners();

  if ( m_CurrentScene )
  {
    m_CurrentScene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &HierarchyOutliner::NodeAdded ) );
    m_CurrentScene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &HierarchyOutliner::NodeRemoved ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user begins to drag an item in the tree control.  Determines
// if the item is allowed to be dragged, and kicks off the drag-and-drop event
// if it is allowed.
// 
void HierarchyOutliner::OnBeginDrag( wxTreeEvent& args )
{
  // You can only drag something if...
  // 1. The drag event contains a valid item to originate the drag
  // 2. The item that the drag originates on is selected
  // 3. There is something in the scene's selected item list (which had better be the
  //    case if item 2 above was true.
  // 4. You cannot drag the root item
  if ( ( args.GetItem().IsOk() )&&
       ( m_TreeCtrl->IsSelected( args.GetItem() ) ) && 
       ( m_CurrentScene->GetSelection().GetItems().Size() > 0 ) &&
       ( args.GetItem() != m_TreeCtrl->GetRootItem() ) )
  {
    args.Allow();
  }
  // else: automatically veto'd (you have to specifically allow a drag operation)
}

///////////////////////////////////////////////////////////////////////////////
// Called at the end of a drag-and-drop event.  Called when the user
// releases the mouse button.  Moves all the selected items to be under a 
// new parent (the item that was dropped onto in the tree).  You cannot drop
// onto an item that is already selected (your command will be ignored if
// you do).  All the reparenting is handled through the undo stack so that
// it can be undone and redone.  This function does not update the tree
// hierarchy; that is handled in a callback.
// 
void HierarchyOutliner::OnEndDrag( wxTreeEvent& args )
{
  const wxTreeItemId& dropItem = args.GetItem();
  
  // You can only drop on a valid item that's not already selected.
  if ( dropItem.IsOk() && !m_TreeCtrl->IsSelected( dropItem ) )
  {
    m_TreeCtrl->Freeze();

    // Reparent every selected item into the item that was dropped on
    Editor::HierarchyNode* newParent = GetTreeItemData( dropItem )->GetHierarchyNode();
    HELIUM_ASSERT( newParent );

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    const OS_SelectableDumbPtr& selection = m_CurrentScene->GetSelection().GetItems();
    OS_SelectableDumbPtr::Iterator selItr = selection.Begin();
    const OS_SelectableDumbPtr::Iterator selEnd = selection.End();
    for ( ; selItr != selEnd; ++selItr )
    {
      Editor::HierarchyNode* hNode = Reflect::ObjectCast< Editor::HierarchyNode >( *selItr );
      if ( hNode )
      {
        batch->Push( new ParentCommand( hNode, newParent ) );
      }
    }

    m_CurrentScene->Push( batch );

    m_TreeCtrl->Thaw();
  }
  // else: drop target was not valid
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a hierarchy node has its parent changed.  Updates the
// tree control to Reflect the parent change.
// 
void HierarchyOutliner::ParentChanged( const ParentChangedArgs& args )
{
  EDITOR_SCENE_SCOPE_TIMER( ("") );

  Editor::HierarchyNode* child = args.m_Node;
  m_TreeCtrl->Freeze();

  // Delete the item and re-add it to the tree to update the hierarchy
  DeleteItem( child );
  RecurseAddHierarchyNode( child );

  m_TreeCtrl->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene node is added to the scene.  Recursively adds the
// item (and all of its children) to the scene.
// 
void HierarchyOutliner::NodeAdded( const NodeChangeArgs& args )
{
  EDITOR_SCENE_SCOPE_TIMER( ("") );

  if ( args.m_Node->HasType( Reflect::GetType<Editor::HierarchyNode>() ) )
  {
    m_TreeCtrl->Freeze();
    bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
    m_TreeCtrl->DisableSorting();

    Editor::HierarchyNode* hierarchyNode = Reflect::DangerousCast< Editor::HierarchyNode >( args.m_Node );
    AddHierarchyNode( hierarchyNode );

    m_TreeCtrl->EnableSorting( isSortingEnabled );
    if ( m_TreeCtrl->IsSortingEnabled() )
    {
      // Find the item we just added
      M_TreeItems::const_iterator found = m_Items.find( hierarchyNode );
      if ( found != m_Items.end() )
      {
        // Default to sorting the children of the item we just added (will be changed below if needed)
        wxTreeItemId itemToSort = found->second;
        wxTreeItemId parent = m_TreeCtrl->GetItemParent( itemToSort );
        if ( parent.IsOk() )
        {
          // If the item has a parent, start sorting with the parent
          itemToSort = parent;
        }
        
        Sort( itemToSort );
      }
    }
    m_TreeCtrl->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene node is removed from a scene.  Removes the tree 
// item (and its children) from the tree control.
// 
void HierarchyOutliner::NodeRemoved( const NodeChangeArgs& args )
{
  EDITOR_SCENE_SCOPE_TIMER( ("") );

  if ( args.m_Node->HasType( Reflect::GetType<Editor::HierarchyNode>() ) )
  {
    Editor::HierarchyNode* hierarchyNode = Reflect::DangerousCast< Editor::HierarchyNode >( args.m_Node );
    hierarchyNode->RemoveParentChangedListener( ParentChangedSignature::Delegate ( this, &HierarchyOutliner::ParentChanged ) );
  }

  DeleteItem( args.m_Node );
}
