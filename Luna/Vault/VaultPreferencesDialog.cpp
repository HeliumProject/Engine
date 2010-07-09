#include "Precompile.h"
#include "VaultPreferencesDialog.h"

#include "Application.h"

#include "Vault/Vault.h"
#include "Vault/VaultPreferences.h"

using namespace Luna;

VaultPreferencesDialog::VaultPreferencesDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: PreferencesDialog( parent, id, title, pos, size, style )
{
}

VaultPreferencesDialog::~VaultPreferencesDialog()
{
}

void VaultPreferencesDialog::GetPreferences( Reflect::V_Element& preferences )
{
  __super::GetPreferences( preferences );

  preferences.push_back( wxGetApp().GetVault().GetVaultPreferences() );
}
