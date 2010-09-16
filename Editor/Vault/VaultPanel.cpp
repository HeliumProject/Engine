#include "Precompile.h"

#include "VaultPanel.h"
#include "Editor/ArtProvider.h"

#include "ListResultsView.h"
#include "ThumbnailView.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
VaultPanel::VaultPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: VaultPanelGenerated( parent, id, pos, size, style )
, m_CurrentView( NULL )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Options ) );

        m_CurrentView = m_ListResultsView;
        m_CurrentMode = VaultViewModes::Details;
        m_ListResultsView->Show();

        m_ListResultsView->InitResults();

        Layout();
        Thaw();
    }

    m_VaultSearch.AddSearchResultsAvailableListener( Editor::SearchResultsAvailableSignature::Delegate( this, &VaultPanel::OnSearchResultsAvailable ) );
}

VaultPanel::~VaultPanel()
{
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

    if ( !IsShown() )
    {
        Show();
        GetParent()->Update();
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
    if ( view != m_CurrentMode )
    {
        // Detach
        if ( m_CurrentView )
        {
            m_CurrentView->Hide();
            GetSizer()->Detach( m_CurrentView );
            m_CurrentView = NULL;
        }

        // Switch
        m_CurrentMode = view;
        switch ( m_CurrentMode )
        {
        default:
        case VaultViewModes::Details:
        case VaultViewModes::List:
            m_ListResultsView->ShowDetails( m_CurrentMode == VaultViewModes::Details ? true : false );
            m_CurrentView = m_ListResultsView;
            break;

        case VaultViewModes::ThumbnailsSmall:
            //m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Small );
        case VaultViewModes::ThumbnailsMedium:
            //m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Medium );
        case VaultViewModes::ThumbnailsLarge:
            //m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Large );
        case VaultViewModes::ThumbnailsCustom:
            //m_ThumbnailView->SetZoom( m_CustomThumbnailSize );

            //m_CurrentView = m_ThumbnailView;
            break;
        }

        // Attach
        if ( m_CurrentView )
        {
            m_CurrentView->Show();
            m_CurrentView->Layout();

            GetSizer()->Add( m_CurrentView, 1, wxALL | wxEXPAND, 0 );
            GetSizer()->FitInside( this );
            Layout();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
VaultViewMode VaultPanel::GetViewMode() const
{
    return m_CurrentMode;
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::SetResults( VaultSearchResults* results )
{
    m_ListResultsView->SetResults( results );

    //std::vector< tstring > unused;
    //ResultChangeArgs args;

    //switch ( m_CurrentMode )
    //{
    //default:
    //case VaultViewModes::Details:
    //case VaultViewModes::List:
    //    m_ListResultsView->SetResults( results );
    //    break;

    //case VaultViewModes::ThumbnailsSmall:
    //case VaultViewModes::ThumbnailsMedium:
    //case VaultViewModes::ThumbnailsLarge:
    //case VaultViewModes::ThumbnailsCustom:
    //    //m_ThumbnailView->SetResults( results );
    //    //args.m_NumSelected = m_ThumbnailView->GetSelectedPaths( unused );
    //    //args.m_HighlightPath = m_ThumbnailView->GetHighlightedPath();
    //    break;
    //}

    //m_ResultsChanged.Raise( args );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::ClearResults()
{
    switch ( m_CurrentMode )
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
    switch ( m_CurrentMode )
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
u32 VaultPanel::GetSelectedPaths( std::set< Helium::Path >& paths )
{
    switch ( m_CurrentMode )
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

    return static_cast< u32 >( paths.size() );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnSearchResultsAvailable( const Editor::SearchResultsAvailableArgs& args )
{
    SetResults( args.m_SearchResults );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnSearchCancelButtonClick( wxCommandEvent& event )
{
    m_VaultSearch.StopSearchThreadAndWait();
    event.Skip(false);
}

void VaultPanel::OnSearchGoButtonClick( wxCommandEvent& event )
{
    wxString queryString = m_SearchCtrl->GetLineText(0);
    queryString.Trim(true);  // trim white-space right 
    queryString.Trim(false); // trim white-space left

    Search( queryString.wx_str() );
    event.Skip(false);
}

void VaultPanel::OnSearchTextEnter( wxCommandEvent& event )
{
    wxString queryString = m_SearchCtrl->GetLineText(0);
    queryString.Trim(true);  // trim white-space right 
    queryString.Trim(false); // trim white-space left

    Search( queryString.wx_str() );
    event.Skip(false);
}

void VaultPanel::OnVaultSettingsButtonClick( wxCommandEvent& event )
{
    event.Skip();
}
