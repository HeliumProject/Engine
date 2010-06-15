///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RegionsGenerated__
#define __RegionsGenerated__

namespace Luna{ class SortTreeCtrl; }

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class RegionsPanelBase
	///////////////////////////////////////////////////////////////////////////////
	class RegionsPanelBase : public wxPanel 
	{
		private:
		
		protected:
			wxBoxSizer* m_Sizer;
			Luna::SortTreeCtrl* m_RegionTree;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnBeginLabelEdit( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnEndLabelEdit( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeItemCollapsing( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeItemMenu( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeKeyDown( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeSelChanged( wxTreeEvent& event ){ event.Skip(); }
			
		
		public:
			RegionsPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 416,416 ), long style = wxTAB_TRAVERSAL );
			~RegionsPanelBase();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ChooserDialog
	///////////////////////////////////////////////////////////////////////////////
	class ChooserDialog : public wxDialog 
	{
		private:
		
		protected:
			wxStdDialogButtonSizer* m_ButtonSizer;
			wxButton* m_ButtonSizerOK;
			wxButton* m_ButtonSizerCancel;
		
		public:
			wxCheckListBox* m_ListBox;
			ChooserDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 388,432 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
			~ChooserDialog();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class RegionCreateDialog
	///////////////////////////////////////////////////////////////////////////////
	class RegionCreateDialog : public wxDialog 
	{
		private:
		
		protected:
			
			
			wxStaticText* m_Label;
			
			wxStdDialogButtonSizer* m_sdbSizer3;
			wxButton* m_sdbSizer3OK;
			wxButton* m_sdbSizer3Cancel;
		
		public:
			wxTextCtrl* m_TextCtrl;
			RegionCreateDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Create New Region"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 342,135 ), long style = wxDEFAULT_DIALOG_STYLE );
			~RegionCreateDialog();
		
	};
	
} // namespace Luna

#endif //__RegionsGenerated__
