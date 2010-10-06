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

#pragma TODO( "Use overlays for the add/create icons below" )
        m_AddFile->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::FileAdd ) );
        m_DeleteFile->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::FileDelete ) );

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

    m_DataViewCtrl->EnableDragSource( wxDF_UNICODETEXT );
    m_DataViewCtrl->EnableDropTarget( wxDF_UNICODETEXT );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEventHandler( ProjectPanel::OnBeginDrag ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEventHandler( ProjectPanel::OnDropPossible ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP, wxDataViewEventHandler( ProjectPanel::OnDrop ), NULL, this );

    std::set< tstring > extension;
    Asset::AssetClass::GetExtensions( extension );
    m_DropTarget = new FileDropTarget( extension );
    m_DropTarget->AddListener( FileDroppedSignature::Delegate( this, &ProjectPanel::OnDroppedFiles ) );
    SetDropTarget( m_DropTarget );
}

ProjectPanel::~ProjectPanel()
{
    m_OptionsButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( ProjectPanel::OnOptionsMenuOpen ), NULL, this );
    m_OptionsButton->Disconnect( wxEVT_MENU_CLOSE, wxMenuEventHandler( ProjectPanel::OnOptionsMenuClose ), NULL, this );

    m_DataViewCtrl->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEventHandler( ProjectPanel::OnBeginDrag ), NULL, this );
    m_DataViewCtrl->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEventHandler( ProjectPanel::OnDropPossible ), NULL, this );
    m_DataViewCtrl->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP, wxDataViewEventHandler( ProjectPanel::OnDrop ), NULL, this );
}

void ProjectPanel::SetProject( Project* project )
{
    m_Project = project;
    m_Model = new ProjectViewModel();

    m_Model->SetProject( project );

    m_Model->ResetColumns();
    m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::Name ) );
    m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::Details ) );

    // the ctrl will now hold ownership via reference count
    m_DataViewCtrl->AssociateModel( m_Model.get() );
}

void ProjectPanel::OnAddFile( wxCommandEvent& event )
{
    if ( m_Project )
    {
        FileDialog openDlg( this, TXT( "Open" ), m_Project->a_Path.Get().Directory().c_str() );

        if ( openDlg.ShowModal() == wxID_OK )
        {
            Path path( (const wxChar*)openDlg.GetPath().c_str() );
            m_Project->AddPath( path );
        }
    }
}

void ProjectPanel::OnDeleteFile( wxCommandEvent& event )
{
    if ( m_Project )
    {
        for ( Helium::OrderedSet< Path* >::Iterator itr = m_Selected.Begin(), end = m_Selected.End(); itr != end; ++itr )
        {
            m_Project->RemovePath( *(*itr) );
        }
    }
}

void ProjectPanel::OnOptionsMenuOpen( wxMenuEvent& event )
{
    event.Skip();
    if ( event.GetMenu() == m_OptionsMenu )
    {
        // refresh menu's view toggles
    }
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

void ProjectPanel::OnBeginDrag( wxDataViewEvent& event )
{
    wxDataViewItem item( event.GetItem() );

    //// only allow drags for item, not containers
    //if (m_music_model->IsContainer( item ) )
    //{
    //    event.Veto();
    //    return;
    //}

    //MyMusicTreeModelNode *node = (MyMusicTreeModelNode*) item.GetID();
    //wxTextDataObject *obj = new wxTextDataObject;
    //obj->SetText( node->m_title );
    //event.SetDataObject( obj );
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

void ProjectPanel::OnDroppedFiles( const FileDroppedArgs& args )
{
    HELIUM_ASSERT( m_Project );

    Asset::AssetClassPtr asset = Asset::AssetClass::Create( args.m_Path );

    if ( asset.ReferencesObject() )
    {
        if ( asset->GetSourcePath().Exists() )
        {
            m_Project->AddPath( asset->GetSourcePath().GetRelativePath( m_Project->a_Path.Get() ) );
        }
    }
}
