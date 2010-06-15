///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "Luna/UI/Button.h"
#include "Luna/UI/MenuButton.h"

#include "LunaGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

DrawerFrameBase::DrawerFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 100,100 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_EmbeddedWindowSizer = new wxBoxSizer( wxVERTICAL );
	
	mainSizer->Add( m_EmbeddedWindowSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

DrawerFrameBase::~DrawerFrameBase()
{
}

ToolsPanelGenerated::ToolsPanelGenerated( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 170,325 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonCreateEntity = new Luna::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCreateEntity, 0, wxALL, 0 );
	
	m_ButtonCreateVolume = new Luna::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCreateVolume, 0, wxALL, 0 );
	
	m_ButtonCreateClue = new Luna::Button( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCreateClue, 0, wxALL, 0 );
	
	m_ButtonCurve = new Luna::MenuButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW );
	bSizer10->Add( m_ButtonCurve, 0, wxALL, 0 );
	
	bSizer31->Add( bSizer10, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer31, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Class:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer21->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("agorian_beast"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer21->Add( m_staticText2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bpButton445 = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer21->Add( m_bpButton445, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );
	
	bSizer221->Add( bSizer21, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	m_CurrentAssetThumbnail = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_CurrentAssetThumbnail->SetMinSize( wxSize( 64,64 ) );
	
	bSizer29->Add( m_CurrentAssetThumbnail, 1, wxALL|wxEXPAND, 5 );
	
	bSizer221->Add( bSizer29, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Select New Class"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_button1, 0, wxALL|wxEXPAND, 1 );
	
	bSizer221->Add( bSizer22, 0, wxEXPAND, 0 );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, wxT("Show Pointer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox1->SetValue(true);
	
	bSizer23->Add( m_checkBox1, 0, wxALL, 5 );
	
	bSizer221->Add( bSizer23, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer231;
	bSizer231 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox11 = new wxCheckBox( this, wxID_ANY, wxT("Show Bounds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox11->SetValue(true);
	
	bSizer231->Add( m_checkBox11, 0, wxALL, 5 );
	
	bSizer221->Add( bSizer231, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer232;
	bSizer232 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox12 = new wxCheckBox( this, wxID_ANY, wxT("Show Geometry"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer232->Add( m_checkBox12, 0, wxALL, 5 );
	
	bSizer221->Add( bSizer232, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("Snap"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer30->Add( m_button2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Randomize"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer30->Add( m_button3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
	
	bSizer221->Add( bSizer30, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer221, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

ToolsPanelGenerated::~ToolsPanelGenerated()
{
}
