#include "Precompile.h"
#include "BrowserFrame.h"

#include "Application.h"
#include "Browser.h"
#include "BrowserEvents.h"
#include "BrowserPreferencesDialog.h"
#include "BrowserSearch.h"
#include "CollectionManager.h"
#include "CollectionsPanel.h"
#include "FoldersPanel.h"
#include "NavigationPanel.h"
#include "PreviewPanel.h"
#include "ResultsPanel.h"
#include "SearchQuery.h"

#include "Pipeline/Asset/AssetFile.h"
#include "Pipeline/Asset/AssetFolder.h"
#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/Classes/ShaderAsset.h"
#include "Pipeline/Component/ComponentHandle.h"
#include "Editor/DocumentManager.h"
#include "Application/RCS/RCS.h"
#include "Scene/SceneManager.h"
#include "Application/UI/AutoCompleteComboBox.h"
#include "Application/UI/Button.h"
#include "Application/UI/ImageManager.h"
#include "Application/UI/MenuButton.h"
#include "Platform/Exception.h"
#include "Platform/Process.h"

#include <wx/clipbrd.h>

using namespace Luna;

static const u32 s_ToggleButtonStyle = 
( Nocturnal::ButtonStyles::BU_BITMAP
 | Nocturnal::ButtonStyles::BU_TOGGLE
 | Nocturnal::ButtonStyles::BU_CENTER );

static const tchar* s_BrowserHelpText = TXT( "FFFFFFFFFFFFFFFUUUUUUUUUUUUUUUUUUUUUUUU FIXME PUT HELP TEXT HERE" );

///////////////////////////////////////////////////////////////////////////////
/// Class HelpPanel
///////////////////////////////////////////////////////////////////////////////
class Luna::HelpPanel : public HelpPanelGenerated 
{	
public:
    HelpPanel( BrowserFrame* browserFrame )
        : HelpPanelGenerated( browserFrame )
        , m_BrowserFrame( browserFrame )
    {
    }
    virtual ~HelpPanel()
    {
    }

private:
    BrowserFrame* m_BrowserFrame;
};


///////////////////////////////////////////////////////////////////////////////
/// Class Browser
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( BrowserFrame, BrowserFrameGenerated )
EVT_MENU( BrowserMenu::AdvancedSearch, BrowserFrame::OnAdvancedSearch )
EVT_BUTTON( BrowserMenu::AdvancedSearchGo, BrowserFrame::OnAdvancedSearchGoButton )
EVT_BUTTON( BrowserMenu::AdvancedSearchCancel, BrowserFrame::OnAdvancedSearchCancelButton )
EVT_MENU( BrowserMenu::Cut, BrowserFrame::OnCut )
EVT_MENU( BrowserMenu::Copy, BrowserFrame::OnCopy )
EVT_MENU( BrowserMenu::Paste, BrowserFrame::OnPaste )
EVT_MENU( BrowserMenu::Open, BrowserFrame::OnOpen )
EVT_MENU( BrowserMenu::Preview, BrowserFrame::OnPreview )
EVT_MENU( BrowserMenu::CheckOut, BrowserFrame::OnCheckOut )
EVT_MENU( BrowserMenu::History, BrowserFrame::OnRevisionHistory )
EVT_MENU( BrowserMenu::CopyPathClean, BrowserFrame::OnCopyPath )
EVT_MENU( BrowserMenu::CopyPathWindows, BrowserFrame::OnCopyPath )
EVT_MENU( BrowserMenu::ShowInFolders, BrowserFrame::OnShowInFolders )
EVT_MENU( BrowserMenu::ShowInPerforce, BrowserFrame::OnShowInPerforce )
EVT_MENU( BrowserMenu::ShowInWindowsExplorer, BrowserFrame::OnShowInWindowsExplorer )
EVT_MENU( BrowserMenu::Preferences, BrowserFrame::OnPreferences )
EVT_MENU( BrowserMenu::NewCollectionFromSelection, BrowserFrame::OnNewCollectionFromSelection )
EVT_MENU( BrowserMenu::NewDepedencyCollectionFromSelection, BrowserFrame::OnNewCollectionFromSelection )
EVT_MENU( BrowserMenu::NewUsageCollectionFromSelection, BrowserFrame::OnNewCollectionFromSelection )
igEVT_UPDATE_STATUS( wxID_ANY, BrowserFrame::OnUpdateStatusBar )
END_EVENT_TABLE()


BrowserFrame::BrowserFrame( Browser* browser, BrowserSearch* browserSearch, SearchHistory* searchHistory, wxWindow* parent )
: BrowserFrameGenerated( parent, wxID_ANY, wxT( "Asset Vault" ), wxDefaultPosition, wxSize( 840, 550 ) )//, id, title, pos, size, style )
, m_Browser( browser )
, m_BrowserSearch( browserSearch )
, m_SearchHistory( searchHistory )
, m_PreferencePrefix( TXT( "BrowserFrame" ) )
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
#pragma TODO( "Populate the CollectionsPanel from BrowserPreferences" )

    NOC_ASSERT( m_Browser );

    //
    // Set the task bar icon
    //
    wxIconBundle iconBundle;

    wxIcon tempIcon;
    tempIcon.CopyFromBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "vault.png" ) ) );
    iconBundle.AddIcon( tempIcon );

    tempIcon.CopyFromBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "vault.png" ), Nocturnal::IconSizes::Size32 ) );
    iconBundle.AddIcon( tempIcon );

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
        info.Name( BrowserMenu::Label( BrowserMenu::FoldersPanel ).c_str() );
        info.DestroyOnClose( false );
        info.Caption( BrowserMenu::Label( BrowserMenu::FoldersPanel ).c_str() );
        info.Dock();
        info.Left();
        info.Layer( 2 );
        info.Floatable( false );
        info.PaneBorder( false );
        info.MinSize( 200, 250 );
        info.BestSize( 230, 300 );

        m_FrameManager.AddPane( m_FoldersPanel, info );

        tstring defaultPath = m_Browser->GetBrowserPreferences()->GetDefaultFolderPath();
        if ( !defaultPath.empty() )
        {
            m_FoldersPanel->SetPath( defaultPath );
        }

        // Status bar
        m_StatusBar = new BrowserStatusBar( this );
        SetStatusBar( m_StatusBar );

        // Connect Folders
        wxTreeCtrl* tree = m_FoldersPanel->GetTreeCtrl();
        tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( BrowserFrame::OnFolderSelected ), NULL, this );
        tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( BrowserFrame::OnFolderSelected ), NULL, this );
    }

    //
    // Collections Panel
    //
    {
        m_CollectionsPanel = new CollectionsPanel( this );

        wxAuiPaneInfo info;
        info.Name( BrowserMenu::Label( BrowserMenu::CollectionsPanel ).c_str() );
        info.DestroyOnClose( false );
        info.Caption( BrowserMenu::Label( BrowserMenu::CollectionsPanel ).c_str() );
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

        m_HelpPanel->m_HelpTextCtrl->SetDefaultStyle( m_DefaultTextAttr );
        ( *m_HelpPanel->m_HelpTextCtrl ) << s_BrowserHelpText;

        wxAuiPaneInfo info;
        info.Name( BrowserMenu::Label( BrowserMenu::HelpPanel ).c_str() );
        info.DestroyOnClose( false );
        info.Caption( BrowserMenu::Label( BrowserMenu::HelpPanel ).c_str() );
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
    // View Menu Button
    //
    {
        m_OptionsMenu = new wxMenu();

        m_ThumbnailViewMenu = new wxMenu();
        {
            wxMenuItem* smallMenuItem = new wxMenuItem(
                m_ThumbnailViewMenu,
                BrowserMenu::ViewSmall,
                BrowserMenu::Label( BrowserMenu::ViewSmall ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Small ),
                BrowserMenu::Label( BrowserMenu::ViewSmall ).c_str(),
                wxITEM_CHECK );
            m_ThumbnailViewMenu->Append( smallMenuItem );
            Connect( BrowserMenu::ViewSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnOptionsMenuSelect ), NULL, this );

            wxMenuItem* mediumMenuItem = new wxMenuItem( 
                m_ThumbnailViewMenu, 
                BrowserMenu::ViewMedium, 
                BrowserMenu::Label( BrowserMenu::ViewMedium ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Medium ),
                BrowserMenu::Label( BrowserMenu::ViewMedium ),
                wxITEM_CHECK );
            m_ThumbnailViewMenu->Append( mediumMenuItem );
            Connect( BrowserMenu::ViewMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnOptionsMenuSelect ), NULL, this );

            wxMenuItem* largeMenuItem = new wxMenuItem(
                m_ThumbnailViewMenu,
                BrowserMenu::ViewLarge,
                BrowserMenu::Label( BrowserMenu::ViewLarge ) + TXT( " " ) + ThumbnailSizes::Label( ThumbnailSizes::Large ),
                BrowserMenu::Label( BrowserMenu::ViewLarge ),
                wxITEM_CHECK );
            m_ThumbnailViewMenu->Append( largeMenuItem );
            Connect( BrowserMenu::ViewLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnOptionsMenuSelect ), NULL, this );
        }

#pragma TODO( "Toggle the Folders and Collections, etc... panels' states in panelsMenu based on loaded preferences." )
        m_PanelsMenu = new wxMenu();
        CreatePanelsMenu( m_PanelsMenu );
        UpdatePanelsMenu( m_PanelsMenu );

        m_OptionsMenu->Append( BrowserMenu::AdvancedSearch, BrowserMenu::Label( BrowserMenu::AdvancedSearch ) );
        m_OptionsMenu->AppendSeparator();
        m_OptionsMenu->Append( wxID_ANY, wxT( "Thumbnail Size" ), m_ThumbnailViewMenu );
        m_OptionsMenu->AppendSeparator();
        m_OptionsMenu->Append( wxID_ANY, wxT( "Show Panels" ), m_PanelsMenu );
        m_OptionsMenu->AppendSeparator();
        m_OptionsMenu->Append( BrowserMenu::Preferences, BrowserMenu::Label( BrowserMenu::Preferences ) );

        m_NavigationPanel->m_OptionsButton->SetContextMenu( m_OptionsMenu );
        m_NavigationPanel->m_OptionsButton->SetHoldDelay( 0.0f );

        m_NavigationPanel->m_OptionsButton->Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( BrowserFrame::OnOptionsMenuOpen ), NULL, this );
        m_NavigationPanel->m_OptionsButton->Connect( wxEVT_MENU_CLOSE, wxMenuEventHandler( BrowserFrame::OnOptionsMenuClose ), NULL, this );
    }

    //
    // Load Preferences
    //
    m_Browser->GetBrowserPreferences()->GetWindowSettings( this, &m_FrameManager );
    m_Browser->GetBrowserPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &BrowserFrame::OnPreferencesChanged ) );
    m_CurrentViewOption = m_Browser->GetBrowserPreferences()->GetThumbnailMode();
    UpdateResultsView( m_Browser->GetBrowserPreferences()->GetThumbnailSize() );

    //
    // Connect Events
    //
    //Connect( GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BrowserFrame::OnClose ), NULL, this );

    // Connect Listeners
    m_BrowserSearch->AddRequestSearchListener( Luna::RequestSearchSignature::Delegate( this, &BrowserFrame::OnRequestSearch ) );
    m_BrowserSearch->AddBeginSearchingListener( Luna::BeginSearchingSignature::Delegate( this, &BrowserFrame::OnBeginSearching ) );
    m_BrowserSearch->AddResultsAvailableListener( Luna::ResultsAvailableSignature::Delegate( this, &BrowserFrame::OnResultsAvailable ) );
    m_BrowserSearch->AddSearchCompleteListener( Luna::SearchCompleteSignature::Delegate( this, &BrowserFrame::OnSearchComplete ) );

    m_ResultsPanel->AddResultsChangedListener( ResultSignature::Delegate( this, &BrowserFrame::OnResultsPanelUpdated ) );
}

BrowserFrame::~BrowserFrame()
{
    // Disconnect Events
    //Disconnect( GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( BrowserFrame::OnClose ), NULL, this );

    // Disconnect ViewMenu
    Disconnect( BrowserMenu::ViewSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnOptionsMenuSelect ), NULL, this );
    Disconnect( BrowserMenu::ViewMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnOptionsMenuSelect ), NULL, this );
    Disconnect( BrowserMenu::ViewLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnOptionsMenuSelect ), NULL, this );

    // Disconnect Folders
    wxTreeCtrl* tree = m_FoldersPanel->GetTreeCtrl();
    tree->Disconnect( tree->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( BrowserFrame::OnFolderSelected ), NULL, this );
    tree->Disconnect( tree->GetId(), wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( BrowserFrame::OnFolderSelected ), NULL, this );

    // Disconnect Listeners
    m_BrowserSearch->RemoveRequestSearchListener( Luna::RequestSearchSignature::Delegate( this, &BrowserFrame::OnRequestSearch ) );
    m_BrowserSearch->RemoveBeginSearchingListener( Luna::BeginSearchingSignature::Delegate( this, &BrowserFrame::OnBeginSearching ) );
    m_BrowserSearch->RemoveResultsAvailableListener( Luna::ResultsAvailableSignature::Delegate( this, &BrowserFrame::OnResultsAvailable ) );
    m_BrowserSearch->RemoveSearchCompleteListener( Luna::SearchCompleteSignature::Delegate( this, &BrowserFrame::OnSearchComplete ) );

    m_ResultsPanel->RemoveResultsChangedListener( ResultSignature::Delegate( this, &BrowserFrame::OnResultsPanelUpdated ) );
}

/////////////////////////////////////////////////////////////////////////////
void BrowserFrame::SaveWindowState()
{
}

/////////////////////////////////////////////////////////////////////////////
const tstring& BrowserFrame::GetPreferencePrefix() const
{
    return m_PreferencePrefix;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the string in the NavBar and starts the search query.
void BrowserFrame::Search( const tstring& queryString, const AssetCollection* collection )
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

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::GetSelectedFilesAndFolders( Asset::V_AssetFiles& files, Asset::V_AssetFolders& folders )
{
    m_ResultsPanel->GetSelectedFilesAndFolders( files, folders );
}

///////////////////////////////////////////////////////////////////////////////
bool BrowserFrame::IsPreviewable( Asset::AssetFile* file )
{
    NOC_ASSERT( file );

    Asset::AssetClassPtr asset = Asset::AssetFile::GetAssetClass( file );
    if ( asset.ReferencesObject() )
    {
         return asset->GetPath().Exists();
    }


    return false;
}

///////////////////////////////////////////////////////////////////////////////
wxMenu* BrowserFrame::GetNewAssetMenu( bool forceEnableAll )
{
    bool enableItems = forceEnableAll || InFolder();

    if ( !m_MenuItemToAssetType.empty() )
    {
        std::map< i32, i32 >::const_iterator itr = m_MenuItemToAssetType.begin();
        std::map< i32, i32 >::const_iterator end = m_MenuItemToAssetType.end();
        for ( ; itr != end; ++itr )
        {
            i32 menuItemID = itr->first;
            Disconnect( menuItemID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnNew ), NULL, this );
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
        Connect( menuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserFrame::OnNew ), NULL, this );
    }

    return m_MenuItemToAssetType.empty() ? NULL : newMenu;
}

///////////////////////////////////////////////////////////////////////////////
bool BrowserFrame::InFolder()
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
void BrowserFrame::OnOptionsMenuOpen( wxMenuEvent& event )
{
    event.Skip();
    if ( event.GetMenu() == m_OptionsMenu )
    {
        UpdatePanelsMenu( m_PanelsMenu );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnOptionsMenuClose( wxMenuEvent& event )
{
    m_NavigationPanel->m_NavBarComboBox->SetFocus();
    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnOptionsMenuSelect( wxCommandEvent& event )
{
    event.Skip();

    ViewOptionID id = (ViewOptionID) event.GetId();
    m_CurrentViewOption = id;
    UpdateResultsView();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnFolderSelected( wxTreeEvent& event )
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
void BrowserFrame::OnAdvancedSearch( wxCommandEvent& event )
{
    m_ResultsPanel->SetViewMode( ViewModes::AdvancedSearch );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnAdvancedSearchGoButton( wxCommandEvent& event )
{
    // TODO: submit the form
    m_ResultsPanel->SetViewMode( ViewModes::Thumbnail );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnAdvancedSearchCancelButton( wxCommandEvent& event )
{
    m_ResultsPanel->SetViewMode( ViewModes::Thumbnail );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnOpen( wxCommandEvent& event )
{
    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders; // ignored
    m_ResultsPanel->GetSelectedFilesAndFolders( files, folders );
    for ( Asset::V_AssetFiles::const_iterator fileItr = files.begin(), fileEnd = files.end();
        fileItr != fileEnd; ++fileItr )
    {
        Nocturnal::Path path (( *fileItr )->GetFilePath() );
        if ( path.Exists() )
        {
#pragma TODO( "Open the file for editing" )
NOC_BREAK();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnPreview( wxCommandEvent& event )
{
    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders; // ignored
    m_ResultsPanel->GetSelectedFilesAndFolders( files, folders );
    if ( !files.empty() )
    {
        Asset::AssetFile* file = *files.begin();
        Asset::AssetClassPtr asset = Asset::AssetFile::GetAssetClass( file );

        if ( !asset.ReferencesObject() )
        {
            tostringstream msg;
            msg << TXT( "Failed to load asset '" ) << file->GetFilePath() << TXT( "'. Unable to show preview." );
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
void BrowserFrame::OnSync( wxCommandEvent& event )
{
    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders;
    m_ResultsPanel->GetSelectedFilesAndFolders( files, folders );

    // Sync the files
    for ( Asset::V_AssetFiles::const_iterator fileItr = files.begin(), fileEnd = files.end();
        fileItr != fileEnd; ++fileItr )
    {
        try
        {
            RCS::File rcsFile( ( *fileItr )->GetFilePath() );
            rcsFile.Sync();
        }
        catch ( const Nocturnal::Exception& e )
        {
            wxMessageBox( e.What(), TXT( "Sync Failed!" ), wxCENTER | wxICON_ERROR | wxOK, this );
        }
    }

    // Sync the folders
    for ( Asset::V_AssetFolders::const_iterator folderItr = folders.begin(), folderEnd = folders.end();
        folderItr != folderEnd; ++folderItr )
    {
        tstring path = ( *folderItr )->GetFullPath();
        Nocturnal::Path::GuaranteeSlash( path );
        path += TXT( "..." );

        try
        {
            RCS::File rcsFile( path );
            rcsFile.Sync();
        }
        catch ( const Nocturnal::Exception& e )
        {
            wxMessageBox( e.What(), TXT( "Sync Failed!" ), wxCENTER | wxICON_ERROR | wxOK, this );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnCheckOut( wxCommandEvent& event )
{
    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders;
    m_ResultsPanel->GetSelectedFilesAndFolders( files, folders );
    if ( !folders.empty() )
    {
        if ( wxYES != 
            wxMessageBox( TXT( "Your selection includes folders.  Are you sure that you want to check out all of the contents of the selected folders?  This could result in checking out a lot of files." ), 
            TXT( "Check Out Folders?" ), 
            wxCENTER | wxYES_NO | wxICON_WARNING,
            this ) )
        {
            // Operation cancelled
            return;
        }
    }

    // Check out the files
    for ( Asset::V_AssetFiles::const_iterator fileItr = files.begin(), fileEnd = files.end();
        fileItr != fileEnd; ++fileItr )
    {
        try
        {
            RCS::File rcsFile( ( *fileItr )->GetFilePath() );
            rcsFile.Edit();
        }
        catch ( const Nocturnal::Exception& e )
        {
            wxMessageBox( e.What(), TXT( "Check Out Failed!" ), wxCENTER | wxICON_ERROR | wxOK, this );
        }
    }

    // Check out the folders
    for ( Asset::V_AssetFolders::const_iterator folderItr = folders.begin(), folderEnd = folders.end();
        folderItr != folderEnd; ++folderItr )
    {
        tstring path = ( *folderItr )->GetFullPath();
        Nocturnal::Path::GuaranteeSlash( path );
        path += TXT( "..." );

        try
        {
            RCS::File rcsFile( path );
            rcsFile.Edit();
        }
        catch ( const Nocturnal::Exception& e )
        {
            wxMessageBox( e.What(), TXT( "Check Out Failed!" ), wxCENTER | wxICON_ERROR | wxOK, this );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnRevisionHistory( wxCommandEvent& event )
{
    std::vector< tstring > paths;
    if ( m_ResultsPanel->GetSelectedPaths( paths ) == 1 )
    {
        tstring path = paths.front();
        tstring command = TXT( "p4win.exe -H \"" ) + path + TXT( "\"" );

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
void BrowserFrame::OnCopyPath( wxCommandEvent& event )
{
    std::vector< tstring > paths;
    if ( m_ResultsPanel->GetSelectedPaths( paths, event.GetId() == BrowserMenu::CopyPathClean ) )
    {
        wxString text;
        wxTextDataObject* dataObject = new wxTextDataObject();
        for ( std::vector< tstring >::const_iterator pathItr = paths.begin(),
            pathEnd = paths.end(); pathItr != pathEnd; ++pathItr )
        {
            if ( !text.empty() )
            {
                text += TXT( "\n" );
            }
            text += *pathItr;
        }

        if ( wxTheClipboard->Open() )
        {
            wxTheClipboard->SetData( new wxTextDataObject( text ) );
            wxTheClipboard->Close();
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnShowInFolders( wxCommandEvent& event )
{
    std::vector< tstring > paths;
    if ( m_ResultsPanel->GetSelectedPaths( paths )  == 1 )
    {
        wxBusyCursor bc;

        Nocturnal::Path path( paths.front() );
        if ( path.Exists() )
        {
            Search( path.Get() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnShowInPerforce( wxCommandEvent& event )
{
    std::vector< tstring > paths;
    if ( m_ResultsPanel->GetSelectedPaths( paths )  == 1 )
    {
        tstring path = paths.front();
        tstring command = TXT( "p4win.exe -s \"" ) + path + TXT( "\"" );

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
void BrowserFrame::OnShowInWindowsExplorer( wxCommandEvent& event )
{
    std::vector< tstring > paths;
    if ( m_ResultsPanel->GetSelectedPaths( paths ) == 1 )
    {
        tstring command = TXT( "explorer.exe " );
        Nocturnal::Path path( paths.front() );
        if ( path.IsFile() )
        {
            command += TXT( "/select," );
        }
        command += TXT( "\"" ) + path.Native() + TXT( "\"" );

        Platform::Execute( command );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnNewCollectionFromSelection( wxCommandEvent& event )
{
    wxBusyCursor busyCursor;
    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders;
    GetSelectedFilesAndFolders( files, folders );
    AssetCollectionPtr collection;

    if ( files.size() )
    {
        if ( event.GetId() == BrowserMenu::NewCollectionFromSelection )
        {
            collection = new AssetCollection( TXT( "New Collection" ), AssetCollectionFlags::CanRename | AssetCollectionFlags::CanHandleDragAndDrop );
            std::set< Nocturnal::Path > fileRefs;
            for ( Asset::V_AssetFiles::const_iterator fileItr = files.begin(), fileEnd = files.end(); fileItr != fileEnd; ++fileItr )
            {
                fileRefs.insert( (*fileItr)->GetPath() );
            }

            if ( !fileRefs.empty() )
            {
                collection->AddAssets( fileRefs );
            }
        }
        else
        {
            const bool reverse = event.GetId() == BrowserMenu::NewUsageCollectionFromSelection;
            Asset::AssetFile* file = *files.begin();
            DependencyCollectionPtr dependencyCollection = new DependencyCollection( file->GetShortName(), AssetCollectionFlags::Dynamic, reverse );
            dependencyCollection->SetRoot( file->GetPath() );
            dependencyCollection->LoadDependencies();
            collection = dependencyCollection;
        }
    }

    if ( collection )
    {
        CollectionManager* collectionManager = m_Browser->GetBrowserPreferences()->GetCollectionManager();
        tstring name;
        collectionManager->GetUniqueName( name, collection->GetName().c_str() );
        collection->SetName( name );
        collectionManager->AddCollection( collection );
    }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnRefresh( wxCommandEvent& args )
{
    m_SearchHistory->RunCurrentQuery();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnNew( wxCommandEvent& args )
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
void BrowserFrame::OnNewFolder( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnCut( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnCopy( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnPaste( wxCommandEvent& args )
{

}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnPreferences( wxCommandEvent& event )
{
    BrowserPreferencesDialog dlg( this );
    dlg.ShowModal();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnUpdateStatusBar( Luna::UpdateStatusEvent& event )
{
    m_StatusBar->SetStatusText( event.GetText() );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::OnClose( wxCloseEvent& event )
{
    event.Skip();

    m_Browser->GetBrowserPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &BrowserFrame::OnPreferencesChanged ) );

    m_Browser->GetBrowserPreferences()->SetThumbnailMode( m_CurrentViewOption );
    m_Browser->GetBrowserPreferences()->SetWindowSettings( this, &m_FrameManager );

    tstring path;
    m_FoldersPanel->GetPath( path );
    m_Browser->GetBrowserPreferences()->SetDefaultFolderPath( path );
    m_Browser->OnCloseBrowser();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search is requested
// 
void BrowserFrame::OnRequestSearch( const Luna::RequestSearchArgs& args )
{
    UpdateNavBar( args.m_SearchQuery );
    m_ResultsPanel->ClearResults();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search is starting.
// 
void BrowserFrame::OnBeginSearching( const Luna::BeginSearchingArgs& args )
{
    m_IsSearching = true;
    m_ResultsPanel->ClearResults();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search results are ready to be displayed.
// 
void BrowserFrame::OnResultsAvailable( const Luna::ResultsAvailableArgs& args )
{
    m_ResultsPanel->SetResults( args.m_SearchResults );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when search is done - update status bar and other UI elements.
// 
void BrowserFrame::OnSearchComplete( const Luna::SearchCompleteArgs& args )
{
    m_IsSearching = false;
#pragma TODO ("Rachel: figure out how to add EndSearch listener and hook up ResultsPanel::SelectPath to select this path.")
    Asset::V_AssetFiles files;
    Asset::V_AssetFolders folders;
    m_ResultsPanel->GetSelectedFilesAndFolders( files, folders );
    u32 numFolders = m_ResultsPanel->GetNumFolders();
    u32 numFiles = m_ResultsPanel->GetNumFiles();
    UpdateStatusBar( numFolders, numFiles, files.size() + folders.size(), TXT( "" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Update the UI when the preferences change
// 
void BrowserFrame::OnPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
    m_CurrentViewOption = m_Browser->GetBrowserPreferences()->GetThumbnailMode();
    UpdateResultsView( m_Browser->GetBrowserPreferences()->GetThumbnailSize() );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for changes to the ResultsPanel.  Updates the status bar accordingly.
// 
void BrowserFrame::OnResultsPanelUpdated( const ResultChangeArgs& args )
{
    u32 numFolders = m_ResultsPanel->GetNumFolders();
    u32 numFiles = m_ResultsPanel->GetNumFiles();
    UpdateStatusBar( numFolders, numFiles, args.m_NumSelected, args.m_HighlightPath );
}

///////////////////////////////////////////////////////////////////////////////
// Disconnect folder events and set the path
// 
void BrowserFrame::SetFolderPath( const tstring& folderPath )
{
    m_IgnoreFolderSelect = true;
    m_FoldersPanel->SetPath( folderPath );
    m_IgnoreFolderSelect = false;
}


///////////////////////////////////////////////////////////////////////////////
void BrowserFrame::UpdateNavBar( const SearchQueryPtr& searchQuery )
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
void BrowserFrame::UpdateResultsView( u16 customSize )
{
    m_ThumbnailViewMenu->Check( BrowserMenu::ViewSmall, false );
    m_ThumbnailViewMenu->Check( BrowserMenu::ViewMedium, false );
    m_ThumbnailViewMenu->Check( BrowserMenu::ViewLarge, false );

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
void BrowserFrame::UpdateStatusBar( size_t numFolders, size_t numFiles, size_t numSelected, const tstring& hover )
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
