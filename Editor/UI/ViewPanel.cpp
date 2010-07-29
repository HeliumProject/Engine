#include "Precompile.h"

#include "ViewPanel.h"
#include "ArtProvider.h"

using namespace Editor;

ViewPanel::ViewPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: ViewPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
//        Freeze();

        m_ToolbarView->FindById( ID_FrameOrigin )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::FrameOrigin ) );
        m_ToolbarView->FindById( ID_FrameSelected )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::FrameSelected ) );

        m_ToolbarView->FindById( ID_PreviousView )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::PreviousView ) );
        m_ToolbarView->FindById( ID_NextView )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::NextView ) );

        m_ToolbarView->FindById( ID_HighlightMode )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::HighlightMode ) );

        m_ToolbarView->FindById( ID_CameraOrbit )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::PerspectiveCamera ) );
        m_ToolbarView->FindById( ID_CameraFront )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::FrontOrthoCamera ) );
        m_ToolbarView->FindById( ID_CameraSide )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::SideOrthoCamera ) );
        m_ToolbarView->FindById( ID_CameraTop )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::TopOrthoCamera ) );

        m_ToolbarView->FindById( ID_ShowAxes )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowAxes ) );
        m_ToolbarView->FindById( ID_ShowGrid )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowGrid ) );
        m_ToolbarView->FindById( ID_ShowBounds )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowBounds ) );
        m_ToolbarView->FindById( ID_ShowStatistics )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowStatistics ) );

        m_ToolbarView->FindById( ID_FrustumCull )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::FrustumCull ) );
        m_ToolbarView->FindById( ID_BackfaceCull )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::BackfaceCull ) );

        m_ToolbarView->FindById( ID_ShadingWireframe )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ShadingWireframe ) );
        m_ToolbarView->FindById( ID_ShadingMaterial )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ShadingMaterial ) );

        m_ToolbarView->FindById( ID_ColorModeScene )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeScene ) );
        m_ToolbarView->FindById( ID_ColorModeLayer )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeLayer ) );
        m_ToolbarView->FindById( ID_ColorModeNodeType )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeNodeType ) );
        m_ToolbarView->FindById( ID_ColorModeScale )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeScale ) );
        m_ToolbarView->FindById( ID_ColorModeScaleGradient )->SetNormalBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeScaleGradient ) );

        m_ToolbarView->Realize();

        Layout();
        //Thaw();
    }

    m_Viewport = new Editor::Viewport( m_ViewContainerPanel, -1, wxPoint(0,0), wxSize(150,250), wxNO_BORDER | wxWANTS_CHARS | wxEXPAND );
    m_ViewContainerPanel->GetSizer()->Add( m_Viewport, 1, wxEXPAND | wxALL, 5 );

    m_ToolbarView->FindById( ID_HighlightMode )->SetToggle( m_Viewport->IsHighlighting() );

    m_ToolbarView->FindById( ID_CameraOrbit )->SetToggle( m_Viewport->GetCameraMode() == CameraModes::Orbit );
    m_ToolbarView->FindById( ID_CameraFront )->SetToggle( m_Viewport->GetCameraMode() == CameraModes::Front );
    m_ToolbarView->FindById( ID_CameraSide )->SetToggle( m_Viewport->GetCameraMode() == CameraModes::Side );
    m_ToolbarView->FindById( ID_CameraTop )->SetToggle( m_Viewport->GetCameraMode() == CameraModes::Top );

    m_ToolbarView->FindById( ID_ShowAxes )->SetToggle( m_Viewport->IsAxesVisible() );
    m_ToolbarView->FindById( ID_ShowGrid )->SetToggle( m_Viewport->IsGridVisible() );
    m_ToolbarView->FindById( ID_ShowBounds )->SetToggle( m_Viewport->IsBoundsVisible() );
    m_ToolbarView->FindById( ID_ShowStatistics )->SetToggle( m_Viewport->IsStatisticsVisible() );

    m_ToolbarView->FindById( ID_FrustumCull )->SetToggle( m_Viewport->GetCamera()->IsViewFrustumCulling() );
    m_ToolbarView->FindById( ID_BackfaceCull )->SetToggle( m_Viewport->GetCamera()->IsBackFaceCulling() );

    m_ToolbarView->FindById( ID_ShadingWireframe )->SetToggle( m_Viewport->GetCamera()->GetShadingMode() == ShadingModes::Wireframe );
    m_ToolbarView->FindById( ID_ShadingMaterial )->SetToggle( m_Viewport->GetCamera()->GetShadingMode() == ShadingModes::Material );

    //ViewColorMode colorMode = MainFramePreferences()->GetViewPreferences()->GetColorMode();
    //M_IDToColorMode::const_iterator colorModeItr = m_ColorModeLookup.begin();
    //M_IDToColorMode::const_iterator colorModeEnd = m_ColorModeLookup.end();
    //for ( ; colorModeItr != colorModeEnd; ++colorModeItr )
    //{
    //    m_ViewColorMenu->Check( colorModeItr->first, colorModeItr->second == colorMode );
    //}

    Layout();
}
