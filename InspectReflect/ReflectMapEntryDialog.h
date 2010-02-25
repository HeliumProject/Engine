///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  7 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ReflectMapEntryDialog__
#define __ReflectMapEntryDialog__

#include <wx/wx.h>

#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

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
				ReflectMapEntryDialog( wxWindow* parent, int id = wxID_ANY, wxString title = wxEmptyString, wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_DIALOG_STYLE );
			
		};
		
} // namespace Inspect

#endif //__ReflectMapEntryDialog__
