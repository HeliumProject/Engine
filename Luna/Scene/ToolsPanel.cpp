#include "Precompile.h"
#include "ToolsPanel.h"
#include "SceneEditor.h"

#include "Application/UI/ArtProvider.h"
#include <wx/wrapsizer.h>

using namespace Luna;

ToolsPanel::ToolsPanel( SceneEditor* sceneEditor, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel( sceneEditor, id, pos, size, style )
, m_SceneEditor( sceneEditor )
{
    SetMinSize( size );
}

void ToolsPanel::Create( wxScrolledWindow* properties )
{
    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer( wxVERTICAL );

    wxWrapSizer* wrapSizer = new wxWrapSizer();

    for ( i32 i=0; i<ToolModes::Count; i++ )
    {
        ToolInfo* info = &ToolInfos[ i ];

        wxBitmapToggleButton* btn = new wxBitmapToggleButton( this, info->m_ID, wxArtProvider::GetBitmap( info->m_Bitmap, wxART_OTHER, wxSize( 16, 16 ) ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxBU_AUTODRAW );
        btn->SetToolTip( info->m_Description );

        // connect its event handler to us
        btn->Connect( btn->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolsPanel::OnToggleButton ), NULL, this );

        wrapSizer->Add( btn, 0, wxALL, 0 );
        m_Buttons.push_back( btn );
    }

    mainSizer->Add( wrapSizer, 0, wxEXPAND, 5 );

    m_Divider = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    mainSizer->Add( m_Divider, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );

    m_Properties = properties;
    m_Properties->Layout();
    mainSizer->Add( m_Properties, 1, wxEXPAND | wxALL, 0 );

    SetSizer( mainSizer );
    Layout();
}

void ToolsPanel::OnToggleButton( wxCommandEvent& e )
{
    m_SceneEditor->GetEventHandler()->ProcessEvent( wxCommandEvent ( wxEVT_COMMAND_MENU_SELECTED, e.GetId() ) );
}

void ToolsPanel::ToggleTool( i32 selectedTool )
{
    for ( std::vector<wxBitmapToggleButton*>::const_iterator itr = m_Buttons.begin(), end = m_Buttons.end(); itr != end; ++itr )
    {
        (*itr)->SetValue( (*itr)->GetId() == selectedTool );
    }
}
