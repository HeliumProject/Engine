///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LiveGenerated__
#define __LiveGenerated__

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class LivePanel
	///////////////////////////////////////////////////////////////////////////////
	class LivePanel : public wxPanel 
	{
		private:
		
		protected:
			wxStaticText* m_staticText2;
			wxStaticText* m_staticText21;
			wxStaticLine* m_staticline1;
			wxStaticLine* m_staticline11;
		
		public:
			wxListbook* m_listbook1;
			wxPanel* m_GeneralPanel;
			wxButton* m_ButtonStartUberview;
			wxButton* m_ButtonResetConnection;
			wxTextCtrl* m_StatusTextControl;
			wxTextCtrl* m_BackTraceText;
			wxPanel* m_OptionsPanel;
			wxCheckBox* m_AutoStartUberviewCheckbox;
			wxCheckBox* m_ForceAutoStartUberviewCheckbox;
			wxCheckBox* m_AutoDisplayWindowCheckbox;
			wxPanel* m_TargetsPanel;
			wxListBox* m_TargetList;
			wxButton* m_ButtonRefreshTargetList;
			wxPanel* m_LightingPanel;
			wxListBox* m_LightingEnvironmentList;
			wxButton* m_ButtonRefreshLightingList;
			wxTextCtrl* m_FilterText;
			LivePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 426,324 ), long style = wxTAB_TRAVERSAL );
			~LivePanel();
		
	};
	
} // namespace Luna

#endif //__LiveGenerated__
