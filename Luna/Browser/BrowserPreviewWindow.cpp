#include "Precompile.h"
#include "BrowserPreviewWindow.h"
#include "Browser.h"
#include "BrowserFrame.h"

using namespace Luna;

BEGIN_EVENT_TABLE( BrowserPreviewWindow, PreviewWindow )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
BrowserPreviewWindow::BrowserPreviewWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: PreviewWindow( parent, id, pos, size, style, name )
, m_BrowserFrame( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
BrowserPreviewWindow::~BrowserPreviewWindow()
{
  if ( m_BrowserFrame )
  {
    BrowserPreferences* preferences = m_BrowserFrame->GetBrowser()->GetBrowserPreferences();
    preferences->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &BrowserPreviewWindow::OnPreferencesChanged ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Part of initialization.  You must call this function before you can call
// any others.
// 
void BrowserPreviewWindow::SetBrowserFrame( BrowserFrame* browserFrame )
{
  m_BrowserFrame = browserFrame;

  BrowserPreferences* preferences = m_BrowserFrame->GetBrowser()->GetBrowserPreferences();
  __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );

  preferences->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &BrowserPreviewWindow::OnPreferencesChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to pass the setting to the preferences.
// 
void BrowserPreviewWindow::DisplayReferenceAxis( bool display )
{
  BrowserPreferences* preferences = m_BrowserFrame->GetBrowser()->GetBrowserPreferences();
  preferences->SetDisplayPreviewAxis( display );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the browser preferences are edited.  Updates the Axis display
// setting.
// 
void BrowserPreviewWindow::OnPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
  BrowserPreferences* preferences = m_BrowserFrame->GetBrowser()->GetBrowserPreferences();
  if ( args.m_Element == preferences )
  {
    if ( args.m_Field == preferences->DisplayPreviewAxisField() || args.m_Field == NULL )
    {
      __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );
    }
  }
}
