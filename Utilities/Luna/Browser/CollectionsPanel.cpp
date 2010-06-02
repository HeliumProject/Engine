#include "Precompile.h"
#include "CollectionsPanel.h"

#include "Browser.h"

#include "Asset/AssetFile.h"
#include "Asset/AssetFolder.h"
#include "Common/String/Utilities.h"
#include "Editor/SessionManager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/LunaSpecs.h"
#include "Inspect/ClipboardDataObject.h"
#include "Inspect/ClipboardFileList.h"
#include "Inspect/DropTarget.h"
#include "Inspect/ReflectClipboardData.h"
#include "Scene/SceneManager.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/ImageManager.h"
#include "UIToolKit/MenuButton.h"
#include "UIToolKit/SortTreeCtrl.h"


using namespace Luna;

static const std::string s_EmptyString = std::string("");

BEGIN_EVENT_TABLE( CollectionsPanel, CollectionsPanelGenerated )
EVT_MENU( BrowserMenu::ShowCollection, CollectionsPanel::OnShowCollection )
EVT_MENU( BrowserMenu::NewCollection, CollectionsPanel::OnNewCollection )
EVT_MENU( BrowserMenu::NewDependencyCollection, CollectionsPanel::OnNewCollection )
EVT_MENU( BrowserMenu::NewUsageCollection, CollectionsPanel::OnNewCollection )
EVT_MENU( BrowserMenu::OpenCollection, CollectionsPanel::OnOpenCollection )
EVT_MENU( BrowserMenu::CloseCollection, CollectionsPanel::OnCloseCollection )
EVT_MENU( BrowserMenu::RenameCollection, CollectionsPanel::OnRenameCollection )
EVT_MENU( BrowserMenu::DeleteCollection, CollectionsPanel::OnCloseCollection )
EVT_MENU( BrowserMenu::ImportCollection, CollectionsPanel::OnOpenCollection )
EVT_MENU( BrowserMenu::ImportIntoCollection, CollectionsPanel::OnImportIntoCollection )
EVT_MENU( BrowserMenu::SaveCollection, CollectionsPanel::OnSaveCollection )
EVT_MENU( BrowserMenu::AddToCollection, CollectionsPanel::OnAddToCollection )
EVT_MENU( BrowserMenu::RemoveFromCollection, CollectionsPanel::OnRemoveFromCollection )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
/// Class AssetCollectionItemData
///////////////////////////////////////////////////////////////////////////////
class Luna::AssetCollectionItemData : public wxTreeItemData
{
protected:
    AssetCollection* m_AssetCollection;

public:
    AssetCollectionItemData( AssetCollection* collection )
        : m_AssetCollection( collection )
    {
    }

    virtual ~AssetCollectionItemData()
    {
    }

    template <class T>
    T* GetCollection()
    {
        return Reflect::ObjectCast<T>( m_AssetCollection );
    }
};

///////////////////////////////////////////////////////////////////////////////
/// Class CollectionsPanel
///////////////////////////////////////////////////////////////////////////////
CollectionsPanel::CollectionsPanel( BrowserFrame* browserFrame )
: CollectionsPanelGenerated( browserFrame )
, m_BrowserFrame( browserFrame )
, m_CollectionManager( NULL )
, m_DragOriginatedHere( false )
{
    m_MyCollectionsTreeCtrl->SetImageList( UIToolKit::GlobalImageManager().GetGuiImageList() );
    m_TempCollectionsTreeCtrl->SetImageList( UIToolKit::GlobalImageManager().GetGuiImageList() );

    m_ContainerImageIndex = UIToolKit::GlobalImageManager().GetImageIndex( "folder_16.png" );
    m_DependencyImageIndex = UIToolKit::GlobalImageManager().GetImageIndex( "chart_organisation_16.png" );
    m_UsageImageIndex = UIToolKit::GlobalImageManager().GetImageIndex( "chart_organisation_reverse_16.png" );

    m_MyCollectionsToolBar->SetToolBitmapSize( wxSize( 16, 16 ) );
    m_MyCollectionsToolBar->AddTool( ID_NewCollection, "", UIToolKit::GlobalImageManager().GetBitmap( "folder_add_16.png" ), BrowserMenu::Label( ID_NewCollection ) );
    m_MyCollectionsToolBar->AddTool( ID_NewDependencyCollection, "", UIToolKit::GlobalImageManager().GetBitmap( "chart_organisation_add_16.png" ), BrowserMenu::Label( ID_NewDependencyCollection ) + " - Files this asset depends on." );
    m_MyCollectionsToolBar->AddTool( ID_NewUsageCollection, "", UIToolKit::GlobalImageManager().GetBitmap( "chart_organisation_reverse_add_16.png" ), BrowserMenu::Label( ID_NewUsageCollection ) + " - Files that use this asset." );
    m_MyCollectionsToolBar->Realize();

    Connect( wxEVT_SIZE, wxSizeEventHandler( CollectionsPanel::OnSizeCollectionsPanel ), NULL, this );

    m_BrowserFrame->GetBrowser()->GetBrowserPreferences()->AddPreferencesLoadedListener( PreferencesLoadedSignature::Delegate( this, &CollectionsPanel::OnPreferencesLoaded ) );
    m_BrowserFrame->GetBrowser()->GetBrowserPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnPrefrencesChanged ) );

    // Drag-and-drop 
    // No need to delete drop target, wx takes care of that after calling SetDropTarget.
    Inspect::DropTarget* dropTarget = new Inspect::DropTarget();
    dropTarget->SetDragOverCallback( Inspect::DragOverCallback::Delegate( this, &CollectionsPanel::DragOver ) );
    dropTarget->SetDropCallback( Inspect::DropCallback::Delegate( this, &CollectionsPanel::Drop ) );
    dropTarget->SetDragLeaveCallback( Inspect::DragLeaveCallback::Delegate( this, &CollectionsPanel::DragLeave ) );
    m_MyCollectionsTreeCtrl->SetDropTarget( dropTarget );

    UpdateCollectionManager();
}

CollectionsPanel::~CollectionsPanel()
{
    Disconnect( wxEVT_SIZE, wxSizeEventHandler( CollectionsPanel::OnSizeCollectionsPanel ), NULL, this );

    m_BrowserFrame->GetBrowser()->GetBrowserPreferences()->RemovePreferencesLoadedListener( PreferencesLoadedSignature::Delegate( this, &CollectionsPanel::OnPreferencesLoaded ) );
    m_BrowserFrame->GetBrowser()->GetBrowserPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnPrefrencesChanged ) );

    for ( M_AssetCollections::const_iterator itr = m_CollectionManager->GetCollections().begin(),
        end = m_CollectionManager->GetCollections().end(); itr != end; ++itr )
    {    
        AssetCollection* collection = itr->second;
        collection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnCollectionModified ) );
    }

    DisconnectCollectionManagerListeners();

    m_CollectionToItemIDs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnSizeCollectionsPanel( wxSizeEvent& event )
{
    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnTempCollectionsDoubleClick( wxTreeEvent& event )
{
    event.Skip();

    // this just makes the code below more generic in case we want to abstract it later
    UIToolKit::SortTreeCtrl* treeCtrl = m_TempCollectionsTreeCtrl;

    wxTreeItemId item = treeCtrl->GetSelection();
    if ( !item )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( collection )
    {
        m_BrowserFrame->Search( s_EmptyString, collection );
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnMyCollectionsTitleMenu( wxMouseEvent& event )
{
    event.Skip();

    wxMenu* newMenu = new wxMenu();
    newMenu->Append( ID_NewCollection, BrowserMenu::Label( ID_NewCollection ), BrowserMenu::Label( ID_NewCollection ) + " - Manually add and remove assets to this static collection." );
    newMenu->Append( ID_NewDependencyCollection, BrowserMenu::Label( ID_NewDependencyCollection ) + " - Files this asset depends on." );
    newMenu->Append( ID_NewUsageCollection, BrowserMenu::Label( ID_NewUsageCollection ), BrowserMenu::Label( ID_NewUsageCollection ) + " - Files that use this asset." );

    wxMenu menu;
    menu.Append( wxID_ANY, "New Collection...", newMenu );
    menu.AppendSeparator();
    //menu.Append( ID_OpenCollection, BrowserMenu::Label( ID_OpenCollection ) );
    menu.Append( ID_ImportCollection, BrowserMenu::Label( ID_ImportCollection ) );

    // Show the menu
    wxPoint pos = event.GetPosition();
    PopupMenu( &menu );
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnMyCollectionsBeginLabelEdit( wxTreeEvent& event )
{
    // this just makes the code below more generic in case we want to abstract it later
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;

    wxTreeItemId item = event.GetItem();
    if ( !item )
    {
        event.Veto(); 
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( m_MyCollectionsTreeCtrl, item );
    if ( !baseData )
    {
        event.Veto();
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( !collection || !collection->CanRename() )
    {
        event.Veto();
        return;
    }


    treeCtrl->SetItemText( item, collection->GetName() );
    //treeCtrl->EditLabel( item );

    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnMyCollectionsEndLabelEdit( wxTreeEvent& event )
{
    // this just makes the code below more generic in case we want to abstract it later
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;

    event.Veto();

    if( event.IsEditCancelled() )
    {
        return;
    }

    wxTreeItemId item = event.GetItem();
    if ( !item )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( !collection || !collection->CanRename() )
    {
        return;
    }

    wxString labelValue = event.GetLabel();
    labelValue.Trim(true);  // trim white-space right 
    labelValue.Trim(false); // trim white-space left

    if ( labelValue.empty() )
    {
        return;
    }

    std::string errors;

    collection->Freeze();
    bool renameResult = m_CollectionManager->RenameCollection( collection, labelValue.c_str(), errors );
    collection->Thaw();
    if ( !renameResult )
    {
        wxMessageBox( errors.c_str(), "MyCollection Name Already In Use", wxCENTER | wxICON_WARNING | wxOK, this );
        //treeCtrl->EditLabel( item );
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnMyCollectionsDoubleClick( wxTreeEvent& event )
{
    // this just makes the code below more generic in case we want to abstract it later
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;

    wxTreeItemId item = treeCtrl->GetSelection();
    if ( !item )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( collection )
    {
        m_BrowserFrame->Search( s_EmptyString, collection );
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnMyCollectionsMenu( wxTreeEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;

    wxTreeItemId item = event.GetItem();
    if ( !item )
    {
        return;
    }

    wxTreeItemId rootID = treeCtrl->GetRootItem();
    if( item != rootID )
    {
        treeCtrl->SelectItem( item );

        AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
        if ( !baseData )
        {
            return;
        }

        AssetCollection* collection = baseData->GetCollection<AssetCollection>();
        if ( collection )
        {
            Asset::V_AssetFiles files;
            Asset::V_AssetFolders folders;
            m_BrowserFrame->GetSelectedFilesAndFolders( files, folders );

            wxMenu menu;

            menu.Append( ID_ShowCollection, BrowserMenu::Label( ID_ShowCollection ), BrowserMenu::Label( ID_ShowCollection ), wxITEM_NORMAL );
            menu.AppendSeparator();
            menu.Append( ID_AddToCollection, BrowserMenu::Label( ID_AddToCollection ), BrowserMenu::Label( ID_AddToCollection ), wxITEM_NORMAL );
            menu.Append( ID_RemoveFromCollection, BrowserMenu::Label( ID_RemoveFromCollection ), BrowserMenu::Label( ID_RemoveFromCollection ), wxITEM_NORMAL );
            menu.AppendSeparator();
            menu.Append( ID_ImportIntoCollection, BrowserMenu::Label( ID_ImportIntoCollection ), BrowserMenu::Label( ID_ImportIntoCollection ), wxITEM_NORMAL );
            menu.Append( ID_SaveCollection, BrowserMenu::Label( ID_SaveCollection ), BrowserMenu::Label( ID_SaveCollection ), wxITEM_NORMAL );
            menu.AppendSeparator();
            menu.Append( ID_RenameCollection, BrowserMenu::Label( ID_RenameCollection ), BrowserMenu::Label( ID_RenameCollection ), wxITEM_NORMAL );
            //menu.Append( ID_CloseCollection, BrowserMenu::Label( ID_CloseCollection ), BrowserMenu::Label( ID_CloseCollection ), wxITEM_NORMAL );
            menu.Append( ID_DeleteCollection, BrowserMenu::Label( ID_DeleteCollection ), BrowserMenu::Label( ID_DeleteCollection ), wxITEM_NORMAL );


            menu.Enable( ID_AddToCollection, !collection->IsDynamic() && !files.empty() );
            menu.Enable( ID_RemoveFromCollection, !collection->IsDynamic() && !files.empty() );

            menu.Enable( ID_ImportIntoCollection, !collection->IsDynamic() );
            menu.Enable( ID_SaveCollection, true );

            menu.Enable( ID_RenameCollection, collection->CanRename() );
            //menu.Enable( ID_CloseCollection, !collection->IsTemporary() );
            menu.Enable( ID_DeleteCollection, !collection->IsTemporary() );

            // Show the menu
            PopupMenu( &menu );
        }
    } 
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnShowCollection( wxCommandEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;
    wxTreeItemId rootID = treeCtrl->GetRootItem();

    wxTreeItemId item = treeCtrl->GetSelection();
    if( !item || item == rootID )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( collection )
    {
        m_BrowserFrame->Search( s_EmptyString, collection );
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnNewCollection( wxCommandEvent& event )
{
    int eventID = event.GetId();
    AssetCollection* collection = NULL;
    switch ( eventID )
    {
    default:
        collection = NULL;
        break;

    case ID_NewCollection:
        collection = NewCollection( m_CollectionManager, Reflect::GetType< Luna::AssetCollection >() );
        break;

    case ID_NewDependencyCollection:
        collection = NewCollection( m_CollectionManager, Reflect::GetType< Luna::DependencyCollection >() );
        break;

    case ID_NewUsageCollection:
        collection = NewCollection( m_CollectionManager, Reflect::GetType< Luna::DependencyCollection >() );
        Reflect::ObjectCast<DependencyCollection>( collection )->SetReverse( true );
        break;
    }

    M_CollectionToItemID::iterator findItem = m_CollectionToItemIDs.find( collection->GetFileReference().GetHash() );
    if ( findItem != m_CollectionToItemIDs.end()
        && findItem->second.IsOk() )
    {
        m_MyCollectionsTreeCtrl->EditLabel( findItem->second );
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnOpenCollection( wxCommandEvent& event )
{
    UIToolKit::FileDialog browserDlg( this, BrowserMenu::Label( ID_OpenCollection ), "", "", "",
        UIToolKit::FileDialogStyles::DefaultOpen | UIToolKit::FileDialogStyles::ShowAllFilesFilter | UIToolKit::FileDialogStyles::ExportFile );

    browserDlg.AddFilter( FinderSpecs::Luna::ASSET_COLLECTION_RB_DECORATION.GetDialogFilter() );
    browserDlg.AddFilter( FinderSpecs::Luna::ASSET_COLLECTION_XML_DECORATION.GetDialogFilter() );
    browserDlg.SetFilterIndex( FinderSpecs::Luna::ASSET_COLLECTION_RB_DECORATION.GetDialogFilter() );

    if ( browserDlg.ShowModal() == wxID_OK )
    {
        AssetCollection* collection = NULL;

        if ( event.GetId() == ID_OpenCollection )
        {
            collection = m_CollectionManager->OpenCollection( browserDlg.GetPath().c_str() );
        }
        else
        {
            collection = m_CollectionManager->ImportCollection( browserDlg.GetPath().c_str() );
        }

        if ( collection )
        {
            m_BrowserFrame->Search( s_EmptyString, collection );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnCloseCollection( wxCommandEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;
    wxTreeItemId rootID = treeCtrl->GetRootItem();

    wxTreeItemId item = treeCtrl->GetSelection();
    if( !item || item == rootID )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( collection && !collection->IsTemporary() )
    {
        if ( event.GetId() == ID_DeleteCollection )
        {
            std::string warning = "Are you sure you'd like permenantly delete your collection \"";
            warning += collection->GetName() + "\"?";

            if ( wxYES == 
                wxMessageBox( warning,
                "Delete Colleciton?", 
                wxCENTER | wxYES_NO | wxICON_WARNING,
                this ) )
            {
                m_CollectionManager->DeleteCollection( collection );
            }
        }
        else
        {
            m_CollectionManager->CloseCollection( collection );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnRenameCollection( wxCommandEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;
    wxTreeItemId rootID = treeCtrl->GetRootItem();

    wxTreeItemId item = treeCtrl->GetSelection();
    if( !item || item == rootID )
    {
        return;
    }

    treeCtrl->EditLabel( item );
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnImportIntoCollection( wxCommandEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;
    wxTreeItemId rootID = treeCtrl->GetRootItem();

    wxTreeItemId item = treeCtrl->GetSelection();
    if( !item || item == rootID )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( !collection || collection->IsDynamic() )
    {
        return;
    }

    UIToolKit::FileDialog browserDlg( this, BrowserMenu::Label( ID_ImportIntoCollection ), "", "", "",
        UIToolKit::FileDialogStyles::DefaultOpen | UIToolKit::FileDialogStyles::ShowAllFilesFilter | UIToolKit::FileDialogStyles::ExportFile );

    browserDlg.AddFilter( FinderSpecs::Luna::ASSET_COLLECTION_RB_DECORATION.GetDialogFilter() );
    browserDlg.AddFilter( FinderSpecs::Luna::ASSET_COLLECTION_XML_DECORATION.GetDialogFilter() );
    browserDlg.SetFilterIndex( FinderSpecs::Luna::ASSET_COLLECTION_RB_DECORATION.GetDialogFilter() );

    if ( browserDlg.ShowModal() == wxID_OK )
    {
        collection->Freeze();
        m_CollectionManager->ImportIntoStaticCollection( collection, browserDlg.GetPath().c_str() );
        collection->Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnSaveCollection( wxCommandEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;
    wxTreeItemId rootID = treeCtrl->GetRootItem();

    wxTreeItemId item = treeCtrl->GetSelection();
    if( !item || item == rootID )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( collection )
    {
        collection->GetFileReference().Resolve();
        Nocturnal::Path collectionPath = collection->GetFileReference().GetFile().GetPath();
        std::string defaultDir( collectionPath.Directory() );
        std::string defaultFile( collectionPath.Filename() );

        UIToolKit::FileDialog browserDlg( this, BrowserMenu::Label( ID_SaveCollection ), defaultDir.c_str(), defaultFile.c_str(), "",
            UIToolKit::FileDialogStyles::DefaultSave | UIToolKit::FileDialogStyles::ShowAllFilesFilter | UIToolKit::FileDialogStyles::ExportFile );

        browserDlg.AddFilter( FinderSpecs::Luna::ASSET_COLLECTION_RB_DECORATION.GetDialogFilter() );
        browserDlg.AddFilter( FinderSpecs::Luna::ASSET_COLLECTION_XML_DECORATION.GetDialogFilter() );
        browserDlg.SetFilterIndex( FinderSpecs::Luna::ASSET_COLLECTION_XML_DECORATION.GetDialogFilter() );

        if ( browserDlg.ShowModal() == wxID_OK )
        {
            m_CollectionManager->SaveCollection( collection, browserDlg.GetPath().c_str() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnAddToCollection( wxCommandEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;
    wxTreeItemId rootID = treeCtrl->GetRootItem();

    wxTreeItemId item = treeCtrl->GetSelection();
    if( !item || item == rootID )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( !collection || collection->IsDynamic() )
    {
        return;
    }

    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders;
    m_BrowserFrame->GetSelectedFilesAndFolders( files, folders );

    if ( !folders.empty() )
    {
        // TODO warn them that you cant add folders
    }

    if ( !files.empty() )
    {
        collection->Freeze();
        for ( Asset::V_AssetFiles::const_iterator fileItr = files.begin(), fileEnd = files.end();
            fileItr != fileEnd; ++fileItr )
        {
            collection->AddAsset( *( (*fileItr)->GetFileReference() ) );
        }
        collection->Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnRemoveFromCollection( wxCommandEvent& event )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;
    wxTreeItemId rootID = treeCtrl->GetRootItem();

    wxTreeItemId item = treeCtrl->GetSelection();
    if( !item || item == rootID )
    {
        return;
    }

    AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    if ( !baseData )
    {
        return;
    }

    AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    if ( !collection || collection->IsDynamic() )
    {
        return;
    }

    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders;
    m_BrowserFrame->GetSelectedFilesAndFolders( files, folders );

    if ( !folders.empty() )
    {
        // TODO warn them that you cant add folders
    }

    if ( !files.empty() )
    {
        collection->Freeze();
        for ( Asset::V_AssetFiles::const_iterator fileItr = files.begin(), fileEnd = files.end();
            fileItr != fileEnd; ++fileItr )
        {
            collection->RemoveAsset( *( (*fileItr)->GetFileReference() ) );
        }
        collection->Thaw();
    }
}


///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnPreferencesLoaded( const PreferencesLoadedArgs& args )
{
    UpdateCollectionManager();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnPrefrencesChanged( const Reflect::ElementChangeArgs& args )
{
    UpdateCollectionManager();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnAssetCollectionsChanged( const Reflect::ElementChangeArgs& args )
{
    NOC_ASSERT( m_CollectionManager == args.m_Element );
    for ( M_AssetCollections::const_iterator collectionItr = m_CollectionManager->GetCollections().begin(), 
        collectionEnd = m_CollectionManager->GetCollections().end(); collectionItr != collectionEnd; ++collectionItr )
    {
        AssetCollection* collection = collectionItr->second;
        collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnCollectionModified ) );
    }
    UpdateCollections();
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::OnCollectionModified( const Reflect::ElementChangeArgs& args )
{
    const AssetCollection* collection = Reflect::ConstObjectCast<AssetCollection>( args.m_Element );
    if ( !collection )
        return;

    collection->GetFileReference().Resolve();
    M_CollectionToItemID::iterator findItem = m_CollectionToItemIDs.find( collection->GetFileReference().GetHash() );
    if ( findItem != m_CollectionToItemIDs.end()
        && findItem->second.IsOk() )
    {
        wxTreeItemId itemID = findItem->second;

        UIToolKit::SortTreeCtrl* treeCtrl = collection->IsTemporary() ? m_TempCollectionsTreeCtrl : m_MyCollectionsTreeCtrl; 
        treeCtrl->SetItemText( itemID, collection->GetDisplayName() );

        int iconIndex = m_ContainerImageIndex;
        if ( collection->GetType() == Reflect::GetType< Luna::AssetCollection >() )
        {
            iconIndex = m_ContainerImageIndex;
        }
        else if ( collection->GetType() == Reflect::GetType< Luna::DependencyCollection >() )
        {
            if ( Reflect::ConstObjectCast<Luna::DependencyCollection>( collection )->IsReverse() )
            {
                iconIndex = m_UsageImageIndex;
            }
            else
            {
                iconIndex = m_DependencyImageIndex;
            }
        }
        treeCtrl->SetItemImage( itemID, iconIndex, wxTreeItemIcon_Normal );
    }

}

void CollectionsPanel::OnCollectionAdded( const CollectionManagerArgs& args )
{
    args.m_Collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnCollectionModified ) );
}

void CollectionsPanel::OnCollectionRemoving( const CollectionManagerArgs& args )
{
    args.m_Collection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnCollectionModified ) );
}

void CollectionsPanel::OnClearAllCollections( const CollectionManagerArgs& args )
{
    for ( M_AssetCollections::const_iterator collectionItr = m_CollectionManager->GetCollections().begin(), 
        collectionEnd = m_CollectionManager->GetCollections().end(); collectionItr != collectionEnd; ++collectionItr )
    {
        AssetCollection* collection = collectionItr->second;
        collection->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnCollectionModified ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for hit testing a point during a drag and drop operation.
// 
wxTreeItemId CollectionsPanel::DragHitTest( UIToolKit::SortTreeCtrl* treeCtrl, wxPoint point )
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
wxDragResult CollectionsPanel::DragOver( const Inspect::DragArgs& args )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;

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
            AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
            if ( baseData )
            {
                AssetCollection* collection = baseData->GetCollection<AssetCollection>();
                if ( !collection || !collection->CanHandleDragAndDrop() )
                {
                    result = wxDragNone;
                }
            }
            else
            {
                result = wxDragNone;
            }
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
wxDragResult CollectionsPanel::Drop( const Inspect::DragArgs& args )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;

    wxDragResult result = wxDragNone;

#pragma TODO( "Reimplement for File::References" )
    //// get the IDs
    //std::string assetName;
    //S_tuid assetIDs;
    //bool gotAllAssetIds = GetAssetIDsFromClipBoard( args.m_ClipboardData->FromBuffer(), assetIDs, assetName );

    //if ( !assetIDs.empty() )
    //{
    //    wxTreeItemId item = DragHitTest( treeCtrl, wxPoint( args.m_X, args.m_Y ) );
    //    if ( item.IsOk() )
    //    {
    //        AssetCollectionItemData* baseData = GetItemData( treeCtrl, item );
    //        if ( baseData )
    //        {
    //            AssetCollection* collection = baseData->GetCollection<AssetCollection>();
    //            if ( collection && collection->CanHandleDragAndDrop() )
    //            {
    //                if ( collection->GetType() == Reflect::GetType< Luna::DependencyCollection >() )
    //                {
    //                    if ( assetIDs.size() == 1 )
    //                    {
    //                        tuid assetID = (*assetIDs.begin());
    //                        DependencyCollection* dependencyCollection = Reflect::ObjectCast<DependencyCollection>( collection );
    //                        if ( dependencyCollection->GetRootID() != assetID  )
    //                        {
    //                            std::string warning = "Are you sure you'd like to change the existing asset from\n";
    //                            warning += dependencyCollection->GetAssetName() + " to ";
    //                            warning += assetName + "?";

    //                            if ( ( dependencyCollection->GetRootID() == TUID::Null )
    //                                || ( wxYES == 
    //                                wxMessageBox( warning,
    //                                "Replace Existing Asset?", 
    //                                wxCENTER | wxYES_NO | wxICON_WARNING,
    //                                this ) ) )
    //                            {
    //                                FileSystem::StripExtension( assetName );
    //                                m_CollectionManager->GetUniqueName( assetName, assetName.c_str() );

    //                                dependencyCollection->IsLoading( true );
    //                                dependencyCollection->SetName( assetName );

    //                                dependencyCollection->Freeze();

    //                                std::string filePath;
    //                                AssetCollection::CreateFilePath( assetName, filePath );
    //                                dependencyCollection->SetFilePath( filePath );

    //                                dependencyCollection->SetRootID( assetID );
    //                                dependencyCollection->LoadDependencies();
    //                                dependencyCollection->Thaw();
    //                            }

    //                        }
    //                    }
    //                }
    //                else
    //                {
    //                    collection->AddAssetIDs( assetIDs );
    //                }
    //            }
    //        }
    //    }
    //}

    //if ( m_DragOverItem.IsOk() )
    //{
    //    treeCtrl->SetItemDropHighlight( m_DragOverItem, false );
    //    m_DragOverItem.Unset();
    //}

    //if ( !gotAllAssetIds )
    //{
    //    wxMessageBox( "Some files could not be added to your collection", "Error", wxCENTER | wxOK | wxICON_ERROR, this );
    //}

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a drag operation leaves this control.  Clears the 
// highlighted drop item.
// 
void CollectionsPanel::DragLeave( Nocturnal::Void )
{
    UIToolKit::SortTreeCtrl* treeCtrl = m_MyCollectionsTreeCtrl;

    if ( m_DragOverItem.IsOk() )
    {
        treeCtrl->SetItemDropHighlight( m_DragOverItem, false );
        m_DragOverItem.Unset();
    }
}

///////////////////////////////////////////////////////////////////////////////
AssetCollection* CollectionsPanel::NewCollection( CollectionManager* collectionManager, const i32 typeID, const std::string& tryName )
{
    std::string name = tryName.empty() ? "New Collection" : tryName;
    collectionManager->GetUniqueName( name, name.c_str() );

    AssetCollectionPtr collection = NULL;
    bool added = false;
    {
        collection = Reflect::AssertCast< AssetCollection >( Reflect::Registry::GetInstance()->CreateInstance( typeID ) );
        collection->SetName( name );

        u32 flags = collection->GetFlags();
        flags |= AssetCollectionFlags::CanRename | AssetCollectionFlags::CanHandleDragAndDrop;
        collection->SetFlags( flags );

        if ( !collectionManager->AddCollection( collection ) )
        {
            // TODO: warn or something?
        }
    }

    return collection;
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::UpdateCollectionManager()
{
    if ( m_CollectionManager )
    {
        DisconnectCollectionManagerListeners();
    }

    m_CollectionManager = m_BrowserFrame->GetBrowser()->GetBrowserPreferences()->GetCollectionManager();
    ConnectCollectionManagerListeners();
    ConnectCollectionListeners();
    UpdateCollections();
}

void CollectionsPanel::ConnectCollectionManagerListeners()
{
    m_CollectionManager->AddClearAllListener( CollectionManagerSignature::Delegate( this, &CollectionsPanel::OnClearAllCollections ) );
    m_CollectionManager->AddCollectionAddedListener( CollectionManagerSignature::Delegate( this, &CollectionsPanel::OnCollectionAdded ) );
    m_CollectionManager->AddCollectionRemovingListener( CollectionManagerSignature::Delegate( this, &CollectionsPanel::OnCollectionRemoving ) );
    m_CollectionManager->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnAssetCollectionsChanged ) );
}

void CollectionsPanel::DisconnectCollectionManagerListeners()
{
    m_CollectionManager->RemoveClearAllListener( CollectionManagerSignature::Delegate( this, &CollectionsPanel::OnClearAllCollections ) );
    m_CollectionManager->RemoveCollectionAddedListener( CollectionManagerSignature::Delegate( this, &CollectionsPanel::OnCollectionAdded ) );
    m_CollectionManager->RemoveCollectionRemovingListener( CollectionManagerSignature::Delegate( this, &CollectionsPanel::OnCollectionRemoving ) );
    m_CollectionManager->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnAssetCollectionsChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::ConnectCollectionListeners()
{
    for ( M_AssetCollections::const_iterator itr = m_CollectionManager->GetCollections().begin(),
        end = m_CollectionManager->GetCollections().end(); itr != end; ++itr )
    {    
        AssetCollection* collection = itr->second;
        collection->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &CollectionsPanel::OnCollectionModified ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::UpdateCollections()
{
    m_CollectionToItemIDs.clear();

    PrePopulateTreeCtrl( m_TempCollectionsTreeCtrl );
    PrePopulateTreeCtrl( m_MyCollectionsTreeCtrl );
    {   
        u32 tempItemsAdded = 0;
        u32 myItemsAdded = 0;
        for ( M_AssetCollections::const_iterator itr = m_CollectionManager->GetCollections().begin(),
            end = m_CollectionManager->GetCollections().end(); itr != end; ++itr )
        {    
            AssetCollection* collection = itr->second;
            NOC_ASSERT( collection );

            if ( !collection )
                continue;

            int iconIndex = m_ContainerImageIndex;
            if ( collection->GetType() == Reflect::GetType< Luna::AssetCollection >() )
            {
                iconIndex = m_ContainerImageIndex;
            }
            else if ( collection->GetType() == Reflect::GetType< Luna::DependencyCollection >() )
            {
                if ( Reflect::ObjectCast<DependencyCollection>( collection )->IsReverse() )
                {
                    iconIndex = m_UsageImageIndex;
                }
                else
                {
                    iconIndex = m_DependencyImageIndex;
                }
            }

            UIToolKit::SortTreeCtrl* treeCtrl = NULL;
            if ( collection->IsTemporary() )
            {
                treeCtrl = m_TempCollectionsTreeCtrl;
                ++tempItemsAdded;
            }
            else
            {
                treeCtrl = m_MyCollectionsTreeCtrl;
                ++myItemsAdded;
            }

            wxTreeItemId itemID = treeCtrl->AppendItem(
                treeCtrl->GetRootItem(),
                collection->GetDisplayName(),
                iconIndex,
                -1,
                new AssetCollectionItemData( collection ) );

            collection->GetFileReference().Resolve();
            m_CollectionToItemIDs.insert( M_CollectionToItemID::value_type( collection->GetFileReference().GetHash(), itemID ) );
        }

        m_TempCollectionsTreeCtrl->Enable( tempItemsAdded > 0 );
        if ( tempItemsAdded < 1 )
        {
            wxTreeItemId itemID = m_TempCollectionsTreeCtrl->AppendItem( m_TempCollectionsTreeCtrl->GetRootItem(), "None Available" );
        }
    }
    PostPopulateTreeCtrl( m_MyCollectionsTreeCtrl );
    PostPopulateTreeCtrl( m_TempCollectionsTreeCtrl );
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::PrePopulateTreeCtrl( UIToolKit::SortTreeCtrl* treeCtrl )
{
    treeCtrl->Freeze();
    treeCtrl->DeleteAllItems();

    // Always add the root
    wxTreeItemId root = treeCtrl->AddRoot( "HIDDEN ROOT" );
}

///////////////////////////////////////////////////////////////////////////////
void CollectionsPanel::PostPopulateTreeCtrl( UIToolKit::SortTreeCtrl* treeCtrl )
{
    treeCtrl->SortChildren( treeCtrl->GetRootItem() );
    treeCtrl->Thaw();

    // Make sure everything is visible if it can be
    int charHeight = treeCtrl->GetCharHeight() + 10; // 10 to account for 5 above/below spacing
    int numChildren = (int)treeCtrl->GetChildrenCount( treeCtrl->GetRootItem() );
    int newHeight = ( numChildren * charHeight ) + 10; // another 10 - same reason as above

    wxSize ctrlSize = treeCtrl->GetSize();
    ctrlSize.SetHeight( newHeight );
    treeCtrl->SetMinSize( ctrlSize );

    GetSizer()->Layout();

    Layout();
    Refresh();
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to get the interface to the data stored on each tree item.
// 
AssetCollectionItemData* CollectionsPanel::GetItemData( UIToolKit::SortTreeCtrl* treeCtrl, const wxTreeItemId& id )
{
    AssetCollectionItemData* data = NULL;
    if ( id.IsOk() )
    {
        wxTreeItemData* baseData = treeCtrl->GetItemData( id );
        if ( baseData )
        {
            data = reinterpret_cast< AssetCollectionItemData* >( baseData );
        }

        if ( !data )
        {
            Console::Error( "Tree item (%x: %s) does not have asset data associated with it\n", id.m_pItem, treeCtrl->GetItemText( id ).c_str() );
            NOC_BREAK();
        }
    }
    else
    {
        Console::Error( "Tree item does not have asset data associated with it!\n" );
        NOC_BREAK();
    }



    return data;
}
