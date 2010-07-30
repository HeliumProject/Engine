///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ReflectMapEntryDialog__
#define __ReflectMapEntryDialog__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

namespace Helium
{
	namespace Inspect
	{
		
		///////////////////////////////////////////////////////////////////////////////
		/// Class ReflectMapEntryDialog
		///////////////////////////////////////////////////////////////////////////////
		class ReflectMapEntryDialog : public wxDialog 
		{
			private:
			
			protected:
				wxPanel* m_Panel;
				wxStaticText* m_StaticKey;
				wxStaticText* m_StaticValue;
				wxStdDialogButtonSizer* m_ButtonSizer;
				wxButton* m_ButtonSizerOK;
				wxButton* m_ButtonSizerCancel;
			
			public:
				wxTextCtrl* m_Key;
				wxTextCtrl* m_Value;
				
				ReflectMapEntryDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
				~ReflectMapEntryDialog();
			
		};
		
	} // namespace Inspect
} // namespace Helium

#endif //__ReflectMapEntryDialog__
