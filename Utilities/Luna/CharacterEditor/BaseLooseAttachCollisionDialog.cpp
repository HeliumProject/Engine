///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "BaseLooseAttachCollisionDialog.h"

///////////////////////////////////////////////////////////////////////////

BaseLooseAttachCollisionDialog::BaseLooseAttachCollisionDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->AddGrowableRow( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_textUnselectedCollisionVolumes = new wxStaticText( this, wxID_ANY, wxT("Unselected Collision Volumes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textUnselectedCollisionVolumes->Wrap( -1 );
	fgSizer4->Add( m_textUnselectedCollisionVolumes, 0, wxALL, 5 );
	
	m_listUnselectedCollisionVolumes = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_HSCROLL|wxLB_SORT ); 
	fgSizer4->Add( m_listUnselectedCollisionVolumes, 1, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAddAll = new wxButton( this, wxID_ANY, wxT(">>"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonAddAll, 0, wxALL, 5 );
	
	m_buttonAddSelected = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAddSelected->SetToolTip( wxT("Add Selected") );
	
	bSizer4->Add( m_buttonAddSelected, 0, wxALL, 5 );
	
	m_buttonRemoveSelected = new wxButton( this, wxID_ANY, wxT("<"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonRemoveSelected->SetToolTip( wxT("Remove Selected") );
	
	bSizer4->Add( m_buttonRemoveSelected, 0, wxALL, 5 );
	
	m_buttonRemoveAll = new wxButton( this, wxID_ANY, wxT("<<"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonRemoveAll->SetToolTip( wxT("Remove All") );
	
	bSizer4->Add( m_buttonRemoveAll, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer5->AddGrowableCol( 0 );
	fgSizer5->AddGrowableRow( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_textSelectedCollisionVolumes = new wxStaticText( this, wxID_ANY, wxT("Selected Collision Volumes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textSelectedCollisionVolumes->Wrap( -1 );
	fgSizer5->Add( m_textSelectedCollisionVolumes, 0, wxALL, 5 );
	
	m_listSelectedCollisionVolumes = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED|wxLB_HSCROLL|wxLB_SORT ); 
	fgSizer5->Add( m_listSelectedCollisionVolumes, 1, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	fgSizer2->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	fgSizer2->Add( m_sdbSizer1, 1, wxEXPAND|wxALIGN_BOTTOM, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();
	
	// Connect Events
	m_buttonAddAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnAddAll ), NULL, this );
	m_buttonAddSelected->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnAddSelected ), NULL, this );
	m_buttonRemoveSelected->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnRemoveSelected ), NULL, this );
	m_buttonRemoveAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnRemoveAll ), NULL, this );
}

BaseLooseAttachCollisionDialog::~BaseLooseAttachCollisionDialog()
{
	// Disconnect Events
	m_buttonAddAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnAddAll ), NULL, this );
	m_buttonAddSelected->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnAddSelected ), NULL, this );
	m_buttonRemoveSelected->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnRemoveSelected ), NULL, this );
	m_buttonRemoveAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BaseLooseAttachCollisionDialog::OnRemoveAll ), NULL, this );
}
