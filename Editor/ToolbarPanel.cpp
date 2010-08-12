#include "Precompile.h"

#include "ToolbarPanel.h"
#include "ArtProvider.h"

#include "EditorIDs.h"

using namespace Helium;
using namespace Helium::Editor;

ToolbarPanel::ToolbarPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: ToolbarPanelGenerated( parent, id, pos, size, style )
{
    SetHelpText( TXT( "This is the Toolbar, it provides access to commonly used actions and tools." ) );

#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_NewSceneButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::Scene ) );
        m_OpenButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::Open ) );
        m_SaveAllButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::SaveAll ) );
        m_CutButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::Cut ) );
        m_CopyButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::Copy ) );
        m_PasteButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::Paste ) );
        m_DeleteButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::Delete ) );
        m_UndoButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::Undo ) );
        m_RedoButton->SetBitmap( wxArtProvider::GetBitmap( Helium::ArtIDs::Redo ) );

        m_PlayButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::Play ) );
        m_PauseButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::Pause ) );
        m_StopButton->SetBitmap( wxArtProvider::GetBitmap( Editor::ArtIDs::Stop ) );

        Layout();
        Thaw();
    }

    m_NewSceneButton->SetHelpText( TXT( "New Scene\n\nClicking this will create a new scene." ) );
    m_OpenButton->SetHelpText( TXT( "Open\n\nClicking this button will bring up a file browser, allowing you to open files in the editor." ) );
    m_SaveAllButton->SetHelpText( TXT( "Save All\n\nClicking this button will save all your work." ) );
    m_CutButton->SetHelpText( TXT( "Cut\n\nClicking this will cut the selected items to the clipboard." ) );
    m_CopyButton->SetHelpText( TXT( "Copy\n\nClicking this will copy the selected items to the clipboard." ) );
    m_PasteButton->SetHelpText( TXT( "Paste\n\nClicking this will paste the contents of the clipboard." ) );
    m_DeleteButton->SetHelpText( TXT( "Delete\n\nClicking this will delete the selected items." ) );
    m_UndoButton->SetHelpText( TXT( "Undo\n\nClicking this will undo an action." ) );
    m_RedoButton->SetHelpText( TXT( "Redo\n\nClicking this will redo an action." ) );

    m_PlayButton->SetHelpText( TXT( "Play\n\nClicking this will start the game in the editing window." ) );
    m_PauseButton->SetHelpText( TXT( "Pause\n\nClicking this will pause a currently running game session." ) );
    m_StopButton->SetHelpText( TXT( "Stop\n\nClicking this will stop a currently running game session." ) );

    m_SaveAllButton->Disable();

    std::map< ToolType, wxPanel* > toolTypePanels;
    for ( i32 i=0; i < ToolModes::Count; ++i )
    {
        ToolInfo* info = &ToolInfos[ i ];

        std::map< ToolType, wxPanel* >::iterator itr = toolTypePanels.find( info->m_Type );

        wxPanel* typePanel = NULL;

        if ( itr == toolTypePanels.end() )
        {
            typePanel = new wxPanel( m_ToolsPanel );
            typePanel->SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
            toolTypePanels[ info->m_Type ] = typePanel;
            m_ToolsPanel->GetSizer()->Add( typePanel, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxEXPAND, 0 );
            //m_ToolsPanel->GetSizer()->Add( new wxStaticLine( m_ToolsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL ), 0, wxEXPAND | wxALL, 2 );
            m_ToolsPanel->GetSizer()->Add( 20, 0 ); // add spacer to the right of the panel
        }

        if ( !typePanel )
        {
            typePanel = (*itr).second;
        }

        wxBitmapToggleButton* btn = new wxBitmapToggleButton( typePanel, info->m_ID, wxArtProvider::GetBitmap( info->m_Bitmap, wxART_OTHER, wxSize( 16, 16 ) ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
        btn->SetToolTip( info->m_Description );
        btn->SetHelpText( info->m_Description );

        // connect its event handler to us
        btn->Connect( btn->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );

        typePanel->GetSizer()->Add( btn, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
        m_ToolsButtons.push_back( btn );
    }

    m_VaultSearchBox->SetHelpText( TXT( "This is the Vault search box.  Entering text here and pressing enter will search the Asset Vault." ) );
    m_ToolsPropertiesPanel->SetHelpText( TXT( "This is the tools properties area.  Depending on the tool you have selected, this will show its options." ) );

    Layout();
}

void ToolbarPanel::ToggleTool( i32 selectedTool )
{
    for ( std::vector< wxBitmapToggleButton* >::const_iterator itr = m_ToolsButtons.begin(), end = m_ToolsButtons.end(); itr != end; ++itr )
    {
        (*itr)->SetValue( (*itr)->GetId() == selectedTool );
    }
}

void ToolbarPanel::OnToggleToolButton( wxCommandEvent& event )
{
    GetParent()->GetEventHandler()->ProcessEvent( wxCommandEvent ( wxEVT_COMMAND_MENU_SELECTED, event.GetId() ) );
}

void ToolbarPanel::SetSaveButtonState( bool enabled )
{
    if ( enabled )
    {
        m_SaveAllButton->Enable();
    }
    else
    {
        m_SaveAllButton->Disable();
    }
}
