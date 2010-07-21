#include "Precompile.h"

#include "ToolsPanel.h"
#include "ArtProvider.h"

#include "Scene/SceneEditorIDs.h"

#include <wx/wrapsizer.h>
#include <wx/sizer.h>

using namespace Luna;

ToolsPanel::ToolsPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: ToolsPanelGenerated( parent, id, pos, size, style )
{
    SetMinSize( size );
}

void ToolsPanel::Create( wxScrolledWindow* propertiesScrolledWindow )
{
    wxWrapSizer* wrapSizer = new wxWrapSizer();

    for ( i32 i=0; i<ToolModes::Count; i++ )
    {
        ToolInfo* info = &ToolInfos[ i ];

        wxBitmapToggleButton* btn = new wxBitmapToggleButton( m_ToolsContainerPanel, info->m_ID, wxArtProvider::GetBitmap( info->m_Bitmap, wxART_OTHER, wxSize( 16, 16 ) ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxBU_AUTODRAW );
        btn->SetToolTip( info->m_Description );

        // connect its event handler to us
        btn->Connect( btn->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolsPanel::OnToggleButton ), NULL, this );

        wrapSizer->Add( btn, 0, wxALL, 0 );
        m_Buttons.push_back( btn );
    }

    wrapSizer->InformFirstDirection( wxHORIZONTAL, m_ToolsContainerPanel->GetSize().GetX(), m_ToolsContainerPanel->GetSize().GetY() );
    m_ToolsContainerPanel->SetSizer( wrapSizer );
    m_ToolsContainerPanel->Layout();

    wxBoxSizer* propertiesSizer = new wxBoxSizer( wxVERTICAL );
    m_PropertiesScrolledWindow = propertiesScrolledWindow;
    m_PropertiesScrolledWindow->Layout();
    propertiesSizer->Add( m_PropertiesScrolledWindow, 1, wxEXPAND | wxALL, 0 );
    m_PropertiesPanel->SetSizer( propertiesSizer );

    Layout();
}

void ToolsPanel::OnToggleButton( wxCommandEvent& e )
{
    GetParent()->GetEventHandler()->ProcessEvent( wxCommandEvent ( wxEVT_COMMAND_MENU_SELECTED, e.GetId() ) );
}

void ToolsPanel::ToggleTool( i32 selectedTool )
{
    for ( std::vector<wxBitmapToggleButton*>::const_iterator itr = m_Buttons.begin(), end = m_Buttons.end(); itr != end; ++itr )
    {
        (*itr)->SetValue( (*itr)->GetId() == selectedTool );
    }
}
