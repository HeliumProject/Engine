#include "Precompile.h"
#include "VaultFrame.h"

#include "Application.h"
#include "Vault.h"
#include "VaultEvents.h"
#include "VaultPreferencesDialog.h"
#include "VaultSearch.h"
#include "CollectionManager.h"
#include "CollectionsPanel.h"
#include "FoldersPanel.h"
#include "NavigationPanel.h"
#include "PreviewPanel.h"
#include "ResultsPanel.h"
#include "SearchQuery.h"
#include "HelpPanel.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/Classes/ShaderAsset.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Editor/DocumentManager.h"
#include "Application/RCS/RCS.h"
#include "Scene/SceneManager.h"
#include "Application/UI/AutoCompleteComboBox.h"
#include "Application/UI/ArtProvider.h"
#include "Application/UI/MenuButton.h"
#include "Platform/Exception.h"
#include "Platform/Process.h"

#include <wx/clipbrd.h>

using namespace Luna;

static const tchar* s_VaultHelpText = TXT( "FFFFFFFFFFFFFFFUUUUUUUUUUUUUUUUUUUUUUUU FIXME PUT HELP TEXT HERE" );

///////////////////////////////////////////////////////////////////////////////
/// Class Vault
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( VaultFrame, VaultFrameGenerated )
EVT_MENU( VaultMenu::AdvancedSearch, VaultFrame::OnAdvancedSearch )
EVT_BUTTON( VaultMenu::AdvancedSearchGo, VaultFrame::OnAdvancedSearchGoButton )
EVT_BUTTON( VaultMenu::AdvancedSearchCancel, VaultFrame::OnAdvancedSearchCancelButton )
EVT_MENU( VaultMenu::Cut, VaultFrame::OnCut )
EVT_MENU( VaultMenu::Copy, VaultFrame::OnCopy )
EVT_MENU( VaultMenu::Paste, VaultFrame::OnPaste )
EVT_MENU( VaultMenu::Open, VaultFrame::OnOpen )
EVT_MENU( VaultMenu::Preview, VaultFrame::OnPreview )
EVT_MENU( VaultMenu::CheckOut, VaultFrame::OnCheckOut )
EVT_MENU( VaultMenu::History, VaultFrame::OnRevisionHistory )
EVT_MENU( VaultMenu::CopyPath, VaultFrame::OnCopyPath )
EVT_MENU( VaultMenu::CopyPathNative, VaultFrame::OnCopyPath )
EVT_MENU( VaultMenu::ShowInFolders, VaultFrame::OnShowInFolders )
EVT_MENU( VaultMenu::ShowInPerforce, VaultFrame::OnShowInPerforce )
EVT_MENU( VaultMenu::ShowInWindowsExplorer, VaultFrame::OnShowInWindowsExplorer )
EVT_MENU( VaultMenu::Preferences, VaultFrame::OnPreferences )
EVT_MENU( VaultMenu::NewCollectionFromSelection, VaultFrame::OnNewCollectionFromSelection )
EVT_MENU( VaultMenu::NewDepedencyCollectionFromSelection, VaultFrame::OnNewCollectionFromSelection )
EVT_MENU( VaultMenu::NewUsageCollectionFromSelection, VaultFrame::OnNewCollectionFromSelection )
igEVT_UPDATE_STATUS( wxID_ANY, VaultFrame::OnUpdateStatusBar )
END_EVENT_TABLE()


VaultFrame::VaultFrame( Vault* browser, VaultSearch* browserSearch, SearchHistory* searchHistory, wxWindow* parent )
: VaultFrameGenerated( parent, wxID_ANY, wxT( "Asset Vault" ), wxDefaultPosition, wxSize( 840, 550 ) )//, id, title, pos, size, style )
, m_Vault( browser )
, m_VaultSearch( browserSearch )
, m_SearchHistory( searchHistory )
, m_PreferencePrefix( TXT( "VaultFrame" ) )
, m_NavigationPanel( NULL )
, m_ResultsPanel( NULL )
, m_FoldersPanel( NULL )
, m_CollectionsPanel( NULL )
, m_HelpPanel( NULL )
, m_StatusBar( NULL )
, m_CurrentViewOption( ViewOptionIDs::Medium )
, m_OptionsMenu( NULL )
, m_ThumbnailViewMenu( NULL )
, m_PanelsMenu( NULL )
, m_IsSearching( false )
, m_IgnoreFolderSelect( false )
{
#pragma TODO( "Populate the CollectionsPanel from VaultPreferences" )

    NOC_ASSERT( m_Vault );

    //
    // Set the task bar icon
    //
    wxIconBundle iconBundle;
    iconBundle.AddIcon( wxArtProvider::GetIcon( wxART_FIND ) );
    iconBundle.AddIcon( wxArtProvider::GetIcon( wxART_FIND, wxART_OTHER, wxSize( 32, 32 ) ) );
    SetIcons( iconBundle );

    //
    // Navigation Tool Bar
    //
    {
        m_NavigationPanel =  new NavigationPanel( this, m_SearchHistory );

        // Add to the AUI Manager
        wxAuiPaneInfo info;
        info.Name( wxT( "NavigationBar" ) );
        info.DestroyOnClose( false );
        info.CaptionVisible( false );
        info.CloseButton( false );
        info.Floatable( false );
        info.Dock();
        info.Top();
        info.Layer( 2 );
        info.PaneBorder( false );
        info.DockFixed( true );

        m_FrameManager.AddPane( m_NavigationPanel, info );
    }

    //
    // Center Results List Ctrl
    //
    {
#pragma TODO( "pass in the right root directory to the ResultsPanel" )
        m_ResultsPanel = new ResultsPanel( TXT( "" ), this );

        wxAuiPaneInfo info;
        info.Name( wxT( "CenterPanel" ) );
        info.CenterPane();
        info.Layer( 0 );
        info.PaneBorder( false );    

        m_FrameManager.AddPane( m_ResultsPanel, info );
    }

    //
    // Preview Panel
    //
    {
        m_PreviewPanel = new PreviewPanel( this );

        wxAuiPaneInfo info;
        info.Name( TXT( "PreviewPanel" ) );
        info.DestroyOnClose( false );
        info.Caption( TXT( "Preview" ) );
        info.Dock();
        info.Right();
        info.Layer( 1 );
        info.Floatable( false );
        info.PaneBorder( false );
        info.MinSize( 200, 250 );
        info.BestSize( 250, 300 );

        m_FrameManager.AddPane( m_PreviewPanel, info );
    }

    //
    // Folder Panel
    //
    {
        m_FoldersPanel = new FoldersPanel( this );

        wxAuiPaneInfo info;
        info.Name( VaultMenu::Label( VaultMenu::FoldersPanel ).c_str() );
        info.DestroyOnClose( false );
        info.Caption( VaultMenu::Label( VaultMenu::FoldersPanel ).c_str() );
        info.Dock();
        info.Left();
        info.Layer( 2 );
        info.Floatable( false );
        info.PaneBorder( false );
        info.MinSize( 200, 250 );
        info.BestSize( 230, 300 );

        m_FrameManager.AddPane( m_FoldersPanel, info );

        tstring defaultPath = m_Vault->GetVaultPreferences()->GetDefaultFolderPath();
        if ( !defaultPath.empty() )
        {
            m_FoldersPanel->SetPath( defaultPath );
        }

        // Status bar
        m_StatusBar = new wxStatusBar( this );
        SetStatusBar( m_StatusBar );

        // Connect Folders
        wxTreeCtrl* tree = m_FoldersPanel->GetTreeCtrl();
        tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( VaultFrame::OnFolderSelected ), NULL, this );
        tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( VaultFrame::OnFolderSelected ), NULL, this );
    }

    //
    // Collections Panel
    //
    {
        m_CollectionsPanel = new CollectionsPanel( this );

        wxAuiPaneInfo info;
        info.Name( VaultMenu::Label( VaultMenu::CollectionsPanel ).c_str() );
        info.DestroyOnClose( false );
        info.Caption( VaultMenu::Label( VaultMenu::CollectionsPanel ).c_str() );
        info.Dock();
        info.Right();
        info.Position( 1 );
        info.Layer( 2 );
        info.Floatable( false );
        info.PaneBorder( false );
        info.MinSize( 200, 250 );
        info.BestSize( 250, 300 );

        m_FrameManager.AddPane( m_CollectionsPanel, info );
    }

    //
    // Help Panel
    //
    {
        m_HelpPanel = new HelpPanel( this );

        const wxFont& defaultFont = m_HelpPanel->GetFont();
        m_DefaultTextAttr.SetFont( defaultFont );

        wxFont titleFont( defaultFont );
        titleFont.SetWeight( wxFONTWEIGHT_BOLD );
        m_TitleTextAttr.SetFlags( wxTEXT_ATTR_FONT_FACE );
        m_TitleTextAttr.SetFont( titleFont );

        wxFont msgFont( defaultFont );
        msgFont.SetStyle( wxFONTSTYLE_ITALIC );
        m_ItalicTextAttr.SetFlags( wxTEXT_ATTR_FONT_FACE );
        m_ItalicTextAttr.SetFont( msgFont );

        m_HelpPanel->SetHelpText( s_VaultHelpText );

        wxAuiPaneInfo info;
        info.Name( VaultMenu::Label( VaultMenu::HelpPanel ).c_str() );
        info.DestroyOnClose( false );
        info.Caption( VaultMenu::Label( VaultMenu::HelpPanel ).c_str() );
        info.Dock();
        info.Right();
        info.Position( 3 );
        info.Layer( 2 );
        info.Floatable( false );
        info.PaneBorder( false );
        info.MinSize( 200, 250 );
        info.BestSize( 250, 300 );

        m_FrameManager.AddPane( m_HelpPanel, info );
    }

    m_FrameManager.Update();

    //
    // Viewport Menu Button
    //
    {
        m_OptionsMenu = new wxMenu();

        m_ThumbnailViewMenu = new wxMenu();
        {
            wxMenuItem* smallMenuItem = new wxMenuItem(
                m_ThumbnailViewMenu,
                VaultMenu::ViewSmall,
                VaultMenu::Label( VaultMenu::ViewSmall ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Small ),
                VaultMenu::Label( VaultMenu::ViewSmall ).c_str(),
                wxITEM_CHECK );
            m_ThumbnailViewMenu->Append( smallMenuItem );
            Connect( VaultMenu::ViewSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnOptionsMenuSelect ), NULL, this );

            wxMenuItem* mediumMenuItem = new wxMenuItem( 
                m_ThumbnailViewMenu, 
                VaultMenu::ViewMedium, 
                VaultMenu::Label( VaultMenu::ViewMedium ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Medium ),
                VaultMenu::Label( VaultMenu::ViewMedium ),
                wxITEM_CHECK );
            m_ThumbnailViewMenu->Append( mediumMenuItem );
            Connect( VaultMenu::ViewMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnOptionsMenuSelect ), NULL, this );

            wxMenuItem* largeMenuItem = new wxMenuItem(
                m_ThumbnailViewMenu,
                VaultMenu::ViewLarge,
                VaultMenu::Label( VaultMenu::ViewLarge ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Large ),
                VaultMenu::Label( VaultMenu::ViewLarge ),
                wxITEM_CHECK );
            m_ThumbnailViewMenu->Append( largeMenuItem );
            Connect( VaultMenu::ViewLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnOptionsMenuSelect ), NULL, this );
        }

#pragma TODO( "Toggle the Folders and Collections, etc... panels' states in panelsMenu based on loaded preferences." )
        m_PanelsMenu = new wxMenu();
        CreatePanelsMenu( m_PanelsMenu );
        UpdatePanelsMenu( m_PanelsMenu );

        m_OptionsMenu->Append( VaultMenu::AdvancedSearch, VaultMenu::Label( VaultMenu::AdvancedSearch ) );
        m_OptionsMenu->AppendSeparator();
        m_OptionsMenu->Append( wxID_ANY, wxT( "Thumbnail Size" ), m_ThumbnailViewMenu );
        m_OptionsMenu->AppendSeparator();
        m_OptionsMenu->Append( wxID_ANY, wxT( "Show Panels" ), m_PanelsMenu );
        m_OptionsMenu->AppendSeparator();
        m_OptionsMenu->Append( VaultMenu::Preferences, VaultMenu::Label( VaultMenu::Preferences ) );

        m_NavigationPanel->m_OptionsButton->SetContextMenu( m_OptionsMenu );
        m_NavigationPanel->m_OptionsButton->SetHoldDelay( 0.0f );

        m_NavigationPanel->m_OptionsButton->Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( VaultFrame::OnOptionsMenuOpen ), NULL, this );
        m_NavigationPanel->m_OptionsButton->Connect( wxEVT_MENU_CLOSE, wxMenuEventHandler( VaultFrame::OnOptionsMenuClose ), NULL, this );
    }

    //
    // Load Preferences
    //
    m_Vault->GetVaultPreferences()->GetWindowSettings( this, &m_FrameManager );
    m_Vault->GetVaultPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultFrame::OnPreferencesChanged ) );
    m_CurrentViewOption = m_Vault->GetVaultPreferences()->GetThumbnailMode();
    UpdateResultsView( m_Vault->GetVaultPreferences()->GetThumbnailSize() );

    //
    // Connect Events
    //
    //Connect( GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( VaultFrame::OnClose ), NULL, this );

    // Connect Listeners
    m_VaultSearch->AddRequestSearchListener( Luna::RequestSearchSignature::Delegate( this, &VaultFrame::OnRequestSearch ) );
    m_VaultSearch->AddBeginSearchingListener( Luna::BeginSearchingSignature::Delegate( this, &VaultFrame::OnBeginSearching ) );
    m_VaultSearch->AddResultsAvailableListener( Luna::ResultsAvailableSignature::Delegate( this, &VaultFrame::OnResultsAvailable ) );
    m_VaultSearch->AddSearchCompleteListener( Luna::SearchCompleteSignature::Delegate( this, &VaultFrame::OnSearchComplete ) );

    m_ResultsPanel->AddResultsChangedListener( ResultSignature::Delegate( this, &VaultFrame::OnResultsPanelUpdated ) );
}

VaultFrame::~VaultFrame()
{
    // Disconnect Events
    //Disconnect( GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( VaultFrame::OnClose ), NULL, this );

    // Disconnect ViewMenu
    Disconnect( VaultMenu::ViewSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnOptionsMenuSelect ), NULL, this );
    Disconnect( VaultMenu::ViewMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnOptionsMenuSelect ), NULL, this );
    Disconnect( VaultMenu::ViewLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnOptionsMenuSelect ), NULL, this );

    // Disconnect Folders
    wxTreeCtrl* tree = m_FoldersPanel->GetTreeCtrl();
    tree->Disconnect( tree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( VaultFrame::OnFolderSelected ), NULL, this );
    tree->Disconnect( tree->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( VaultFrame::OnFolderSelected ), NULL, this );

    // Disconnect Listeners
    m_VaultSearch->RemoveRequestSearchListener( Luna::RequestSearchSignature::Delegate( this, &VaultFrame::OnRequestSearch ) );
    m_VaultSearch->RemoveBeginSearchingListener( Luna::BeginSearchingSignature::Delegate( this, &VaultFrame::OnBeginSearching ) );
    m_VaultSearch->RemoveResultsAvailableListener( Luna::ResultsAvailableSignature::Delegate( this, &VaultFrame::OnResultsAvailable ) );
    m_VaultSearch->RemoveSearchCompleteListener( Luna::SearchCompleteSignature::Delegate( this, &VaultFrame::OnSearchComplete ) );

    m_ResultsPanel->RemoveResultsChangedListener( ResultSignature::Delegate( this, &VaultFrame::OnResultsPanelUpdated ) );
}

/////////////////////////////////////////////////////////////////////////////
void VaultFrame::SaveWindowState()
{
}

/////////////////////////////////////////////////////////////////////////////
const tstring& VaultFrame::GetPreferencePrefix() const
{
    return m_PreferencePrefix;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the string in the NavBar and starts the search query.
void VaultFrame::Search( const tstring& queryString, const AssetCollection* collection )
{
    wxBusyCursor bc;
    if ( queryString.empty() && !collection )
        return;

    tstring errors;
    if ( !SearchQuery::ParseQueryString( queryString, errors ) )
    {
        wxMessageBox( errors.c_str(), TXT( "Search Errors" ), wxCENTER | wxICON_WARNING | wxOK, this );
        return;
    }

    m_SearchHistory->RunNewQuery( queryString, collection );
}

void VaultFrame::GetSelectedPaths( std::set< Nocturnal::Path >& paths )
{
    m_ResultsPanel->GetSelectedPaths( paths );
}

wxMenu* VaultFrame::GetNewAssetMenu( bool forceEnableAll )
{
    bool enableItems = forceEnableAll || InFolder();

    if ( !m_MenuItemToAssetType.empty() )
    {
        std::map< i32, i32 >::const_iterator itr = m_MenuItemToAssetType.begin();
        std::map< i32, i32 >::const_iterator end = m_MenuItemToAssetType.end();
        for ( ; itr != end; ++itr )
        {
            i32 menuItemID = itr->first;
            Disconnect( menuItemID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnNew ), NULL, this );
        }
    }

    m_MenuItemToAssetType.clear();

    wxMenu* newMenu = new wxMenu();

    // Menu for creating new assets
    typedef std::map< const Reflect::Class*, wxMenu* > M_SubMenus;
    M_SubMenus subMenus;

    // Add a submenu for shaders since there's so many different kinds.
    // Additional submenus could be added here as well.
    const Reflect::Class* shaderBase = Reflect::GetClass< Asset::ShaderAsset >();
    wxMenu* shaderSubMenu = subMenus.insert( M_SubMenus::value_type( shaderBase, new wxMenu() ) ).first->second;

    wxMenuItem* shaderSubMenuItem = new wxMenuItem( newMenu, wxID_ANY, shaderBase->m_UIName.c_str(), shaderBase->m_UIName.c_str(), wxITEM_NORMAL, shaderSubMenu );
    newMenu->Append( shaderSubMenuItem );
    shaderSubMenuItem->Enable( enableItems );

    // Populate the New asset menu
    std::vector< i32 >::const_iterator assetItr = Asset::g_AssetClassTypes.begin();
    std::vector< i32 >::const_iterator assetEnd = Asset::g_AssetClassTypes.end();
    for ( ; assetItr != assetEnd; ++assetItr )
    {
        const i32 typeID = (*assetItr);
        const Reflect::Class* typeInfo = Reflect::Registry::GetInstance()->GetClass( typeID );

        wxMenuItem* menuItem = NULL;

        M_SubMenus::const_iterator foundSubMenu = subMenus.find( Reflect::Registry::GetInstance()->GetClass( typeInfo->m_Base ) );
        if ( foundSubMenu != subMenus.end() )
        {
            menuItem = foundSubMenu->second->Append( wxID_ANY, typeInfo->m_UIName.c_str() );
        }
        else
        {
            menuItem = newMenu->Append( wxID_ANY, typeInfo->m_UIName.c_str() );
        }

        menuItem->Enable( enableItems );

        // Map the menu item ID to the asset class ID so that when we get a menu item
        // callback, we know which type of asset to create.
        m_MenuItemToAssetType.insert( std::map< i32, i32 >::value_type( menuItem->GetId(), typeID ) );

        // Connect a callback for when the menu item is selected.  No need to disconnect
        // this handler since the lifetime of this class is tied to the menu.
        Connect( menuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultFrame::OnNew ), NULL, this );
    }

    return m_MenuItemToAssetType.empty() ? NULL : newMenu;
}

///////////////////////////////////////////////////////////////////////////////
bool VaultFrame::InFolder()
{
    const SearchQuery* curQuery = m_SearchHistory->GetCurrentQuery();
    if ( curQuery )
    {
        return ( curQuery->GetSearchType() == SearchTypes::Folder );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Called when a menu is about to be opened.  Enables and disables items in the
// menu as appropriate.
// 
void VaultFrame::OnOptionsMenuOpen( wxMenuEvent& event )
{
    event.Skip();
    if ( event.GetMenu() == m_OptionsMenu )
    {
        UpdatePanelsMenu( m_PanelsMenu );
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnOptionsMenuClose( wxMenuEvent& event )
{
    m_NavigationPanel->m_NavBarComboBox->SetFocus();
    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnOptionsMenuSelect( wxCommandEvent& event )
{
    event.Skip();

    ViewOptionID id = (ViewOptionID) event.GetId();
    m_CurrentViewOption = id;
    UpdateResultsView();
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnFolderSelected( wxTreeEvent& event )
{
    if ( m_IgnoreFolderSelect )
        return;

    tstring queryString;
    m_FoldersPanel->GetPath( queryString );
    if ( !queryString.empty() )
    {
        Nocturnal::Path::Normalize( queryString );
        Nocturnal::Path::GuaranteeSlash( queryString );
        Search( queryString );
        event.Skip();
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnAdvancedSearch( wxCommandEvent& event )
{
    m_ResultsPanel->SetViewMode( ViewModes::AdvancedSearch );
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnAdvancedSearchGoButton( wxCommandEvent& event )
{
    // TODO: submit the form
    m_ResultsPanel->SetViewMode( ViewModes::Thumbnail );
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnAdvancedSearchCancelButton( wxCommandEvent& event )
{
    m_ResultsPanel->SetViewMode( ViewModes::Thumbnail );
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnOpen( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );
    for ( std::set< Nocturnal::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
    {
        if ( (*itr).Exists() && (*itr).IsFile() )
        {
#pragma TODO( "Open the file for editing" )
            NOC_BREAK();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnPreview( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );
    if ( !paths.empty() )
    {
        const Nocturnal::Path& path = *paths.begin();
        Asset::AssetClassPtr asset = Asset::AssetClass::LoadAssetClass( path );

        if ( !asset.ReferencesObject() )
        {
            tostringstream msg;
            msg << TXT( "Failed to load asset '" ) << path.c_str() << TXT( "'. Unable to show preview." );
            wxMessageBox( msg.str(), TXT( "Error" ), wxOK | wxCENTER | wxICON_ERROR, this );
            return;
        }

        // Load the preview
        m_PreviewPanel->Preview( asset );

        // Show preview window if it's not already showing
        wxAuiPaneInfo& settings = m_FrameManager.GetPane( m_PreviewPanel );
        if ( !settings.IsShown() )
        {
            settings.Show();
            m_FrameManager.Update();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnSync( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );

    // Sync the files
    for ( std::set< Nocturnal::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& path = *itr;

        try
        {
            tstring spec;
            if ( path.IsDirectory() )
            {
                spec = path.Get() + TXT( "..." );
            }
            else
            {
                spec = path.Get();
            }

            RCS::File rcsFile( spec );
            rcsFile.Sync();
        }
        catch ( const Nocturnal::Exception& e )
        {
            wxMessageBox( e.What(), TXT( "Sync Failed!" ), wxCENTER | wxICON_ERROR | wxOK, this );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnCheckOut( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );

    bool checkoutDirectories = false;

    for ( std::set< Nocturnal::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& path = *itr;

        if ( path.IsDirectory() )
        {

            const tchar* checkoutFiles = TXT( "Let the OS handle this as an exception" );
            const tchar* checkoutFilesAndFolders = TXT( "Skip this break point once" );

            wxArrayString choices;
            choices.Add(checkoutFiles);
            choices.Add(checkoutFilesAndFolders);
            wxString choice = ::wxGetSingleChoice( TXT( "Your selection includes folders.  Checking out folders may cause many files to be checked out.  How would you like to proceed?" ),
                                                   TXT( "Folders in selection" ),
                                                   choices );

            if ( choice.empty() )
            {
                // they clicked cancel
                return;
            }
            else if ( choice == checkoutFiles )
            {
                // do nothing, already set to not check out directories above
            }
            else if ( choice == checkoutFilesAndFolders )
            {
                checkoutDirectories = true;
            }
            else
            {
                NOC_BREAK();
            }
        }
    }

    for ( std::set< Nocturnal::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& path = *itr;
        try
        {
            tstring spec;
            if ( path.IsDirectory() )
            {
                if ( checkoutDirectories )
                {
                    spec = path.Get() + TXT( "..." );
                }
                else
                {
                    continue;
                }
            }
            else
            {
                spec = path.Get();
            }

            RCS::File rcsFile( spec );
            rcsFile.Edit();
        }
        catch ( const Nocturnal::Exception& e )
        {
            wxMessageBox( e.What(), TXT( "Check Out Failed!" ), wxCENTER | wxICON_ERROR | wxOK, this );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnRevisionHistory( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );

    if ( paths.size() == 1 )
    {
        const Nocturnal::Path& path = *paths.begin();
        tstring command = TXT( "p4win.exe -H \"" ) + path.Get() + TXT( "\"" );

        if ( Platform::Execute( command ) == -1 )
        {
            tstring error = Platform::GetErrorString();
            error += TXT( "\nMake sure that you have p4win properly installed." );
            wxMessageBox( error.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnCopyPath( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    if ( m_ResultsPanel->GetSelectedPaths( paths ) )
    {
        wxString text;
        wxTextDataObject* dataObject = new wxTextDataObject();
        for ( std::set< Nocturnal::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
        {
            if ( !text.empty() )
            {
                text += TXT( "\n" );
            }

            if ( event.GetId() == VaultMenu::CopyPath )
            {
                text += (*itr).c_str();
            }
            else if ( event.GetId() == VaultMenu::CopyPathNative )
            {
                text += (*itr).Native().c_str();
            }
            else
            {
                NOC_BREAK();
            }
        }

        if ( wxTheClipboard->Open() )
        {
            wxTheClipboard->SetData( new wxTextDataObject( text ) );
            wxTheClipboard->Close();
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnShowInFolders( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );
    if ( paths.size() == 1 )
    {
        wxBusyCursor bc;

        const Nocturnal::Path& path = *( paths.begin() );
        if ( path.Exists() )
        {
            Search( path.Get() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnShowInPerforce( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );
    if ( paths.size() == 1 )
    {
        const Nocturnal::Path& path = *( paths.begin() );
        tstring command = tstring( TXT( "p4win.exe -s \"" ) ) + path.c_str() + TXT( "\"" );

        if ( Platform::Execute( command ) == -1 )
        {
            tstring error = Platform::GetErrorString();
            error += TXT( "\nMake sure that you have p4win properly installed." );
            wxMessageBox( error.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnShowInWindowsExplorer( wxCommandEvent& event )
{
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );
    if ( paths.size() == 1 )
    {
        tstring command = TXT( "explorer.exe " );
        const Nocturnal::Path& path = *( paths.begin() );
        if ( path.IsFile() )
        {
            command += TXT( "/select," );
        }
        command += TXT( "\"" ) + path.Native() + TXT( "\"" );

        Platform::Execute( command );
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnNewCollectionFromSelection( wxCommandEvent& event )
{
    wxBusyCursor busyCursor;
    std::set< Nocturnal::Path > paths;
    GetSelectedPaths( paths );
    AssetCollectionPtr collection;

    if ( paths.size() )
    {
        if ( event.GetId() == VaultMenu::NewCollectionFromSelection )
        {
            collection = new AssetCollection( TXT( "New Collection" ), AssetCollectionFlags::CanRename | AssetCollectionFlags::CanHandleDragAndDrop );
            collection->AddAssets( paths );
        }
        else
        {
            NOC_BREAK();
        }
    }

    if ( collection )
    {
        CollectionManager* collectionManager = m_Vault->GetVaultPreferences()->GetCollectionManager();
        tstring name;
        collectionManager->GetUniqueName( name, collection->GetName().c_str() );
        collection->SetName( name );
        collectionManager->AddCollection( collection );
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnRefresh( wxCommandEvent& args )
{
    m_SearchHistory->RunCurrentQuery();
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnNew( wxCommandEvent& args )
{
#pragma TODO( "Reimplement without the wizard" )
    //std::map< i32, i32 >::const_iterator found = m_MenuItemToAssetType.find( args.GetId() );
    //if ( found != m_MenuItemToAssetType.end() )
    //{
    //    const i32 assetTypeID = found->second;

    //    ::AssetManager::CreateAssetWizard wizard( this, assetTypeID );

    //    tstring defaultDir;
    //    const SearchQuery* curQuery = m_SearchHistory->GetCurrentQuery();
    //    if ( curQuery 
    //        && ( curQuery->GetSearchType() == SearchTypes::Folder ) )
    //    {
    //        wizard.SetDefaultDirectory( curQuery->GetQueryString() );

    //        if ( wizard.Run() )
    //        {
    //            wxBusyCursor bc;

    //            Asset::AssetClassPtr assetClass = wizard.GetAssetClass();
    //            if ( !assetClass.ReferencesObject() )
    //            {
    //                Log::Error( "CreateAssetWizard returned a NULL asset when attempting to create new asset at location %s.", wizard.GetNewFileLocation().c_str() );
    //            }
    //            else
    //            {
    //                // re-run the search query to show the new asset
    //                m_SearchHistory->RunNewQuery( wizard.GetNewFileLocation(), NULL );
    //            }
    //        }
    //    }
    //}
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnNewFolder( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnCut( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnCopy( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnPaste( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnPreferences( wxCommandEvent& event )
{
    VaultPreferencesDialog dlg( this );
    dlg.ShowModal();
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnUpdateStatusBar( Luna::UpdateStatusEvent& event )
{
    m_StatusBar->SetStatusText( event.GetText() );
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::OnClose( wxCloseEvent& event )
{
    event.Skip();

    m_Vault->GetVaultPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultFrame::OnPreferencesChanged ) );

    m_Vault->GetVaultPreferences()->SetThumbnailMode( m_CurrentViewOption );
    m_Vault->GetVaultPreferences()->SetWindowSettings( this, &m_FrameManager );

    tstring path;
    m_FoldersPanel->GetPath( path );
    m_Vault->GetVaultPreferences()->SetDefaultFolderPath( path );
    m_Vault->OnCloseVault();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search is requested
// 
void VaultFrame::OnRequestSearch( const Luna::RequestSearchArgs& args )
{
    UpdateNavBar( args.m_SearchQuery );
    m_ResultsPanel->ClearResults();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search is starting.
// 
void VaultFrame::OnBeginSearching( const Luna::BeginSearchingArgs& args )
{
    m_IsSearching = true;
    m_ResultsPanel->ClearResults();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search results are ready to be displayed.
// 
void VaultFrame::OnResultsAvailable( const Luna::ResultsAvailableArgs& args )
{
    m_ResultsPanel->SetResults( args.m_SearchResults );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search is done - update status bar and other UI elements.
// 
void VaultFrame::OnSearchComplete( const Luna::SearchCompleteArgs& args )
{
    m_IsSearching = false;
#pragma TODO ("Rachel: figure out how to add EndSearch listener and hook up ResultsPanel::SelectPath to select this path.")
    std::set< Nocturnal::Path > paths;
    m_ResultsPanel->GetSelectedPaths( paths );
    u32 numFolders = m_ResultsPanel->GetNumFolders();
    u32 numFiles = m_ResultsPanel->GetNumFiles();
    UpdateStatusBar( numFolders, numFiles, paths.size(), TXT( "" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Update the UI when the preferences change
// 
void VaultFrame::OnPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
    m_CurrentViewOption = m_Vault->GetVaultPreferences()->GetThumbnailMode();
    UpdateResultsView( m_Vault->GetVaultPreferences()->GetThumbnailSize() );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for changes to the ResultsPanel.  Updates the status bar accordingly.
// 
void VaultFrame::OnResultsPanelUpdated( const ResultChangeArgs& args )
{
    u32 numFolders = m_ResultsPanel->GetNumFolders();
    u32 numFiles = m_ResultsPanel->GetNumFiles();
    UpdateStatusBar( numFolders, numFiles, args.m_NumSelected, args.m_HighlightPath );
}

///////////////////////////////////////////////////////////////////////////////
// Disconnect folder events and set the path
// 
void VaultFrame::SetFolderPath( const tstring& folderPath )
{
    m_IgnoreFolderSelect = true;
    m_FoldersPanel->SetPath( folderPath );
    m_IgnoreFolderSelect = false;
}


///////////////////////////////////////////////////////////////////////////////
void VaultFrame::UpdateNavBar( const SearchQueryPtr& searchQuery )
{
    bool isFolder = false;
    if ( searchQuery->GetSearchType() == SearchTypes::File )
    {
        Nocturnal::Path path( searchQuery->GetQueryString() );
        SetFolderPath( path.Directory() );
    }
    else if ( searchQuery->GetSearchType() == SearchTypes::Folder )
    {
        isFolder = true;
        SetFolderPath( searchQuery->GetQueryString() );
    }
    else
    {
        m_FoldersPanel->Unselect();
    }

    const tstring& queryString = searchQuery->GetQueryString();
    m_NavigationPanel->SetNavBarValue( queryString, isFolder );
}

///////////////////////////////////////////////////////////////////////////////
void VaultFrame::UpdateResultsView( u16 customSize )
{
    m_ThumbnailViewMenu->Check( VaultMenu::ViewSmall, false );
    m_ThumbnailViewMenu->Check( VaultMenu::ViewMedium, false );
    m_ThumbnailViewMenu->Check( VaultMenu::ViewLarge, false );

    switch( m_CurrentViewOption )
    {
    case ViewOptionIDs::Small:
        m_ResultsPanel->SetViewMode( ViewModes::Thumbnail );
        m_ResultsPanel->SetThumbnailSize( ThumbnailSizes::Small );
        m_ThumbnailViewMenu->Check( m_CurrentViewOption, true );
        break;

    case ViewOptionIDs::Medium:
        m_ResultsPanel->SetViewMode( ViewModes::Thumbnail );
        m_ResultsPanel->SetThumbnailSize( ThumbnailSizes::Medium );
        m_ThumbnailViewMenu->Check( m_CurrentViewOption, true );
        break;

    case ViewOptionIDs::Large:
        m_ResultsPanel->SetViewMode( ViewModes::Thumbnail );
        m_ResultsPanel->SetThumbnailSize( ThumbnailSizes::Large );
        m_ThumbnailViewMenu->Check( m_CurrentViewOption, true );
        break;

    default:
        m_ResultsPanel->SetThumbnailSize( customSize );
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Sets the status bar message (main area, left side of status bar) based on
// current search progress.
// 
void VaultFrame::UpdateStatusBar( size_t numFolders, size_t numFiles, size_t numSelected, const tstring& hover )
{
    tstringstream status;

    if ( !hover.empty() )
    {
        status << hover;
    }
    else
    {
        if ( m_IsSearching )
        {
            status << "Searching... ";
        }

        if ( !m_IsSearching && !numFolders && !numFiles )
        {
            status << "No results found";
        }
        else
        {
            if ( numFolders )
            {
                status << numFolders << " folder";
                if ( numFolders > 1 )
                {
                    status << "s";
                }
            }

            if ( numFiles )
            {
                if ( numFolders )
                {
                    status << ", ";
                }
                status << numFiles << " file";
                if ( numFiles > 1 )
                {
                    status << "s";
                }
            }

            if ( numSelected )
            {
                if ( numFolders || numFiles )
                {
                    status << ", ";
                }
                status << numSelected << " selected";
            }
        }
    }

    Luna::UpdateStatusEvent evt;
    evt.SetEventObject( this );
    evt.SetText( status.str() );
    wxPostEvent( this, evt );
}
