///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __typediag__
#define __typediag__

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TypeDialog
///////////////////////////////////////////////////////////////////////////////
class TypeDialog : public wxDialog 
{
	private:
	
	protected:
		wxButton* m_ok;
		wxButton* m_cancel;
	
	public:
		wxTextCtrl* type;
		TypeDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 197,90 ), long style = wxDEFAULT_DIALOG_STYLE );
		~TypeDialog();
	
};

#endif //__typediag__
