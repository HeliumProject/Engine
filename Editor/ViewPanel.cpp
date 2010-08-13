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

        m_FrameOriginButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::FrameOrigin ) );
        m_FrameSelectedButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::FrameSelected ) );

        m_PreviousViewButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::PreviousView ) );
        m_NextViewButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::NextView ) );

        m_HighlightModeToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::HighlightMode ) );

        m_OrbitCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::PerspectiveCamera ) );
        m_FrontCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::FrontOrthoCamera ) );
        m_SideCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::SideOrthoCamera ) );
        m_TopCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::TopOrthoCamera ) );

        m_ShowAxesToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ShowAxes ) );
        m_ShowGridToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ShowGrid ) );
        m_ShowBoundsToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ShowBounds ) );
        m_ShowStatisticsToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ShowStatistics ) );

        m_FrustumCullingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::FrustumCulling ) );
        m_BackfaceCullingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::BackfaceCulling ) );

        m_WireframeShadingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ShadingWireframe ) );
        m_MaterialShadingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ShadingMaterial ) );

        m_ColorModeSceneToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ColorModeScene ) );
        m_ColorModeLayerToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ColorModeLayer ) );
        m_ColorModeNodeTypeToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ColorModeNodeType ) );
        m_ColorModeScaleToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ColorModeScale ) );
        m_ColorModeScaleGradientToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::ColorModeScaleGradient ) );

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

    m_ViewCanvas = new Editor::ViewCanvas( m_ViewContainerPanel, -1, wxPoint(0,0), wxSize(150,250), wxNO_BORDER | wxWANTS_CHARS | wxEXPAND );
    m_ViewContainerPanel->GetSizer()->Add( m_ViewCanvas, 1, wxEXPAND | wxALL, 0 );

    m_HighlightModeToggleButton->SetValue( m_ViewCanvas->GetViewport().IsHighlighting() );

    m_OrbitCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == Core::CameraModes::Orbit );
    m_FrontCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == Core::CameraModes::Front );
    m_SideCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == Core::CameraModes::Side );
    m_TopCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == Core::CameraModes::Top );

    m_ShowAxesToggleButton->SetValue( m_ViewCanvas->GetViewport().IsAxesVisible() );
    m_ShowGridToggleButton->SetValue( m_ViewCanvas->GetViewport().IsGridVisible() );
    m_ShowBoundsToggleButton->SetValue( m_ViewCanvas->GetViewport().IsBoundsVisible() );
    m_ShowStatisticsToggleButton->SetValue( m_ViewCanvas->GetViewport().IsStatisticsVisible() );

    m_FrustumCullingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->IsViewFrustumCulling() );
    m_BackfaceCullingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->IsBackFaceCulling() );

    m_WireframeShadingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->GetShadingMode() == Core::ShadingModes::Wireframe );
    m_MaterialShadingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->GetShadingMode() == Core::ShadingModes::Material );

    //ViewColorMode colorMode = MainFramePreferences()->GetViewPreferences()->GetColorMode();
    //M_IDToColorMode::const_iterator colorModeItr = m_ColorModeLookup.begin();
    //M_IDToColorMode::const_iterator colorModeEnd = m_ColorModeLookup.end();
    //for ( ; colorModeItr != colorModeEnd; ++colorModeItr )
    //{
    //    m_ViewColorMenu->Check( colorModeItr->first, colorModeItr->second == colorMode );
    //}

    Layout();
}
