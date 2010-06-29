///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "SceneLiveGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

InnerLiveLinkPanel::InnerLiveLinkPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxT("Sync") ), wxHORIZONTAL );
	
	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer( wxVERTICAL );
	
	m_ToggleLiveSync = new wxToggleButton( m_panel2, wxID_ANY, wxT("Scene"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer82->Add( m_ToggleLiveSync, 1, wxALL|wxEXPAND, 5 );
	
	m_ToggleLiveGameSync = new wxToggleButton( m_panel2, wxID_ANY, wxT("Game"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer82->Add( m_ToggleLiveGameSync, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer1->Add( bSizer82, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxVERTICAL );
	
	m_ToggleCameraSync = new wxToggleButton( m_panel2, wxID_ANY, wxT("Camera"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer91->Add( m_ToggleCameraSync, 1, wxALL|wxEXPAND, 5 );
	
	m_ToggleLightingSync = new wxToggleButton( m_panel2, wxID_ANY, wxT("Baked Lighting"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer91->Add( m_ToggleLightingSync, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer1->Add( bSizer91, 1, wxEXPAND, 5 );
	
	bSizer8->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxT("Build") ), wxVERTICAL );
	
	m_ButtonBuildAll = new wxButton( m_panel2, wxID_ANY, wxT("All Entities in the Scene"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_ButtonBuildAll, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonBuildCollisionDb = new wxButton( m_panel2, wxID_ANY, wxT("Collision Db"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_ButtonBuildCollisionDb, 1, wxALL|wxEXPAND, 5 );
	
	m_ButtonBuildStaticDb = new wxButton( m_panel2, wxID_ANY, wxT("Static Db"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonBuildStaticDb->SetToolTip( wxT("Enables Moby Shadows") );
	
	bSizer81->Add( m_ButtonBuildStaticDb, 1, wxALL|wxEXPAND, 5 );
	
	m_ButtonBuildLightingData = new wxButton( m_panel2, wxID_ANY, wxT("Lighting Data"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_ButtonBuildLightingData, 0, wxALL, 5 );
	
	sbSizer2->Add( bSizer81, 1, wxEXPAND, 5 );
	
	bSizer8->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( m_panel2, wxID_ANY, wxT("Link Status:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer9->Add( m_staticText2, 0, wxALL, 5 );
	
	m_TextLinkStatus = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_TextLinkStatus, 1, wxALL, 5 );
	
	bSizer8->Add( bSizer9, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxT("Screenshot") ), wxVERTICAL );
	
	m_ButtonScreenshot = new wxButton( m_panel2, wxID_ANY, wxT("Console Screenshot"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer6->Add( m_ButtonScreenshot, 1, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( sbSizer6, 0, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer8 );
	m_panel2->Layout();
	bSizer8->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, wxT("General"), true );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Simulated Mobys"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer4->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_SimulatedMobys = new wxListBox( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_SORT ); 
	bSizer4->Add( m_SimulatedMobys, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, wxEmptyString ), wxHORIZONTAL );
	
	m_ButtonAdd = new wxButton( m_panel1, wxID_ANY, wxT("Add Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer8->Add( m_ButtonAdd, 0, wxALL, 5 );
	
	m_ToggleSimulate = new wxToggleButton( m_panel1, wxID_ANY, wxT("Start Simulation"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer8->Add( m_ToggleSimulate, 1, wxALL, 5 );
	
	bSizer4->Add( sbSizer8, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_RadioAll = new wxRadioButton( m_panel1, wxID_ANY, wxT("All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_RadioAll, 0, wxALL, 5 );
	
	m_RadioSelected = new wxRadioButton( m_panel1, wxID_ANY, wxT("Selected"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_RadioSelected, 0, wxALL, 5 );
	
	m_RadioHighlighted = new wxRadioButton( m_panel1, wxID_ANY, wxT("Highlighted"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_RadioHighlighted, 0, wxALL, 5 );
	
	sbSizer7->Add( bSizer10, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonRemove = new wxButton( m_panel1, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_ButtonRemove, 1, wxALL, 5 );
	
	m_ButtonSync = new wxButton( m_panel1, wxID_ANY, wxT("Sync"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_ButtonSync, 1, wxALL, 5 );
	
	m_ButtonReset = new wxButton( m_panel1, wxID_ANY, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_ButtonReset, 1, wxALL, 5 );
	
	sbSizer7->Add( bSizer11, 0, wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer7, 0, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, wxT("Physics Sim"), false );
	
	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

InnerLiveLinkPanel::~InnerLiveLinkPanel()
{
}
