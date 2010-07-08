///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PerforcePanels__
#define __PerforcePanels__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checklst.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/gauge.h>

///////////////////////////////////////////////////////////////////////////

namespace PerforceUI
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class GeneratedSubmitPanel
	///////////////////////////////////////////////////////////////////////////////
	class GeneratedSubmitPanel : public wxPanel 
	{
		private:
		
		protected:
			enum
			{
				SelectAll = 1000,
				UnselectAll,
				SpecNotes,
			};
			
			wxPanel* m_TitlePanel;
			wxStaticText* m_TitleStaticText;
			wxStaticText* m_PanelDescriptionStaticText;
			wxPanel* m_ClientDetailsPanel;
			wxStaticText* m_ChangeStaticTextTitle;
			wxStaticText* m_ClientStaticTextTitle;
			wxStaticText* m_UserStaticTextTitle;
			
			wxStaticText* m_ChangeStaticText;
			wxStaticText* m_ClientStaticText;
			wxStaticText* m_UserStaticText;
			
			
			wxStaticText* m_DateStaticTextTitle;
			wxStaticText* m_StatusStaticTextTitle;
			
			wxStaticText* m_DateStaticText;
			wxStaticText* m_StatusStaticText;
			wxStaticText* m_DescriptionStaticText;
			
			wxTextCtrl* m_DescriptionTextCtrl;
			wxPanel* m_JobStatusPanel;
			
			wxStaticText* m_JobStatusStaticText;
			wxChoice* m_JobStatusChoice;
			wxStaticText* m_FilesStaticText;
			
			wxCheckListBox* m_FileCheckList;
			wxListBox* m_FileListBox;
			
			wxPanel* m_SelectAllButtonsPanel;
			wxButton* m_SelectAllButton;
			
			wxButton* m_UnselectButton;
			
			wxStaticText* m_RequiredFieldsStaticText;
			wxPanel* m_CommitButtonsPanel;
			wxCheckBox* m_ReopenCheckBox;
			wxButton* m_SubmitButton;
			wxButton* m_UpdateButton;
			wxButton* m_CancelButton;
			wxButton* m_SpecNoteButton;
			wxButton* m_HelpButton;
		
		public:
			GeneratedSubmitPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 550,400 ), long style = wxTAB_TRAVERSAL );
			~GeneratedSubmitPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class GeneratedWaitPanel
	///////////////////////////////////////////////////////////////////////////////
	class GeneratedWaitPanel : public wxPanel 
	{
		private:
		
		protected:
			
			
			
			
			
			
		
		public:
			wxGauge* m_Gauge;
			wxButton* m_CancelButton;
			GeneratedWaitPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 260,40 ), long style = wxTAB_TRAVERSAL );
			~GeneratedWaitPanel();
		
	};
	
} // namespace PerforceUI

#endif //__PerforcePanels__
