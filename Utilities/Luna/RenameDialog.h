#pragma once

#include "CommonGenerated.h"
#include "Common/Compiler.h"

namespace Luna
{
  class RenameDialog : public RenameDialogBase 
  {
  public:
    RenameDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Rename"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 330,100 ), long style = wxDEFAULT_DIALOG_STYLE );

  private:
    virtual void OnTextEnter( wxCommandEvent& args ) NOC_OVERRIDE;
  };
}
