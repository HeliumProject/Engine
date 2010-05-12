#pragma once

#include "API.h"
#include "PreferencesBase.h"

#include <map>

#include "Reflect/Element.h"
#include "wx/wx.h"

namespace Inspect
{
  class Canvas;
  typedef Nocturnal::SmartPtr<Canvas> CanvasPtr;
}

namespace Reflect
{
  class Element;
  typedef Nocturnal::SmartPtr<Element> ElementPtr;
}

namespace Luna
{
  class PreferenceInfo : public Nocturnal::RefCountBase<PreferenceInfo>
  {
  public:
    PreferenceInfo( Reflect::ElementPtr& source, Reflect::ElementPtr& clone, Inspect::CanvasPtr& canvas );

    Reflect::ElementPtr m_Source;
    Reflect::ElementPtr m_Clone;
    Inspect::CanvasPtr m_Canvas;
  };
  
  typedef Nocturnal::SmartPtr<PreferenceInfo> PreferenceInfoPtr;
  typedef std::map<int, PreferenceInfoPtr> M_PreferenceInfo;

  class PreferencesDialog NOC_ABSTRACT : public wxDialog
  {
  public:
    PreferencesDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 644,375 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~PreferencesDialog();

    virtual int ShowModal();
  
  protected:
    virtual void GetPreferences( Reflect::V_Element& preferences );
    
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
