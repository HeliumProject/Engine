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

    wxBitmapToggleButton* btn = new wxBitmapToggleButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT | wxBU_AUTODRAW );
    m_Buttons.push_back( btn );
    autoFlexSizer->Add( btn, 0, wxALL, 0 );

#if 0
    // setup
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsSelect, wxT("Select"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Select ), wxNullBitmap, wxT("Select items from the workspace"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsTranslate, wxT("Translate"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Translate ), wxNullBitmap, wxT("Translate items"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsRotate, wxT("Rotate"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Rotate ), wxNullBitmap, wxT("Rotate selected items"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsScale, wxT("Scale"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Scale ), wxNullBitmap, wxT("Scale selected items"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsDuplicate, wxT("Duplicate"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Duplicate ), wxNullBitmap, wxT("Duplicate the selected object numerous times"));
    m_ToolsToolBar->AddSeparator();
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsLocatorCreate, wxT("Create Locator"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ), wxNullBitmap, wxT("Place locator objects (such as bug locators)"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsVolumeCreate, wxT("Create Volume"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ), wxNullBitmap, wxT("Place volume objects (items for setting up gameplay)"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsEntityCreate, wxT("Create Entity"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ), wxNullBitmap, wxT("Place entity objects (such as art instances or characters)"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsCurveCreate, wxT("Create Curve"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ), wxNullBitmap, wxT("Create curve objects (Linear, B-Spline, or Catmull-Rom Spline)"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsCurveEdit, wxT("Edit Curve"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ), wxNullBitmap, wxT("Edit created curves (modify or create/delete control points)"));
    m_ToolsToolBar->AddCheckTool(SceneEditorIDs::ID_ToolsNavMesh, wxT("Edit NavMesh"), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ), wxNullBitmap, wxT("Create NavMesh or add new verts and tris"));
    m_ToolsToolBar->Realize();
    m_ToolsToolBar->ToggleTool( SceneEditorIDs::ID_ToolsSelect, true );
#endif

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

void ToolsPanel::ToggleTool( i32 selectedTool )
{
    for ( std::vector<wxBitmapToggleButton*>::const_iterator itr = m_Buttons.begin(), end = m_Buttons.end(); itr != end; ++itr )
    {
        (*itr)->SetValue( (*itr)->GetId() == selectedTool );
    }
}
