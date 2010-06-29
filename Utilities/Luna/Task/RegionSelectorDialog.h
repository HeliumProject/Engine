#pragma once

#include <set>

#include "Luna/API.h"

#include "Platform/Types.h"

namespace Luna 
{
  class RegionSelectorDialog : public wxDialog 
  {
  protected:
    wxListBox* m_listBox1;
    wxStaticText* m_messageText;
    wxButton* m_button2;
    wxButton* m_button3;
    wxButton* m_button1;

    tstring& m_SelectedRegion;

  public:
    RegionSelectorDialog( wxWindow* parent, const tstring& title, const tstring& msg, const std::set< tstring >& regionNames, tstring& selectedRegion, int id = -1, wxPoint pos = wxDefaultPosition, 
      wxSize size = wxSize( -1, -1 ), int style = wxCAPTION );
    virtual ~RegionSelectorDialog();

  private:
    void OnOK( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnAll( wxCommandEvent& event );

    DECLARE_EVENT_TABLE();
  };
}
