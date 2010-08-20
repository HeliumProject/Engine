#include "Application/Inspect/Controls/InspectValue.h"
#include "Application/Inspect/Controls/InspectCanvas.h"

using namespace Helium;
using namespace Helium::Inspect;

#ifdef INSPECT_REFACTOR

class StdTextBox : public wxTextCtrl
{
public:
  Value* m_TextBox;

  bool m_Override;

  StdTextBox( wxWindow* parent, Value* textBox )
    : wxTextCtrl()
    , m_TextBox (textBox)
    , m_Override (false)
  {
    i32 flags = wxTE_PROCESS_ENTER;
    if ( m_TextBox->IsReadOnly() )
    {
      flags |= wxTE_READONLY;
    }

    Create( parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, flags );
  }

  void OnConfirm(wxCommandEvent& e)
  {
    if (!m_Override)
    {
      m_TextBox->Write();
    }
  }

  void OnSetFocus(wxFocusEvent& event)
  {
    SetSelection(0, GetLastPosition());

    event.Skip();
  }

  void OnKillFocus(wxFocusEvent& e)
  {
    if (!m_Override && !m_TextBox->IsReadOnly())
    {
      m_TextBox->Write();
    }
  }
  
  void OnKeyDown(wxKeyEvent& e)
  {
    if ( e.ControlDown() && ( ( e.GetKeyCode() == 'A' ) || ( e.GetKeyCode() == 'a' ) ) )
    {
      SetSelection( -1, -1 );
    }
    else
    {
      e.Skip();
    }
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(StdTextBox, wxTextCtrl)
EVT_TEXT_ENTER(wxID_ANY, StdTextBox::OnConfirm)
EVT_SET_FOCUS(StdTextBox::OnSetFocus)
EVT_KILL_FOCUS(StdTextBox::OnKillFocus)
EVT_KEY_DOWN(StdTextBox::OnKeyDown)
END_EVENT_TABLE()

Value::Value()
: m_Required( false )
, m_Justify( kLeft )
, m_Highlight( false )
{
  m_ContextMenu = new ContextMenu (this);
  m_ContextMenu->AddItem( TXT( "Set To Default" ), ContextMenuSignature::Delegate(this, &Value::SetToDefault));
}

bool Value::Process(const tstring& key, const tstring& value)
{
  if (__super::Process(key, value))
    return true;

  if (key == TEXTBOX_ATTR_REQUIRED)
  {
    if (value == ATTR_VALUE_TRUE)
    {
      m_Required = true;
    }
    else if (value == ATTR_VALUE_FALSE)
    {
      m_Required = false;
    }
  }
  else if (key == TEXTBOX_ATTR_JUSTIFY)
  {
    if (value == TEXTBOX_ATTR_JUSTIFY_LEFT)
    {
      m_Justify = kLeft;
      return true;
    }
    else if (value == TEXTBOX_ATTR_JUSTIFY_RIGHT)
    {
      m_Justify = kRight;
      return true;
    }
  }

  return false;
}

void Value::SetDefaultAppearance(bool def)
{
  SetHighlight(def);
}

void Value::SetToDefault(const ContextMenuEventArgs& event)
{
  event.m_Control->SetDefault();

  event.m_Control->Read();
}

void Value::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if (m_Window != NULL)
    return;

  m_Window = new StdTextBox( parent->GetWindow(), this );
  wxSize size( -1, m_Canvas->GetStdSize( Math::SingleAxes::Y ) );
  m_Window->SetSize( size );
  m_Window->SetMinSize( size );
  m_Window->SetMaxSize( size );

  __super::Realize(parent);

  if ( !IsBound() )
  {
    SetText( m_Text ); // Calls SetJustification for us
  }

  SetHighlight( m_Highlight );
}
 
void Value::Read()
{
  if ( IsBound() )
  {
    ReadData( m_Text );

    UpdateUI( m_Text );

    __super::Read();
  }
}

bool Value::Write()
{
  bool result = false;

  if ( IsBound() )
  {
    StdTextBox* textCtrl = Control::Cast< StdTextBox >( this );
    m_Text = textCtrl->GetValue().c_str();

    textCtrl->m_Override = true;
    result = WriteData( m_Text );
    textCtrl->m_Override = false;

    textCtrl->SetSelection( 0, static_cast< long >( m_Text.size() ) );
  }

  return result;
}

void Value::GetText(tstring& text)
{
  if ( IsBound() )
  {
    ReadData( m_Text );
  }

  text = m_Text;
}

void Value::SetText(const tstring& text)
{
  m_Text = text;

  if ( IsBound() )
  {
    WriteData( m_Text );
  }

  UpdateUI( m_Text );
}

Value::Justify Value::GetJustification() const
{
  return m_Justify;
}

void Value::SetJustification( Value::Justify justification )
{
  m_Justify = justification;
  
  if ( IsRealized() )
  {
    StdTextBox* textCtrl = Control::Cast< StdTextBox >( this );
    switch ( m_Justify )
    {
    case kLeft:
      textCtrl->SetInsertionPoint( 0 );
      break;

    case kRight:
      textCtrl->SetInsertionPointEnd();
      break;
    }
  }
}

void Value::SetHighlight(bool highlighted)
{
  m_Highlight = highlighted;

  if ( IsRealized() )
  {
    if ( m_Highlight )
    {
      if ( m_Required )
      {
        m_Window->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
        m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
      }
      else
      {
        m_Window->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
      }
    }
    else
    {
      m_Window->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
      if ( !IsReadOnly() )
      {
        m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
      }
      else
      {
        m_Window->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
      }
    }
  }
}

void Value::SetReadOnly(bool readOnly)
{
  if ( m_IsReadOnly != readOnly )
  {
    m_IsReadOnly = readOnly;

    if ( IsRealized() )
    {
      StdTextBox* textCtrl = Control::Cast< StdTextBox >( this );
      textCtrl->SetEditable( !m_IsReadOnly );
    }
  }
}

void Value::UpdateUI( const tstring& text )
{
  if ( IsRealized() )
  {
    StdTextBox* textCtrl = Control::Cast< StdTextBox >( this );

    textCtrl->m_Override = true;
    textCtrl->SetValue( text.c_str() );
    textCtrl->m_Override = false;

    SetJustification( m_Justify );
  }
}

#endif