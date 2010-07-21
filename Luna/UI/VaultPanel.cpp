#include "Precompile.h"

#include "VaultPanel.h"
#include "ArtProvider.h"

using namespace Luna;

VaultPanel::VaultPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: VaultPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_BackButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Back ) );
        m_ForwardButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Forward ) );
        m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Options ) );

        Layout();
        Thaw();
    }

}
