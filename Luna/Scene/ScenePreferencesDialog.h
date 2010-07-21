#pragma once

#include "Luna/API.h"

#include "PreferencesDialog.h"

namespace Luna
{
  class ScenePreferencesDialog : public PreferencesDialog
  {
  public:
    ScenePreferencesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 644,375 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~ScenePreferencesDialog();

  protected:
    virtual void GetPreferences( Reflect::V_Element& preferences ) NOC_OVERRIDE;
  };
}
