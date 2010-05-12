#include "Precompile.h"

#include "TaskOptionsDialog.h"

// Event table
BEGIN_EVENT_TABLE( TaskOptionsDialog, wxDialog )
EVT_BUTTON( wxID_OK, TaskOptionsDialog::OnOK )
EVT_BUTTON( wxID_CANCEL, TaskOptionsDialog::OnCancel )
EVT_BUTTON( wxID_CLOSE, TaskOptionsDialog::OnCancel )
END_EVENT_TABLE()

TaskOptionsDialog::TaskOptionsDialog( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
  this->SetSizeHints( wxDefaultSize, wxDefaultSize );

  wxBoxSizer* bSizer2;
  bSizer2 = new wxBoxSizer( wxVERTICAL );

  wxFlexGridSizer* fgSizer2;
  fgSizer2 = new wxFlexGridSizer( 0, 0, 0, 0 );
  fgSizer2->AddGrowableCol( 0 );
  fgSizer2->AddGrowableRow( 0 );

  m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN|wxTAB_TRAVERSAL );
  fgSizer2->Add( m_panel2, 1, wxALL|wxEXPAND, 5 );

  bSizer2->Add( fgSizer2, 1, wxEXPAND, 5 );

  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer( wxHORIZONTAL );

  m_button3 = new wxButton( this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer3->Add( m_button3, 0, wxALL, 5 );

  m_button4 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer3->Add( m_button4, 0, wxALL, 5 );

  bSizer2->Add( bSizer3, 0, wxALIGN_CENTER, 5 );

  this->SetSizer( bSizer2 );
  this->Layout();
}

wxPanel* TaskOptionsDialog::GetPanel()
{
  return m_panel2;
}

void TaskOptionsDialog::OnOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}

void TaskOptionsDialog::OnCancel( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}