#include "Precompile.h"
#include "ScenePreferencesDialog.h"

#include "Scene/ScenePreferences.h"

using namespace Luna;

ScenePreferencesDialog::ScenePreferencesDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: PreferencesDialog( parent, id, title, pos, size, style )
{
}

ScenePreferencesDialog::~ScenePreferencesDialog()
{
}

void ScenePreferencesDialog::GetPreferences( Reflect::V_Element& preferences )
{
  __super::GetPreferences( preferences );

  ScenePreferencesPtr sceneEditorPreferences = SceneEditorPreferences();
  // TODO: Put this back in when we remove these from the rest of the UI
  // preferences.push_back( sceneEditorPreferences->GetViewPreferencesPtr() );
  preferences.push_back( sceneEditorPreferences->GetGridPreferencesPtr() );
}
