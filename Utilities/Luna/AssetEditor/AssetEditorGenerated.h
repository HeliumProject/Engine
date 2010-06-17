///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __AssetEditorGenerated__
#define __AssetEditorGenerated__

namespace Luna{ class ShaderGroupPropGrid; }
namespace Nocturnal{ class AutoCompleteComboBox; }

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/statline.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/treectrl.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/statbmp.h>
#include <wx/statbox.h>
#include <wx/listctrl.h>
#include <wx/radiobut.h>
#include <wx/propgrid/manager.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	#define ID_DEFAULT wxID_ANY // Default
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ReplaceAnimSetsDlg
	///////////////////////////////////////////////////////////////////////////////
	class ReplaceAnimSetsDlg : public wxDialog 
	{
		private:
		
		protected:
			wxStaticText* m_StaticText;
			wxListBox* m_ListBox;
			wxStaticText* m_StaticPrompt;
			wxStdDialogButtonSizer* m_ButtonSizer;
			wxButton* m_ButtonSizerYes;
			wxButton* m_ButtonSizerNo;
		
		public:
			ReplaceAnimSetsDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Replace Existing Animation Sets?"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,240 ), long style = wxDEFAULT_DIALOG_STYLE );
			~ReplaceAnimSetsDlg();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class MultiAnimPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class MultiAnimPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxStaticText* m_staticText5;
			wxChoice* m_MainEntityChoice;
			wxStaticText* m_staticText51;
			wxChoice* m_MainJointChoice;
			wxStaticText* m_staticText11;
			wxChoice* m_AttachEntityChoice;
			wxStaticText* m_staticText111;
			wxChoice* m_AttachJointChoice;
			wxStaticText* m_staticText1;
			Nocturnal::AutoCompleteComboBox* m_AnimClipCombo;
			wxStaticLine* m_staticline1;
			
			wxBitmapButton* m_ButtonSnapBegin;
			wxBitmapButton* m_ButtonPlayPause;
			wxBitmapButton* m_ButtonSnapEnd;
			
			wxSlider* m_TimeSlider;
			wxPanel* m_panel1;
			wxBitmapButton* m_ButtonEnableJointAttach;
			wxBitmapButton* m_ButtonForceLoop;
			wxBitmapButton* m_ButtonFrameEntities;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnMainEntityChoice( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnMainJointChoice( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnAttachEntityChoice( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnAttachJointChoice( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnAnimClipChoice( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnAnimClipTextChoice( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnButtonSnapBegin( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnButtonPlayPause( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnButtonSnapEnd( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnScroll( wxScrollEvent& event ){ event.Skip(); }
			virtual void OnButtonEnableJointAttach( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnButtonForceLoop( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnButtonFrameEntities( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			MultiAnimPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 610,216 ), long style = wxTAB_TRAVERSAL );
			~MultiAnimPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class AnimGroupPanel
	///////////////////////////////////////////////////////////////////////////////
	class AnimGroupPanel : public wxPanel 
	{
		private:
		
		protected:
			wxStaticText* m_StaticDescription;
			wxStaticText* m_StaticCategory;
		
		public:
			wxChoice* m_Category;
			wxPropertyGrid* m_PropertyGrid;
			AnimGroupPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 445,300 ), long style = wxTAB_TRAVERSAL );
			~AnimGroupPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ComponentChooserPanel
	///////////////////////////////////////////////////////////////////////////////
	class ComponentChooserPanel : public wxPanel 
	{
		private:
		
		protected:
			wxStaticLine* m_StaticLine;
			
			wxButton* m_ButtonOk;
			wxButton* m_ButtonCancel;
		
		public:
			wxTreeCtrl* m_TreeCtrl;
			wxTextCtrl* m_Description;
			ComponentChooserPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 380,375 ), long style = wxTAB_TRAVERSAL );
			~ComponentChooserPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ComponentCategoriesPanel
	///////////////////////////////////////////////////////////////////////////////
	class ComponentCategoriesPanel : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			wxNotebook* m_Tabs;
			wxPanel* m_MessagePanel;
			wxStaticBitmap* m_Icon;
			wxTextCtrl* m_Message;
			ComponentCategoriesPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 332,350 ), long style = wxTAB_TRAVERSAL );
			~ComponentCategoriesPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ComponentCategoryPanel
	///////////////////////////////////////////////////////////////////////////////
	class ComponentCategoryPanel : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			wxStaticText* m_ShortDescription;
			wxListCtrl* m_ListCtrl;
			ComponentCategoryPanel( wxWindow* parent, wxWindowID id = ID_DEFAULT, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 430,400 ), long style = wxTAB_TRAVERSAL );
			~ComponentCategoryPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class PromptNewExistingPanel
	///////////////////////////////////////////////////////////////////////////////
	class PromptNewExistingPanel : public wxPanel 
	{
		private:
		
		protected:
			
		
		public:
			wxStaticText* m_Description;
			wxRadioButton* m_RadioBtnExisting;
			wxTextCtrl* m_FilePathExisting;
			wxBitmapButton* m_ButtonExistingFinder;
			wxButton* m_ButtonExisting;
			wxRadioButton* m_RadioBtnNew;
			wxTextCtrl* m_FilePathNew;
			PromptNewExistingPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
			~PromptNewExistingPanel();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class SwapShadersPanel
	///////////////////////////////////////////////////////////////////////////////
	class SwapShadersPanel : public wxPanel 
	{
		private:
		
		protected:
			wxStaticText* m_StaticTextChoice;
			wxStaticText* m_StaticTextInfo;
			
			
			wxStdDialogButtonSizer* m_ButtonSizer;
			wxButton* m_ButtonSizerOK;
			wxButton* m_ButtonSizerCancel;
		
		public:
			wxChoice* m_DefaultGroup;
			wxButton* m_ButtonNew;
			wxButton* m_ButtonRemove;
			wxStaticBitmap* m_Thumbnail;
			Luna::ShaderGroupPropGrid* m_PropertyGrid;
			SwapShadersPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 447,300 ), long style = wxTAB_TRAVERSAL );
			~SwapShadersPanel();
		
	};
	
} // namespace Luna

#endif //__AssetEditorGenerated__
