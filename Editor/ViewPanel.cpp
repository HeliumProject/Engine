#include "Precompile.h"

#include "ViewPanel.h"
#include "ArtProvider.h"

#include <wx/tglbtn.h>

using namespace Helium;
using namespace Helium::Editor;

ViewPanel::ViewPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: ViewPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
//        Freeze();

        m_FrameOriginButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::FrameOrigin ) );
        m_FrameSelectedButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::FrameSelected ) );

        m_PreviousViewButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::PreviousView ) );
        m_NextViewButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::NextView ) );

        m_HighlightModeToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::HighlightMode ) );

        m_OrbitCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::PerspectiveCamera ) );
        m_FrontCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::FrontOrthoCamera ) );
        m_SideCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::SideOrthoCamera ) );
        m_TopCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::TopOrthoCamera ) );

        m_ShowAxesToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowAxes ) );
        m_ShowGridToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowGrid ) );
        m_ShowBoundsToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowBounds ) );
        m_ShowStatisticsToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ShowStatistics ) );

        m_FrustumCullingToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::FrustumCulling ) );
        m_BackfaceCullingToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::BackfaceCulling ) );

        m_WireframeShadingToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ShadingWireframe ) );
        m_MaterialShadingToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ShadingMaterial ) );

        m_ColorModeSceneToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeScene ) );
        m_ColorModeLayerToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeLayer ) );
        m_ColorModeNodeTypeToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeNodeType ) );
        m_ColorModeScaleToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeScale ) );
        m_ColorModeScaleGradientToggleButton->SetLabel( wxArtProvider::GetBitmap( Editor::ArtIDs::ColorModeScaleGradient ) );

        m_ToolPanel->Layout();

        Layout();
        //Thaw();
    }

    m_FrameOriginButton->SetHelpText( TXT( "Frame the origin in the viewport." ) );
    m_FrameSelectedButton->SetHelpText( TXT( "Frame the selected item in the viewport." ) );

    m_PreviousViewButton->SetHelpText( TXT( "Switch to the previous camera view." ) );
    m_NextViewButton->SetHelpText( TXT( "Switch to the next camera view." ) );

    m_HighlightModeToggleButton->SetHelpText( TXT( "Toggle Highlight mode." ) );

    m_OrbitCameraToggleButton->SetHelpText( TXT( "Use the orbit camera." ) );
    m_FrontCameraToggleButton->SetHelpText( TXT( "Use the front camera." ) );
    m_SideCameraToggleButton->SetHelpText( TXT( "Use the side camera." ) );
    m_TopCameraToggleButton->SetHelpText( TXT( "Use the top camera." ) );

    m_ShowAxesToggleButton->SetHelpText( TXT( "Toggle drawing the axes in the viewport." ) );
    m_ShowGridToggleButton->SetHelpText( TXT( "Toggle drawing the grid in the viewport." ) );
    m_ShowBoundsToggleButton->SetHelpText( TXT( "Toggle drawing object bounds in the viewport." ) );
    m_ShowStatisticsToggleButton->SetHelpText( TXT( "Toggle showing statistics for the current scene." ) );

    m_FrustumCullingToggleButton->SetHelpText( TXT( "Toggle frustum culling." ) );
    m_BackfaceCullingToggleButton->SetHelpText( TXT( "Toggle backface culling." ) );

    m_WireframeShadingToggleButton->SetHelpText( TXT( "Toggle wireframe mode." ) );
    m_MaterialShadingToggleButton->SetHelpText( TXT( "Toggle material shading mode." ) );

    m_ColorModeSceneToggleButton->SetHelpText( TXT( "Toggle scene coloring mode." ) );
    m_ColorModeLayerToggleButton->SetHelpText( TXT( "Toggle layer coloring mode." ) );
    m_ColorModeNodeTypeToggleButton->SetHelpText( TXT( "Toggle node type coloring mode." ) );
    m_ColorModeScaleToggleButton->SetHelpText( TXT( "Toggle scale coloring mode." ) );
    m_ColorModeScaleGradientToggleButton->SetHelpText( TXT( "Toggle scale gradient coloring mode." ) );

    m_Viewport = new Editor::Viewport( m_ViewContainerPanel, -1, wxPoint(0,0), wxSize(150,250), wxNO_BORDER | wxWANTS_CHARS | wxEXPAND );
    m_ViewContainerPanel->GetSizer()->Add( m_Viewport, 1, wxEXPAND | wxALL, 0 );

    m_HighlightModeToggleButton->SetValue( m_Viewport->IsHighlighting() );

    m_OrbitCameraToggleButton->SetValue( m_Viewport->GetCameraMode() == CameraModes::Orbit );
    m_FrontCameraToggleButton->SetValue( m_Viewport->GetCameraMode() == CameraModes::Front );
    m_SideCameraToggleButton->SetValue( m_Viewport->GetCameraMode() == CameraModes::Side );
    m_TopCameraToggleButton->SetValue( m_Viewport->GetCameraMode() == CameraModes::Top );

    m_ShowAxesToggleButton->SetValue( m_Viewport->IsAxesVisible() );
    m_ShowGridToggleButton->SetValue( m_Viewport->IsGridVisible() );
    m_ShowBoundsToggleButton->SetValue( m_Viewport->IsBoundsVisible() );
    m_ShowStatisticsToggleButton->SetValue( m_Viewport->IsStatisticsVisible() );

    m_FrustumCullingToggleButton->SetValue( m_Viewport->GetCamera()->IsViewFrustumCulling() );
    m_BackfaceCullingToggleButton->SetValue( m_Viewport->GetCamera()->IsBackFaceCulling() );

    m_WireframeShadingToggleButton->SetValue( m_Viewport->GetCamera()->GetShadingMode() == ShadingModes::Wireframe );
    m_MaterialShadingToggleButton->SetValue( m_Viewport->GetCamera()->GetShadingMode() == ShadingModes::Material );

    //ViewColorMode colorMode = MainFramePreferences()->GetViewPreferences()->GetColorMode();
    //M_IDToColorMode::const_iterator colorModeItr = m_ColorModeLookup.begin();
    //M_IDToColorMode::const_iterator colorModeEnd = m_ColorModeLookup.end();
    //for ( ; colorModeItr != colorModeEnd; ++colorModeItr )
    //{
    //    m_ViewColorMenu->Check( colorModeItr->first, colorModeItr->second == colorMode );
    //}

    Layout();
}
