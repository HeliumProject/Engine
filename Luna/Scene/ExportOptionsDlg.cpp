#include "Precompile.h"
#include "ExportOptionsDlg.h"

// Using
using namespace Luna;

// Static event table
BEGIN_EVENT_TABLE( ExportOptionsDlg, wxDialog )
  EVT_INIT_DIALOG( ExportOptionsDlg::OnInit )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ExportOptionsDlg::ExportOptionsDlg( wxWindow* parent, bool& dependencies, bool& hierarchy )
: wxDialog( parent, -1, wxT( "Export" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, wxT( "LunaExportOptionsDialog" ) )
, m_DependencyValidator( &dependencies )
, m_DependencyCheckbox( this, -1, wxT( "Include dependencies (layers, etc...)" ), wxDefaultPosition, wxDefaultSize, 0, m_DependencyValidator, wxT( "Checkbox_Dependency" ) )
, m_HierarchyValidator( &hierarchy )
, m_HierarchyCheckbox( this, -1, wxT( "Include hierarchy (parents and children)" ), wxDefaultPosition, wxDefaultSize, 0, m_HierarchyValidator, wxT( "Checkbox_Hierarchy" ) )
{
  m_DependencyValidator.SetWindow( &m_DependencyCheckbox );
  m_HierarchyValidator.SetWindow( &m_HierarchyCheckbox );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ExportOptionsDlg::~ExportOptionsDlg()
{
}

///////////////////////////////////////////////////////////////////////////////
// Called before the dialog is shown and initializes all the controls.
// 
void ExportOptionsDlg::OnInit( wxInitDialogEvent& event )
{
  wxBoxSizer* topmostSizer = new wxBoxSizer( wxVERTICAL );

  // Message explaining the options
  wxString msg = wxT( "Options" );
  wxStaticBoxSizer* groupBox = new wxStaticBoxSizer( wxHORIZONTAL, this, msg );
  wxBoxSizer* innerGroupBoxSizer = new wxBoxSizer( wxVERTICAL );

  // Checkboxes inside of the group box
  innerGroupBoxSizer->Add( &m_DependencyCheckbox, wxSizerFlags().Border( wxALL, 10 ) );
  innerGroupBoxSizer->Add( &m_HierarchyCheckbox, wxSizerFlags().Border( wxLEFT | wxRIGHT | wxBOTTOM, 10 ) );
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
