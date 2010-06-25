#include "Dialog.h"
#include "Application/UI/PerforceUI/PerforceUIPanel.h"

using namespace PerforceUI;

///////////////////////////////////////////////////////////////////////////////
Dialog::Dialog
( 
 wxWindow* parent,
 int id,
 int changelistNumber,
 const tstring& description,
 wxString title
 )
 : wxDialog( parent, id, title, wxDefaultPosition, wxSize( 560,410 ), wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX )
{
  m_Panel = new Panel( this, wxID_ANY, changelistNumber, description );

  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );

  this->SetSizer( sizer );
  sizer->Fit( this );

	this->SetSizeHints( 560, 410 );
	this->Centre( wxBOTH );
}

///////////////////////////////////////////////////////////////////////////////
Dialog::~Dialog()
{
}

///////////////////////////////////////////////////////////////////////////////
int Dialog::ShowModal()
{
  int result = wxID_CANCEL;

  result = __super::ShowModal();

  return result;
}

///////////////////////////////////////////////////////////////////////////////
void Dialog::EndModal( int retCode )
{
  m_Panel->CommitChanges();

  __super::EndModal( retCode );
}

///////////////////////////////////////////////////////////////////////////////
Panel* Dialog::GetPanel() const
{
  return m_Panel; 
}
