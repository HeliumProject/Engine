#include "Precompile.h"
#include "RenameDialog.h"

using namespace Luna;

RenameDialog::RenameDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: RenameDialogBase( parent, id, title, pos, size, style )
{
}

void RenameDialog::OnTextEnter( wxCommandEvent& args )
{
  EndModal( wxID_OK );
}
