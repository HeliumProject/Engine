#include "Precompile.h"

#include "ToolbarPanel.h"
#include "ArtProvider.h"

#include "Scene/SceneEditorIDs.h"

using namespace Luna;

ToolbarPanel::ToolbarPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: ToolbarPanelGenerated( parent, id, pos, size, style )
{
#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_NewSceneButton->SetBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::NewScene ) );
        m_OpenButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Open ) );
        m_SaveAllButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::SaveAll ) );
        m_CutButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Cut ) );
        m_CopyButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Copy ) );
        m_PasteButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Paste ) );
        m_DeleteButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Delete ) );
        m_UndoButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Undo ) );
        m_RedoButton->SetBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Redo ) );

        Layout();
        Thaw();
    }

    m_SaveAllButton->Disable();

    for ( i32 i=0; i < ToolModes::Count; ++i )
    {
        ToolInfo* info = &ToolInfos[ i ];

        wxBitmapToggleButton* btn = new wxBitmapToggleButton( m_ToolsPanel, info->m_ID, wxArtProvider::GetBitmap( info->m_Bitmap, wxART_OTHER, wxSize( 16, 16 ) ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxBU_AUTODRAW );
        btn->SetToolTip( info->m_Description );

        // connect its event handler to us
        btn->Connect( btn->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolbarPanel::OnToggleToolButton ), NULL, this );

        m_ToolsPanel->GetSizer()->Add( btn );
        m_ToolsButtons.push_back( btn );
    }
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
