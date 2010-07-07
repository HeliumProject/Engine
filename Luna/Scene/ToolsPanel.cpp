#include "Precompile.h"
#include "ToolsPanel.h"

#include "CameraModeButton.h"
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
	
#if ( 1 )

	m_button10 = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ) );
	autoFlexSizer->Add( m_button10, 0, wxALL, 0 );

	m_button11 = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ) );
	autoFlexSizer->Add( m_button11, 0, wxALL, 0 );
	
	m_button12 = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ) );
	autoFlexSizer->Add( m_button12, 0, wxALL, 0 );
	
	m_button13 = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ) );
	autoFlexSizer->Add( m_button13, 0, wxALL, 0 );
	
	m_button14 = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ) );
	autoFlexSizer->Add( m_button14, 0, wxALL, 0 );
	
	m_button15 = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ) );
	autoFlexSizer->Add( m_button15, 0, wxALL, 0 );
	
	m_button16 = new wxBitmapButton( this, wxID_ANY, wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown, wxART_OTHER, wxSize( 32, 32 ) ) );
	autoFlexSizer->Add( m_button16, 0, wxALL, 0 );

  autoFlexSizer->Add( new CameraModeButton( this, m_SceneEditor ), 0, wxALL, 0 );

#elif ( 0 )

	m_button10 = new wxButton( this, wxID_ANY, wxT("a"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	autoFlexSizer->Add( m_button10, 0, wxALL, 5 );

	m_button11 = new wxButton( this, wxID_ANY, wxT("b"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	autoFlexSizer->Add( m_button11, 0, wxALL, 5 );
	
	m_button12 = new wxButton( this, wxID_ANY, wxT("c"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	autoFlexSizer->Add( m_button12, 0, wxALL, 5 );
	
	m_button13 = new wxButton( this, wxID_ANY, wxT("dddddd"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	autoFlexSizer->Add( m_button13, 0, wxALL, 5 );
	
	m_button14 = new wxButton( this, wxID_ANY, wxT("e"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	autoFlexSizer->Add( m_button14, 0, wxALL, 5 );
	
	m_button15 = new wxButton( this, wxID_ANY, wxT("f"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	autoFlexSizer->Add( m_button15, 0, wxALL, 5 );
	
	m_button16 = new wxButton( this, wxID_ANY, wxT("g"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	autoFlexSizer->Add( m_button16, 0, wxALL, 5 );
	
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
