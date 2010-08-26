#include "Precompile.h"
#include "VaultPreviewWindow.h"
#include "VaultPanel.h"
#include "VaultSettings.h"
#include "Editor/App.h"

using namespace Helium;
using namespace Helium::Editor;

BEGIN_EVENT_TABLE( VaultPreviewWindow, RenderWindow )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
VaultPreviewWindow::VaultPreviewWindow( Core::SettingsManager* settingsManager, wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: RenderWindow( parent, id, pos, size, style, name )
, m_SettingsManager( settingsManager )
, m_VaultPanel( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
VaultPreviewWindow::~VaultPreviewWindow()
{
    if ( m_VaultPanel )
    {
        VaultSettings* settings = m_SettingsManager->GetSettings< VaultSettings >();
        settings->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultPreviewWindow::OnSettingsChanged ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Part of initialization.  You must call this function before you can call
// any others.
// 
void VaultPreviewWindow::SetVaultPanel( VaultPanel* vaultPanel )
{
    m_VaultPanel = vaultPanel;

    VaultSettings* settings = m_SettingsManager->GetSettings< VaultSettings >();
    __super::DisplayReferenceAxis( settings->DisplayPreviewAxis() );

    settings->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &VaultPreviewWindow::OnSettingsChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to pass the setting to the preferences.
// 
void VaultPreviewWindow::DisplayReferenceAxis( bool display )
{
    VaultSettings* settings = m_SettingsManager->GetSettings< VaultSettings >();
    settings->SetDisplayPreviewAxis( display );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the browser preferences are edited.  Updates the Axis display
// setting.
// 
void VaultPreviewWindow::OnSettingsChanged( const Reflect::ElementChangeArgs& args )
{
    VaultSettings* settings = m_SettingsManager->GetSettings< VaultSettings >();
    if ( args.m_Element == settings )
    {
        if ( args.m_Field->m_Name == settings->DisplayPreviewAxisField()->m_Name || args.m_Field == NULL )
        {
            __super::DisplayReferenceAxis( settings->DisplayPreviewAxis() );
        }
    }
}
