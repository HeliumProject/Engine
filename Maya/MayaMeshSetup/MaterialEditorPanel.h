///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __MaterialEditorPanel__
#define __MaterialEditorPanel__

namespace UIToolKit{ class AutoCompleteComboBox; }

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/scrolwin.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

namespace Maya
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class MaterialEditorPanel
	///////////////////////////////////////////////////////////////////////////////
	class MaterialEditorPanel : public wxPanel 
	{
		private:
		
		protected:
			wxStaticText* m_Heading1;
			wxStaticText* m_StaticVisual;
			wxStaticText* m_StaticAcoustic;
			wxStaticText* m_StaticPhysics;
			wxStaticLine* m_Line;
			wxStaticText* m_Heading2;
			wxRadioBox* m_radioBox2;
			wxStaticText* m_Heading11;
			wxStaticText* m_StaticVisual1;
			wxStaticLine* m_Line1;
			wxStaticText* m_Heading21;
			wxRadioBox* m_radioBox21;
			wxStaticText* m_Heading111;
			wxStaticText* m_StaticVisual11;
			wxStaticLine* m_Line11;
			wxStaticText* m_Heading211;
			wxRadioBox* m_radioBox211;
			wxStaticText* m_Heading112;
			wxStaticText* m_StaticVisual12;
			wxStaticLine* m_Line12;
			wxStaticText* m_Heading212;
			wxRadioBox* m_radioBox212;
			wxStaticText* m_TextDisplayOff;
		
		public:
			wxNotebook* m_Tabs;
			wxPanel* m_AllTab;
			UIToolKit::AutoCompleteComboBox* m_ComboVisual;
			UIToolKit::AutoCompleteComboBox* m_ComboAcoustic;
			UIToolKit::AutoCompleteComboBox* m_ComboPhysics;
			wxRadioButton* m_RadioOff;
			wxRadioButton* m_RadioVisual;
			wxRadioButton* m_RadioAcoustic;
			wxRadioButton* m_RadioPhysics;
			wxPanel* m_VisualTab;
			UIToolKit::AutoCompleteComboBox* m_VisualTabCombo;
			wxRadioButton* m_RadioVisualTabOff;
			wxRadioButton* m_RadioVisualTabOn;
			wxPanel* m_AcousticTab;
			UIToolKit::AutoCompleteComboBox* m_AcousticTabCombo;
			wxRadioButton* m_RadioAcousticTabOff;
			wxRadioButton* m_RadioAcousticTabOn;
			wxPanel* m_PhysicsTab;
			UIToolKit::AutoCompleteComboBox* m_PhysicsTabCombo;
			wxRadioButton* m_RadioPhysicsTabOff;
			wxRadioButton* m_RadioPhysicsTabOn;
			wxButton* m_ButtonApply;
			wxButton* m_ButtonSelect;
			wxButton* m_ButtonClear;
			wxScrolledWindow* m_KeyScrollWindow;
			MaterialEditorPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 525,375 ), long style = wxTAB_TRAVERSAL );
			~MaterialEditorPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class TempMaterialEditorFrame
	///////////////////////////////////////////////////////////////////////////////
	class TempMaterialEditorFrame : public wxFrame 
	{
		private:
		
		protected:
			wxPanel* m_Panel;
			wxPanel* colorBox;
			wxStaticText* rowName;
			wxStatusBar* m_StatusBar;
		
		public:
			TempMaterialEditorFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Material Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,284 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
			~TempMaterialEditorFrame();
		
	};
	
} // namespace Maya

#endif //__MaterialEditorPanel__
