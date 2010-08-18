#include "Precompile.h"
#include "VaultPreviewWindow.h"
#include "Vault.h"
#include "VaultFrame.h"
#include "Editor/App.h"

using namespace Helium;
using namespace Helium::Editor;

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
    VaultSettings* preferences = wxGetApp().GetSettings()->GetVaultSettings();
    preferences->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultPreviewWindow::OnSettingsChanged ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Part of initialization.  You must call this function before you can call
// any others.
// 
void VaultPreviewWindow::SetVaultFrame( VaultFrame* browserFrame )
{
  m_VaultFrame = browserFrame;

  VaultSettings* preferences = wxGetApp().GetSettings()->GetVaultSettings();
  __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );

  preferences->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultPreviewWindow::OnSettingsChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to pass the setting to the preferences.
// 
void VaultPreviewWindow::DisplayReferenceAxis( bool display )
{
  VaultSettings* preferences = wxGetApp().GetSettings()->GetVaultSettings();
  preferences->SetDisplayPreviewAxis( display );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the browser preferences are edited.  Updates the Axis display
// setting.
// 
void VaultPreviewWindow::OnSettingsChanged( const Reflect::ElementChangeArgs& args )
{
  VaultSettings* preferences = wxGetApp().GetSettings()->GetVaultSettings();
  if ( args.m_Element == preferences )
  {
    if ( args.m_Field == preferences->DisplayPreviewAxisField() || args.m_Field == NULL )
    {
      __super::DisplayReferenceAxis( preferences->DisplayPreviewAxis() );
    }
  }
}
