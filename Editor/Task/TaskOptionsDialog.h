#pragma once

class wxPanel;
class wxButton;

///////////////////////////////////////////////////////////////////////////////
/// Class BuilderOptionsDlg
///////////////////////////////////////////////////////////////////////////////
class TaskOptionsDialog : public wxDialog 
{
private:

protected:
  wxPanel* m_panel2;
  wxButton* m_button3;
  wxButton* m_button4;

public:
  TaskOptionsDialog( wxWindow* parent, int id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 300, 300 ), int style = wxDEFAULT_DIALOG_STYLE );

  wxPanel* GetPanel();

  void OnOK( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );

  DECLARE_EVENT_TABLE();
};