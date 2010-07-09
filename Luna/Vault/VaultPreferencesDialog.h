#pragma once

#include "Luna/API.h"

#include "Editor/PreferencesDialog.h"

namespace Luna
{
  class VaultPreferencesDialog : public PreferencesDialog
  {
  public:
    VaultPreferencesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT( "Vault Preferences" ), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 644,375 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~VaultPreferencesDialog();

  protected:
    virtual void GetPreferences( Reflect::V_Element& preferences ) NOC_OVERRIDE;
  };
}
