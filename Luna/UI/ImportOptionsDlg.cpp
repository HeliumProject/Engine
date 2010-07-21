#include "Precompile.h"
#include "ImportOptionsDlg.h"

// Using
using namespace Luna;

// Static event table
BEGIN_EVENT_TABLE( ImportOptionsDlg, wxDialog )
  EVT_INIT_DIALOG( ImportOptionsDlg::OnInit )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ImportOptionsDlg::ImportOptionsDlg( wxWindow* parent, bool& update )
: wxDialog( parent, -1, wxT( "Import" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, wxT( "LunaImportOptionsDialog" ) )
, m_UpdateValidator( &update )
, m_UpdateCheckbox( this, -1, wxT( "Update state of existing objects only" ), wxDefaultPosition, wxDefaultSize, 0, m_UpdateValidator, wxT( "Checkbox_Update" ) )
{
  m_UpdateValidator.SetWindow( &m_UpdateCheckbox );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ImportOptionsDlg::~ImportOptionsDlg()
{
}

///////////////////////////////////////////////////////////////////////////////
// Called before the dialog is shown and initializes all the controls.
// 
void ImportOptionsDlg::OnInit( wxInitDialogEvent& event )
{
  wxBoxSizer* topmostSizer = new wxBoxSizer( wxVERTICAL );

  // Message explaining the options
  wxString msg = wxT( "Options" );
  wxStaticBoxSizer* groupBox = new wxStaticBoxSizer( wxHORIZONTAL, this, msg );
  wxBoxSizer* innerGroupBoxSizer = new wxBoxSizer( wxVERTICAL );

  // Checkboxes inside of the group box
  innerGroupBoxSizer->Add( &m_UpdateCheckbox, wxSizerFlags().Border( wxALL, 10 ) );
  innerGroupBoxSizer->Layout();
  groupBox->Add( innerGroupBoxSizer );
  groupBox->Layout();

  // Ok/Cancel buttons at the bottom
  wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );
  buttonSizer->Add( CreateButtonSizer( wxOK ) );
  buttonSizer->Add( 40, 1, 0, wxEXPAND );
  buttonSizer->Add( CreateButtonSizer( wxCANCEL ) );
  buttonSizer->Layout();

  // Add all the controls in the proper order
  topmostSizer->Add( groupBox, wxSizerFlags().Border( wxALL, 10 ) );
  topmostSizer->Add( buttonSizer, wxSizerFlags().Border( wxBOTTOM | wxLEFT | wxRIGHT, 10 ).Center() );
  topmostSizer->Layout();
  
  // Assign the topmost sizer to this dialog, otherwise things don't layout correctly
  SetSizer( topmostSizer );
  topmostSizer->SetSizeHints( this );

  // Let the base class do its initialization
  event.Skip();
}
