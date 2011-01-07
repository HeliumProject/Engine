#include "Precompile.h"

#include "ToolbarPanel.h"
#include "ArtProvider.h"

#include "EditorIDs.h"

#include "Editor/App.h"
#include "Editor/Dialogs/FileDialog.h"

#include "Editor/Controls/Drawer/DrawerPanel.h"

using namespace Helium;
using namespace Helium::SceneGraph;
using namespace Helium::Editor;

ToolbarPanel::ToolbarPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: ToolbarPanelGenerated( parent, id, pos, size, style )
, m_CommandQueue( this )
, m_ToolPropertiesGenerator( &m_ToolPropertiesCanvas )
, m_ToolPropertiesManager( &m_ToolPropertiesGenerator, &m_CommandQueue )
{
    m_ToolPropertiesCanvas.SetPanel( m_ToolsPropertiesPanel );

    SetHelpText( TXT( "This is the Toolbar, it provides access to commonly used actions and tools." ) );

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    // Tool Bar Drawers
    //m_TestPanelGenerated = new TestPanelGenerated( m_DrawerPanel );
    //m_TestPanelGenerated->Hide();
    //m_TestDrawer = new Drawer( m_DrawerPanel, m_TestPanelGenerated, wxT( "TestDrawer" ), wxArtProvider::GetBitmap( ArtIDs::Editor::Helium ) );
    //m_DrawerPanel->AddDrawer( m_TestDrawer );
    //m_FrameManager.AddPane( m_TestPanelGenerated, m_TestDrawerPane );
    //m_ExcludeFromPanelsMenu.insert( m_TestDrawerPane.name );
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_SelectButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::SelectTool ) );
        m_TranslateButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::TranslateTool ) );
        m_RotateButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::RotateTool ) );
        m_ScaleButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::ScaleTool ) );
        
        m_DuplicateToolButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::DuplicateTool ) );
        m_LocatorToolButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::Locator ) );
        m_VolumeToolButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::Volume ) );
        m_EntityToolButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::Entity ) );
        m_CurveToolLocator->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::Curve ) );
        m_CurveEditToolButton->SetLabel( wxArtProvider::GetBitmap( ArtIDs::Editor::CurveEdit ) );

        m_PlayButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Play ) );
        m_PlayButton->Enable( false );
        m_PauseButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Pause ) );
        m_PauseButton->Enable( false );
        m_StopButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Stop ) );
        m_StopButton->Enable( false );

        Layout();
        Thaw();
    }

    m_SelectButton->SetHelpText( TXT ( "Select\n\nSelect items from the workspace" ) );
    m_TranslateButton->SetHelpText( TXT ( "Translate\n\nTranslate items" ) );
    m_RotateButton->SetHelpText( TXT ( "Rotate\n\nRotate selected items" ) );
    m_ScaleButton->SetHelpText( TXT ( "Scale\n\nScale selected items" ) );
    
    m_DuplicateToolButton->SetHelpText( TXT ( "Duplicate\n\nDuplicate the selected object numerous times" ) );
    m_LocatorToolButton->SetHelpText( TXT ( "Locator\n\nPlace locator objects (such as bug locators)" ) );
    m_VolumeToolButton->SetHelpText( TXT ( "Volume\n\nPlace volume objects (items for setting up gameplay)" ) );
    m_EntityToolButton->SetHelpText( TXT ( "Entity\n\nPlace entity objects (such as art instances or characters)" ) );
    m_CurveToolLocator->SetHelpText( TXT ( "Curve\n\nCreate curve objects (Linear, B-Spline, or Catmull-Rom Spline)" ) );
    m_CurveEditToolButton->SetHelpText( TXT ( "Edit Curve\n\nEdit created curves (modify or create/delete control points)" ) );

    m_PlayButton->SetHelpText( TXT( "Play\n\nClicking this will start the game in the editing window." ) );
    m_PauseButton->SetHelpText( TXT( "Pause\n\nClicking this will pause a currently running game session." ) );
    m_StopButton->SetHelpText( TXT( "Stop\n\nClicking this will stop a currently running game session." ) );

    m_VaultSearchBox->SetHelpText( TXT( "This is the Vault search box.  Entering text here and pressing enter will search the Asset Vault." ) );
    m_ToolsPropertiesPanel->SetHelpText( TXT( "This is the tools properties area.  Depending on the tool you have selected, this will show its options." ) );

    m_ToolsButtons.push_back( m_SelectButton );
    m_ToolsButtons.push_back( m_TranslateButton );
    m_ToolsButtons.push_back( m_RotateButton );
    m_ToolsButtons.push_back( m_ScaleButton );
    m_ToolsButtons.push_back( m_DuplicateToolButton );
    m_ToolsButtons.push_back( m_LocatorToolButton );
    m_ToolsButtons.push_back( m_VolumeToolButton );
    m_ToolsButtons.push_back( m_EntityToolButton );
    m_ToolsButtons.push_back( m_CurveToolLocator );
    m_ToolsButtons.push_back( m_CurveEditToolButton );

    m_SelectButton->SetId( EventIds::ID_ToolsSelect );
    m_SelectButton->Connect( EventIds::ID_ToolsSelect, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_TranslateButton->SetId( EventIds::ID_ToolsTranslate );
    m_TranslateButton->Connect( EventIds::ID_ToolsTranslate, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_RotateButton->SetId( EventIds::ID_ToolsRotate );
    m_RotateButton->Connect( EventIds::ID_ToolsRotate, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_ScaleButton->SetId( EventIds::ID_ToolsScale );
    m_ScaleButton->Connect( EventIds::ID_ToolsScale, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_DuplicateToolButton->SetId( EventIds::ID_ToolsDuplicate );
    m_DuplicateToolButton->Connect( EventIds::ID_ToolsDuplicate, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_LocatorToolButton->SetId( EventIds::ID_ToolsLocatorCreate );
    m_LocatorToolButton->Connect( EventIds::ID_ToolsLocatorCreate, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_VolumeToolButton->SetId( EventIds::ID_ToolsVolumeCreate );
    m_VolumeToolButton->Connect( EventIds::ID_ToolsVolumeCreate, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_EntityToolButton->SetId( EventIds::ID_ToolsEntityCreate );
    m_EntityToolButton->Connect( EventIds::ID_ToolsEntityCreate, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_CurveToolLocator->SetId( EventIds::ID_ToolsCurveCreate );
    m_CurveToolLocator->Connect( EventIds::ID_ToolsCurveCreate, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );
    
    m_CurveEditToolButton->SetId( EventIds::ID_ToolsCurveEdit );
    m_CurveEditToolButton->Connect( EventIds::ID_ToolsCurveEdit, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );

    Layout();
}

void ToolbarPanel::ToggleTool( int32_t selectedTool )
{
    for ( std::vector< wxBitmapToggleButton* >::const_iterator itr = m_ToolsButtons.begin(), end = m_ToolsButtons.end(); itr != end; ++itr )
    {
        (*itr)->SetValue( (*itr)->GetId() == selectedTool );
    }
}

void ToolbarPanel::EnableTools( const bool enable )
{
    m_SelectButton->Enable( enable );
    m_TranslateButton->Enable( enable );
    m_RotateButton->Enable( enable );
    m_ScaleButton->Enable( enable );

    m_DuplicateToolButton->Enable( enable );
    m_LocatorToolButton->Enable( enable );
    m_VolumeToolButton->Enable( enable );
    m_EntityToolButton->Enable( enable );
    m_CurveToolLocator->Enable( enable );
    m_CurveEditToolButton->Enable( enable );

    Refresh();
}

void ToolbarPanel::OnToggleToolButton( wxCommandEvent& event )
{
    GetParent()->GetEventHandler()->ProcessEvent( wxCommandEvent ( wxEVT_COMMAND_MENU_SELECTED, event.GetId() ) );
}
