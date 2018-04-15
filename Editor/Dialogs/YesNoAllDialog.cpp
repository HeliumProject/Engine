#include "Precompile.h"
#include "Editor/Dialogs/YesNoAllDialog.h"

using namespace Helium;
using namespace Helium::Editor;

// Static GUI event table
BEGIN_EVENT_TABLE( YesNoAllDialog, wxDialog )
EVT_BUTTON( wxID_ANY, YesNoAllDialog::OnButtonClick )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// 
// 
YesNoAllDialog::YesNoAllDialog( wxWindow* parent, const std::string& title, const std::string& msg, int id, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title.c_str(), pos, size, style )
{
	wxBoxSizer* sizerMain;
	sizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerTop;
	sizerTop = new wxBoxSizer( wxVERTICAL );
	
	m_Text = new wxTextCtrl( this, wxID_ANY, msg.c_str(), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxNO_BORDER );
	m_Text->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	sizerTop->Add( m_Text, 1, wxALL|wxEXPAND, 5 );
	
	sizerMain->Add( sizerTop, 1, wxEXPAND, 5 );
	
	wxBoxSizer* sizerBottom;
	sizerBottom = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* sizerLine;
	sizerLine = new wxBoxSizer( wxHORIZONTAL );
	
	m_Separator = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	sizerLine->Add( m_Separator, 1, wxALL, 5 );
	
	sizerBottom->Add( sizerLine, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizerButtons;
	sizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonYes = new wxButton( this, wxID_YES, wxT("Yes"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	sizerButtons->Add( m_ButtonYes, 0, wxALL, 5 );
	
	m_ButtonYesAll = new wxButton( this, wxID_YESTOALL, wxT("Yes All"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	sizerButtons->Add( m_ButtonYesAll, 0, wxALL, 5 );
	
	m_ButtonNo = new wxButton( this, wxID_NO, wxT("No"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	sizerButtons->Add( m_ButtonNo, 0, wxALL, 5 );
	
	m_ButtonNoAll = new wxButton( this, wxID_NOTOALL, wxT("No All"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	sizerButtons->Add( m_ButtonNoAll, 0, wxALL, 5 );
	
	m_ButtonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	sizerButtons->Add( m_ButtonCancel, 0, wxALL, 5 );
	
	sizerBottom->Add( sizerButtons, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	sizerMain->Add( sizerBottom, 0, wxEXPAND, 5 );
	
	this->SetSizer( sizerMain );
	this->Layout();

  m_ButtonYes->SetFocus();
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
YesNoAllDialog::~YesNoAllDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void YesNoAllDialog::SetButtonToolTip( int buttonId, const std::string& tooltip )
{

  switch ( buttonId )
  {
  case wxID_YES:
	  m_ButtonYes->SetToolTip( tooltip.c_str() );
    break;

  case wxID_YESTOALL:
	  m_ButtonYesAll->SetToolTip( tooltip.c_str() );
    break;

  case wxID_NO:
	  m_ButtonNo->SetToolTip( tooltip.c_str() );
    break;

  case wxID_NOTOALL:
	  m_ButtonNoAll->SetToolTip( tooltip.c_str() );
    break;

  case wxID_CANCEL:
	  m_ButtonCancel->SetToolTip( tooltip.c_str() );
    break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void YesNoAllDialog::OnButtonClick( wxCommandEvent& args )
{
  EndModal( args.GetId() );
}
