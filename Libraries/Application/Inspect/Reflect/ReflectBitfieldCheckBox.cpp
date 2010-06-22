#include "ReflectBitfieldCheckBox.h"

#include "Application/Inspect/Widgets/Container.h"
#include "Application/Inspect/Data/StringData.h"
#include "Foundation/String/Tokenize.h"

#include <wx/checkbox.h>
#include <wx/panel.h>

using namespace Reflect;
using namespace Inspect;

const i32 CHECKBOXWIDTH = 16;
const i32 CHECKBOXHEIGHT = 22;
static const char* CHECKBOX_DEFAULT_INDICATOR = "*";

class CustomCheckBox : public wxCheckBox
{
public:
  ReflectBitfieldCheckBox* m_CheckBox;

  bool m_Override;

  CustomCheckBox( wxWindow* parent, ReflectBitfieldCheckBox* cb )
    : wxCheckBox( parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxCHK_3STATE )
    , m_CheckBox( cb )
    , m_Override( false )
  {

  }

  void OnChecked( wxCommandEvent& )
  {
    if (!m_Override)
    {
      m_CheckBox->Write();
    }
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE( CustomCheckBox, wxCheckBox )
EVT_CHECKBOX( wxID_ANY, CustomCheckBox::OnChecked )
END_EVENT_TABLE();

class CustomCheckBoxWindow : public wxPanel
{
public:
  CustomCheckBox* m_CheckBox;
  
  CustomCheckBoxWindow( wxWindow* parent, ReflectBitfieldCheckBox* cb, int width, int height )
  : wxPanel( parent, wxID_ANY )
  {
    m_CheckBox = new CustomCheckBox( this, cb );
    
    wxSize size( width, height );
    m_CheckBox->SetSize( size );
    m_CheckBox->SetMinSize( size );
    m_CheckBox->SetMaxSize( size );

    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();
    sizer->Add( m_CheckBox );
    sizer->Add( 1, 0, 1, wxEXPAND );
    Layout();
  }
};

ReflectBitfieldCheckBox::ReflectBitfieldCheckBox()
: m_State( CheckBoxStates::Unchecked )
{
}

void ReflectBitfieldCheckBox::Realize( Container* parent )
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if ( m_Window != NULL )
    return;

  m_Window = new CustomCheckBoxWindow( parent->GetWindow(), this, parent->GetStringWidth( CHECKBOX_DEFAULT_INDICATOR ) * 2 + CHECKBOXWIDTH, CHECKBOXHEIGHT );

  __super::Realize( parent );
}

void ReflectBitfieldCheckBox::Read()
{
  UpdateUI( GetChecked() );
  __super::Read();
}

bool ReflectBitfieldCheckBox::Write()
{
  bool wasDataWritten = false;

  if ( IsRealized() )
  {
    m_State = GetUIState();
    
    wasDataWritten = WriteBitfield();
  }

  return wasDataWritten;
}

CheckBoxStates::CheckBoxState ReflectBitfieldCheckBox::GetChecked()
{
  // If we have data, use it (otherwise, just use the state)
  if ( IsBound() && !m_BitfieldString.empty() )
  {
    std::vector< std::string > strs;
    ReadAll( strs );

    bool previous = false;
    const std::vector< std::string >::const_iterator strBegin = strs.begin();
    std::vector< std::string >::const_iterator strItr = strBegin;
    const std::vector< std::string >::const_iterator strEnd = strs.end();
    for ( ; strItr != strEnd; ++strItr )
    {
#pragma TODO( "There should be a version of Tokenize that builds sets instead of vectors to make all these searches faster" )
      std::vector< std::string > tokens;
      ::Tokenize( *strItr, tokens, "\\|" );
      bool found = std::find( tokens.begin(), tokens.end(), m_BitfieldString ) != tokens.end();
      m_State = found ? CheckBoxStates::Checked : CheckBoxStates::Unchecked;

      if ( strItr == strBegin )
      {
        previous = found;
      }
      else
      {
        if ( previous != found )
        {
          m_State = CheckBoxStates::Tristate;
          break;
        }
      }
    }
  }

  return m_State;
}

void ReflectBitfieldCheckBox::SetChecked( CheckBoxStates::CheckBoxState checked )
{
  // keep our latent state up to date (for easy debugging)
  m_State = checked;

  WriteBitfield();

  UpdateUI( m_State );
}

void ReflectBitfieldCheckBox::SetBitfieldString( const std::string& value )
{
  m_BitfieldString = value;
}

bool ReflectBitfieldCheckBox::IsDefault() const
{
  bool isDefault = false;
  if ( !IsBound() || m_Window == NULL )
  {
    isDefault = false;
  }
  else
  {
    std::vector< std::string > tokens;
    ::Tokenize( m_Default, tokens, "\\|" );
    bool shouldBeChecked = std::find( tokens.begin(), tokens.end(), m_BitfieldString ) != tokens.end();
    switch ( GetUIState() )
    {
    case CheckBoxStates::Checked:
      isDefault = shouldBeChecked ? true : false;
      break;

    case CheckBoxStates::Unchecked:
      isDefault = !shouldBeChecked ? true : false;
      break;

    case CheckBoxStates::Tristate:
      isDefault = false;
      break;
    }
  }

  return isDefault;
}

void ReflectBitfieldCheckBox::SetDefaultAppearance( bool def ) 
{
  if ( IsRealized() )
  {
    std::string label;
    if ( def )
    {
      label = CHECKBOX_DEFAULT_INDICATOR;
    }

    CustomCheckBoxWindow* window = (CustomCheckBoxWindow*) m_Window;
    if ( label != window->m_CheckBox->GetLabel().c_str() )
    {
      window->m_CheckBox->SetLabel( label.c_str() );
    }
  }
}

bool ReflectBitfieldCheckBox::WriteBitfield()
{
  bool dataWasUpdated = false;

  // if we have data, write to it
  if ( IsBound() && !m_BitfieldString.empty() )
  {
    if ( m_State != CheckBoxStates::Tristate )
    {
      std::vector< std::string > newVals;
      std::vector< std::string > strs;
      ReadAll( strs );

      std::vector< std::string >::const_iterator strItr = strs.begin();
      std::vector< std::string >::const_iterator strEnd = strs.end();
      for ( ; strItr != strEnd; ++strItr )
      {
        const std::string& current = *strItr;
        std::string newVal = current;
        std::vector< std::string > tokens;
        ::Tokenize( current, tokens, "\\|" );
        std::vector< std::string >::iterator foundItr = std::find( tokens.begin(), tokens.end(), m_BitfieldString );
        if ( ( m_State == CheckBoxStates::Checked ) && ( foundItr == tokens.end() ) )
        {
          tokens.push_back( m_BitfieldString );
          newVals.push_back( BuildBitfieldString( tokens, "|" ) );
        }
        else if ( ( m_State == CheckBoxStates::Unchecked ) && ( foundItr != tokens.end() ) )
        {
          tokens.erase( foundItr );
          newVals.push_back( BuildBitfieldString( tokens, "|" ) );
        }
        else
        {
          newVals.push_back( current );
        }
      }

      if ( newVals != strs )
      {
        WriteAll( newVals );
      }

      dataWasUpdated = true;
    }
  }
  return dataWasUpdated;
}

std::string ReflectBitfieldCheckBox::BuildBitfieldString( std::vector< std::string > tokens, const std::string& delimiter )
{
  std::string result;
  std::vector< std::string >::const_iterator itr = tokens.begin();
  std::vector< std::string >::const_iterator end = tokens.end();
  for ( ; itr != end; ++itr )
  {
    if ( !result.empty() )
    {
      result += delimiter;
    }
    result += *itr;
  }
  return result;
}

void ReflectBitfieldCheckBox::UpdateUI( CheckBoxStates::CheckBoxState state )
{
  // if we have UI, update it
  if ( IsRealized() )
  {
    CustomCheckBoxWindow* checkBox = Control::Cast< CustomCheckBoxWindow >( this );
    checkBox->m_CheckBox->m_Override = true;

    switch ( state )
    {
    case CheckBoxStates::Checked:
      checkBox->m_CheckBox->Set3StateValue( wxCHK_CHECKED );
      break;

    case CheckBoxStates::Unchecked:
      checkBox->m_CheckBox->Set3StateValue( wxCHK_UNCHECKED );
      break;

    case CheckBoxStates::Tristate:
      checkBox->m_CheckBox->Set3StateValue( wxCHK_UNDETERMINED );
      break;
    }

    checkBox->m_CheckBox->m_Override = false;
  }
}

CheckBoxStates::CheckBoxState ReflectBitfieldCheckBox::GetUIState() const
{
  CheckBoxStates::CheckBoxState state = CheckBoxStates::Unchecked;
  if ( m_Window )
  {
    const CustomCheckBoxWindow* checkBox = Control::Cast< CustomCheckBoxWindow >( this );
    switch ( checkBox->m_CheckBox->Get3StateValue() )
    {
    case wxCHK_UNCHECKED:
      state = CheckBoxStates::Unchecked;
      break;

    case wxCHK_CHECKED:
      state = CheckBoxStates::Checked;
      break;

    case wxCHK_UNDETERMINED:
      state = CheckBoxStates::Tristate;
      break;
    };
  }
  return state;
}
