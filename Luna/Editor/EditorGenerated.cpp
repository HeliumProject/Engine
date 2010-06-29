///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "EditorGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

SubmitChangesPanel::SubmitChangesPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetMinSize( wxSize( 340,250 ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticText = new wxStaticText( this, ID_DEFAULT, wxT("This is your DEFAULT changelist (you may have other changelists):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticText->Wrap( -1 );
	bSizer5->Add( m_StaticText, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer5, 0, wxEXPAND, 5 );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ListBoxFiles = new wxListBox( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB|wxLB_SINGLE|wxLB_SORT ); 
	topSizer->Add( m_ListBoxFiles, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( topSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* captionSizer;
	captionSizer = new wxBoxSizer( wxVERTICAL );
	
	m_StaticQuesion = new wxStaticText( this, ID_DEFAULT, wxT("Would you like to commit the above changes to revision control?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticQuesion->Wrap( -1 );
	captionSizer->Add( m_StaticQuesion, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	mainSizer->Add( captionSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer4->Add( 0, 0, 1, wxALL, 5 );
	
	m_ButtonYes = new wxButton( this, wxID_YES, wxT("Yes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_ButtonYes, 0, wxALL, 5 );
	
	m_ButtonNo = new wxButton( this, wxID_NO, wxT("No - Launch P4V"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_ButtonNo, 0, wxALL, 5 );
	
	m_ButtonCancel = new wxButton( this, wxID_CANCEL, wxT("Canel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_ButtonCancel, 0, wxALL, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxALL, 5 );
	
	mainSizer->Add( bSizer4, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

SubmitChangesPanel::~SubmitChangesPanel()
{
}

ClassChooserPanel::ClassChooserPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* outerSizer;
	outerSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_DirectionsText = new wxTextCtrl( this, ID_DEFAULT, wxT("Choose a Runtime Class. \nThe dropdown shows classes alphabetically.\nThe tree shows classes in their class hierarchy. "), wxDefaultPosition, wxDefaultSize, wxTE_LEFT|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_WORDWRAP|wxNO_BORDER );
	m_DirectionsText->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_DirectionsText->SetMinSize( wxSize( -1,50 ) );
	
	topSizer->Add( m_DirectionsText, 1, wxALL|wxEXPAND, 5 );
	
	outerSizer->Add( topSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* choiceSizer;
	choiceSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Label = new wxStaticText( this, wxID_ANY, wxT("Runtime Class"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Label->Wrap( -1 );
	bSizer8->Add( m_Label, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxArrayString m_DropdownChoices;
	m_Dropdown = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DropdownChoices, 0 );
	m_Dropdown->SetSelection( 0 );
	bSizer8->Add( m_Dropdown, 1, wxALL|wxEXPAND, 5 );
	
	choiceSizer->Add( bSizer8, 1, wxEXPAND, 5 );
	
	outerSizer->Add( choiceSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* treeSizer;
	treeSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Tree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	treeSizer->Add( m_Tree, 1, wxALL|wxEXPAND, 5 );
	
	outerSizer->Add( treeSizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* lineSizer;
	lineSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticLine = new wxStaticLine( this, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	lineSizer->Add( m_StaticLine, 1, wxALL, 5 );
	
	outerSizer->Add( lineSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	
	buttonSizer->Add( 0, 0, 1, wxALL, 5 );
	
	m_Buttons = new wxStdDialogButtonSizer();
	m_ButtonsOK = new wxButton( this, wxID_OK );
	m_Buttons->AddButton( m_ButtonsOK );
	m_ButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_Buttons->AddButton( m_ButtonsCancel );
	m_Buttons->Realize();
	buttonSizer->Add( m_Buttons, 0, wxALL, 5 );
	
	outerSizer->Add( buttonSizer, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( outerSizer );
	this->Layout();
}

ClassChooserPanel::~ClassChooserPanel()
{
}
