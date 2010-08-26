#include "Precompile.h"

#include "VaultPanel.h"
#include "Editor/ArtProvider.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
VaultPanel::VaultPanel( VaultSearch* vaultSearch, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: VaultPanelGenerated( parent, id, pos, size, style )
, m_VaultSearch( vaultSearch )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Options ) );

        Layout();
        Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Sets the string in the NavBar and starts the search query.
void VaultPanel::Search( const tstring& queryString )
{
    wxBusyCursor bc;
    if ( queryString.empty() )
        return;

    tstring errors;
    if ( !SearchQuery::ParseQueryString( queryString, errors ) )
    {
        wxMessageBox( errors.c_str(), TXT( "Search Errors" ), wxCENTER | wxICON_WARNING | wxOK, this );
        return;
    }

    //m_SearchHistory->RunNewQuery( queryString );
    // parse the query string
    SearchQueryPtr query = new SearchQuery();
    query->SetQueryString( queryString );
    m_VaultSearch->RequestSearch( query );
}
