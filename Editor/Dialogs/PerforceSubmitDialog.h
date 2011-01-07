#pragma once

#include <wx/dialog.h>

#include "Foundation/RCS/RCS.h"
#include "Editor/Perforce/Perforce.h"

#include "PerforceSubmitPanel.h"

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
                const tstring& description = TXT( "" ),
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