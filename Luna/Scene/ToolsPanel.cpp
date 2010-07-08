#include "Precompile.h"
#include "ToolsPanel.h"
#include "SceneEditor.h"

#include "Application/UI/ArtProvider.h"
#include "Application/UI/AutoFlexSizer.h"

using namespace Luna;

ToolsPanel::ToolsPanel( SceneEditor* sceneEditor, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel( sceneEditor, id, pos, size, style )
, m_SceneEditor( sceneEditor )
{
    SetMinSize( size );

    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer* autoFlexSizer;
    autoFlexSizer = new Nocturnal::AutoFlexSizer();
    autoFlexSizer->SetFlexibleDirection( wxBOTH );
    autoFlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    for ( i32 i=0; i<ToolModes::Count; i++ )
    {
        ToolInfo* info = &ToolInfos[ i ];

        wxBitmapToggleButton* btn = new wxBitmapToggleButton( this, info->m_ID, wxArtProvider::GetBitmap( info->m_Bitmap, wxART_OTHER, wxSize( 32, 32 ) ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxBU_AUTODRAW );
        btn->SetToolTip( info->m_Description );

        // connect its event handler to us
        btn->Connect( btn->GetId(), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( ToolsPanel::OnToggleButton ), NULL, this );

        autoFlexSizer->Add( btn, 0, wxALL, 0 );
        m_Buttons.push_back( btn );
    }

    mainSizer->Add( autoFlexSizer, 0, wxEXPAND, 5 );

    m_Divider = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    mainSizer->Add( m_Divider, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );

    wxBoxSizer* bottomSizer;
    bottomSizer = new wxBoxSizer( wxVERTICAL );

    m_ScrollWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    m_ScrollWindow->SetScrollRate( 5, 5 );
    wxBoxSizer* scrollSizer;
    scrollSizer = new wxBoxSizer( wxVERTICAL );

    m_StaticText = new wxStaticText( m_ScrollWindow, wxID_ANY, wxT("+ Click for Properties"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StaticText->Wrap( -1 );
    scrollSizer->Add( m_StaticText, 0, wxALL, 0 );

    m_ScrollWindow->SetSizer( scrollSizer );
    m_ScrollWindow->Layout();
    scrollSizer->Fit( m_ScrollWindow );
    bottomSizer->Add( m_ScrollWindow, 1, wxEXPAND | wxALL, 0 );

    mainSizer->Add( bottomSizer, 1, wxEXPAND, 5 );

    SetSizer( mainSizer );
    Layout();
}

ToolsPanel::~ToolsPanel()
{
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
