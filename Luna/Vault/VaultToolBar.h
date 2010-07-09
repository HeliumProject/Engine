#pragma once

#include "Luna/API.h"

#include "Platform/Types.h"

class wxTextCtrl;

namespace Luna
{
  ///////////////////////////////////////////////////////////////////////////////
  //class VaultHistory;

  namespace VaultToolBarIDs
  {
    enum VaultToolBarID
    {
      ButtonID = wxID_HIGHEST + 1,
      SearchBoxID,
    };
  }

  ///////////////////////////////////////////////////////////////////////////////
  class VaultToolBar: public wxToolBar
  {
  public:
    VaultToolBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTB_FLAT | wxTB_NODIVIDER,
                const wxString& name = wxT( "Vault" ) );
    virtual ~VaultToolBar();

    wxAuiPaneInfo GetAuiPaneInfo( int position = -1 );

  protected:
    void OnButtonClick( wxCommandEvent& args );
    void OnKeyDown( wxKeyEvent& evt );
    void OnTextEnter( wxCommandEvent& args );

  private:
    wxTextCtrl*  m_SearchBox;
  };

} // namespace Luna