#include "Precompile.h"

#include "Editor/App.h"
#include "Editor/EditorIDs.h"
#include "Editor/EditorGenerated.h"

#include "Pipeline/SceneGraph/CameraSettings.h"

#include "ViewPanel.h"
#include "ArtProvider.h"

#include <wx/tglbtn.h>

using namespace Helium;
using namespace Helium::Editor;
using namespace Helium::SceneGraph;

wxSize ViewPanel::DefaultIconSize( 16, 16 );

ViewPanel::ViewPanel( SettingsManager* settingsManager, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: ViewPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        //Freeze();

        m_FrameOriginButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::FrameOrigin ) );
        m_FrameSelectedButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::FrameSelected ) );

        m_PreviousViewButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::PreviousView ) );
        m_NextViewButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::NextView ) );

        m_HighlightModeToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::HighlightMode ) );

        m_OrbitCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::PerspectiveCamera ) );
        m_FrontCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::FrontOrthoCamera ) );
        m_SideCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::SideOrthoCamera ) );
        m_TopCameraToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::TopOrthoCamera ) );

        m_ShowAxesToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ShowAxes ) );
        m_ShowGridToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ShowGrid ) );
        m_ShowBoundsToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ShowBounds ) );
        m_ShowStatisticsToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ShowStatistics ) );

        m_FrustumCullingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::FrustumCulling ) );
        m_BackfaceCullingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::BackfaceCulling ) );

        m_WireframeShadingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ShadingWireframe ) );
        m_MaterialShadingToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ShadingMaterial ) );

        m_ColorModeSceneToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ColorModeScene ) );
        m_ColorModeLayerToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ColorModeLayer ) );
        m_ColorModeNodeTypeToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ColorModeNodeType ) );
        m_ColorModeScaleToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ColorModeScale ) );
        m_ColorModeScaleGradientToggleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ColorModeScaleGradient ) );

        m_ToolPanel->Layout();

        Layout();
        //Thaw();
    }

#pragma TODO( "Remove this block when wxFormBuilder fully supports wxToggleButton" )
    {
        m_OrbitCameraToggleButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
	    m_FrontCameraToggleButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
	    m_SideCameraToggleButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
	    m_TopCameraToggleButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
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

    m_ViewCanvas = new Editor::ViewCanvas( settingsManager, m_ViewContainerPanel, -1, wxPoint(0,0), wxSize(150,250), wxNO_BORDER | wxWANTS_CHARS | wxEXPAND );
    m_ViewContainerPanel->GetSizer()->Add( m_ViewCanvas, 1, wxEXPAND | wxALL, 0 );

    RefreshButtonStates();

    //ViewColorMode colorMode = MainFramePreferences()->GetViewPreferences()->GetColorMode();
    //M_IDToColorMode::const_iterator colorModeItr = m_ColorModeLookup.begin();
    //M_IDToColorMode::const_iterator colorModeEnd = m_ColorModeLookup.end();
    //for ( ; colorModeItr != colorModeEnd; ++colorModeItr )
    //{
    //    m_ViewColorMenu->Check( colorModeItr->first, colorModeItr->second == colorMode );
    //}

    Connect( wxEVT_CHAR, wxKeyEventHandler( ViewPanel::OnChar ), NULL, this );

    Connect( ViewPanelEvents::Wireframe, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnRenderMode ), NULL, this );
    Connect( ViewPanelEvents::Material, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnRenderMode ), NULL, this );
    Connect( ViewPanelEvents::Texture, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnRenderMode ), NULL, this );
    Connect( ViewPanelEvents::OrbitCamera, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
    Connect( ViewPanelEvents::FrontCamera, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
    Connect( ViewPanelEvents::SideCamera, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
    Connect( ViewPanelEvents::TopCamera, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnCamera ), NULL, this );
    Connect( ViewPanelEvents::FrameOrigin, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnFrameOrigin ), NULL, this );
    Connect( ViewPanelEvents::FrameSelected, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnFrameSelected ), NULL, this );
    Connect( ViewPanelEvents::ToggleHighlightMode, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnToggleHighlightMode ), NULL, this );
    Connect( ViewPanelEvents::NextView, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnNextView ), NULL, this );
    Connect( ViewPanelEvents::PreviousView, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ViewPanel::OnPreviousView ), NULL, this );

    Layout();
}

void ViewPanel::RefreshButtonStates()
{
    m_HighlightModeToggleButton->SetValue( m_ViewCanvas->GetViewport().IsHighlighting() );

    m_OrbitCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == SceneGraph::CameraMode::Orbit );
    m_FrontCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == SceneGraph::CameraMode::Front );
    m_SideCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == SceneGraph::CameraMode::Side );
    m_TopCameraToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCameraMode() == SceneGraph::CameraMode::Top );

    m_ShowAxesToggleButton->SetValue( m_ViewCanvas->GetViewport().IsAxesVisible() );
    m_ShowGridToggleButton->SetValue( m_ViewCanvas->GetViewport().IsGridVisible() );
    m_ShowBoundsToggleButton->SetValue( m_ViewCanvas->GetViewport().IsBoundsVisible() );
    m_ShowStatisticsToggleButton->SetValue( m_ViewCanvas->GetViewport().IsStatisticsVisible() );

    m_FrustumCullingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->IsViewFrustumCulling() );
    m_BackfaceCullingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->IsBackFaceCulling() );

    m_WireframeShadingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->GetShadingMode() == SceneGraph::ShadingMode::Wireframe );
    m_MaterialShadingToggleButton->SetValue( m_ViewCanvas->GetViewport().GetCamera()->GetShadingMode() == SceneGraph::ShadingMode::Material );
}

void ViewPanel::OnChar( wxKeyEvent& event )
{
    int keyCode = event.GetKeyCode();

    switch ( keyCode )
    {
    case WXK_SPACE:
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::NextView ) );
        event.Skip( false );
        break;

    case WXK_UP:
        wxGetApp().GetFrame()->GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkUp) );
        event.Skip(false);
        break;

    case WXK_DOWN:
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkDown) );
        event.Skip(false);
        break;

    case WXK_RIGHT:
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkForward) );
        event.Skip(false);
        break;

    case WXK_LEFT:
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkBackward) );
        event.Skip(false);
        break;

    case WXK_INSERT:
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsPivot) );
        event.Skip(false);
        break;

    case WXK_DELETE:
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, wxID_DELETE ) );
        event.Skip(false);
        break;

    case WXK_ESCAPE:
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED ) );
        event.Skip(false);
        break;

        //
        // ASCII has some strange key codes for ctrl-<letter> combos
        //
        //01 |   1         Ctrl-a         SOH 
        //02 |   2         Ctrl-b         STX 
        //03 |   3         Ctrl-c         ETX 
        //04 |   4         Ctrl-d         EOT 
        //05 |   5         Ctrl-e         ENQ 
        //06 |   6         Ctrl-f         ACK 
        //07 |   7         Ctrl-g         BEL 
        //08 |   8         Ctrl-h         BS 
        //09 |   9  Tab    Ctrl-i         HT 
        //0A |  10         Ctrl-j         LF 
        //0B |  11         Ctrl-k         VT 
        //0C |  12         Ctrl-l         FF 
        //0D |  13  Enter  Ctrl-m         CR 
        //0E |  14         Ctrl-n         SO 
        //0F |  15         Ctrl-o         SI 
        //10 |  16         Ctrl-p         DLE 
        //11 |  17         Ctrl-q         DC1 
        //12 |  18         Ctrl-r         DC2 
        //13 |  19         Ctrl-s         DC3 
        //14 |  20         Ctrl-t         DC4 
        //15 |  21         Ctrl-u         NAK 
        //16 |  22         Ctrl-v         SYN 
        //17 |  23         Ctrl-w         ETB 
        //18 |  24         Ctrl-x         CAN 
        //19 |  25         Ctrl-y         EM 
        //1A |  26         Ctrl-z         SUB 
        //1B |  27  Esc    Ctrl-[         ESC 
        //1C |  28         Ctrl-\         FS 
        //1D |  29         Ctrl-]         GS 

    case 1: // ctrl-a
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, wxID_SELECTALL ) );
        event.Skip( false );
        break;

    case 22: // ctrl-v
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, wxID_PASTE ) );
        event.Skip( false );
        break;

    case 24: // ctrl-x
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, wxID_CUT ) );
        event.Skip( false );
        break;

    case wxT( '4' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::Wireframe ) );
        event.Skip( false );
        break;

    case wxT( '5' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::Material ) );
        event.Skip( false );
        break;

    case wxT( '6' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::Texture ) );
        event.Skip( false );
        break;

    case wxT( '7' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::OrbitCamera ) );
        event.Skip( false );
        break;

    case wxT( '8' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::FrontCamera ) );
        event.Skip( false );
        break;

    case wxT( '9' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::SideCamera ) );
        event.Skip( false );
        break;

    case wxT( '0' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::TopCamera ) );
        event.Skip( false );
        break;

    case wxT( 'o' ):
    case wxT( 'O' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::FrameOrigin ) );
        event.Skip( false );
        break;

    case wxT( 'f' ):
    case wxT( 'F' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::FrameSelected ) );
        event.Skip( false );
        break;

    case wxT( 'h' ):
    case wxT( 'H' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::ToggleHighlightMode ) );
        event.Skip( false );
        break;

    case wxT( ']' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::NextView ) );
        event.Skip( false );
        break;

    case wxT( '[' ):
        GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ViewPanelEvents::PreviousView ) );
        event.Skip( false );
        break;

    default:
        event.Skip();
        event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
        break;
    }
}

void ViewPanel::OnRenderMode( wxCommandEvent& event )
{
    switch ( event.GetId() )
    {
    case ViewPanelEvents::Wireframe:
        {
            m_ViewCanvas->GetViewport().GetCamera()->SetShadingMode( ShadingMode::Wireframe );
            Refresh();
            event.Skip( false );
            break;
        }

    case ViewPanelEvents::Material:
        {
            m_ViewCanvas->GetViewport().GetCamera()->SetShadingMode( ShadingMode::Material );
            Refresh();
            event.Skip( false );
            break;
        }

    case ViewPanelEvents::Texture:
        {
            m_ViewCanvas->GetViewport().GetCamera()->SetShadingMode( ShadingMode::Texture );
            Refresh();
            event.Skip( false );
            break;
        }

    default:
        event.Skip();
        event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
        break;
    }
}

void ViewPanel::OnCamera( wxCommandEvent& event )
{
    switch ( event.GetId() )
    {
    case ViewPanelEvents::OrbitCamera:
    case ID_OrbitCamera:
        {
            m_ViewCanvas->GetViewport().SetCameraMode( CameraMode::Orbit );
            Refresh();
            event.Skip( false );
            break;
        }

    case ViewPanelEvents::FrontCamera:
    case ID_FrontCamera:
        {
            m_ViewCanvas->GetViewport().SetCameraMode( CameraMode::Front );
            Refresh();
            event.Skip( false );
            break;
        }

    case ViewPanelEvents::SideCamera:
    case ID_SideCamera:
        {
            m_ViewCanvas->GetViewport().SetCameraMode( CameraMode::Side );
            Refresh();
            event.Skip( false );
            break;
        }

    case ViewPanelEvents::TopCamera:
    case ID_TopCamera:
        {
            m_ViewCanvas->GetViewport().SetCameraMode( CameraMode::Top );
            Refresh();
            event.Skip( false );
            break;
        }

    default:
        event.Skip();
        event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
        break;
    }

    RefreshButtonStates();
}

void ViewPanel::OnFrameOrigin( wxCommandEvent& event )
{
    m_ViewCanvas->GetViewport().GetCamera()->Reset();
    Refresh();
    event.Skip( false );
}

void ViewPanel::OnFrameSelected( wxCommandEvent& event )
{
    if ( !wxGetApp().GetFrame()->GetSceneManager().HasCurrentScene() )
    {
        event.Skip();
        event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
        return;
    }

    wxGetApp().GetFrame()->GetSceneManager().GetCurrentScene()->FrameSelected();
    Refresh();
    event.Skip( false );
}

void ViewPanel::OnToggleHighlightMode( wxCommandEvent& event )
{
    m_ViewCanvas->GetViewport().SetHighlighting( !m_ViewCanvas->GetViewport().IsHighlighting() );
    m_HighlightModeToggleButton->SetValue( m_ViewCanvas->GetViewport().IsHighlighting() );
    Refresh();
    event.Skip( false );
}

void ViewPanel::OnNextView( wxCommandEvent& event )
{
    m_ViewCanvas->GetViewport().NextCameraMode();
    Refresh();
    RefreshButtonStates();
}

void ViewPanel::OnPreviousView( wxCommandEvent& event )
{
    m_ViewCanvas->GetViewport().PreviousCameraMode();
    Refresh();
    RefreshButtonStates();
}
