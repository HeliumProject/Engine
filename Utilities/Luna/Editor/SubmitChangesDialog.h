#pragma once

#include "Common/Compiler.h"


namespace Luna
{
  // Forwards
  class SubmitChangesPanel;

  /////////////////////////////////////////////////////////////////////////////
  // Dialog prompting the user to checkin their default changelist.  Their choice
  // is immediately carried out by this dialog.  Call ShowModal() to kick things
  // off.
  // 
  class SubmitChangesDialog : public wxDialog
  {
  private:
    SubmitChangesPanel* m_Panel;

  public:
    SubmitChangesDialog( wxWindow* parent );
    virtual ~SubmitChangesDialog();
    virtual int ShowModal() NOC_OVERRIDE;

  private:
    void Commit();

  private:
    void OnButtonClicked( wxCommandEvent& args );
  };
}
