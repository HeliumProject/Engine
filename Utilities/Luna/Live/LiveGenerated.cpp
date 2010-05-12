///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "LiveGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

LivePanel::LivePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_listbook1 = new wxListbook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_GeneralPanel = new wxPanel( m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_ButtonStartUberview = new wxButton( m_GeneralPanel, wxID_ANY, wxT("Start Uberview"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_ButtonStartUberview, 0, wxALL|wxEXPAND, 5 );
	
	m_ButtonResetConnection = new wxButton( m_GeneralPanel, wxID_ANY, wxT("Reset Connection"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_ButtonResetConnection, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Viewer Status"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer51->Add( m_staticText2, 0, wxALL, 5 );
	
	m_StatusTextControl = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	bSizer51->Add( m_StatusTextControl, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer51, 0, wxALIGN_LEFT, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText21 = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Last Backtrace"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer7->Add( m_staticText21, 0, wxALL, 5 );
	
	m_BackTraceText = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
	bSizer7->Add( m_BackTraceText, 1, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( bSizer7, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer6, 1, wxEXPAND, 0 );
	
	m_staticline1 = new wxStaticLine( m_GeneralPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_GeneralPanel->SetSizer( bSizer1 );
	m_GeneralPanel->Layout();
	bSizer1->Fit( m_GeneralPanel );
	m_listbook1->AddPage( m_GeneralPanel, wxT("General"), false );
	m_OptionsPanel = new wxPanel( m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );
	
	m_AutoStartUberviewCheckbox = new wxCheckBox( m_OptionsPanel, wxID_ANY, wxT("Auto-start Uberview"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AutoStartUberviewCheckbox->SetValue(true);
	
	bSizer61->Add( m_AutoStartUberviewCheckbox, 0, wxALL, 5 );
	
	m_ForceAutoStartUberviewCheckbox = new wxCheckBox( m_OptionsPanel, wxID_ANY, wxT("Never ask"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ForceAutoStartUberviewCheckbox->SetValue(true);
	
	bSizer61->Add( m_ForceAutoStartUberviewCheckbox, 0, wxLEFT, 20 );
	
	m_AutoDisplayWindowCheckbox = new wxCheckBox( m_OptionsPanel, wxID_ANY, wxT("Auto-display This Window"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AutoDisplayWindowCheckbox->SetValue(true);
	
	bSizer61->Add( m_AutoDisplayWindowCheckbox, 0, wxALL, 5 );
	
	bSizer11->Add( bSizer61, 1, wxEXPAND, 5 );
	
	m_staticline11 = new wxStaticLine( m_OptionsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer11->Add( m_staticline11, 0, wxEXPAND | wxALL, 5 );
	
	m_OptionsPanel->SetSizer( bSizer11 );
	m_OptionsPanel->Layout();
	bSizer11->Fit( m_OptionsPanel );
	m_listbook1->AddPage( m_OptionsPanel, wxT("Options"), true );
	m_TargetsPanel = new wxPanel( m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_TargetList = new wxListBox( m_TargetsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_TargetList, 1, wxALL|wxEXPAND, 5 );
	
	m_ButtonRefreshTargetList = new wxButton( m_TargetsPanel, wxID_ANY, wxT("Refresh Target List"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_ButtonRefreshTargetList, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_TargetsPanel->SetSizer( bSizer5 );
	m_TargetsPanel->Layout();
	bSizer5->Fit( m_TargetsPanel );
	m_listbook1->AddPage( m_TargetsPanel, wxT("Targets"), false );
	m_LightingPanel = new wxPanel( m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxVERTICAL );
	
	m_LightingEnvironmentList = new wxListBox( m_LightingPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer52->Add( m_LightingEnvironmentList, 1, wxALL|wxEXPAND, 5 );
	
	m_ButtonRefreshLightingList = new wxButton( m_LightingPanel, wxID_ANY, wxT("Refresh List"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_ButtonRefreshLightingList, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_FilterText = new wxTextCtrl( m_LightingPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_FilterText, 0, wxALL, 2 );
	
	m_LightingPanel->SetSizer( bSizer52 );
	m_LightingPanel->Layout();
	bSizer52->Fit( m_LightingPanel );
	m_listbook1->AddPage( m_LightingPanel, wxT("Lighting"), false );
	#ifndef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook1ListView = m_listbook1->GetListView();
	long m_listbook1Flags = m_listbook1ListView->GetWindowStyleFlag();
	m_listbook1Flags = ( m_listbook1Flags & ~wxLC_ICON ) | wxLC_SMALL_ICON;
	m_listbook1ListView->SetWindowStyleFlag( m_listbook1Flags );
	#endif
	
	bSizer4->Add( m_listbook1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
}

LivePanel::~LivePanel()
{
}
