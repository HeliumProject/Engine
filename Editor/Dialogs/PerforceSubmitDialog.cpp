#include "Precompile.h"
#include "PerforceSubmitDialog.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
PerforceSubmitDialog::PerforceSubmitDialog( wxWindow* parent, int id, int changelistNumber, const tstring& description, wxString title )
 : wxDialog( parent, id, title, wxDefaultPosition, wxSize( 560,410 ), wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX )
{
  m_Panel = new PerforceSubmitPanel( this, wxID_ANY, changelistNumber, description );

  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );

  this->SetSizer( sizer );
  sizer->Fit( this );

	this->SetSizeHints( 560, 410 );
	this->Centre( wxBOTH );
}

///////////////////////////////////////////////////////////////////////////////
PerforceSubmitDialog::~PerforceSubmitDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
int PerforceSubmitDialog::ShowModal()
{
  int result = wxID_CANCEL;

  result = wxDialog::ShowModal();

  return result;
}

///////////////////////////////////////////////////////////////////////////////
void PerforceSubmitDialog::EndModal( int retCode )
{
  m_Panel->CommitChanges();

  wxDialog::EndModal( retCode );
}

///////////////////////////////////////////////////////////////////////////////
PerforceSubmitPanel* PerforceSubmitDialog::GetPanel() const
{
  return m_Panel; 
}
