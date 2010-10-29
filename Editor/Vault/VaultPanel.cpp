#include "Precompile.h"

#include "VaultPanel.h"
#include "Editor/ArtProvider.h"

#include "ListResultsView.h"
#include "ThumbnailView.h"

#include "Editor/App.h"
#include "Editor/Controls/MenuButton.h"
#include "Editor/Vault/VaultSettings.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
VaultPanel::VaultPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: VaultPanelGenerated( parent, id, pos, size, style )
, m_VaultSettings( NULL )
, m_CurrentView( NULL )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Options, wxART_OTHER, wxSize(16, 16) ) );
        m_OptionsButton->SetMargins( 3, 3 );

        m_CurrentView = m_ListResultsView;
        m_CurrentViewMode = VaultViewModes::Details;
        m_ListResultsView->Show();

        m_ListResultsView->InitResults();

        Layout();
        Thaw();
    }

    {
        m_OptionsMenu = new wxMenu();

        wxMenuItem* detailsMenuItem = new wxMenuItem(
            m_OptionsMenu,
            VaultMenu::ViewResultDetails,
            VaultMenu::Label( VaultMenu::ViewResultDetails ),
            VaultMenu::Label( VaultMenu::ViewResultDetails ).c_str(),
            wxITEM_RADIO );
        m_OptionsMenu->Append( detailsMenuItem );
        Connect( VaultMenu::ViewResultDetails, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

        wxMenuItem* listMenuItem = new wxMenuItem(
            m_OptionsMenu,
            VaultMenu::ViewResultList,
            VaultMenu::Label( VaultMenu::ViewResultList ),
            VaultMenu::Label( VaultMenu::ViewResultList ).c_str(),
            wxITEM_RADIO );
        m_OptionsMenu->Append( listMenuItem );
        Connect( VaultMenu::ViewResultList, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

        wxMenuItem* smallMenuItem = new wxMenuItem(
            m_OptionsMenu,
            VaultMenu::ViewThumbnailsSmall,
            VaultMenu::Label( VaultMenu::ViewThumbnailsSmall ) + std::string( " " ) + VaultThumbnailsSizes::Label( VaultThumbnailsSizes::Small ),
            VaultMenu::Label( VaultMenu::ViewThumbnailsSmall ).c_str(),
            wxITEM_RADIO );
        smallMenuItem->Enable( false );
        m_OptionsMenu->Append( smallMenuItem );
        Connect( VaultMenu::ViewThumbnailsSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

        wxMenuItem* mediumMenuItem = new wxMenuItem( 
            m_OptionsMenu, 
            VaultMenu::ViewThumbnailsMedium, 
            VaultMenu::Label( VaultMenu::ViewThumbnailsMedium ) + std::string( " " ) + VaultThumbnailsSizes::Label( VaultThumbnailsSizes::Medium ),
            VaultMenu::Label( VaultMenu::ViewThumbnailsMedium ),
            wxITEM_RADIO );
        mediumMenuItem->Enable( false );
        m_OptionsMenu->Append( mediumMenuItem );
        Connect( VaultMenu::ViewThumbnailsMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

        wxMenuItem* largeMenuItem = new wxMenuItem(
            m_OptionsMenu,
            VaultMenu::ViewThumbnailsLarge,
            VaultMenu::Label( VaultMenu::ViewThumbnailsLarge ) + std::string( " " ) + VaultThumbnailsSizes::Label( VaultThumbnailsSizes::Large ),
            VaultMenu::Label( VaultMenu::ViewThumbnailsLarge ),
            wxITEM_RADIO );
        largeMenuItem->Enable( false );
        m_OptionsMenu->Append( largeMenuItem );
        Connect( VaultMenu::ViewThumbnailsLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
    }

    m_OptionsButton->SetContextMenu( m_OptionsMenu );
    m_OptionsButton->SetHoldDelay( 0.0f );
    m_OptionsButton->Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( VaultPanel::OnOptionsMenuOpen ), NULL, this );
    m_OptionsButton->Connect( wxEVT_MENU_CLOSE, wxMenuEventHandler( VaultPanel::OnOptionsMenuClose ), NULL, this );
    m_OptionsButton->Enable( true );

    // settings
    m_VaultSettings = wxGetApp().GetSettingsManager()->GetSettings< VaultSettings >();
    if ( m_VaultSettings )
    {
        m_CurrentViewMode = m_VaultSettings->m_VaultViewMode;
        m_CurrentThumbnailSize = m_VaultSettings->m_ThumbnailSize;
        Math::Clamp( m_CurrentThumbnailSize, VaultThumbnailsSizes::Small, VaultThumbnailsSizes::Large );
        //m_VaultSettings->m_WindowSettings->ApplyToWindow( this, m_FrameManager, true );
    }
    
    m_VaultSearch.AddSearchResultsAvailableListener( Editor::SearchResultsAvailableSignature::Delegate( this, &VaultPanel::OnSearchResultsAvailable ) );
}

VaultPanel::~VaultPanel()
{
    Disconnect( VaultMenu::ViewResultDetails, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
    Disconnect( VaultMenu::ViewResultList, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
    Disconnect( VaultMenu::ViewThumbnailsSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
    Disconnect( VaultMenu::ViewThumbnailsMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
    Disconnect( VaultMenu::ViewThumbnailsLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

    m_OptionsButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( VaultPanel::OnOptionsMenuOpen ), NULL, this );
    m_OptionsButton->Disconnect( wxEVT_MENU_CLOSE, wxMenuEventHandler( VaultPanel::OnOptionsMenuClose ), NULL, this );

    m_VaultSettings = NULL;

    m_VaultSearch.AddSearchResultsAvailableListener( Editor::SearchResultsAvailableSignature::Delegate( this, &VaultPanel::OnSearchResultsAvailable ) );
}

void VaultPanel::SetDirectory( const Helium::Path& directory )
{
    m_VaultSearch.SetDirectory( directory );
}

const Path& VaultPanel::GetDirectory() const
{
    return m_VaultSearch.GetDirectory();
}

///////////////////////////////////////////////////////////////////////////////
// Sets the string in the NavBar and starts the search query.
void VaultPanel::Search( const tstring& queryString )
{
    wxBusyCursor bc;
    if ( queryString.empty() )
    {
        return;
    }

    m_SearchCtrl->Clear();
    m_SearchCtrl->SetValue( queryString.c_str() );

    // parse the query string
    VaultSearchQueryPtr query = new VaultSearchQuery();

    tstring errors;
    if ( !query->SetQueryString( queryString, errors ) )
    {
        wxMessageBox( errors.c_str(), TXT( "Search Errors" ), wxCENTER | wxICON_WARNING | wxOK, this );
        return;
    }

    m_VaultSearch.StartSearchThread( query );
}


///////////////////////////////////////////////////////////////////////////////
void VaultPanel::SetViewMode( VaultViewMode view )
{
    if ( view != m_CurrentViewMode )
    {
        // Detach
        if ( m_CurrentView )
        {
            m_CurrentView->Hide();
            //GetSizer()->Detach( m_CurrentView );
            m_CurrentView = NULL;
        }

        // Switch
        m_CurrentViewMode = view;
        switch ( m_CurrentViewMode )
        {
        default:
        case VaultViewModes::Details:
            {
                m_ListResultsView->ShowDetails( true );
                m_CurrentView = m_ListResultsView;
            }
            break;

        case VaultViewModes::List:
            {
                m_ListResultsView->ShowDetails( false );
                m_CurrentView = m_ListResultsView;
            }
            break;

        case VaultViewModes::ThumbnailsSmall:
            {
                m_CurrentThumbnailSize = VaultThumbnailsSizes::Small;
                //m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Small );
                //m_CurrentView = m_ThumbnailView;
            }
            break;
        case VaultViewModes::ThumbnailsMedium:
            {
                m_CurrentThumbnailSize = VaultThumbnailsSizes::Medium;
                //m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Medium );
                //m_CurrentView = m_ThumbnailView;
            }
            break;
        case VaultViewModes::ThumbnailsLarge:
            {
                m_CurrentThumbnailSize = VaultThumbnailsSizes::Large;
                //m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Large );
                //m_CurrentView = m_ThumbnailView;
            }
            break;
        case VaultViewModes::ThumbnailsCustom:
            {
                //m_ThumbnailView->SetZoom( m_CurrentThumbnailSize );
                //m_CurrentView = m_ThumbnailView;
            }
            break;
        }

        // Attach
        if ( m_CurrentView )
        {
            m_CurrentView->Show();
            m_CurrentView->Layout();

            //GetSizer()->Add( m_CurrentView, 1, wxALL | wxEXPAND, 0 );
            GetSizer()->FitInside( this );
            Layout();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
VaultViewMode VaultPanel::GetViewMode() const
{
    return m_CurrentViewMode;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::SetResults( VaultSearchResults* results )
{
    //std::vector< tstring > unused;
    //ResultChangeArgs args;

    switch ( m_CurrentViewMode )
    {
    default:
    case VaultViewModes::Details:
    case VaultViewModes::List:
        m_ListResultsView->SetResults( results );
        break;

        //case VaultViewModes::ThumbnailsSmall:
        //case VaultViewModes::ThumbnailsMedium:
        //case VaultViewModes::ThumbnailsLarge:
        //case VaultViewModes::ThumbnailsCustom:
        //    //m_ThumbnailView->SetResults( results );
        //    //args.m_NumSelected = m_ThumbnailView->GetSelectedPaths( unused );
        //    //args.m_HighlightPath = m_ThumbnailView->GetHighlightedPath();
        //    break;
    }

    //m_ResultsChanged.Raise( args );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::ClearResults()
{
    switch ( m_CurrentViewMode )
    {
    default:
    case VaultViewModes::Details:
    case VaultViewModes::List:
        m_ListResultsView->ClearResults();
        break;

    case VaultViewModes::ThumbnailsSmall:
    case VaultViewModes::ThumbnailsMedium:
    case VaultViewModes::ThumbnailsLarge:
    case VaultViewModes::ThumbnailsCustom:
        //m_ThumbnailView->ClearResults();
        break;
    }

    //ResultChangeArgs args;
    //m_ResultsChanged.Raise( args );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::SelectPath( const Helium::Path& path )
{
    switch ( m_CurrentViewMode )
    {
    default:
    case VaultViewModes::Details:
    case VaultViewModes::List:
        m_ListResultsView->SelectPath( path );
        break;

    case VaultViewModes::ThumbnailsSmall:
    case VaultViewModes::ThumbnailsMedium:
    case VaultViewModes::ThumbnailsLarge:
    case VaultViewModes::ThumbnailsCustom:
        //m_ThumbnailView->SelectPath( path );
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
uint32_t VaultPanel::GetSelectedPaths( std::set< Helium::Path >& paths )
{
    switch ( m_CurrentViewMode )
    {
    default:
    case VaultViewModes::Details:
        break;

    case VaultViewModes::List:
        break;

    case VaultViewModes::ThumbnailsSmall:
    case VaultViewModes::ThumbnailsMedium:
    case VaultViewModes::ThumbnailsLarge:
    case VaultViewModes::ThumbnailsCustom:
        //m_ThumbnailView->GetSelectedPaths( paths );
        break;
    }

    return static_cast< uint32_t >( paths.size() );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::SaveSettings()
{
    if ( m_VaultSettings )
    {
        m_VaultSettings->m_VaultViewMode = m_CurrentViewMode;
        m_VaultSettings->m_ThumbnailSize = m_CurrentThumbnailSize;
        Math::Clamp( m_VaultSettings->m_ThumbnailSize, VaultThumbnailsSizes::Small, VaultThumbnailsSizes::Large );
        //m_VaultSettings->m_WindowSettings->SetFromWindow( this, m_FrameManager );
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnSearchResultsAvailable( const Editor::SearchResultsAvailableArgs& args )
{
    SetResults( args.m_SearchResults );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::StartSearchFromField()
{
    wxString queryString = m_SearchCtrl->GetLineText(0);
    queryString.Trim(true);  // trim white-space right 
    queryString.Trim(false); // trim white-space left

    Search( queryString.wx_str() );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnSearchCancelButtonClick( wxCommandEvent& event )
{
    m_VaultSearch.StopSearchThreadAndWait();
    event.Skip(false);
}

void VaultPanel::OnSearchGoButtonClick( wxCommandEvent& event )
{
    StartSearchFromField();
    event.Skip(false);
}

void VaultPanel::OnSearchTextEnter( wxCommandEvent& event )
{
    StartSearchFromField();
    event.Skip(false);
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnOptionsMenuOpen( wxMenuEvent& event )
{
    event.Skip();
    if ( event.GetMenu() == m_OptionsMenu )
    {
        // refresh menu's view toggles
    }
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnOptionsMenuClose( wxMenuEvent& event )
{
    m_SearchCtrl->SetFocus();
    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnOptionsMenuSelect( wxCommandEvent& event )
{
    event.Skip();

    VaultViewMode id = VaultViewModes::Details;

    switch( event.GetId() )
    {
    default:
    case VaultMenu::ViewResultDetails:
        id = VaultViewModes::Details;
        break;

    case VaultMenu::ViewResultList:
        id = VaultViewModes::List;
        break;

    case VaultMenu::ViewThumbnailsSmall:
        id = VaultViewModes::ThumbnailsSmall;
        break;

    case VaultMenu::ViewThumbnailsMedium:
        id = VaultViewModes::ThumbnailsMedium;
        break;

    case VaultMenu::ViewThumbnailsLarge:
        id = VaultViewModes::ThumbnailsLarge;
        break;
    };

    SetViewMode( id );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnClose( wxCloseEvent& event )
{
    SaveSettings();
}
