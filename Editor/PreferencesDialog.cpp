#include "Precompile.h"
#include "PreferencesDialog.h"

#include <wx/wx.h>
#include <wx/listbook.h>

#include "Core/Scene/PropertiesGenerator.h"
#include "Application/Inspect/Controls/Canvas.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectInterpreter.h"

using namespace Helium;
using namespace Helium::Editor;

PreferenceInfo::PreferenceInfo( Reflect::ElementPtr& source, Reflect::ElementPtr& clone, Inspect::CanvasPtr& canvas )
: m_Source( source )
, m_Clone( clone )
, m_Canvas( canvas )
{
}

PreferencesDialog::PreferencesDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: wxDialog( parent, id, title, pos, size, style )
, m_PreferenceSizer( NULL )
, m_CurrentPreference( NULL )
{
}

PreferencesDialog::~PreferencesDialog()
{
}

int PreferencesDialog::ShowModal( Editor::Preferences* prefs )
{
  m_PreferenceSizer = new wxBoxSizer( wxVERTICAL );
  m_CurrentPreference = NULL;
  m_PreferenceInfo.clear();

#pragma TODO("Automate this")
  Reflect::V_Element preferences;
  preferences.push_back( prefs->GetScenePreferences() );
  preferences.push_back( prefs->GetViewportPreferences() );
  preferences.push_back( prefs->GetGridPreferences() );
  preferences.push_back( prefs->GetVaultPreferences() );

  wxListBox* propertiesListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 130 /* 207 */, -1 ) );
  propertiesListBox->Connect( propertiesListBox->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( PreferencesDialog::OnPreferencesChanged ), NULL, this );
  
  wxSizer* propertiesSizer = new wxBoxSizer( wxHORIZONTAL );
  propertiesSizer->Add( propertiesListBox, 0, wxEXPAND | wxALL, 6 );
  propertiesSizer->Add( 6, 0, 0 );
  propertiesSizer->Add( m_PreferenceSizer, 1, wxEXPAND | wxALL, 6 );
  propertiesSizer->Add( 6, 0, 0 );
  
  Inspect::V_Control canvasControls;
  for ( Reflect::V_Element::iterator itr = preferences.begin(), end = preferences.end(); itr != end; ++itr )
  {
    Reflect::ElementPtr clone = (*itr)->Clone();
    clone->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &PreferencesDialog::OnRefreshElements ) );

    Inspect::CanvasPtr canvas = new Inspect::Canvas();
    canvasControls.push_back( canvas );
    canvas->SetPanelsExpanded( true );

    Inspect::TreeCanvasCtrl* canvasWindow = new Inspect::TreeCanvasCtrl( this );
    canvas->SetControl( canvasWindow );

    m_PreferenceSizer->Add( canvasWindow, 1, wxEXPAND, 0 );
    m_PreferenceSizer->Show( canvasWindow, false );

    Inspect::ReflectInterpreterPtr interpreter = new Inspect::ReflectInterpreter( canvas );
    std::vector< Reflect::Element* > elems;
    elems.push_back( clone );
    interpreter->Interpret( elems );

    int index = propertiesListBox->Append( (*itr)->GetTitle() );
    m_PreferenceInfo.insert( std::make_pair( index, new PreferenceInfo( (*itr), clone, canvas ) ) );
  }
  
  wxButton* restoreDefaults = new wxButton( this, wxID_ANY, wxT( "Restore Defaults" ), wxDefaultPosition, wxDefaultSize, 0 );
  restoreDefaults->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDialog::OnRestoreDefaults ), NULL, this );

  wxButton* apply = new wxButton( this, wxID_ANY, wxT( "Apply" ), wxDefaultPosition, wxDefaultSize, 0 );
  apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDialog::OnApply ), NULL, this );

  wxBoxSizer* propertiesButtonSizer = new wxBoxSizer( wxHORIZONTAL );
  propertiesButtonSizer->Add( restoreDefaults, 0, 0, 0 );
  propertiesButtonSizer->Add( apply, 0, wxLEFT, 5 );

  m_PreferenceSizer->Add( 0, 6, 0 );
  m_PreferenceSizer->Add( propertiesButtonSizer, 0, wxALIGN_RIGHT, 0 );
  m_PreferenceSizer->Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND | wxTOP, 5 );
  
  wxButton* okButton = new wxButton( this, wxID_ANY, wxT( "OK" ), wxDefaultPosition, wxDefaultSize, 0 );
  okButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDialog::OnOk ), NULL, this );

  wxButton* cancelButton = new wxButton( this, wxID_ANY, wxT( "Cancel" ), wxDefaultPosition, wxDefaultSize, 0 );
  cancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDialog::OnCancel ), NULL, this );

  wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );
  buttonSizer->Add( okButton, 0, wxALL, 5 );
  buttonSizer->Add( cancelButton, 0, wxUP | wxDOWN | wxRIGHT, 5 );
  buttonSizer->Add( 7, 0, 0 );

  wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->Add( 0, 1, 0 );
  mainSizer->Add( propertiesSizer, 1, wxEXPAND, 0 );
  mainSizer->Add( buttonSizer, 0, wxALIGN_RIGHT, 0 );
  mainSizer->Add( 0, 8, 0 );
  
  if ( propertiesListBox->GetCount() && ( m_PreferenceInfo.find( 0 ) != m_PreferenceInfo.end() ) )
  {
    propertiesListBox->SetSelection( 0 );
    SelectCanvas( m_PreferenceInfo[ 0 ] );
  }

  SetSizer( mainSizer );
  Layout();
  Centre();
  
  int result = __super::ShowModal();

  if ( result == wxID_OK )
  {
    for ( M_PreferenceInfo::iterator itr = m_PreferenceInfo.begin(), end = m_PreferenceInfo.end(); itr != end; ++itr )
    {
      PreferenceInfo* info = itr->second;
      if ( !info->m_Clone->Equals( info->m_Source ) )
      {
        info->m_Clone->CopyTo( info->m_Source );
        info->m_Source->RaiseChanged();
      }
    }
  }
  
  for ( M_PreferenceInfo::iterator itr = m_PreferenceInfo.begin(), end = m_PreferenceInfo.end(); itr != end; ++itr )
  {
    itr->second->m_Clone->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &PreferencesDialog::OnRefreshElements ) );
  }
  
  return result;
}

void PreferencesDialog::OnRestoreDefaults( wxCommandEvent& args )
{
  if ( !m_CurrentPreference )
  {
    return;
  }
  
  Reflect::ElementPtr defaultElement = Reflect::ObjectCast<Reflect::Element>( Reflect::Registry::GetInstance()->CreateInstance( m_CurrentPreference->m_Clone->GetType() ) );
  if ( !defaultElement )
  {
    return;
  }

  int tries = 10;
  bool changed = false;
  while ( ( tries-- > 0 ) && ( !defaultElement->Equals( m_CurrentPreference->m_Clone ) ) )
  {
    changed = true;
    defaultElement->CopyTo( m_CurrentPreference->m_Clone );
    m_CurrentPreference->m_Clone->RaiseChanged();
  }

  if ( changed )
  {
    m_CurrentPreference->m_Canvas->Freeze();
    m_CurrentPreference->m_Canvas->Read();
    m_CurrentPreference->m_Canvas->Thaw();
  }
}

void PreferencesDialog::OnApply( wxCommandEvent& args )
{
  if ( !m_CurrentPreference )
  {
    return;
  }

  if ( m_CurrentPreference->m_Clone->Equals( m_CurrentPreference->m_Source ) )
  {
    return;
  }

  m_CurrentPreference->m_Clone->CopyTo( m_CurrentPreference->m_Source );
  m_CurrentPreference->m_Source->RaiseChanged();
}

void PreferencesDialog::OnOk( wxCommandEvent& args )
{
  EndModal( wxID_OK );
}

void PreferencesDialog::OnCancel( wxCommandEvent& args )
{
  EndModal( wxID_CANCEL );
}

void PreferencesDialog::OnPreferencesChanged( wxCommandEvent& args )
{
  PreferenceInfo* newPreferenceInfo = NULL;
  
  M_PreferenceInfo::iterator itr = m_PreferenceInfo.find( args.GetInt() );
  if ( itr != m_PreferenceInfo.end() )
  {
    newPreferenceInfo = itr->second;
  }
  
  SelectCanvas( newPreferenceInfo );
}

void PreferencesDialog::OnRefreshElements( const Reflect::ElementChangeArgs& args )
{
  if ( m_CurrentPreference )
  {
    m_CurrentPreference->m_Canvas->Freeze();
    m_CurrentPreference->m_Canvas->Read();
    m_CurrentPreference->m_Canvas->Thaw();
  }
}

void PreferencesDialog::SelectCanvas( PreferenceInfo* preferenceInfo )
{
  if ( !m_PreferenceSizer )
  {
    return;
  }

  if ( m_CurrentPreference )
  {
    m_PreferenceSizer->Show( m_CurrentPreference->m_Canvas->GetControl(), false );
  }
    
  m_CurrentPreference = preferenceInfo;
  
  if ( m_CurrentPreference )
  {
    m_PreferenceSizer->Show( m_CurrentPreference->m_Canvas->GetControl(), true );

    m_CurrentPreference->m_Canvas->Freeze();
    m_CurrentPreference->m_Canvas->Layout();
    m_CurrentPreference->m_Canvas->Read();
    m_CurrentPreference->m_Canvas->Thaw();
  }
    
  m_PreferenceSizer->Layout();
}
