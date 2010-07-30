#pragma once

#include <wx/valgen.h>

namespace Helium
{
    namespace Editor
    {
        class ExportOptionsDlg : public wxDialog
        {
        private:
            wxGenericValidator m_DependencyValidator;
            wxCheckBox m_DependencyCheckbox;

            wxGenericValidator m_HierarchyValidator;
            wxCheckBox m_HierarchyCheckbox;

        public:
            ExportOptionsDlg( wxWindow* parent, bool& dependencies, bool& hierarchy );
            virtual ~ExportOptionsDlg();

        private:
            void OnInit( wxInitDialogEvent& event );

            DECLARE_EVENT_TABLE();
        };
    }
}