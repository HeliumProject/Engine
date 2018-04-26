///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PERFORCEGENERATED_H__
#define __PERFORCEGENERATED_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
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

namespace Helium
{
	namespace Editor
	{
		///////////////////////////////////////////////////////////////////////////////
		/// Class PerforceSubmitPanelGenerated
		///////////////////////////////////////////////////////////////////////////////
		class PerforceSubmitPanelGenerated : public wxPanel 
		{
			private:
			
			protected:
				enum
				{
					SelectAll = 1000,
					UnselectAll,
					SpecNotes
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
				
				PerforceSubmitPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 550,400 ), long style = wxTAB_TRAVERSAL ); 
				~PerforceSubmitPanelGenerated();
			
		};
		
		///////////////////////////////////////////////////////////////////////////////
		/// Class PerforceWaitPanelGenerated
		///////////////////////////////////////////////////////////////////////////////
		class PerforceWaitPanelGenerated : public wxPanel 
		{
			private:
			
			protected:
			
			public:
				wxGauge* m_Gauge;
				wxButton* m_CancelButton;
				
				PerforceWaitPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 260,40 ), long style = wxTAB_TRAVERSAL ); 
				~PerforceWaitPanelGenerated();
			
		};
		
	} // namespace Editor
} // namespace Helium

#endif //__PERFORCEGENERATED_H__
