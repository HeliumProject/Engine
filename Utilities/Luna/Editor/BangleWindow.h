#pragma once

namespace Luna
{
  class PreviewWindow;
  
  typedef std::map< wxCheckBox*, int > M_CheckBoxId;
  
  class BangleWindow : public wxDialog
  {
  public:
    BangleWindow( PreviewWindow* parent, wxWindowID id, const wxString& title = "Bangles", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1, 1 ), long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = "Luna::BangleWindow" );
    virtual ~BangleWindow();
  
    void ClearBangles();
    void RefreshBangles();
    void UpdateSize();
  
  private:
    wxSize m_MinSize;
    PreviewWindow* m_PreviewWindow;
    wxPanel* m_Panel;
    wxBoxSizer* m_CheckBoxSizer;
    
    void OnCheckBoxClicked( wxCommandEvent& args );
  };
}
