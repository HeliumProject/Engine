///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CollisionModePanel__
#define __CollisionModePanel__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/scrolwin.h>

///////////////////////////////////////////////////////////////////////////

namespace Maya
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class CollisionModePanel
	///////////////////////////////////////////////////////////////////////////////
	class CollisionModePanel : public wxPanel 
	{
		private:
		
		protected:
			wxScrolledWindow* m_ScrollWindow;
			wxStaticText* m_TitleSettings;
			
			
			wxStaticLine* m_Separator;
			wxStaticText* m_TitleDisplay;
			wxPanel* m_ColorSwatchEmpty;
			wxPanel* m_ColorSwatchCamIgnore;
			
			wxPanel* m_ColorSwatchImpassable;
			wxPanel* m_ColorSwatchNonTrav;
			
			wxPanel* m_ColorSwatchTransparent;
		
		public:
			wxCheckBox* m_CheckCameraIgnore;
			wxCheckBox* m_CheckImpassable;
			wxCheckBox* m_CheckNonTraversable;
			wxCheckBox* m_CheckTransparent;
			wxButton* m_ButtonApply;
			wxButton* m_ButtonClear;
			wxRadioButton* m_RadioOff;
			wxRadioButton* m_RadioCameraIgnore;
			wxRadioButton* m_RadioImpassable;
			wxRadioButton* m_RadioNonTraversable;
			wxRadioButton* m_RadioTransparent;
			CollisionModePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 286,224 ), long style = wxTAB_TRAVERSAL );
			~CollisionModePanel();
		
	};
	
} // namespace Maya

#endif //__CollisionModePanel__
