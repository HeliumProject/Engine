#include "stdafx.h"
#include "CheckBox.h"
#include "Container.h"

using namespace Inspect;

const i32 CHECKBOXWIDTH = 16;
const i32 CHECKBOXHEIGHT = 22;
static const char* CHECKBOX_DEFAULT_INDICATOR = "*";

class StdCheckBox : public wxCheckBox
{
public:
  CheckBox* m_CheckBox;

  bool m_Override;

  StdCheckBox (wxWindow* parent, CheckBox* cb)
    : wxCheckBox (parent, wxID_ANY, "")
    , m_CheckBox (cb)
    , m_Override (false)
  {

  }

  void OnChecked(wxCommandEvent&)
  {
    if (!m_Override)
    {
      m_CheckBox->Write();
    }
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(StdCheckBox, wxCheckBox)
EVT_CHECKBOX(wxID_ANY, StdCheckBox::OnChecked)
END_EVENT_TABLE();

class StdCheckBoxWindow : public wxPanel
{
public:
  StdCheckBox* m_CheckBox;
  
  StdCheckBoxWindow( wxWindow* parent, CheckBox* cb, int width, int height )
  : wxPanel( parent, wxID_ANY )
  {
    m_CheckBox = new StdCheckBox( this, cb );
    
    wxSize size( width, height );
    m_CheckBox->SetSize( size );
    m_CheckBox->SetMinSize( size );
    m_CheckBox->SetMaxSize( size );

    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();
    sizer->Add( m_CheckBox, 0, wxALIGN_CENTER_VERTICAL );
    sizer->Add( 1, 0, 1, wxEXPAND );
    Layout();
  }
};

CheckBox::CheckBox()
: m_State (false)
{
}

void CheckBox::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if (m_Window != NULL)
    return;

  m_Window = new StdCheckBoxWindow (parent->GetWindow(), this, parent->GetStringWidth( CHECKBOX_DEFAULT_INDICATOR ) * 2 + CHECKBOXWIDTH, CHECKBOXHEIGHT);

  __super::Realize(parent);
}

void CheckBox::Read()
{
  UpdateUI( GetChecked() );
  __super::Read();
}

bool CheckBox::Write()
{
  if ( IsRealized() )
  {
    StdCheckBoxWindow* checkBox = Control::Cast< StdCheckBoxWindow >( this );

    m_State = checkBox->m_CheckBox->GetValue();
    
    if ( IsBound() )
    {
      std::string str ( m_State ? "1" : "0" );
      return WriteData( str );
    }

    return true;
  }

  return false;
}

bool CheckBox::GetChecked()
{
  // If we have data, use it (otherwise, just use the state)
  if ( IsBound() )
  {
    std::string str;
    ReadData( str );
    m_State = atoi( str.c_str() ) != 0;
  }

  return m_State;
}

void CheckBox::SetChecked( bool checked )
{
  // keep our latent state up to date (for easy debugging)
  m_State = checked;

  // if we have data, write to it
  if ( IsBound() )
  {
    WriteData( m_State ? "1" : "0" );
  }

  UpdateUI( m_State );
}

void CheckBox::SetDefaultAppearance( bool def ) 
{
  if ( IsRealized() )
  {
    std::string label;
    if ( def )
    {
      label = "*";
    }

    StdCheckBoxWindow* window = (StdCheckBoxWindow*) m_Window;
    if ( label != window->m_CheckBox->GetLabel().c_str() )
    {
      window->m_CheckBox->SetLabel( label.c_str() );
    }
  }
}

void CheckBox::UpdateUI( bool state )
{
  // if we have UI, update it
  if ( IsRealized() )
  {
    StdCheckBoxWindow* checkBox = Control::Cast< StdCheckBoxWindow >( this );
    checkBox->m_CheckBox->m_Override = true;
    checkBox->m_CheckBox->SetValue( state );
    checkBox->m_CheckBox->m_Override = false;
  }
}
