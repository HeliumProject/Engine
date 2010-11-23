#include "Precompile.h"

#include "ProjectPanel.h"
#include "ArtProvider.h"

#include "Pipeline/Asset/AssetClass.h"

#include "Editor/App.h"
#include "Editor/FileDialog.h"
#include "Editor/Controls/MenuButton.h"
#include "Editor/MainFrame.h"

using namespace Helium;
using namespace Helium::Editor;

ProjectPanel::ProjectPanel( wxWindow *parent, DocumentManager* documentManager )
: ProjectPanelGenerated( parent )
, m_DocumentManager( documentManager )
, m_Project( NULL )
, m_Model( NULL )
, m_OptionsMenu( NULL )
, m_ContextMenu( NULL )
, m_DropTarget( NULL )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Options, wxART_OTHER, wxSize(16, 16) ) );
        m_OptionsButton->SetMargins( 3, 3 );
        m_OptionsButton->Hide();
        m_OptionsButtonStaticLine->Hide();

        m_ProjectManagementPanel->Layout();

        m_OpenProjectPanel->Hide();
        m_DataViewCtrl->Show();

        Layout();
        Thaw();
    }

    SetHelpText( TXT( "This is the project outliner.  Manage what's included in your project here." ) );

    m_ProjectNameStaticText->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ProjectPanel::OnOpenProject ), NULL, this );

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
    m_OptionsButton->Enable( false );

    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( ProjectPanel::OnSelectionChanged ), NULL, this );

    m_ContextMenu = new wxMenu();
    {
        wxMenuItem* addItem = m_ContextMenu->Append( wxNewId(), wxT( "Add Item(s)..." ), wxT( "Allows you to add items to the project." ) );
        Connect( addItem->GetId(), wxCommandEventHandler( ProjectPanel::OnAddItems ), NULL, this );

        wxMenuItem* deleteItem = m_ContextMenu->Append( wxNewId(), wxT( "Remove Selected Item(s)" ), wxT( "Removes the selected item(s) from the project." ) );
        Connect( deleteItem->GetId(), wxCommandEventHandler( ProjectPanel::OnDeleteItems ), NULL, this );
    }
    m_DataViewCtrl->Connect( wxEVT_CONTEXT_MENU, wxContextMenuEventHandler( ProjectPanel::OnContextMenu ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxContextMenuEventHandler( ProjectPanel::OnContextMenu ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( ProjectPanel::OnActivateItem ), NULL, this );

    std::set< tstring > extension;
    Asset::AssetClass::GetExtensions( extension );

#pragma TODO("Why isn't hrb part of the AssetClass extensions?")
    extension.insert( TXT( "hrb" ) );
    extension.insert( TXT( "obj" ) );

    m_DataViewCtrl->EnableDropTarget( wxDF_FILENAME );
    m_DropTarget = new FileDropTarget( extension );
    //m_DropTarget->AddDragOverListener( FileDragEnterSignature::Delegate( this, &ProjectPanel::DragEnter ) );
    m_DropTarget->AddDragOverListener( FileDragOverSignature::Delegate( this, &ProjectPanel::OnDragOver ) );
    //m_DropTarget->AddDragLeaveListener( FileDragLeaveSignature::Delegate( this, &ProjectPanel::DragLeave ) );
    m_DropTarget->AddDroppedListener( FileDroppedSignature::Delegate( this, &ProjectPanel::OnDroppedFiles ) );
    m_DataViewCtrl->GetMainWindow()->SetDropTarget( m_DropTarget );
}

ProjectPanel::~ProjectPanel()
{
    if ( m_Project )
    {
        CloseProject();
    }

    m_OptionsButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( ProjectPanel::OnOptionsMenuOpen ), NULL, this );
    m_OptionsButton->Disconnect( wxEVT_MENU_CLOSE, wxMenuEventHandler( ProjectPanel::OnOptionsMenuClose ), NULL, this );

    if ( m_Model )
    {
        m_DocumentManager->e_DocumentOpened.RemoveMethod( m_Model.get(), &ProjectViewModel::OnDocumentOpened );
        m_DocumentManager->e_DocumenClosed.RemoveMethod( m_Model.get(), &ProjectViewModel::OnDocumenClosed );
        m_Model = NULL;
    }

    Disconnect( wxEVT_CONTEXT_MENU, wxContextMenuEventHandler( ProjectPanel::OnContextMenu ), NULL, this );
}

void ProjectPanel::OpenProject( Project* project, const Document* document )
{
    if ( project == m_Project )
    {
        return;
    }

    if ( m_Project )
    {
        CloseProject();
    }

    m_Project = project;
    if ( m_Project )
    {
        ProjectViewModelNode* node = NULL;

        if ( !m_Model )
        {
            // create the model
            m_Model = new ProjectViewModel( m_DocumentManager );
            node = m_Model->OpenProject( project, document );

            m_DocumentManager->e_DocumentOpened.AddMethod( m_Model.get(), &ProjectViewModel::OnDocumentOpened );
            m_DocumentManager->e_DocumenClosed.AddMethod( m_Model.get(), &ProjectViewModel::OnDocumenClosed );

            m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::Name ) );
            m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::FileSize ) );

            // the ctrl will now hold ownership via reference count
            m_DataViewCtrl->AssociateModel( m_Model.get() );
        }
        else
        {
            node = m_Model->OpenProject( project, document );
        }

        if ( node )
        {
            m_OptionsButton->Show();
            m_OptionsButtonStaticLine->Show();

            m_OptionsButton->Enable( true );

            m_ProjectNameStaticText->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ProjectPanel::OnOpenProject ), NULL, this );
            m_ProjectNameStaticText->SetLabel( m_Project->a_Path.Get().Basename() );

            //m_OpenProjectPanel->Hide();
            //m_DataViewCtrl->Show();
            Layout();

#pragma TODO ( "Remove HELIUM_IS_PROJECT_VIEW_ROOT_NODE_VISIBLE after usibility test" )
#if HELIUM_IS_PROJECT_VIEW_ROOT_NODE_VISIBLE
            m_DataViewCtrl->Expand( wxDataViewItem( (void*)node ) );
#endif
        }
    }
}

void ProjectPanel::CloseProject()
{
    HELIUM_ASSERT( m_Project );

    if ( m_Model )
    {
        m_Model->CloseProject();
    }

    m_Project = NULL;

    m_ProjectNameStaticText->SetLabel( TXT( "Open Project..." ) );
    m_ProjectNameStaticText->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ProjectPanel::OnOpenProject ), NULL, this );

    m_OptionsButtonStaticLine->Hide();
    m_OptionsButton->Hide();
    m_OptionsButton->Enable( false );

    //m_OpenProjectPanel->Show();
    //m_DataViewCtrl->Hide();
    Layout();
}

void ProjectPanel::SetActive( const Path& path, bool active )
{
    HELIUM_ASSERT( m_Project );
    HELIUM_ASSERT( m_Model );

    m_Model->SetActive( path, active );
}

void ProjectPanel::OnContextMenu( wxContextMenuEvent& event )
{
    if ( !m_Project )
    {
        event.Skip();
        return;
    }

    wxPoint point = wxGetMousePosition();
    PopupMenu( m_ContextMenu );
}

void ProjectPanel::OnActivateItem( wxDataViewEvent& event )
{
    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( event.GetItem().GetID() );
    if ( !node )
    {
        return;
    }

    const Path& path = node->GetPath();
    if ( !path.empty() )
    {
        if ( path.FullExtension() == TXT( "scene.hrb" ) )
        {
            wxGetApp().GetFrame()->OpenScene( path );
        }
    }

}

void ProjectPanel::OnOpenProject( wxMouseEvent& event )
{
    FileDialog openDlg( this, TXT( "Open Project..." ) );

    if ( openDlg.ShowModal() == wxID_OK )
    {
        wxGetApp().GetFrame()->OpenProject( (const wxChar*)openDlg.GetPath().c_str() );
    }
}

void ProjectPanel::OnAddItems( wxCommandEvent& event )
{
}

void ProjectPanel::OnDeleteItems( wxCommandEvent& event )
{
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
}

///////////////////////////////////////////////////////////////////////////////
void ProjectPanel::OnDragOver( FileDroppedArgs& args )
{
    Path path( args.m_Path );

    // it's a project file
    if ( _tcsicmp( path.FullExtension().c_str(), TXT( "project.hrb" ) ) == 0 ) 
    {
        // allow user to drop a project in
    }
    else if ( m_Project )
    {
        wxDataViewItem item;
        wxDataViewColumn* column;
        m_DataViewCtrl->HitTest( wxPoint( args.m_X, args.m_Y ), item, column );

        if ( item.IsOk() && !m_Model->IsDropPossible( item ) )
        {
            args.m_DragResult = wxDragNone;
        }    
    }
    else
    {
        args.m_DragResult = wxDragNone;
    }
}

void ProjectPanel::OnDroppedFiles( const FileDroppedArgs& args )
{
    Path path( args.m_Path );

    // it's a project file
    if ( _tcsicmp( path.FullExtension().c_str(), TXT( "project.hrb" ) ) == 0 ) 
    {
        wxGetApp().GetFrame()->OpenProject( path );
    }
    else if ( m_Project )
    {
#pragma TODO( "Handle opening a scene" )
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

            DocumentPtr document = new Document( asset->GetSourcePath() );

            tstring error;
            bool result = m_DocumentManager->OpenDocument( document, error );
            HELIUM_ASSERT( result );
        }
    }
    else
    {
#pragma TODO( "Offer to make the user a project" )
    }
}

