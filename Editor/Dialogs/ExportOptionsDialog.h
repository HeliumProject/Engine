#pragma once

#include <wx/valgen.h>

namespace Helium
{
    namespace Editor
    {
        class ExportOptionsDialog : public wxDialog
        {
        private:
            wxGenericValidator m_DependencyValidator;
            wxCheckBox m_DependencyCheckbox;

            wxGenericValidator m_HierarchyValidator;
            wxCheckBox m_HierarchyCheckbox;

        public:
            ExportOptionsDialog( wxWindow* parent, bool& dependencies, bool& hierarchy );
            virtual ~ExportOptionsDialog();

        private:
            void OnInit( wxInitDialogEvent& event );

            DECLARE_EVENT_TABLE();
        };
    }
}