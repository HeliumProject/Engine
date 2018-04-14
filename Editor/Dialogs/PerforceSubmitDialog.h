#pragma once

#include <wx/dialog.h>

#include "Application/RCS.h"

#include "Editor/Perforce/Perforce.h"
#include "Editor/PerforceSubmitPanel.h"

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