#include "Precompile.h"
#include "FoldersPanel.h"

#include "DirectoryCtrl.h"

#include "Application/Inspect/Clipboard/ClipboardDataObject.h"
#include "Application/Inspect/Clipboard/ClipboardFileList.h"
#include "Application/Inspect/DragDrop/DropTarget.h"
#include "Application/Inspect/Clipboard/ReflectClipboardData.h"
#include "Editor/Controls/Tree/SortTreeCtrl.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
FoldersPanel::FoldersPanel( VaultFrame* browserFrame )
: FoldersPanelGenerated( browserFrame )
, m_VaultFrame( browserFrame )
{

  // Drag-and-drop 
  // No need to delete drop target, wx takes care of that after calling SetDropTarget.
  Inspect::DropTarget* dropTarget = new Inspect::DropTarget();
  dropTarget->SetDragOverCallback( Inspect::DragOverCallback::Delegate( this, &FoldersPanel::DragOver ) );
  dropTarget->SetDropCallback( Inspect::DropCallback::Delegate( this, &FoldersPanel::Drop ) );
  dropTarget->SetDragLeaveCallback( Inspect::DragLeaveCallback::Delegate( this, &FoldersPanel::DragLeave ) );

  m_FoldersTreeCtrl->SetDropTarget( dropTarget );
}

///////////////////////////////////////////////////////////////////////////////
FoldersPanel::~FoldersPanel()
{
}

///////////////////////////////////////////////////////////////////////////////
wxTreeCtrl* FoldersPanel::GetTreeCtrl()
{
  return m_FoldersTreeCtrl->GetTreeCtrl();
}

///////////////////////////////////////////////////////////////////////////////
SortTreeCtrl* FoldersPanel::GetSortTreeCtrl()
{
  return reinterpret_cast<SortTreeCtrl*>( m_FoldersTreeCtrl->GetTreeCtrl() );
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::Unselect()
{
  wxTreeCtrl* tree = m_FoldersTreeCtrl->GetTreeCtrl();
  if ( tree )
  {
    tree->Unselect();
  }
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::SetPath( const tstring& path )
{
  m_FoldersTreeCtrl->SetPath( path );
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::GetPath( tstring& path ) const
{
  path = m_FoldersTreeCtrl->GetPath().c_str();
}



///////////////////////////////////////////////////////////////////////////////
// Helper function for hit testing a point during a drag and drop operation.
// 
wxTreeItemId FoldersPanel::DragHitTest( SortTreeCtrl* treeCtrl, wxPoint point )
{
  int flags = 0;
  wxTreeItemId item = treeCtrl->HitTest( point, flags );

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
// Callback for when a drag operation is occurring over this control.  Highlights
// the tree item that will receive the drop if one were to take place.
// 
wxDragResult FoldersPanel::DragOver( const Inspect::DragArgs& args )
{
  SortTreeCtrl* treeCtrl = GetSortTreeCtrl();

  wxDragResult result = args.m_Default;
  wxTreeItemId item = DragHitTest( treeCtrl, wxPoint( args.m_X, args.m_Y ) );
  if ( item != m_DragOverItem )
  {
    if ( m_DragOverItem.IsOk() && !treeCtrl->IsSelected( m_DragOverItem ) )
    {
      treeCtrl->SetItemDropHighlight( m_DragOverItem, false );
    }

    m_DragOverItem = item;

    if ( m_DragOverItem.IsOk() && !treeCtrl->IsSelected( m_DragOverItem ) )
    {
      treeCtrl->SetItemDropHighlight( m_DragOverItem, true );
    }
  }

  if ( item.IsOk() )
  {
    if ( m_DragOriginatedHere && treeCtrl->IsSelected( item ) )
    {
      // You can't drop on an item that is already selected (it's the same as what you are dragging!)
      result = wxDragNone;
    }
    else 
    {
      result = wxDragNone;
      //AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
      //if ( baseData )
      //{
      //  AssetCollection* collection = baseData->GetCollection<AssetCollection>();
      //  if ( !collection || !collection->CanHandleDragAndDrop() )
      //  {
      //    result = wxDragNone;
      //  }
      //}
      //else
      //{
      //  result = wxDragNone;
      //}
    }
  }
  else
  {
    result = wxDragNone;
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when items are dropped onto the tree.
// 
wxDragResult FoldersPanel::Drop( const Inspect::DragArgs& args )
{
  SortTreeCtrl* treeCtrl = GetSortTreeCtrl();
  
  wxDragResult result = wxDragNone;

#pragma TODO( "reimplement using Helium::Path" )

  // get the IDs
  //tstring assetName;
  //S_tuid assetIDs;
  //bool gotAllAssetIds = GetAssetIDsFromClipBoard( args.m_ClipboardData->FromBuffer(), assetIDs, assetName );

  //if ( !assetIDs.empty() )
  //{
  //  wxTreeItemId item = DragHitTest( treeCtrl, wxPoint( args.m_X, args.m_Y ) );
  //  if ( item.IsOk() )
  //  {
  //    //AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
  //    //if ( baseData )
  //    //{
  //    //  AssetCollection* collection = baseData->GetCollection<AssetCollection>();
  //    //  if ( collection && collection->CanHandleDragAndDrop() )
  //    //  {
  //    //    if ( collection->GetType() == Reflect::GetType< Editor::DependencyCollection >() )
  //    //    {
  //    //      if ( assetIDs.size() == 1 )
  //    //      {
  //    //        tuid assetID = (*assetIDs.begin());
  //    //        DependencyCollection* dependencyCollection = Reflect::ObjectCast<DependencyCollection>( collection );
  //    //        if ( dependencyCollection->GetRootID() != assetID  )
  //    //        {
  //    //          tstring warning = "Are you sure you'd like to change the existing asset from\n";
  //    //          warning += dependencyCollection->GetAssetName() + " to ";
  //    //          warning += assetName + "?";

  //    //          if ( ( dependencyCollection->GetRootID() == TUID::Null )
  //    //            || ( wxYES == 
  //    //            wxMessageBox( warning,
  //    //            "Replace Existing Asset?", 
  //    //            wxCENTER | wxYES_NO | wxICON_WARNING,
  //    //            this ) ) )
  //    //          {
  //    //            FileSystem::StripExtension( assetName );
  //    //            m_CollectionManager->GetUniqueName( assetName, assetName.c_str() );

  //    //            dependencyCollection->IsLoading( true );
  //    //            dependencyCollection->SetName( assetName );

  //    //            dependencyCollection->Freeze();

  //    //            tstring filePath;
  //    //            AssetCollection::CreateFilePath( assetName, filePath );
  //    //            dependencyCollection->SetFilePath( filePath );

  //    //            dependencyCollection->SetRootID( assetID );
  //    //            dependencyCollection->LoadDependencies();
  //    //            dependencyCollection->Thaw();
  //    //          }

  //    //        }
  //    //      }
  //    //    }
  //    //    else
  //    //    {
  //    //      collection->AddAssetIDs( assetIDs );
  //    //    }
  //    //  }
  //    //}
  //  }
  //}

  if ( m_DragOverItem.IsOk() )
  {
    treeCtrl->SetItemDropHighlight( m_DragOverItem, false );
    m_DragOverItem.Unset();
  }

  //if ( !gotAllAssetIds )
  //{
  //  wxMessageBox( "Some files could not be added to your collection", "Error", wxCENTER | wxOK | wxICON_ERROR, this );
  //}

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a drag operation leaves this control.  Clears the 
// highlighted drop item.
// 
void FoldersPanel::DragLeave( Helium::Void )
{
  SortTreeCtrl* treeCtrl = GetSortTreeCtrl();
  
  if ( m_DragOverItem.IsOk() )
  {
    treeCtrl->SetItemDropHighlight( m_DragOverItem, false );
    m_DragOverItem.Unset();
  }
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::OnTreeBeginDrag( wxTreeEvent& event )
{
  event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::OnTreeItemDelete( wxTreeEvent& event )
{
  event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::OnTreeEndDrag( wxTreeEvent& event )
{
  event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::OnTreeItemMenu( wxTreeEvent& event )
{
  event.Skip();

  //bool inFolder = m_VaultFrame->InFolder();

  //wxMenu menu;

  //// Open
  //{
  //  menu.Append( ID_Open, VaultMenu::Label( ID_Open ) );
  //  menu.AppendSeparator();
  //}

  ////Perforce
  //{
  //  wxMenu* p4Menu = new wxMenu;
  //  p4Menu->Append( ID_CheckOut, VaultMenu::Label( ID_CheckOut ) );
  //  p4Menu->Append( ID_ShowInPerforce, VaultMenu::Label( ID_ShowInPerforce ) );
  //  wxMenuItem* currentItem = menu.AppendSubMenu( p4Menu, "Perforce" );
  //  p4Menu->Enable( ID_CheckOut, true );
  //  p4Menu->Enable( ID_ShowInPerforce, true );
  //  menu.Enable( currentItem->GetId(), p4Menu->IsEnabled( ID_CheckOut ) && p4Menu->IsEnabled( ID_ShowInPerforce ) );
  //}

  //// Show In...
  //{
  //  menu.Append( ID_ShowInWindows, VaultMenu::Label( ID_ShowInWindows ) );
  //}

  //// Copy To Clipboard...
  //{
  //  wxMenu* copyToClipboardMenu = new wxMenu;
  //  copyToClipboardMenu->Append( ID_CopyPathWindows, VaultMenu::Label( ID_CopyPathWindows ) );
  //  copyToClipboardMenu->Append( ID_CopyPathClean, VaultMenu::Label( ID_CopyPathClean ) );
  //  wxMenuItem* currentItem = menu.AppendSubMenu( copyToClipboardMenu, "Copy To Clipboard" );
  //  menu.Enable( currentItem->GetId(), true );
  //}

  //// New
  //{
  //  menu.AppendSeparator();
  //  wxMenu* newMenu = m_VaultFrame->GetNewAssetMenu( true );
  //  newMenu->PrependSeparator();
  //  newMenu->Prepend( ID_NewFolder, VaultMenu::Label( ID_NewFolder ) );
  //  newMenu->Enable( ID_NewFolder, inFolder );

  //  wxMenuItem* menuItem = new wxMenuItem( &menu, ID_New, VaultMenu::Label( ID_New ), VaultMenu::Label( ID_New ), wxITEM_NORMAL, newMenu );
  //  menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_NEW ) );
  //  menu.Append( menuItem );
  //  menuItem->Enable( inFolder );
  //}

  //// Cut, Copy, Paste, Delete, Rename
  //{
  //  menu.AppendSeparator();
  //  menu.Append( ID_Cut, VaultMenu::Label( ID_Cut ) );
  //  menu.Enable( ID_Cut, false );

  //  menu.Append( ID_Copy, VaultMenu::Label( ID_Copy ) );
  //  menu.Enable( ID_Copy, false );

  //  menu.Append( ID_Paste, VaultMenu::Label( ID_Paste ) );
  //  menu.Enable( ID_Paste, false );

  //  menu.AppendSeparator();
  //  menu.Append( ID_Delete, VaultMenu::Label( ID_Delete ) );
  //  menu.Enable( ID_Delete, false );

  //  menu.Append( ID_Rename, VaultMenu::Label( ID_Rename ) );
  //  menu.Enable( ID_Rename, false );
  //}

  //// Properties
  //{
  //  menu.AppendSeparator();
  //  menu.Append( ID_Properties, VaultMenu::Label( ID_Properties ) );
  //  menu.Enable( ID_Properties, false );
  //}

  //// Show the menu
  //PopupMenu( &menu );
}

///////////////////////////////////////////////////////////////////////////////
void FoldersPanel::OnTreeItemRightClick( wxTreeEvent& event )
{
  event.Skip();
}