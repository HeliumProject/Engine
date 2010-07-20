#include "Precompile.h"

#include "ViewPanel.h"
#include "ArtProvider.h"

using namespace Luna;

ViewPanel::ViewPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: ViewPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_ToolbarView->FindById( ID_CameraOrbit )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::PerspectiveCamera ) );
        m_ToolbarView->FindById( ID_CameraFront )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::FrontOrthoCamera ) );
        m_ToolbarView->FindById( ID_CameraSide )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::SideOrthoCamera ) );
        m_ToolbarView->FindById( ID_CameraTop )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::TopOrthoCamera ) );

        m_ToolbarView->Realize();

        Layout();
        Thaw();
    }

    m_Viewport = new Luna::Viewport( m_ViewPanel, -1, wxPoint(0,0), wxSize(150,250), wxNO_BORDER | wxWANTS_CHARS | wxEXPAND );
    m_ViewPanel->GetSizer()->Add( m_Viewport, 1, wxEXPAND | wxALL, 5 );

    Layout();
}
