#include "EditorPch.h"

#include "ToolbarPanel.h"
#include "ArtProvider.h"

#include "EditorIDs.h"

#include "Editor/App.h"
#include "Editor/Dialogs/FileDialog.h"

#include "Editor/Controls/Drawer/DrawerManager.h"

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
    m_ToolPropertiesCanvas.SetDrawerManager( &m_DrawerManager );

    //SetHelpText( TXT( "This is the Toolbar, it provides access to commonly used actions and tools." ) );

#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_SelectButton->SetOptions( PanelButtonOptions::Toggle );
        m_SelectBitmap->SetArtID( ArtIDs::Editor::SelectTool );
        
        m_TranslateButton->SetOptions( PanelButtonOptions::Toggle );
        m_TranslateBitmap->SetArtID( ArtIDs::Editor::TranslateTool );
        
        m_RotateButton->SetOptions( PanelButtonOptions::Toggle );
        m_RotateBitmap->SetArtID( ArtIDs::Editor::RotateTool );
        
        m_ScaleButton->SetOptions( PanelButtonOptions::Toggle );
        m_ScaleBitmap->SetArtID( ArtIDs::Editor::ScaleTool );

        m_DuplicateToolButton->SetOptions( PanelButtonOptions::Toggle );
        m_DuplicateToolBitmap->SetArtID( ArtIDs::Editor::DuplicateTool );
        
        m_LocatorToolButton->SetOptions( PanelButtonOptions::Toggle );
        m_LocatorToolBitmap->SetArtID( ArtIDs::Editor::Locator );
        
        m_VolumeToolButton->SetOptions( PanelButtonOptions::Toggle );
        m_VolumeToolBitmap->SetArtID( ArtIDs::Editor::Volume );
        
        m_EntityToolButton->SetOptions( PanelButtonOptions::Toggle );
        m_EntityToolBitmap->SetArtID( ArtIDs::Editor::Entity );
        
        m_CurveToolButton->SetOptions( PanelButtonOptions::Toggle );
        m_CurveToolBitmap->SetArtID( ArtIDs::Editor::Curve );
        
        m_CurveEditToolButton->SetOptions( PanelButtonOptions::Toggle );
        m_CurveEditToolBitmap->SetArtID( ArtIDs::Editor::CurveEdit );

        m_PlayButton->SetOptions( PanelButtonOptions::Toggle );
        m_PlayBitmap->SetArtID( ArtIDs::Actions::Play );
        m_PlayButton->Enable( false );
        
        m_PauseButton->SetOptions( PanelButtonOptions::Toggle );
        m_PauseBitmap->SetArtID( ArtIDs::Actions::Pause );
        m_PauseButton->Enable( false );
        
        m_StopButton->SetOptions( PanelButtonOptions::Toggle );
        m_StopBitmap->SetArtID( ArtIDs::Actions::Stop );
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
    m_CurveToolButton->SetHelpText( TXT ( "Curve\n\nCreate curve objects (Linear, B-Spline, or Catmull-Rom Spline)" ) );
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
    m_ToolsButtons.push_back( m_CurveToolButton );
    m_ToolsButtons.push_back( m_CurveEditToolButton );

    Layout();
}

void ToolbarPanel::ToggleTool( int32_t selectedTool )
{
    for ( std::vector< EditorButton* >::iterator itr = m_ToolsButtons.begin(), end = m_ToolsButtons.end(); itr != end; ++itr )
    {
        (*itr)->SetValue( (*itr)->GetId() == selectedTool );
    }

    Refresh();
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
    m_CurveToolButton->Enable( enable );
    m_CurveEditToolButton->Enable( enable );

    Refresh();
}
