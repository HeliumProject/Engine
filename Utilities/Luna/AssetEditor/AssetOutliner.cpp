#include "Precompile.h"
#include "AssetOutliner.h"

#include "AssetEditorIDs.h"
#include "AssetEditor.h"
#include "AssetManager.h"
#include "AssetOutlineItemData.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Foundation/Log.h"
#include "Application/Inspect/DragDrop/ClipboardDataObject.h"
#include "Application/Inspect/DragDrop/ClipboardFileList.h"
#include "Application/Inspect/DragDrop/DropTarget.h"
#include "Application/Inspect/DragDrop/ReflectClipboardData.h"
#include "Application/UI/ImageManager.h"


// Using
using namespace Luna;

// Static event table
BEGIN_EVENT_TABLE( AssetOutliner, wxEvtHandler )
EVT_TREE_SEL_CHANGED( AssetEditorIDs::AssetOutlinerTreeControl, OnSelectionChanged )
EVT_TREE_ITEM_RIGHT_CLICK( AssetEditorIDs::AssetOutlinerTreeControl, OnTreeItemRightClick )
EVT_TREE_ITEM_ACTIVATED( AssetEditorIDs::AssetOutlinerTreeControl, OnTreeItemActivated )
EVT_TREE_BEGIN_DRAG( AssetEditorIDs::AssetOutlinerTreeControl, OnBeginDrag )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetOutliner::AssetOutliner( AssetEditor* editor, const wxPoint& pos, const wxSize& size )
: m_Editor( editor )
, m_AssetManager( editor->GetAssetManager() )
, m_TreeControl( new Nocturnal::SortTreeCtrl( editor, AssetEditorIDs::AssetOutlinerTreeControl, pos, size, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_MULTIPLE | wxTR_HIDE_ROOT, wxDefaultValidator, "AssetEditorOutliner" ) )
, m_FreezeSelection( false )
, m_DragOriginatedHere( false )
{
  // We want this tree to have multiple root items, but Windows does not allow this.
  // So, there will be an invisible root item, that will not be shown, effectively
  // making any items that are direct children of it appear to be root items.
  m_TreeControl->AddRoot( "INVISIBLE_ROOT_ITEM" );
  m_TreeControl->SetImageList( Nocturnal::GlobalImageManager().GetGuiImageList() );

  // This class will handle all the events for the tree control.
  m_TreeControl->PushEventHandler( this );

  // Listeners
  m_AssetManager->GetRootNode()->AddChildAddedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildAdded ) );
  m_AssetManager->GetRootNode()->AddChildRemovedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildRemoved ) );
  m_AssetManager->GetRootNode()->AddChildRearrangedListener( ChildRearrangeSignature::Delegate( this, &AssetOutliner::NodeChildRearranged ) );
  m_AssetManager->GetSelection().AddChangedListener( SelectionChangedSignature::Delegate( this, &AssetOutliner::SelectionChanged ) );
  m_AssetManager->AddHierarchyChangeStartingListener( HierarchyChangeSignature::Delegate( this, &AssetOutliner::HierarchyChangeStarting ) );
  m_AssetManager->AddHierarchyChangeFinishedListener( HierarchyChangeSignature::Delegate( this, &AssetOutliner::HierarchyChangeFinished ) );

  // Drag-and-drop 
  // No need to delete drop target, wx takes care of that after calling SetDropTarget.
  Inspect::DropTarget* dropTarget = new Inspect::DropTarget();
  dropTarget->SetDragOverCallback( Inspect::DragOverCallback::Delegate( this, &AssetOutliner::DragOver ) );
  dropTarget->SetDropCallback( Inspect::DropCallback::Delegate( this, &AssetOutliner::Drop ) );
  dropTarget->SetDragLeaveCallback( Inspect::DragLeaveCallback::Delegate( this, &AssetOutliner::DragLeave ) );
  m_TreeControl->SetDropTarget( dropTarget );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetOutliner::~AssetOutliner()
{
  m_AssetManager->GetRootNode()->RemoveChildAddedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildAdded ) );
  m_AssetManager->GetRootNode()->RemoveChildRemovedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildRemoved ) );
  m_AssetManager->GetRootNode()->RemoveChildRearrangedListener( ChildRearrangeSignature::Delegate( this, &AssetOutliner::NodeChildRearranged ) );
  m_AssetManager->GetSelection().RemoveChangedListener( SelectionChangedSignature::Delegate( this, &AssetOutliner::SelectionChanged ) );
  m_AssetManager->RemoveHierarchyChangeStartingListener( HierarchyChangeSignature::Delegate( this, &AssetOutliner::HierarchyChangeStarting ) );
  m_AssetManager->RemoveHierarchyChangeFinishedListener( HierarchyChangeSignature::Delegate( this, &AssetOutliner::HierarchyChangeFinished ) );

  m_TreeControl->DeleteAllItems();
  m_TreeControl->PopEventHandler();
}

///////////////////////////////////////////////////////////////////////////////
// Get the window (tree control) for adding this tree to a frame, panel, or other
// piece of encapsulating UI.
// 
wxWindow* const AssetOutliner::GetWindow() const
{
  return m_TreeControl;
}

///////////////////////////////////////////////////////////////////////////////
// Sorts the children of the specified tree item.
// 
void AssetOutliner::Sort( const wxTreeItemId& itemId, bool recursive )
{
  m_TreeControl->Sort( itemId, recursive );
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified item to the tree.  This will create a tree item and 
// insert it into the tree.  If this function returns false, the
// specified item was not added, and the caller is responsible for deleting it 
// if necessary.
// 
bool AssetOutliner::AddItem( Luna::AssetNode* node )
{
  m_TreeControl->Freeze();
  bool isOk = false;

  // Find parent of the node
  NOC_ASSERT( node->GetParent() );
  wxTreeItemId parentItem = m_TreeControl->GetRootItem();
  if ( node->GetParent() != m_AssetManager->GetRootNode() )
  {
    M_TreeItems::iterator foundParent = m_Items.find( node->GetParent() );
    if ( foundParent != m_Items.end() )
    {
      parentItem = foundParent->second;
    }
  }

  // Find the sibling to place this node before
  wxTreeItemId insertAfter;
  OS_AssetNodeSmartPtr::Iterator siblingItr = node->GetParent()->GetChildren().FindPrevSibling( node );
  if ( siblingItr != node->GetParent()->GetChildren().End() )
  {
    const Luna::AssetNodePtr& sibling = *siblingItr;
    M_TreeItems::iterator foundSibling = m_Items.find( sibling );
    if ( foundSibling != m_Items.end() )
    {
      insertAfter = foundSibling->second;
    }
  }

  // Create new tree item for the node
  if ( parentItem.IsOk() )
  {
    wxTreeItemId insertedItem;

    if ( insertAfter.IsOk() )
    {
      // Insert the new item next to the appropriate sibling
      insertedItem = m_TreeControl->InsertItem( parentItem, insertAfter, node->GetName().c_str(), node->GetIconIndex(), node->GetIconIndex(), new AssetOutlineItemData( this, node ) );
    }
    else
    {
      // Insert the new item at the end of the list
      insertedItem = m_TreeControl->AppendItem( parentItem, node->GetName().c_str(), node->GetIconIndex(), node->GetIconIndex(), new AssetOutlineItemData( this, node ) );
    }

    if ( insertedItem.IsOk() )
    {
      if ( node->IsSelected() )
      {
        bool previousFreezeState = m_FreezeSelection;
        m_FreezeSelection = true;
        m_TreeControl->SelectItem( insertedItem );
        m_FreezeSelection = previousFreezeState;
      }

      if ( node->GetStyle() != LabelStyles::Normal )
      {
        SetLabelStyle( insertedItem, node->GetStyle() );
      }

      isOk = m_Items.insert( M_TreeItems::value_type( node, insertedItem ) ).second;
      if ( !isOk )
      {
        Log::Error( "Error while trying to build lookup for node '%s'.\n", node->GetName().c_str() );
        NOC_BREAK();
      }
      else
      {
        // Hookup listeners
        node->AddChildAddedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildAdded ) );
        node->AddChildRemovedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildRemoved ) );
        node->AddChildRearrangedListener( ChildRearrangeSignature::Delegate( this, &AssetOutliner::NodeChildRearranged ) );
        node->AddNameChangedListener( NodeNameChangeSignature::Delegate( this, &AssetOutliner::NodeRenamed ) );
        node->AddIconChangedListener( NodeIconSignature::Delegate( this, &AssetOutliner::NodeIconChanged ) );
        node->AddStyleChangedListener( NodeStyleChangeSignature::Delegate( this, &AssetOutliner::NodeStyleChanged ) );

        // Recursively create children
        OS_AssetNodeSmartPtr::Iterator childItr = node->GetChildren().Begin();
        OS_AssetNodeSmartPtr::Iterator childEnd = node->GetChildren().End();
        for ( ; childItr != childEnd; ++childItr )
        {
          isOk &= AddItem( *childItr );
        }
      }
    }
    else
    {
      Log::Error( "Unable to determine parent for node '%s'.\n", node->GetName().c_str() );
      NOC_BREAK();
    }
  }

  m_TreeControl->Thaw();
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Deletes the specified item and all of its children.
// 
void AssetOutliner::DeleteItem( Luna::AssetNode* node )
{
  M_TreeItems::iterator found = m_Items.find( node );
  if ( found != m_Items.end() )
  {
    // Remove the item from the tree
    m_TreeControl->Delete( found->second );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Will "expand all"/"collapse all" all nodes.
//
bool AssetOutliner::ExpandAll( bool expand )
{
  bool success = true;
  m_TreeControl->Freeze();
  for ( M_TreeItems::iterator itr = m_Items.begin(), end = m_Items.end(); itr != end; ++itr )
  {
    if ( !ExpandNode( itr->first, expand ) )
    {
      success = false;
      break;
    }
  }
  
  m_TreeControl->Thaw();
  return success;
}

///////////////////////////////////////////////////////////////////////////////
// Will "expand all"/"collapse all" a set of nodes.
// 
bool AssetOutliner::ExpandNodes( const S_AssetNodeDumbPtr& nodes, bool expand )
{
  bool success = true;
  m_TreeControl->Freeze();
  for ( S_AssetNodeDumbPtr::const_iterator itr = nodes.begin(), end = nodes.end(); itr != end; ++itr )
  {
    if ( !ExpandNode( *itr, expand ) )
    {
      success = false;
      break;
    }
  }

  m_TreeControl->Thaw();
  return success;
}

///////////////////////////////////////////////////////////////////////////////
// Will "expand all"/"collapse all" on a node.
// 
bool AssetOutliner::ExpandNode( Luna::AssetNode* node, bool expand )
{
  M_TreeItems::iterator found = m_Items.find( node );
  if ( found == m_Items.end() )
  {
    return false;
  }
  
  ExpandTreeItem( found->second, expand );
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Will expand/collapse a tree item and all its children
// 
void AssetOutliner::ExpandTreeItem( wxTreeItemId item, bool expand )
{
  m_TreeControl->Freeze();

  wxTreeItemIdValue cookie; 
  wxTreeItemId child = m_TreeControl->GetFirstChild( item, cookie );
  while ( child.IsOk() )
  {
    ExpandTreeItem( child, expand );
    child = m_TreeControl->GetNextChild( item, cookie );
  }
  
  
  if ( expand )
  {
    if ( !m_TreeControl->IsExpanded( item ) )
    {
      m_TreeControl->Expand( item );
    }
  }
  else
  {
    if ( m_TreeControl->IsExpanded( item ) )
    {
      m_TreeControl->Collapse( item );
    }
  }

  m_TreeControl->Thaw();
}


///////////////////////////////////////////////////////////////////////////////
// Helper function for hit testing a point during a drag and drop operation.
// 
wxTreeItemId AssetOutliner::DragHitTest( wxPoint point )
{
  int flags = 0;
  wxTreeItemId item = m_TreeControl->HitTest( point, flags );

  if ( item.IsOk() )
  {
    // Hit is only successful if you are over the label or icon for an item.
    if ( ( flags & wxTREE_HITTEST_ONITEMLABEL ) != wxTREE_HITTEST_ONITEMLABEL &&
      ( flags & wxTREE_HITTEST_ONITEMICON ) != wxTREE_HITTEST_ONITEMICON )
    {
      item.Unset();
    }
  }


  return item;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the font style for a particular tree item.  See LabelStyles for values 
// of the style flag.
// 
void AssetOutliner::SetLabelStyle( const wxTreeItemId& treeItem, u32 style )
{
  if ( treeItem.IsOk() )
  {
    wxFont font = *wxNORMAL_FONT;

    if ( !LabelStyles::HasFlag( style, LabelStyles::Normal ) )
    {
      if ( LabelStyles::HasFlag( style, LabelStyles::Italic ) )
      {
        font.SetStyle( wxFONTSTYLE_ITALIC );
      }
      if ( LabelStyles::HasFlag( style, LabelStyles::Bold ) )
      {
        font.SetWeight( wxFONTWEIGHT_BOLD );
      }
    }

    m_TreeControl->SetItemFont( treeItem, font );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to get the interface to the data stored on each tree item.
// 
AssetOutlineItemData* AssetOutliner::GetItemData( const wxTreeItemId& id )
{
  AssetOutlineItemData* data = NULL;
  if ( id.IsOk() )
  {
    wxTreeItemData* baseData = m_TreeControl->GetItemData( id );
    if ( baseData )
    {
      data = reinterpret_cast< AssetOutlineItemData* >( baseData );
    }
  }

  if ( !data )
  {
    Log::Error( "Tree item (%x: %s) does not have asset data associated with it\n", id.m_pItem, m_TreeControl->GetItemText( id ).c_str() );
    NOC_BREAK();
  }

  return data;
}

///////////////////////////////////////////////////////////////////////////////
// Debug function for dumping the contents of the tree... seems to come in handy
// from time to time.
// 
void AssetOutliner::DebugDumpTree( const wxTreeItemId& treeItem, std::string prefix )
{
  if ( treeItem != m_TreeControl->GetRootItem() )
  {
    AssetOutlineItemData* data = GetItemData( treeItem );
    Log::Print( prefix.c_str() );
    Log::Print( "%s [%x] [%x] [%x]\n", m_TreeControl->GetItemText( treeItem ).c_str(), treeItem.m_pItem, data->GetId().m_pItem, data );
  }
  wxTreeItemIdValue cookie;
  wxTreeItemId childItem = m_TreeControl->GetFirstChild( treeItem, cookie );
  while ( childItem.IsOk() )
  {
    DebugDumpTree( childItem, prefix + "  " );
    childItem = m_TreeControl->GetNextChild( treeItem, cookie );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a particular tree item is being deleted.  Clears the item
// from our interal lookup table.
// 
void AssetOutliner::ItemDeleted( AssetOutlineItemData* data )
{
  Luna::AssetNode* node = data->GetAssetNode();

  // Remove listeners
  node->RemoveChildAddedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildAdded ) );
  node->RemoveChildRemovedListener( ChildChangeSignature::Delegate( this, &AssetOutliner::NodeChildRemoved ) );
  node->RemoveChildRearrangedListener( ChildRearrangeSignature::Delegate( this, &AssetOutliner::NodeChildRearranged ) );
  node->RemoveNameChangedListener( NodeNameChangeSignature::Delegate( this, &AssetOutliner::NodeRenamed ) );
  node->RemoveIconChangedListener( NodeIconSignature::Delegate( this, &AssetOutliner::NodeIconChanged ) );
  node->RemoveStyleChangedListener( NodeStyleChangeSignature::Delegate( this, &AssetOutliner::NodeStyleChanged ) );

  // Remove node from our lookup table
  m_Items.erase( node );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item is added to the root node.
// 
void AssetOutliner::NodeChildAdded( const ChildChangeArgs& args )
{
  m_TreeControl->Freeze();
  AddItem( args.m_Child );
  m_TreeControl->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item is removed from the root node.
// 
void AssetOutliner::NodeChildRemoved( const ChildChangeArgs& args )
{
  DeleteItem( args.m_Child );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a child node has its location rearranged.  Updates tree 
// tree control to display the new position.
// 
// This is currently not called, Ryan.
// 
void AssetOutliner::NodeChildRearranged( const ChildChangeArgs& args )
{
  if ( args.m_Child )
  {
    M_TreeItems::const_iterator childItr = m_Items.find( args.m_Child );
    if ( childItr != m_Items.end() )
    {
      m_TreeControl->Freeze();
      DeleteItem( args.m_Child );
      // Adding an item checks for its location within the tree.
      AddItem( args.m_Child );
      m_TreeControl->Thaw();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an asset node is renamed.  Updates the tree control with
// the new name for the specified item.
// 
void AssetOutliner::NodeRenamed( const NodeNameChangeArgs& args )
{
  M_TreeItems::iterator found = m_Items.find( args.m_Node );
  if ( found != m_Items.end() )
  {
    const wxTreeItemId& treeItem = found->second;
    m_TreeControl->SetItemText( treeItem, args.m_NewName.c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the icon on a node is changed.  Updates the icon on the
// corresponding tree item.
// 
void AssetOutliner::NodeIconChanged( const NodeIconChangeArgs& args )
{
  M_TreeItems::iterator found = m_Items.find( args.m_Node );
  if ( found != m_Items.end() )
  {
    const wxTreeItemId& treeItem = found->second;
    i32 imageIndex = args.m_Node->GetIconIndex();
    m_TreeControl->SetItemImage( treeItem, imageIndex, wxTreeItemIcon_Normal );
    m_TreeControl->SetItemImage( treeItem, imageIndex, wxTreeItemIcon_Selected );
    m_TreeControl->SetItemImage( treeItem, imageIndex, wxTreeItemIcon_Expanded );
    m_TreeControl->SetItemImage( treeItem, imageIndex, wxTreeItemIcon_SelectedExpanded );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the node style changes.  Updates the font on the UI.
// 
void AssetOutliner::NodeStyleChanged( const NodeStyleChangeArgs& args )
{
  M_TreeItems::iterator found = m_Items.find( args.m_Node );
  if ( found != m_Items.end() )
  {
    SetLabelStyle( found->second, args.m_NewStyle );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a major hierarchy change is about to take place.  Freezes
// the tree control.
// 
void AssetOutliner::HierarchyChangeStarting( const HierarchyChangeArgs& args )
{
  m_TreeControl->Freeze();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a  major hierarchy change has finished.  Thaws the tree
// control.
// 
void AssetOutliner::HierarchyChangeFinished( const HierarchyChangeArgs& args )
{
  m_TreeControl->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when selection has changed.  Updates the selected items in the
// tree control to match the specified selection list.
// 
void AssetOutliner::SelectionChanged( const OS_SelectableDumbPtr& selection )
{
  // Changing selection in the tree results in an event callback to this class
  // to change the asset manager's selection.  This would cause an infinite 
  // feedback loop, so set the flag to not respond to selection change messages
  // until we are done.
  m_FreezeSelection = true;

  // Deselect everything
  m_TreeControl->UnselectAll();

  // Go through the selection list and select the corresponding tree item.
  OS_SelectableDumbPtr::Iterator itr = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End();
  for ( ; itr != end; ++itr )
  {
    Luna::AssetNode* current = Reflect::ObjectCast< Luna::AssetNode >( *itr );
    if ( current )
    {
      M_TreeItems::const_iterator found = m_Items.find( current );
      if ( found != m_Items.end() )
      {
        const wxTreeItemId& treeItem = found->second;
        m_TreeControl->SelectItem( treeItem, true );
      }
    }
  }

  m_FreezeSelection = false;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a drag operation is occurring over this control.  Highlights
// the tree item that will receive the drop if one were to take place.
// 
wxDragResult AssetOutliner::DragOver( const Inspect::DragArgs& args )
{
  wxDragResult result = args.m_Default;
  wxTreeItemId item = DragHitTest( wxPoint( args.m_X, args.m_Y ) );
  if ( item != m_DragOverItem )
  {
    if ( m_DragOverItem.IsOk() && !m_TreeControl->IsSelected( m_DragOverItem ) )
    {
      m_TreeControl->SetItemDropHighlight( m_DragOverItem, false );
    }

    m_DragOverItem = item;

    if ( m_DragOverItem.IsOk() && !m_TreeControl->IsSelected( m_DragOverItem ) )
    {
      m_TreeControl->SetItemDropHighlight( m_DragOverItem, true );
    }
  }

  if ( item.IsOk() )
  {
    if ( m_DragOriginatedHere && m_TreeControl->IsSelected( item ) )
    {
      // You can't drop on an item that is already selected (it's the same as what you are dragging!)
      result = wxDragNone;
    }
    else 
    {
      AssetOutlineItemData* itemData = GetItemData( item );
      if ( itemData )
      {
        Luna::AssetNode* node = itemData->GetAssetNode();
        NOC_ASSERT( node );
        if ( !node->CanHandleClipboardData( args.m_ClipboardData->FromBuffer() ) )
        {
          result = wxDragNone;
        }
      }
    }
  }
  else
  {
    // The outliner will only accept file lists.
    if ( !args.m_ClipboardData->FromBuffer()->HasType( Reflect::GetType< Inspect::ClipboardFileList >() ) )
    {
      result = wxDragNone;
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when items are dropped onto the tree.
// 
wxDragResult AssetOutliner::Drop( const Inspect::DragArgs& args )
{
  wxDragResult result = wxDragNone;
  wxTreeItemId item = DragHitTest( wxPoint( args.m_X, args.m_Y ) );

  if ( item.IsOk() )
  {
    AssetOutlineItemData* itemData = GetItemData( item );
    if ( itemData )
    {
      Luna::AssetNode* node = itemData->GetAssetNode();
      NOC_ASSERT( node );

      // Prepare for drop
      Undo::BatchCommandPtr batch = new Undo::BatchCommand();
      ClipboardOperation op = ClipboardOperations::Move;
      if ( args.m_Default == wxDragCopy )
      {
        op = ClipboardOperations::Copy;
      }

      // Handle the drop
      Luna::AssetClass* assetClass = node->GetAssetClass();
      if ( assetClass->GetAssetManager()->IsEditable( assetClass ) && node->HandleClipboardData( args.m_ClipboardData->FromBuffer(), op, batch ) )
      {
        result = args.m_Default;

        // Finish drag and drop
        switch ( result )
        {
        case wxDragError:
        case wxDragNone:
        case wxDragCancel:
          break;

        case wxDragLink:
          // Not supported
          break;

        case wxDragMove:
          // Tell the parent nodes to delete their selected children.
          for each ( Luna::AssetNode* parent in m_ParentsOfDraggedNodes )
          {
            batch->Push( parent->DeleteSelectedChildren() );
          }

          // NO BREAK ON PURPOSE! 
        case wxDragCopy:
          // Both move and copy need to update the selection
          batch->Push( m_AssetManager->GetSelection().SetItem( node ) );
          m_AssetManager->Push( batch );

          // HACK
          m_AssetManager->ClearUndoQueue();
          break;
        }
      }
    }
  }
  else
  {
    Inspect::ClipboardFileListPtr fileList = Reflect::ObjectCast< Inspect::ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
    if ( fileList.ReferencesObject() )
    {
      std::set< std::string >::const_iterator fileItr = fileList->GetFilePaths().begin();
      std::set< std::string >::const_iterator fileEnd = fileList->GetFilePaths().end();
      for ( ; fileItr != fileEnd; ++fileItr )
      {
        m_AssetManager->GetAssetEditor()->Open( *fileItr );
      }
    }
  }

  if ( m_DragOverItem.IsOk() )
  {
    m_TreeControl->SetItemDropHighlight( m_DragOverItem, false );
    m_DragOverItem.Unset();
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a drag operation leaves this control.  Clears the 
// highlighted drop item.
// 
void AssetOutliner::DragLeave( Nocturnal::Void )
{
  if ( m_DragOverItem.IsOk() )
  {
    m_TreeControl->SetItemDropHighlight( m_DragOverItem, false );
    m_DragOverItem.Unset();
  }
}

///////////////////////////////////////////////////////////////////////////////
// UI callback when the user clicks on items in the tree.  Updates the selection
// on the Asset Manager.
// 
void AssetOutliner::OnSelectionChanged( wxTreeEvent& args )
{
  // If selection is frozen, we are going to ignore this message.  This means that
  // we are in the process of updating the tree and there's no need to process
  // this message.
  if ( m_FreezeSelection )
  {
    return;
  }

  wxArrayTreeItemIds selections;
  const size_t numSelections = m_TreeControl->GetSelections( selections );

  OS_SelectableDumbPtr newSelection;

  for ( size_t i = 0; i < numSelections; i++ )
  {
    AssetOutlineItemData* itemData = GetItemData( selections[i] );
    if ( itemData && itemData->GetAssetNode() )
    {
      newSelection.Append( itemData->GetAssetNode() );
    }
  }  

  m_AssetManager->Push( m_AssetManager->GetSelection().SetItems( newSelection, SelectionChangingSignature::Delegate(), SelectionChangedSignature::Delegate( this, &AssetOutliner::SelectionChanged ) ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a tree item is right-clicked on.  Iterates over the
// selected tree items and builds a context menu that can be applied to all
// the items.  Displays the context menu if it is not empty.
// 
void AssetOutliner::OnTreeItemRightClick( wxTreeEvent& args )
{
  wxArrayTreeItemIds selection;
  const size_t numSelected = m_TreeControl->GetSelections( selection );

  bool foundFirst = false;
  ContextMenuItemSet items;
  if ( numSelected > 0 )
  {
    // Build the context menu items
    for ( size_t index = 0; index < numSelected; ++index )
    {
      AssetOutlineItemData* data = GetItemData( selection.Item( index ) );
      if ( data )
      {
        data->GetAssetNode()->PreShowContextMenu();
        const ContextMenuItemSet& currentItems = data->GetAssetNode()->GetContextMenu();
        if ( !currentItems.IsEmpty() )
        {
          if ( !foundFirst )
          {
            foundFirst = true;
            items = currentItems;
          }
          else
          {
            items = ContextMenuGenerator::Merge( items, currentItems );
          }
        }
        else
        {
          items.Clear();
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
    Luna::ContextMenuPtr contextMenu = new Luna::ContextMenu( m_Editor );
    ContextMenuGenerator::Build( items, contextMenu.Ptr() );
    if ( !contextMenu->IsEmpty() )
    {
      ContextMenuArgsPtr contextMenuArgs = new ContextMenuArgs();
      contextMenu->Popup( m_TreeControl->GetParent(), contextMenuArgs );
      if ( !contextMenuArgs->GetBatch()->IsEmpty() )
      {
        m_AssetManager->Push( contextMenuArgs->GetBatch() );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a tree item is double-clicked.  Allows that tree item to
// handle the message if it wants to.
// 
void AssetOutliner::OnTreeItemActivated( wxTreeEvent& args )
{
  AssetOutlineItemData* data = GetItemData( args.GetItem() );
  if ( data )
  {
    data->GetAssetNode()->ActivateItem();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to use the normal drag-and-drop system.  The implementation in
// the tree control limits dragging to within the tree control itself and doesn't
// properly handle key presses during the drag (such as ESC cancelling the drag).
// Therefore, this function kicks off an application-wide drag operation instead
// of letting the tree handle it itself.
// 
void AssetOutliner::OnBeginDrag( wxTreeEvent& args )
{
  if ( m_AssetManager->GetSelection().GetItems().Size() > 0 )
  {
    bool canBeMoved = true;
    Inspect::ReflectClipboardDataPtr clipboardData = m_AssetManager->CopySelection( m_ParentsOfDraggedNodes, canBeMoved );

    if ( clipboardData.ReferencesObject() )
    {
      // Do drag and drop operation
      Inspect::ClipboardDataObject dataObject;
      dataObject.ToBuffer( clipboardData );
      wxDropSource source( dataObject, m_TreeControl );
      m_DragOverItem.Unset();
      int flags = wxDrag_CopyOnly;
      if ( canBeMoved )
      {
        flags = wxDrag_AllowMove | wxDrag_DefaultMove;
      }

      m_DragOriginatedHere = true;
      wxDragResult result = source.DoDragDrop( flags );
      m_DragOriginatedHere = false;
      m_ParentsOfDraggedNodes.clear();
    }
  }
}
