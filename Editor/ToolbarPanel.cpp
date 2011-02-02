#include "Precompile.h"

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

        m_SelectPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_SelectBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::SelectTool ) );
        
        m_TranslatePanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_TranslateBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::TranslateTool ) );
        
        m_RotatePanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_RotateBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::RotateTool ) );
        
        m_ScalePanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_ScaleBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::ScaleTool ) );

        m_DuplicateToolPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_DuplicateToolBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::DuplicateTool ) );
        
        m_LocatorToolPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_LocatorToolBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::Locator ) );
        
        m_VolumeToolPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_VolumeToolBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::Volume ) );
        
        m_EntityToolPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_EntityToolBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::Entity ) );
        
        m_CurveToolPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_CurveToolBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::Curve ) );
        
        m_CurveEditToolPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_CurveEditToolBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::CurveEdit ) );

        m_PlayPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_PlayBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Play ) );
        m_PlayPanelButton->Enable( false );
        
        m_PausePanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_PauseBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Pause ) );
        m_PausePanelButton->Enable( false );
        
        m_StopPanelButton->SetOptions( PanelButtonOptions::Toggle );
        m_StopBitmap->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Stop ) );
        m_StopPanelButton->Enable( false );

        Layout();
        Thaw();
    }

    m_SelectPanelButton->SetHelpText( TXT ( "Select\n\nSelect items from the workspace" ) );
    m_TranslatePanelButton->SetHelpText( TXT ( "Translate\n\nTranslate items" ) );
    m_RotatePanelButton->SetHelpText( TXT ( "Rotate\n\nRotate selected items" ) );
    m_ScalePanelButton->SetHelpText( TXT ( "Scale\n\nScale selected items" ) );
    
    m_DuplicateToolPanelButton->SetHelpText( TXT ( "Duplicate\n\nDuplicate the selected object numerous times" ) );
    m_LocatorToolPanelButton->SetHelpText( TXT ( "Locator\n\nPlace locator objects (such as bug locators)" ) );
    m_VolumeToolPanelButton->SetHelpText( TXT ( "Volume\n\nPlace volume objects (items for setting up gameplay)" ) );
    m_EntityToolPanelButton->SetHelpText( TXT ( "Entity\n\nPlace entity objects (such as art instances or characters)" ) );
    m_CurveToolPanelButton->SetHelpText( TXT ( "Curve\n\nCreate curve objects (Linear, B-Spline, or Catmull-Rom Spline)" ) );
    m_CurveEditToolPanelButton->SetHelpText( TXT ( "Edit Curve\n\nEdit created curves (modify or create/delete control points)" ) );

    m_PlayPanelButton->SetHelpText( TXT( "Play\n\nClicking this will start the game in the editing window." ) );
    m_PausePanelButton->SetHelpText( TXT( "Pause\n\nClicking this will pause a currently running game session." ) );
    m_StopPanelButton->SetHelpText( TXT( "Stop\n\nClicking this will stop a currently running game session." ) );

    m_VaultSearchBox->SetHelpText( TXT( "This is the Vault search box.  Entering text here and pressing enter will search the Asset Vault." ) );
    m_ToolsPropertiesPanel->SetHelpText( TXT( "This is the tools properties area.  Depending on the tool you have selected, this will show its options." ) );

    m_ToolsButtons.push_back( m_SelectPanelButton );
    m_ToolsButtons.push_back( m_TranslatePanelButton );
    m_ToolsButtons.push_back( m_RotatePanelButton );
    m_ToolsButtons.push_back( m_ScalePanelButton );
    m_ToolsButtons.push_back( m_DuplicateToolPanelButton );
    m_ToolsButtons.push_back( m_LocatorToolPanelButton );
    m_ToolsButtons.push_back( m_VolumeToolPanelButton );
    m_ToolsButtons.push_back( m_EntityToolPanelButton );
    m_ToolsButtons.push_back( m_CurveToolPanelButton );
    m_ToolsButtons.push_back( m_CurveEditToolPanelButton );

    Layout();
}

void ToolbarPanel::ToggleTool( int32_t selectedTool )
{
    for ( std::vector< PanelButton* >::const_iterator itr = m_ToolsButtons.begin(), end = m_ToolsButtons.end(); itr != end; ++itr )
    {
        (*itr)->SetValue( (*itr)->GetId() == selectedTool );
    }

    Refresh();
}

void ToolbarPanel::EnableTools( const bool enable )
{
    m_SelectPanelButton->Enable( enable );
    m_TranslatePanelButton->Enable( enable );
    m_RotatePanelButton->Enable( enable );
    m_ScalePanelButton->Enable( enable );

    m_DuplicateToolPanelButton->Enable( enable );
    m_LocatorToolPanelButton->Enable( enable );
    m_VolumeToolPanelButton->Enable( enable );
    m_EntityToolPanelButton->Enable( enable );
    m_CurveToolPanelButton->Enable( enable );
    m_CurveEditToolPanelButton->Enable( enable );

    Refresh();
}
