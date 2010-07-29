#pragma once

#include <wx/valgen.h>

namespace Editor
{
  class ImportOptionsDlg : public wxDialog
  {
  private:
    wxGenericValidator m_UpdateValidator;
    wxCheckBox m_UpdateCheckbox;

  public:
    ImportOptionsDlg( wxWindow* parent, bool& update );
    virtual ~ImportOptionsDlg();

  private:
    void OnInit( wxInitDialogEvent& event );

    DECLARE_EVENT_TABLE();
  };
}