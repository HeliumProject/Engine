///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SceneLiveGenerated__
#define __SceneLiveGenerated__

#include <wx/string.h>
#include <wx/tglbtn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbox.h>
#include <wx/radiobut.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class InnerLiveLinkPanel
	///////////////////////////////////////////////////////////////////////////////
	class InnerLiveLinkPanel : public wxPanel 
	{
		private:
		
		protected:
			wxNotebook* m_notebook1;
			wxPanel* m_panel2;
			wxStaticText* m_staticText2;
			wxPanel* m_panel1;
			wxStaticText* m_staticText1;
		
		public:
			wxToggleButton* m_ToggleLiveSync;
			wxToggleButton* m_ToggleLiveGameSync;
			wxToggleButton* m_ToggleCameraSync;
			wxToggleButton* m_ToggleLightingSync;
			wxButton* m_ButtonBuildAll;
			wxButton* m_ButtonBuildCollisionDb;
			wxButton* m_ButtonBuildStaticDb;
			wxButton* m_ButtonBuildLightingData;
			wxTextCtrl* m_TextLinkStatus;
			wxButton* m_ButtonScreenshot;
			wxListBox* m_SimulatedMobys;
			wxButton* m_ButtonAdd;
			wxToggleButton* m_ToggleSimulate;
			wxRadioButton* m_RadioAll;
			wxRadioButton* m_RadioSelected;
			wxRadioButton* m_RadioHighlighted;
			wxButton* m_ButtonRemove;
			wxButton* m_ButtonSync;
			wxButton* m_ButtonReset;
			InnerLiveLinkPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 263,310 ), long style = wxTAB_TRAVERSAL );
			~InnerLiveLinkPanel();
		
	};
	
} // namespace Luna

#endif //__SceneLiveGenerated__
