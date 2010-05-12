#include "Precompile.h"
#include "ZoneSelectorDialog.h"

using namespace Luna;

namespace ButtonIDs
{
  enum ButtonID
  {
    All
  };
}
typedef ButtonIDs::ButtonID ButtonID;

BEGIN_EVENT_TABLE(ZoneSelectorDialog, wxDialog)
EVT_BUTTON( wxID_OK, ZoneSelectorDialog::OnOK )
EVT_BUTTON( wxID_CANCEL, ZoneSelectorDialog::OnCancel )
EVT_BUTTON( wxID_CLOSE, ZoneSelectorDialog::OnCancel )
EVT_BUTTON( ButtonIDs::All, ZoneSelectorDialog::OnAll )
END_EVENT_TABLE()

ZoneSelectorDialog::ZoneSelectorDialog( wxWindow* parent, const std::string& title, const std::string& msg, const V_string& zoneNames, S_u32& selectedZones, int id, wxPoint pos, wxSize size, int style ) 
  : wxDialog( parent, id, title.c_str(), pos, size, style )
  , m_SelectedZones( selectedZones )
  , m_NumZones( (u32)zoneNames.size() )
{
  wxBoxSizer* bSizer1;
  bSizer1 = new wxBoxSizer( wxVERTICAL );

  wxFlexGridSizer* fgSizer1;
  fgSizer1 = new wxFlexGridSizer( 1, 1, 0, 0 );
  fgSizer1->AddGrowableCol( 0 );
  fgSizer1->AddGrowableRow( 0 );
  fgSizer1->SetFlexibleDirection( wxBOTH );

  m_listBox1 = new wxListBox( this, wxID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED ); 
  fgSizer1->Add( m_listBox1, 0, wxALL|wxEXPAND, 5 );

  bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

  wxBoxSizer* bSizer6;
  bSizer6 = new wxBoxSizer( wxVERTICAL );

  m_messageText = new wxStaticText( this, wxID_DEFAULT, wxT(msg.c_str()), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer6->Add( m_messageText, 0, wxALIGN_CENTER|wxALL, 5 );

  bSizer1->Add( bSizer6, 0, wxEXPAND, 5 );

  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer( wxHORIZONTAL );

  m_button2 = new wxButton( this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer3->Add( m_button2, 0, wxALIGN_CENTER|wxALL, 5 );

  m_button3 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer3->Add( m_button3, 0, wxALIGN_CENTER|wxALL, 5 );

  m_button1 = new wxButton( this, ButtonIDs::All, wxT("All"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer3->Add( m_button1, 0, wxALIGN_CENTER|wxALL, 5 );

  bSizer1->Add( bSizer3, 0, wxALIGN_CENTER, 5 );

  this->SetSizer( bSizer1 );

  for each ( const std::string& zone in zoneNames )
  {
    m_listBox1->Append( wxT( zone.c_str() ) );
  }

  for each ( const u32 zone in selectedZones )
  {
    m_listBox1->Select( zone );
  }

  this->Layout();
}

ZoneSelectorDialog::~ZoneSelectorDialog()
{
}

void ZoneSelectorDialog::OnOK( wxCommandEvent& event )
{
  m_SelectedZones.clear();

  wxArrayInt selections;
  m_listBox1->GetSelections( selections );

  for ( u32 i = 0; i < selections.size(); ++i )
  {
    m_SelectedZones.insert( selections[ i ] );
  }

  EndModal( wxID_OK );
}

void ZoneSelectorDialog::OnCancel( wxCommandEvent& event )
{
  m_SelectedZones.clear();

  EndModal( wxID_CANCEL );
}

void ZoneSelectorDialog::OnAll( wxCommandEvent& event )
{
  for ( u32 i = 0; i < m_NumZones; ++i )
  {
    m_SelectedZones.insert( i );
  }

  EndModal( ButtonIDs::All );
}