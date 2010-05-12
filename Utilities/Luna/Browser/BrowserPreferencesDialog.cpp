#include "Precompile.h"
#include "BrowserPreferencesDialog.h"

#include "Browser/Browser.h"
#include "Browser/BrowserPreferences.h"

using namespace Luna;

BrowserPreferencesDialog::BrowserPreferencesDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: PreferencesDialog( parent, id, title, pos, size, style )
{
}

BrowserPreferencesDialog::~BrowserPreferencesDialog()
{
}

void BrowserPreferencesDialog::GetPreferences( Reflect::V_Element& preferences )
{
  __super::GetPreferences( preferences );

  preferences.push_back( GlobalBrowser().GetBrowserPreferences() );
}
