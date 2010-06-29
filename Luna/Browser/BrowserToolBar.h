#pragma once

#include "Luna/API.h"

#include "Platform/Types.h"

class wxTextCtrl;

namespace Luna
{
  ///////////////////////////////////////////////////////////////////////////////
  //class BrowserHistory;

  namespace BrowserToolBarIDs
  {
    enum BrowserToolBarID
    {
      ButtonID = wxID_HIGHEST + 1,
      SearchBoxID,
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  class BrowserToolBar: public wxToolBar
  {
  public:
    BrowserToolBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT,
                const wxString& name = wxT( "Asset Vault" ) );
    virtual ~BrowserToolBar();

    wxAuiPaneInfo GetAuiPaneInfo( int position = -1 );

  protected:
    void OnButtonClick( wxCommandEvent& args );
    void OnKeyDown( wxKeyEvent& evt );
    void OnTextEnter( wxCommandEvent& args );

  private:
    wxTextCtrl*  m_SearchBox;
  };

} // namespace Luna