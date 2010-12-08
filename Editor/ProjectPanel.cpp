#include "Precompile.h"

#include "ProjectPanel.h"
#include "ArtProvider.h"

#include "Pipeline/Asset/AssetClass.h"

#include "Editor/App.h"
#include "Editor/Controls/MenuButton.h"
#include "Editor/FileDialog.h"
#include "Editor/EditorSettings.h"
#include "Editor/MainFrame.h"


using namespace Helium;
using namespace Helium::Editor;

#define HELIUM_MAX_RECENT_PROJECTS 5


ProjectPanel::ProjectPanel( wxWindow *parent, DocumentManager* documentManager )
: ProjectPanelGenerated( parent )
, m_DocumentManager( documentManager )
, m_Project( NULL )
, m_Model( NULL )
, m_OptionsMenu( NULL )
, m_DropTarget( NULL )
{
    Freeze();
    {
#pragma TODO( "Remove call(s) to SetBitmap if/when wxFormBuilder supports wxArtProvider" )
        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Options, wxART_OTHER, wxSize(16, 16) ) );
        m_OptionsButton->SetMargins( 3, 3 );
        
        m_RecentProjectsBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::ProjectFolder ) );

        m_OpenProjectButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Find ) );
        m_OpenProjectButton->SetBitmapDisabled( wxArtProvider::GetBitmap( ArtIDs::Actions::Find ).ConvertToImage().ConvertToDisabled() );

        m_CreateNewProjectButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::NewProject ) );
        m_CreateNewProjectButton->SetBitmapDisabled( wxArtProvider::GetBitmap( ArtIDs::Editor::NewProject ).ConvertToImage().ConvertToDisabled() );

        m_ProjectManagementPanel->Hide();
        m_DataViewCtrl->Hide();
        m_OpenProjectPanel->Show();
        PopulateOpenProjectListItems();
    }
    Layout();
    Thaw();

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

    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( ProjectPanel::OnSelectionChanged ), NULL, this );

    {
        wxMenuItem* addItem = m_ContextMenu.Append( wxNewId(), wxT( "Add Item(s)..." ), wxT( "Allows you to add items to the project." ) );
        Connect( addItem->GetId(), wxCommandEventHandler( ProjectPanel::OnAddItems ), NULL, this );

        wxMenuItem* deleteItem = m_ContextMenu.Append( wxNewId(), wxT( "Remove Selected Item(s)" ), wxT( "Removes the selected item(s) from the project." ) );
        Connect( deleteItem->GetId(), wxCommandEventHandler( ProjectPanel::OnDeleteItems ), NULL, this );
    }
    m_DataViewCtrl->Connect( wxEVT_CONTEXT_MENU, wxContextMenuEventHandler( ProjectPanel::OnContextMenu ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxContextMenuEventHandler( ProjectPanel::OnContextMenu ), NULL, this );
    m_DataViewCtrl->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( ProjectPanel::OnActivateItem ), NULL, this );

    m_DataViewCtrl->EnableDropTarget( wxDF_FILENAME );
    m_DropTarget = new FileDropTarget( TXT( "" ) );
    //m_DropTarget->AddDragOverListener( FileDragEnterSignature::Delegate( this, &ProjectPanel::DragEnter ) );
    m_DropTarget->AddDragOverListener( FileDragOverSignature::Delegate( this, &ProjectPanel::OnDragOver ) );
    //m_DropTarget->AddDragLeaveListener( FileDragLeaveSignature::Delegate( this, &ProjectPanel::DragLeave ) );
    m_DropTarget->AddDroppedListener( FileDroppedSignature::Delegate( this, &ProjectPanel::OnDroppedFiles ) );
    m_DataViewCtrl->GetMainWindow()->SetDropTarget( m_DropTarget );
}

ProjectPanel::~ProjectPanel()
{
    if ( m_Model )
    {
        m_DocumentManager->e_DocumentOpened.RemoveMethod( m_Model.get(), &ProjectViewModel::OnDocumentOpened );
        m_DocumentManager->e_DocumenClosed.RemoveMethod( m_Model.get(), &ProjectViewModel::OnDocumenClosed );

        m_Model->CloseProject();
    }

    m_Project = NULL;
    m_Model = NULL;

    m_OptionsButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( ProjectPanel::OnOptionsMenuOpen ), NULL, this );
    m_OptionsButton->Disconnect( wxEVT_MENU_CLOSE, wxMenuEventHandler( ProjectPanel::OnOptionsMenuClose ), NULL, this );

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
        if ( m_Model )
        {
            m_Model->CloseProject();
        }

        m_Project = NULL;
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

            m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::Name ) );
            m_DataViewCtrl->AppendColumn( m_Model->CreateColumn( ProjectModelColumns::FileSize ) );

            // the ctrl will now hold ownership via reference count
            m_DataViewCtrl->AssociateModel( m_Model.get() );
        }
        else
        {
            node = m_Model->OpenProject( project, document );
        }

        m_DocumentManager->e_DocumentOpened.AddMethod( m_Model.get(), &ProjectViewModel::OnDocumentOpened );
        m_DocumentManager->e_DocumenClosed.AddMethod( m_Model.get(), &ProjectViewModel::OnDocumenClosed );

        if ( node )
        {
            m_ProjectNameStaticText->SetLabel( m_Project->a_Path.Get().Basename() );

            m_RecentProjectsPanel->Hide();
            m_OpenProjectPanel->Hide();
            m_ProjectManagementPanel->Show();
            m_DataViewCtrl->Show();
            Layout();

#pragma TODO ( "Remove HELIUM_IS_PROJECT_VIEW_ROOT_NODE_VISIBLE after usibility test" )
#if HELIUM_IS_PROJECT_VIEW_ROOT_NODE_VISIBLE
            m_DataViewCtrl->Expand( wxDataViewItem( (void*)node ) );
#endif
        }
    
        Layout();
    }
}

void ProjectPanel::CloseProject()
{
    HELIUM_ASSERT( m_Project );

    if ( m_Model )
    {
        m_DocumentManager->e_DocumentOpened.RemoveMethod( m_Model.get(), &ProjectViewModel::OnDocumentOpened );
        m_DocumentManager->e_DocumenClosed.RemoveMethod( m_Model.get(), &ProjectViewModel::OnDocumenClosed );

        m_Model->CloseProject();
    }

    m_Project = NULL;

    m_ProjectManagementPanel->Hide();
    m_DataViewCtrl->Hide();
    m_OpenProjectPanel->Show();
    PopulateOpenProjectListItems();

    Layout();
}

void ProjectPanel::SetActive( const Path& path, bool active )
{
    if ( m_Project && m_Model )
    {
        m_Model->SetActive( path, active );
    }
}

void ProjectPanel::OnContextMenu( wxContextMenuEvent& event )
{
    if ( !m_Project )
    {
        event.Skip();
        return;
    }

    wxPoint point = wxGetMousePosition();
    PopupMenu( &m_ContextMenu );
}

void ProjectPanel::OnActivateItem( wxDataViewEvent& event )
{
    ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( event.GetItem().GetID() );
    if ( !node )
    {
        return;
    }

    const Path& path = node->GetPath();
    HELIUM_ASSERT( !path.empty() );

    if ( path.HasExtension( TXT( "scene.hrb" ) ) )
    {
        wxGetApp().GetFrame()->CloseAllScenes();
        wxGetApp().GetFrame()->OpenScene( path.GetAbsolutePath( m_Project->a_Path.Get() ) );
        return;
    }

    // we've gotten to an item we don't know how to activate yet
    HELIUM_BREAK();
}

void ProjectPanel::PopulateOpenProjectListItems()
{
    Freeze();
    {
        m_ProjectMRULookup.clear();
        const std::vector< tstring >& projectMRU = wxGetApp().GetSettingsManager()->GetSettings<GeneralSettings>()->GetMRUProjects();
        
        int mruCount = (int)projectMRU.size();
        if ( mruCount > 0 )
        {
            m_RecentProjectsSizer->Clear( true ); // true - deletes windows cleared from the sizer
                    
            int numberAdded = 0;
            for ( std::vector< tstring >::const_reverse_iterator itr = projectMRU.rbegin(), end = projectMRU.rend();
                itr != end && numberAdded < HELIUM_MAX_RECENT_PROJECTS; ++itr, ++numberAdded )
            {
                Helium::Path path( *itr );
                bool fileExists = path.Exists();

                wxButton* button = new wxButton( m_RecentProjectsPanel, wxNewId(), path.Basename().c_str(), wxDefaultPosition, wxDefaultSize, wxBU_LEFT );
                button->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::ProjectFile ) );
                button->SetBitmapDisabled( wxArtProvider::GetBitmap( ArtIDs::Editor::ProjectFile ).ConvertToImage().ConvertToDisabled() );
                //button->SetLabel( path.Basename().c_str() );
                button->Enable( fileExists );
                m_RecentProjectsSizer->Add( button, 0, wxEXPAND, 5 );

                m_ProjectMRULookup.insert( M_ProjectMRULookup::value_type( button->GetId(), path.Get() ) );

                if ( fileExists )
                {
                    button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectPanel::OnRecentProjectButtonClick ), NULL, this );
                }
            }

            m_RecentProjectsPanel->Show();
        }
        else
        {
            m_RecentProjectsPanel->Hide();
        }
    }

    Layout();
    Thaw();
}

void ProjectPanel::OnRecentProjectButtonClick( wxCommandEvent& event )
{
    if ( m_ProjectMRULookup.find( event.GetId() ) != m_ProjectMRULookup.end() )
    {
        wxGetApp().GetFrame()->OpenProject( m_ProjectMRULookup.find( event.GetId() )->second.c_str() );
        event.Skip( false );
    }
}

void ProjectPanel::OnOpenProjectButtonClick( wxCommandEvent& event )
{
    wxGetApp().GetFrame()->OpenProjectDialog();
}

void ProjectPanel::OnNewProjectButtonClick( wxCommandEvent& event )
{
    wxGetApp().GetFrame()->NewProjectDialog();
}

void ProjectPanel::OnAddItems( wxCommandEvent& event )
{
    HELIUM_BREAK();
}

void ProjectPanel::OnDeleteItems( wxCommandEvent& event )
{
    HELIUM_BREAK();
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
    if ( !m_Project )
    {
        return;
    }

    wxDataViewItem item;
    wxDataViewColumn* column;
    m_DataViewCtrl->HitTest( wxPoint( args.m_X, args.m_Y ), item, column );

    if ( item.IsOk() && !m_Model->IsDropPossible( item ) )
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
    else if ( !m_Project )
    {
        int32_t result = wxMessageBox( wxT( "You don't have a project loaded, but you're trying to add files.\nWould you like to create a new project?" ),  wxT( "No Project Loaded" ), wxYES_NO | wxICON_QUESTION );
        if ( result == wxYES )
        {
            wxGetApp().GetFrame()->NewProjectDialog();
        }
    }

    if ( !m_Project ) // they failed to create a new project above
    {
        return;
    }

    if ( !path.IsUnder( m_Project->a_Path.Get().Directory() ) )
    {
        tstringstream error;
        error << TXT( "You can only add files that live below the project.\nYou must move the file you're trying to drag somewhere below the directory:\n  " ) << m_Project->a_Path.Get().Directory().c_str();
        wxMessageBox( error.str(), TXT( "Error Adding File" ), wxOK | wxICON_ERROR );
        return;
    }

    if ( _tcsicmp( path.FullExtension().c_str(), TXT( "scene.hrb" ) ) == 0 )
    {
        m_Project->AddPath( path );
        return;
    }

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
    else
    {
        // we could not create a known asset type for this file, ask if they'd like to add it anyway
        int32_t result = wxMessageBox( wxT( "You've dragged a type of file into the project that we don't know how to handle.\n\nThat's ok, we can still add the file to the project and it will get included with the game, you just won't be able to do much else with it.\n\nWould you still like to add the file to the project?" ), wxT( "Unknown File Type" ), wxYES_NO | wxICON_QUESTION );
        if ( result == wxYES )
        {
            m_Project->AddPath( path );
        }
    }

#pragma TODO( "Set the item we just added to be selected" )
}

