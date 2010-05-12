#pragma once
#include "baselooseattachcollisiondialog.h"

class LooseAttachCollisionDialog :
  public BaseLooseAttachCollisionDialog
{
public:
  LooseAttachCollisionDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Loose Attachment: Collision Selection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 562,412 ), long style = wxDEFAULT_DIALOG_STYLE )
    : BaseLooseAttachCollisionDialog( parent, id, title, pos, size, style )
  {

  }

  ~LooseAttachCollisionDialog()
  {

  }

protected:

	// Virtual event handlers, overide them in your derived class
	virtual void OnAddAll( wxCommandEvent& event );
	virtual void OnAddSelected( wxCommandEvent& event );
	virtual void OnRemoveSelected( wxCommandEvent& event );
	virtual void OnRemoveAll( wxCommandEvent& event );

};
