#pragma once

#include "RCS/RCS.h"
#include "Perforce/Perforce.h"

#include <wx/dialog.h>

namespace PerforceUI
{
  class Panel;

  class Dialog : public wxDialog 
  {
  public:
    Dialog( wxWindow* parent,
      int id = wxID_ANY,
      int changelistNumber = RCS::DefaultChangesetId,
      const std::string& description = std::string( "" ),
      wxString title = wxT("Perforce Change Specification") );

    virtual ~Dialog();

    virtual int ShowModal();
    virtual void EndModal( int retCode = wxID_CANCEL );

    Panel* GetPanel() const;

  private:
    Panel* m_Panel;
  };
}