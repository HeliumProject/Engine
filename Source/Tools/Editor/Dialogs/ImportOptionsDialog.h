#pragma once

#include <wx/valgen.h>

namespace Helium
{
    namespace Editor
    {
        class ImportOptionsDialog : public wxDialog
        {
        private:
            wxGenericValidator m_UpdateValidator;
            wxCheckBox m_UpdateCheckbox;

        public:
            ImportOptionsDialog( wxWindow* parent, bool& update );
            virtual ~ImportOptionsDialog();

        private:
            void OnInit( wxInitDialogEvent& event );

            DECLARE_EVENT_TABLE();
        };
    }
}