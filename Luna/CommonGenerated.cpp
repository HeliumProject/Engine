///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "CommonGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

RenameDialogBase::RenameDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticText = new wxStaticText( this, wxID_ANY, wxT("New name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticText->Wrap( -1 );
	topSizer->Add( m_StaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	topSizer->Add( m_Text, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	mainSizer->Add( topSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_OK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OK->SetDefault(); 
	bSizer3->Add( m_OK, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxTOP, 5 );
	
	m_Cancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_Cancel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_Text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( RenameDialogBase::OnTextEnter ), NULL, this );
}

RenameDialogBase::~RenameDialogBase()
{
	// Disconnect Events
	m_Text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( RenameDialogBase::OnTextEnter ), NULL, this );
}
