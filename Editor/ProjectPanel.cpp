#include "Precompile.h"

#include "ProjectPanel.h"
#include "ArtProvider.h"

#include "Core/Asset/AssetClass.h"

#include "Editor/FileDialog.h"
#include "Editor/Controls/MenuButton.h"

using namespace Helium;
using namespace Helium::Editor;

ProjectPanel::ProjectPanel( wxWindow *parent )
: ProjectPanelGenerated( parent )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_AddFileButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::FileAdd ) );
        m_AddFileButton->Enable( false );

        m_DeleteFileButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::FileDelete ) );
        m_DeleteFileButton->Enable( false );

        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Options, wxART_OTHER, wxSize(16, 16) ) );
        m_OptionsButton->SetMargins( 3, 3 );

        m_ProjectManagementPanel->Layout();

        Layout();
        Thaw();
    }

    SetHelpText( TXT( "This is the project outliner.  Manage what's included in your project here." ) );

    m_OptionsMenu = new wxMenu();
    {
        //wxMenuItem* detailsMenuItem = new wxMenuItem(
        //    m_OptionsMenu,
        //    VaultMenu::ViewResultDetails,
        //    VaultMenu::Label( VaultMenu::ViewResultDetails ),
        //    VaultMenu::Label( VaultMenu::ViewResultDetails ).c_str(),
        //    wxITEM_RADIO );
        //m_OptionsMenu->Append( detailsMenuItem );
        //Connect( VaultMenu::ViewResultDetails, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ProjectPanel::OnOptionsMenuSelect ), NULL, this );
    }
    m_OptionsButton->SetContextMenu( m_OptionsMenu );
    m_OptionsButton->SetHoldDelay( 0.0f );
    m_OptionsButton->Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( ProjectPanel::OnOptionsMenuOpen ), NULL, this );
    m_OptionsButton->Connect( wxEVT_MENU_CLOSE, wxMenuEventHandler( ProjectPanel::OnOptionsMenuClose ), NULL, this );
    m_OptionsButton->Enable( true );
    m_OptionsButton->Hide();

    m_DataViewCtrl->EnableDropTarget( wxDF_FILENAME );

    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( ProjectPanel::OnSelectionChanged ), NULL, this );
    //m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, wxDataViewEventHandler( ProjectPanel::OnItemExpanding ), NULL, this );

    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEventHandler( ProjectPanel::OnBeginDrag ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEventHandler( ProjectPanel::OnDropPossible ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP, wxDataViewEventHandler( ProjectPanel::OnDrop ), NULL, this );

    std::set< tstring > extension;
    Asset::AssetClass::GetExtensions( extension );

#pragma TODO("Why isn't hrb part of the AssetClass extensions?")
    extension.insert( TXT( "hrb" ) );

    //m_DropTarget = new FileDropTarget( extension );
    //m_DropTarget->AddDroppedListener( FileDroppedSignature::Delegate( this, &ProjectPanel::OnDroppedFiles ) );
    //m_DropTarget->AddDragOverListener( FileDragEnterSignature::Delegate( this, &ProjectPanel::DragEnter ) );
    //m_DropTarget->AddDragOverListener( FileDragOverSignature::Delegate( this, &ProjectPanel::DragOver ) );
    //m_DropTarget->AddDragLeaveListener( FileDragLeaveSignature::Delegate( this, &ProjectPanel::DragLeave ) );
    //m_DropTarget->AddDropListener( FileDroppedSignature::Delegate( this, &ProjectPanel::Drop ) );
    //m_DataViewCtrl->SetDropTarget( m_DropTarget );
}

ProjectPanel::~ProjectPanel()
{
    m_OptionsButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( ProjectPanel::OnOptionsMenuOpen ), NULL, this );
    m_OptionsButton->Disconnect( wxEVT_MENU_CLOSE, wxMenuEventHandler( ProjectPanel::OnOptionsMenuClose ), NULL, this );

    m_DataViewCtrl->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEventHandler( ProjectPanel::OnBeginDrag ), NULL, this );
    m_DataViewCtrl->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEventHandler( ProjectPanel::OnDropPossible ), NULL, this );
    m_DataViewCtrl->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP, wxDataViewEventHandler( ProjectPanel::OnDrop ), NULL, this );

    //m_DropTarget->RemoveDroppedListener( FileDroppedSignature::Delegate( this, &ProjectPanel::OnDroppedFiles ) );    
}

void ProjectPanel::SetProject( Project* project )
{
    if ( m_Project )
    {
        if ( m_Model )
        {
        }
    }

    m_AddFileButton->Enable( false );
    m_DeleteFileButton->Enable( false );
    //m_DragOverItem = wxDataViewItem( 0 );

    m_Project = project;
    if ( m_Project )
    {
        m_AddFileButton->Enable( true );
        m_Model = new ProjectViewModel();

        m_Model->SetProject( project );

        m_Model->ResetColumns();
        m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::Name ) );
        m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::FileSize ) );
        //m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::Details ) );

        // the ctrl will now hold ownership via reference count
        m_DataViewCtrl->AssociateModel( m_Model.get() );
    }
}

void ProjectPanel::OnAddFile( wxCommandEvent& event )
{
    if ( m_Project )
    {
        FileDialog openDlg( this, TXT( "Open" ), m_Project->a_Path.Get().Directory().c_str() );
#pragma TODO("Set file dialog filters from Asset::AssetClass::GetExtensions")
        //openDlg.AddFilters( ...

        if ( openDlg.ShowModal() == wxID_OK )
        {
            Path path( (const wxChar*)openDlg.GetPath().c_str() );

            Asset::AssetClassPtr asset;
            if ( _tcsicmp( path.Extension().c_str(), TXT( "hrb" ) ) == 0 )
            {
                asset = Asset::AssetClass::LoadAssetClass( path );
            }
            else
            {
                asset = Asset::AssetClass::Create( path );
            }

            if ( asset.ReferencesObject() )
            {
                m_Project->AddPath( asset->GetSourcePath() );
            }

            //wxDataViewItem parentItem = m_DataViewCtrl->GetSelection();
            //if ( parentItem.IsOk() )
            //{
            //    m_Model->AddChild( parentItem, path );
            //}
            //else
            //{
            //    m_Model->AddChild( wxDataViewItem(), path );
            //}
        }
    }
}

void ProjectPanel::OnDeleteFile( wxCommandEvent& event )
{
    if ( m_Project )
    {
        wxDataViewItemArray selection;
        int numSeleted = m_DataViewCtrl->GetSelections( selection );

        for( int index = 0; index < numSeleted; ++index )
        {
            if ( selection[index].IsOk() )
            {
                m_Model->Delete( selection[index] );
            }
        }
    }
}

void ProjectPanel::OnOptionsMenuOpen( wxMenuEvent& event )
{
    event.Skip();
    //if ( event.GetMenu() == m_OptionsMenu )
    //{
    //    // refresh menu's view toggles
    //}
}

void ProjectPanel::OnOptionsMenuClose( wxMenuEvent& event )
{
    m_DataViewCtrl->SetFocus();
    event.Skip();
}

void ProjectPanel::OnOptionsMenuSelect( wxCommandEvent& event )
{
    event.Skip();

    //switch( event.GetId() )
    //{
    //default:
    //    break;
    //};
}

void ProjectPanel::OnSelectionChanged( wxDataViewEvent& event )
{
    wxDataViewItemArray selection;
    int numSeleted = m_DataViewCtrl->GetSelections( selection );
    m_DeleteFileButton->Enable( numSeleted > 0 ? true : false );
}

//void ProjectPanel::OnItemExpanding( wxDataViewEvent& event )
//{
//    wxDataViewItem item( event.GetItem() );
//
//    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
//    if ( !node )
//    {
//        return;
//    }
//
//    if( node->GetChildren().size() < 1
//        && node->m_Path.IsDirectory() )
//    {
//        // populate the folder
//    }
//}

void ProjectPanel::OnBeginDrag( wxDataViewEvent& event )
{
    if ( m_Model )
    {
        m_Model->OnBeginDrag( event );
    }
}

void ProjectPanel::OnDropPossible( wxDataViewEvent& event )
{
    wxDataViewItem item( event.GetItem() );

    //// only allow drags for item, not containers
    //if (m_music_model->IsContainer( item ) )
    //    event.Veto();

    //if (event.GetDataFormat() != wxDF_UNICODETEXT)
    //    event.Veto();
}

void ProjectPanel::OnDrop( wxDataViewEvent& event )
{
    wxDataViewItem item( event.GetItem() );

    //// only allow drops for item, not containers
    //if (m_music_model->IsContainer( item ) )
    //{
    //    event.Veto();
    //    return;
    //}

    //if (event.GetDataFormat() != wxDF_UNICODETEXT)
    //{
    //    event.Veto();
    //    return;
    //}

    //wxTextDataObject obj;
    //obj.SetData( wxDF_UNICODETEXT, event.GetDataSize(), event.GetDataBuffer() );

    //wxLogMessage( "Text dropped: %s", obj.GetText() );
}

//void ProjectPanel::OnDroppedFiles( const FileDroppedArgs& args )
//{
//    Path path( args.m_Path );
//
//    // it's a project file
//    if ( _tcsicmp( path.FullExtension().c_str(), TXT( "project.hrb" ) ) == 0 ) 
//    {
//#pragma TODO("Close the current project and open the dropped one")
//        //(MainFrame*)GetParent()->OpenProject( path );
//    }
//    else if ( m_Project )
//    {
//        Asset::AssetClassPtr asset;
//        if ( _tcsicmp( path.Extension().c_str(), TXT( "hrb" ) ) == 0 )
//        {
//            asset = Asset::AssetClass::LoadAssetClass( path );
//        }
//        else
//        {
//            asset = Asset::AssetClass::Create( path );
//        }
//
//        if ( asset.ReferencesObject() )
//        {
//            m_Project->AddPath( asset->GetSourcePath() );
//        }
//    }
//    else
//    {
//    }
//}

///////////////////////////////////////////////////////////////////////////////
//wxDataViewItem ProjectPanel::DragHitTest( wxPoint point )
//{
//    return wxDataViewItem( 0 );
//}

///////////////////////////////////////////////////////////////////////////////
//void ProjectPanel::DragEnter( const FileDroppedArgs& args )
//{
//
//}

///////////////////////////////////////////////////////////////////////////////
//void ProjectPanel::DragOver( const FileDroppedArgs& args )
//{
//
//}

///////////////////////////////////////////////////////////////////////////////
//void ProjectPanel::Drop( const FileDroppedArgs& args )
//{
//
//}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a drag operation leaves this control.  Clears the 
// highlighted drop item.
// 
//void ProjectPanel::DragLeave( Helium::Void )
//{
//  if ( m_DragOverItem.IsOk() )
//  {
//    //m_DataViewCtrl->SetItemDropHighlight( m_DragOverItem, false );
//    m_DragOverItem = wxDataViewItem( 0 );
//  }
//}
