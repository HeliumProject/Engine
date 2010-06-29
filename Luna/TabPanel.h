#pragma once

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Base class for many UI elements in Luna.  Provides a container that can
  // contain one or more other panels.  A tab control is displayed if more
  // than one inner panel is part of this control.
  // 
  class TabPanel : public wxPanel
  {
  public:
    TabPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT( "TabPanel" ) );
    virtual ~TabPanel();

    void AddWindow( wxWindow* child );
    void RemoveWindow( wxWindow* child );

  protected:
    wxAuiNotebook* m_Tabs;
    wxWindow* m_OnlyChild;
    wxSizer* m_Sizer;
  };
}
