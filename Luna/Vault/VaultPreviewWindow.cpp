#include "Precompile.h"
#include "VaultPreviewWindow.h"
#include "Vault.h"
#include "VaultFrame.h"
#include "App.h"

using namespace Luna;

BEGIN_EVENT_TABLE( VaultPreviewWindow, RenderWindow )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
VaultPreviewWindow::VaultPreviewWindow( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: RenderWindow( parent, id, pos, size, style, name )
, m_VaultFrame( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
VaultPreviewWindow::~VaultPreviewWindow()
{
  if ( m_VaultFrame )
  {
    VaultPreferences* preferences = wxGetApp().GetPreferences()->GetVaultPreferences();
    preferences->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultPreviewWindow::OnPreferencesChanged ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Part of initialization.  You must call this function before you can call
// any others.
// 
void VaultPreviewWindow::SetVaultFrame( VaultFrame* browserFrame )
{
  m_VaultFrame = browserFrame;

  VaultPreferences* preferences = wxGetApp().GetPreferences()->GetVaultPreferences();
  __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );

  preferences->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultPreviewWindow::OnPreferencesChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to pass the setting to the preferences.
// 
void VaultPreviewWindow::DisplayReferenceAxis( bool display )
{
  VaultPreferences* preferences = wxGetApp().GetPreferences()->GetVaultPreferences();
  preferences->SetDisplayPreviewAxis( display );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the browser preferences are edited.  Updates the Axis display
// setting.
// 
void VaultPreviewWindow::OnPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
  VaultPreferences* preferences = wxGetApp().GetPreferences()->GetVaultPreferences();
  if ( args.m_Element == preferences )
  {
    if ( args.m_Field == preferences->DisplayPreviewAxisField() || args.m_Field == NULL )
    {
      __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );
    }
  }
}
