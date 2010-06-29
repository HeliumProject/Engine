///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CommonGenerated__
#define __CommonGenerated__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class RenameDialogBase
	///////////////////////////////////////////////////////////////////////////////
	class RenameDialogBase : public wxDialog 
	{
		private:
		
		protected:
			wxStaticText* m_StaticText;
			
			wxButton* m_OK;
			wxButton* m_Cancel;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnTextEnter( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			wxTextCtrl* m_Text;
			RenameDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Rename"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 330,100 ), long style = wxDEFAULT_DIALOG_STYLE );
			~RenameDialogBase();
		
	};
	
} // namespace Luna

#endif //__CommonGenerated__
