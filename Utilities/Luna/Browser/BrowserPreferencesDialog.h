#pragma once

#include "Luna/API.h"

#include "Editor/PreferencesDialog.h"

namespace Luna
{
  class BrowserPreferencesDialog : public PreferencesDialog
  {
  public:
    BrowserPreferencesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Vault Preferences", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 644,375 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~BrowserPreferencesDialog();

  protected:
    virtual void GetPreferences( Reflect::V_Element& preferences ) NOC_OVERRIDE;
  };
}
