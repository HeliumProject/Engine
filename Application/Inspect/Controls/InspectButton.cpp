#include "Application/Inspect/Controls/InspectButton.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/Controls/InspectCanvas.h"

// Using
using namespace Helium;
using namespace Helium::Inspect;

///////////////////////////////////////////////////////////////////////////////
// 
// 
class TextButton : public wxButton
{
public:
  Button* m_Button;

  TextButton( wxWindow* parent, Button* b )
  : wxButton( parent, wxID_ANY )
  , m_Button( b )
  {

  }

  void OnClick( wxCommandEvent& )
  {
    m_Button->Write();
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE( TextButton, wxButton )
  EVT_BUTTON( wxID_ANY, TextButton::OnClick )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// 
// 
class BitmapButton : public wxBitmapButton
{
public:
  Button* m_Button;

  BitmapButton( wxWindow* parent, Button* b )
  : wxBitmapButton( parent, wxID_ANY, wxNullBitmap )
  , m_Button( b )
  {

  }

  void OnClick( wxCommandEvent& )
  {
    m_Button->Write();
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE( BitmapButton, wxBitmapButton )
  EVT_BUTTON( wxID_ANY, BitmapButton::OnClick )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Button::Button()
: m_Text( TXT( "..." ) )
, m_Icon( (wxArtID)wxT( "" ) )
{
  m_FixedWidth = true;
}

///////////////////////////////////////////////////////////////////////////////
// Processes any attributes specific to the button.
// 
bool Button::Process(const tstring& key, const tstring& value)
{
  bool handled = false;
  if ( key == BUTTON_ATTR_TEXT )
  {
    SetText( value );
    handled = true;
  }
  else
  {
    handled = __super::Process( key, value );
  }

  return handled;
}

///////////////////////////////////////////////////////////////////////////////
// Creates the underlying wxWidget control.
// 
void Button::Realize( Container* parent )
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if ( m_Window != NULL )
    return;

  if (!m_Icon.empty())
  {
    m_Window = new BitmapButton( parent->GetWindow(), this );
  }
  else
  {
    m_Window = new TextButton( parent->GetWindow(), this );
  }

  __super::Realize( parent );

  if ( !m_Icon.empty() )
  {
    SetIcon( m_Icon );
  }
  else
  {
    SetText( m_Text );
  }

  wxSize buttonSize( GetStringWidth( m_Text.c_str() ) + 2 * ( m_Canvas->GetPad() + m_Canvas->GetBorder() ), m_Canvas->GetStdSize( Math::SingleAxes::Y ) );
  m_Window->SetSize( buttonSize );
  m_Window->SetMinSize( buttonSize );
  m_Window->SetMaxSize( buttonSize );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the label on this button.
// 
bool Inspect::Button::Write()
{
  bool result = __super::Write();

  if (m_Interpreter != NULL)
  {
    if (!m_Interpreter->RaisePropertyChanging( ChangingArgs (this, NULL, false) ))
    {
      return false;
    }

    m_BoundData->Refresh();

    m_Interpreter->RaisePropertyChanged( this );
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the label on this button.
// 
void Button::SetText(const tstring& text)
{
  if ( IsRealized() )
  {
    // cast protection, only swap text if we were instantiated without an icon
    if ( m_Icon.empty() )
    {
      m_Text = text;
      Control::Cast< wxButton >( this )->SetLabel( m_Text.c_str() );
    }
  }
  else
  {
    // cache
    m_Text = text;
  }
}
  
///////////////////////////////////////////////////////////////////////////////
// Sets the label on this button.
// 
void Button::SetIcon(const wxArtID& icon)
{
  if ( IsRealized() )
  {
    // cast protection, only swap icon if we were instantiated with an icon
    if ( !m_Icon.empty() )
    {
      m_Icon = icon;
      Control::Cast< wxBitmapButton >( this )->SetBitmapLabel( wxArtProvider::GetBitmap( m_Icon ) );
    }
  }
  else
  {
    // cache
    m_Icon = icon;
  }
}