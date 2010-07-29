#pragma once

#include <map>

#include "Foundation/Reflect/Element.h"
#include "Application/Inspect/Controls/Canvas.h"

#include "Editor/API.h"
#include "Editor/Preferences.h"

namespace Editor
{
  class PreferenceInfo : public Helium::RefCountBase<PreferenceInfo>
  {
  public:
    PreferenceInfo( Reflect::ElementPtr& source, Reflect::ElementPtr& clone, Inspect::CanvasPtr& canvas );

    Reflect::ElementPtr m_Source;
    Reflect::ElementPtr m_Clone;
    Inspect::CanvasPtr m_Canvas;
  };
  
  typedef Helium::SmartPtr<PreferenceInfo> PreferenceInfoPtr;
  typedef std::map<int, PreferenceInfoPtr> M_PreferenceInfo;

  class PreferencesDialog : public wxDialog
  {
  public:
    PreferencesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 644,375 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~PreferencesDialog();

    virtual int ShowModal( Editor::Preferences* prefs );
  
  private:
    void OnRestoreDefaults( wxCommandEvent& args );
    void OnApply( wxCommandEvent& args );
    void OnOk( wxCommandEvent& args );
    void OnCancel( wxCommandEvent& args );
    void OnPreferencesChanged( wxCommandEvent& args );
    void OnRefreshElements( const Reflect::ElementChangeArgs& args );

    void SelectCanvas( PreferenceInfo* preferenceInfo );
    
    wxSizer* m_PreferenceSizer;
    M_PreferenceInfo m_PreferenceInfo;

    PreferenceInfo* m_CurrentPreference;
  };
}
