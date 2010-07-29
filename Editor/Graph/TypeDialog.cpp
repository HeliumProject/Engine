#include "Precompile.h"
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TypeDialog.h"

///////////////////////////////////////////////////////////////////////////

TypeDialog::TypeDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	type = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( type, 0, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( bSizer3, 1, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ok = new wxButton( this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ok->SetDefault(); 
	bSizer4->Add( m_ok, 0, wxALL, 5 );
	
	m_cancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_cancel, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer4, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
}

TypeDialog::~TypeDialog()
{
}
