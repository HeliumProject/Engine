#include "Precompile.h"

#include "VaultPanel.h"
#include "Editor/ArtProvider.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
VaultPanel::VaultPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: VaultPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Options ) );

        Layout();
        Thaw();
    }
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
    SearchQueryPtr query = new SearchQuery();

    tstring errors;
    if ( !query->SetQueryString( queryString, errors ) )
    {
        wxMessageBox( errors.c_str(), TXT( "Search Errors" ), wxCENTER | wxICON_WARNING | wxOK, this );
        return;
    }

    m_VaultSearch.StartSearchThread( query );
}

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
