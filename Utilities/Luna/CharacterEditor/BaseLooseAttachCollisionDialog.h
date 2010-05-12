///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BaseLooseAttachCollisionDialog__
#define __BaseLooseAttachCollisionDialog__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BaseLooseAttachCollisionDialog
///////////////////////////////////////////////////////////////////////////////
class BaseLooseAttachCollisionDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_textUnselectedCollisionVolumes;
		wxButton* m_buttonAddAll;
		wxButton* m_buttonAddSelected;
		wxButton* m_buttonRemoveSelected;
		wxButton* m_buttonRemoveAll;
		wxStaticText* m_textSelectedCollisionVolumes;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAddAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAddSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRemoveSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRemoveAll( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		wxListBox* m_listUnselectedCollisionVolumes;
		wxListBox* m_listSelectedCollisionVolumes;
		BaseLooseAttachCollisionDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Loose Attachment: Collision Selection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 562,412 ), long style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
		~BaseLooseAttachCollisionDialog();
	
};

#endif //__BaseLooseAttachCollisionDialog__
