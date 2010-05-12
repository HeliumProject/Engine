#pragma once

#include <wx/valgen.h>

namespace Luna
{
  class ExportOptionsDlg : public wxDialog
  {
  private:
    wxGenericValidator m_DependencyValidator;
    wxCheckBox m_DependencyCheckbox;

    wxGenericValidator m_HierarchyValidator;
    wxCheckBox m_HierarchyCheckbox;

    wxGenericValidator m_BoundedValidator;
    wxCheckBox m_BoundedCheckbox;

    wxGenericValidator m_WorldValidator;
    wxCheckBox m_WorldCheckbox;

  public:
    ExportOptionsDlg( wxWindow* parent, bool& dependencies, bool& hierarchy, bool& bounded, bool& world );
    virtual ~ExportOptionsDlg();

  private:
    void OnInit( wxInitDialogEvent& event );

    DECLARE_EVENT_TABLE();
  };
}