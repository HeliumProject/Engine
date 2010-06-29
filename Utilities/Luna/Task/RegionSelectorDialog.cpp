#include "Precompile.h"
#include "RegionSelectorDialog.h"

using namespace Luna;

BEGIN_EVENT_TABLE(RegionSelectorDialog, wxDialog)
EVT_BUTTON( wxID_OK, RegionSelectorDialog::OnOK )
EVT_BUTTON( wxID_CANCEL, RegionSelectorDialog::OnCancel )
EVT_BUTTON( wxID_CLOSE, RegionSelectorDialog::OnCancel )
END_EVENT_TABLE()

RegionSelectorDialog::RegionSelectorDialog( wxWindow* parent, const tstring& title, const tstring& msg, const std::set< tstring >& regionNames, tstring& selectedRegion, int id, wxPoint pos, wxSize size, int style ) 
  : wxDialog( parent, id, title.c_str(), pos, size, style )
  , m_SelectedRegion( selectedRegion )
{
  wxBoxSizer* bSizer1;
  bSizer1 = new wxBoxSizer( wxVERTICAL );

  wxFlexGridSizer* fgSizer1;
  fgSizer1 = new wxFlexGridSizer( 1, 1, 0, 0 );
  fgSizer1->AddGrowableCol( 0 );
  fgSizer1->AddGrowableRow( 0 );
  fgSizer1->SetFlexibleDirection( wxBOTH );

  m_listBox1 = new wxListBox( this, wxID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
  fgSizer1->Add( m_listBox1, 0, wxALL|wxEXPAND, 5 );

  bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

  wxBoxSizer* bSizer6;
  bSizer6 = new wxBoxSizer( wxVERTICAL );

  m_messageText = new wxStaticText( this, wxID_DEFAULT, msg.c_str(), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer6->Add( m_messageText, 0, wxALIGN_CENTER|wxALL, 5 );

  bSizer1->Add( bSizer6, 0, wxEXPAND, 5 );

  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer( wxHORIZONTAL );

  m_button2 = new wxButton( this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer3->Add( m_button2, 0, wxALIGN_CENTER|wxALL, 5 );

  m_button3 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer3->Add( m_button3, 0, wxALIGN_CENTER|wxALL, 5 );

  bSizer1->Add( bSizer3, 0, wxALIGN_CENTER, 5 );

  this->SetSizer( bSizer1 );

  for each ( const tstring& zone in regionNames )
  {
    m_listBox1->Append( zone.c_str() );
  }

  m_listBox1->Select( 0 );

  this->Layout();
}

RegionSelectorDialog::~RegionSelectorDialog()
{
}

void RegionSelectorDialog::OnOK( wxCommandEvent& event )
{
  m_SelectedRegion = m_listBox1->GetStringSelection();

  EndModal( wxID_OK );
}

void RegionSelectorDialog::OnCancel( wxCommandEvent& event )
{
  m_SelectedRegion.clear();

  EndModal( wxID_CANCEL );
}