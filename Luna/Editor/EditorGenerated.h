///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EditorGenerated__
#define __EditorGenerated__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/treectrl.h>
#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	#define ID_DEFAULT wxID_ANY // Default
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class SubmitChangesPanel
	///////////////////////////////////////////////////////////////////////////////
	class SubmitChangesPanel : public wxPanel 
	{
		private:
		
		protected:
			wxStaticText* m_StaticText;
			wxStaticText* m_StaticQuesion;
			
			
		
		public:
			wxListBox* m_ListBoxFiles;
			wxButton* m_ButtonYes;
			wxButton* m_ButtonNo;
			wxButton* m_ButtonCancel;
			SubmitChangesPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 340,250 ), long style = wxTAB_TRAVERSAL );
			~SubmitChangesPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ClassChooserPanel
	///////////////////////////////////////////////////////////////////////////////
	class ClassChooserPanel : public wxPanel 
	{
		private:
		
		protected:
			wxTextCtrl* m_DirectionsText;
			wxStaticText* m_Label;
			wxStaticLine* m_StaticLine;
			
			wxStdDialogButtonSizer* m_Buttons;
			wxButton* m_ButtonsOK;
			wxButton* m_ButtonsCancel;
		
		public:
			wxChoice* m_Dropdown;
			wxTreeCtrl* m_Tree;
			ClassChooserPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 460,500 ), long style = wxTAB_TRAVERSAL );
			~ClassChooserPanel();
		
	};
	
} // namespace Luna

#endif //__EditorGenerated__
