#pragma once

#include "Application/RCS/RCS.h"
#include "Application/RCS/Providers/Perforce/Perforce.h"

#include <wx/dialog.h>

namespace PerforceUI
{
  class SubmitPanel;

  class SubmitDialog : public wxDialog 
  {
  public:
    SubmitDialog( wxWindow* parent,
      int id = wxID_ANY,
      int changelistNumber = RCS::DefaultChangesetId,
      const tstring& description = TXT( "" ),
      wxString title = wxT("Perforce Change Specification") );

    virtual ~SubmitDialog();

    virtual int ShowModal();
    virtual void EndModal( int retCode = wxID_CANCEL );

    SubmitPanel* GetPanel() const;

  private:
    SubmitPanel* m_Panel;
  };
}