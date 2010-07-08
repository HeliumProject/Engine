#include "Precompile.h"

#include "HelpPanel.h"

using namespace Luna;

HelpPanel::HelpPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, id, pos, size, style, name )
{
    SetHelpText( TXT( "This is the help window.  It will update based on the control the mouse is over." ) );

	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );

    m_HelpRichText = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,200 ), 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS|wxRE_READONLY );
	m_HelpRichText->SetMinSize( wxSize( 200,200 ) );

	sizer->Add( m_HelpRichText, 1, wxEXPAND, 5 );
	
	this->SetSizer( sizer );
	this->Layout();
}

HelpPanel::~HelpPanel()
{
    delete m_HelpRichText;
}

void HelpPanel::SetText( const tchar* text )
{
    m_HelpRichText->SetValue( text );
}