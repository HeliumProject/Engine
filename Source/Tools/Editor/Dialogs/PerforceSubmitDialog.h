#pragma once

#include <wx/dialog.h>

#include "Editor/PerforceSubmitPanel.h"

#if PERFORCE_REFACTOR
#include "Application/RCS.h"
#include "Perforce/Perforce.h"

namespace Helium
{
    namespace Editor
    {
        class PerforceSubmitDialog : public wxDialog 
        {
        public:
            PerforceSubmitDialog( wxWindow* parent,
                int id = wxID_ANY,
                int changelistNumber = RCS::DefaultChangesetId,
                const std::string& description = "",
                wxString title = wxT("Perforce Change Specification") );

            virtual ~PerforceSubmitDialog();

            virtual int ShowModal();
            virtual void EndModal( int retCode = wxID_CANCEL );

            PerforceSubmitPanel* GetPanel() const;

        private:
            PerforceSubmitPanel* m_Panel;
        };
    }
}
#endif