///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LSearchPanel__
#define __LSearchPanel__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/checklst.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/scrolwin.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class LSearchPanel
	///////////////////////////////////////////////////////////////////////////////
	class LSearchPanel : public wxPanel 
	{
		private:
		
		protected:
			wxScrolledWindow* m_ScrollWindow;
			wxNotebook* m_Tabs;
			wxPanel* m_TextPanel;
			wxStaticText* m_StaticName;
			wxChoice* m_SearchOption;
			wxTextCtrl* m_SearchText;
			wxStaticText* m_StaticEngineType;
			
			wxComboBox* m_EngineType;
			wxCheckBox* m_SearchAllZones;
			wxPanel* m_AttributesPanel;
			wxStaticText* m_staticText8;
			wxCheckListBox* m_Attributes;
			wxPanel* m_BoundsPanel;
			wxStaticText* m_StaticBounds;
			wxChoice* m_BoundsOption;
			wxChoice* m_BoundsLessOrGreater;
			wxStaticText* m_staticText9;
			wxTextCtrl* m_BoundsGreaterThan;
			
			wxStaticText* m_staticText10;
			wxTextCtrl* m_BoundsLessThan;
			wxButton* m_SearchButton;
			wxStaticText* m_Status;
			wxListCtrl* m_Results;
			wxButton* m_SelectButton;
		
		public:
			LSearchPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 360,315 ), long style = wxTAB_TRAVERSAL );
			~LSearchPanel();
		
	};
	
} // namespace Luna

#endif //__LSearchPanel__
