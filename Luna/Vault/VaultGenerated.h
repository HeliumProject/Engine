///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __VaultGenerated__
#define __VaultGenerated__

class Frame;
namespace Luna{ class DirectoryCtrl; }
namespace Luna{ class SortTreeCtrl; }
namespace Nocturnal{ class AutoCompleteComboBox; }
namespace Nocturnal{ class MenuButton; }

#include "Editor/Frame.h"
#include <wx/string.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dirctrl.h>
#include <wx/stattext.h>
#include <wx/radiobut.h>
#include <wx/choice.h>
#include <wx/statbmp.h>
#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#include "VaultPreviewWindow.h"

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class VaultFrameGenerated
	///////////////////////////////////////////////////////////////////////////////
	class VaultFrameGenerated : public Frame
	{
		private:
		
		protected:
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
			
		
		public:
			VaultFrameGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Asset Vault"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			~VaultFrameGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class NavigationPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class NavigationPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnBackButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnForwardButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnUpFolderButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnNavBarComboBox( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnNavBarComboBoxMouseWheel( wxMouseEvent& event ){ event.Skip(); }
			virtual void OnNavBarText( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnNavBarTextEnter( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnGoButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnOptionsButtonClick( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			Nocturnal::MenuButton* m_BackButton;
			Nocturnal::MenuButton* m_ForwardButton;
			wxBitmapButton* m_UpFolderButton;
			Nocturnal::AutoCompleteComboBox* m_NavBarComboBox;
			wxBitmapButton* m_GoButton;
			Nocturnal::MenuButton* m_OptionsButton;
			NavigationPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 557,39 ), long style = wxNO_BORDER|wxTAB_TRAVERSAL );
			~NavigationPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class FoldersPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class FoldersPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnTreeBeginDrag( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeItemDelete( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeEndDrag( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeItemMenu( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTreeItemRightClick( wxTreeEvent& event ){ event.Skip(); }
			
		
		public:
			Luna::DirectoryCtrl* m_FoldersTreeCtrl;
			FoldersPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 205,333 ), long style = wxTAB_TRAVERSAL );
			~FoldersPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ResultsPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class ResultsPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			ResultsPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
			~ResultsPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class SearchPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class SearchPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxScrolledWindow* m_ScrolledWindow;
			wxStaticText* m_staticText44;
			wxStaticBitmap* m_CollectionHelpBitmap;
			
			wxStaticBitmap* m_WordsHelpBitmap;
			wxStaticBitmap* m_PhraseHelpBitmap;
			
			wxStaticBitmap* m_FileIDHelpBitmap;
			wxStaticText* m_staticText151;
			wxStaticBitmap* m_ComponentHelpBitmap;
			wxStaticBitmap* m_LevelHelpBitmap;
			wxStaticBitmap* m_ShaderHelpBitmap;
			
			wxStaticLine* m_staticline3;
			
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnMouseLeftDoubleClick( wxMouseEvent& event ){ event.Skip(); }
			virtual void OnMouseLeftDown( wxMouseEvent& event ){ event.Skip(); }
			virtual void OnMouseRightDown( wxMouseEvent& event ){ event.Skip(); }
			virtual void OnFieldTextEnter( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnFolderBrowseButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnFieldText( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnSearchButtonClick( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnCancelButtonClick( wxCommandEvent& event ){ event.Skip(); }
			
		
		public:
			wxRadioButton* m_SearchVaultRadio;
			wxRadioButton* m_SearchCollectionRadio;
			wxChoice* m_CollectionChoice;
			wxTextCtrl* m_WordsTextCtrl;
			wxTextCtrl* m_PhraseTextCtrl;
			wxChoice* m_FileTypeChoice;
			wxChoice* m_FolderChoice;
			wxButton* m_VaultFoldersButton;
			wxComboBox* m_CreatedByComboBox;
			wxChoice* m_AssetTypeChoice;
			wxTextCtrl* m_FileIDTextCtrl;
			wxChoice* m_ComponentNameChoice;
			wxTextCtrl* m_ComponentValueTextCtrl;
			wxTextCtrl* m_LevelTextCtrl;
			wxTextCtrl* m_ShaderTextCtrl;
			wxButton* m_GoButton;
			wxButton* m_CancelButton;
			wxButton* m_ClearButton;
			SearchPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 467,482 ), long style = wxTAB_TRAVERSAL );
			~SearchPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class CollectionsPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class CollectionsPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxToolBar* m_MyCollectionsToolBar;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnSizeCollectionsPanel( wxSizeEvent& event ){ event.Skip(); }
			virtual void OnMyCollectionsTitleMenu( wxMouseEvent& event ){ event.Skip(); }
			virtual void OnNewCollection( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnMyCollectionsBeginLabelEdit( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnMyCollectionsEndLabelEdit( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnMyCollectionsDoubleClick( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnMyCollectionsMenu( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTempCollectionsDoubleClick( wxTreeEvent& event ){ event.Skip(); }
			virtual void OnTempCollectionsMenu( wxTreeEvent& event ){ event.Skip(); }
			
		
		public:
			wxStaticText* m_MyCollectionsTitle;
			wxBitmapButton* m_NewDependencyCollectionButton;
			wxBitmapButton* m_NewCollectionButton;
			Luna::SortTreeCtrl* m_MyCollectionsTreeCtrl;
			wxStaticText* m_TempCollectionsTitle;
			Luna::SortTreeCtrl* m_TempCollectionsTreeCtrl;
			CollectionsPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,397 ), long style = wxTAB_TRAVERSAL );
			~CollectionsPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class PreviewPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class PreviewPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxPanel* m_BottomPanel;
		
		public:
			Luna::VaultPreviewWindow* m_PreviewWindow;
			wxTextCtrl* m_Label;
			PreviewPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDOUBLE_BORDER|wxTAB_TRAVERSAL );
			~PreviewPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class DetailsFrameGenerated
	///////////////////////////////////////////////////////////////////////////////
	class DetailsFrameGenerated : public wxFrame 
	{
		private:
		
		protected:
			wxScrolledWindow* m_ScrollWindow;
			wxStaticText* m_LabelName;
			wxTextCtrl* m_Name;
			wxStaticText* m_LabelFileType;
			wxTextCtrl* m_FileType;
			wxStaticText* m_LabelFileID;
			wxTextCtrl* m_FileID;
			wxStaticText* m_LabelFolder;
			wxTextCtrl* m_Folder;
			wxPanel* m_RevisionPanel;
			wxStaticText* m_LabelPerforce;
			wxStaticText* m_LabelRevisionStatus;
			wxPanel* m_StatusPanel;
			wxStaticBitmap* m_RevisionStatusIcon;
			wxStaticText* m_RevisionStatus;
			wxPanel* m_LastCheckInPanel;
			wxStaticText* m_LabelLastCheckIn;
			wxTextCtrl* m_LastCheckIn;
			wxPanel* m_FirstCheckInPanel;
			wxStaticText* m_LabelFirstCheckIn;
			wxTextCtrl* m_FirstCheckIn;
		
		public:
			DetailsFrameGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,421 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxFRAME_TOOL_WINDOW|wxRESIZE_BORDER|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
			~DetailsFrameGenerated();
		
	};
	
} // namespace Luna

#endif //__VaultGenerated__
