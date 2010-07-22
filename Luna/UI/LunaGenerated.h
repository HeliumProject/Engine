///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LunaGenerated__
#define __LunaGenerated__

#include <wx/intl.h>

namespace Luna{ class Frame; }

#include "Frame.h"
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/srchctrl.h>
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/scrolwin.h>
#include <wx/dataview.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////

namespace Luna
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class MainFrameGenerated
	///////////////////////////////////////////////////////////////////////////////
	class MainFrameGenerated : public Luna::Frame
	{
		private:
		
		protected:
			enum
			{
				ID_NewScene = 1000,
				ID_NewEntity,
				ID_Open,
				ID_Close,
				ID_SaveAll,
				ID_Import,
				ID_ImportFromClipboard,
				ID_Export,
				ID_ExportToClipboard,
				ID_Exit,
				ID_SelectAll,
				ID_InvertSelection,
				ID_Parent,
				ID_Unparent,
				ID_Group,
				ID_Ungroup,
				ID_Center,
				ID_Duplicate,
				ID_SmartDuplicate,
				ID_CopyTransform,
				ID_PasteTransform,
				ID_SnapToCamera,
				ID_SnapCameraTo,
				ID_WalkUp,
				ID_WalkDown,
				ID_WalkForward,
				ID_WalkBackward,
				ID_Preferences,
				ID_About,
				ID_Cut,
				ID_Copy,
				ID_Paste,
				ID_Undo,
				ID_Redo,
			};
			
			wxMenuBar* m_MainMenuBar;
			wxMenu* m_MenuFile;
			wxMenu* m_MenuFileNew;
			wxMenu* m_MenuFileOpenRecent;
			wxMenu* m_MenuEdit;
			wxMenu* m_MenuTools;
			wxMenu* m_MenuPanels;
			wxMenu* m_MenuSettings;
			wxMenu* m_MenuHelp;
			wxToolBar* m_MainToolbar;
			wxSearchCtrl* m_ToolVaultSearch;
			wxStatusBar* m_MainStatusBar;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnNewScene( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnNewEntity( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnSaveAll( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnImport( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnExport( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnUndo( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnRedo( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnCut( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnCopy( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnPaste( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnSelectAll( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnInvertSelection( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnDelete( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnParent( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnUnparent( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnGroup( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnUngroup( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnCenter( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnDuplicate( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnSmartDuplicate( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnCopyTransform( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnPasteTransform( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnSnapToCamera( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnSnapCameraTo( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnPickWalk( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnPreferences( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
			
		
		public:
			
			MainFrameGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1280,1024 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			~MainFrameGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class HelpPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class HelpPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxRichTextCtrl* m_HelpRichText;
		
		public:
			
			HelpPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,200 ), long style = wxTAB_TRAVERSAL );
			~HelpPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class LayersPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class LayersPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			enum
			{
				ID_CreateNewLayer = 1000,
				ID_CreateNewLayerFromSelection,
				ID_DeleteSelectedLayers,
				ID_AddSelectionToLayers,
				ID_RemoveSelectionFromLayers,
				ID_SelectLayerMembers,
				ID_SelectLayers,
			};
			
			wxToolBar* m_LayersToolbar;
			wxPanel* m_LayersPanel;
		
		public:
			
			LayersPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
			~LayersPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class VaultPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class VaultPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxPanel* m_NavigationPanel;
			wxBitmapButton* m_BackButton;
			wxBitmapButton* m_ForwardButton;
			wxSearchCtrl* m_SearchCtrl;
			wxBitmapButton* m_OptionsButton;
			wxListBox* m_SourceListBox;
			wxPanel* m_ResultsPanel;
		
		public:
			
			VaultPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 773,453 ), long style = wxTAB_TRAVERSAL );
			~VaultPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class DetailsPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class DetailsPanelGenerated : public wxPanel 
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
			
			DetailsPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,421 ), long style = wxTAB_TRAVERSAL );
			~DetailsPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ToolsPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class ToolsPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxPanel* m_ToolsContainerPanel;
			wxPanel* m_PropertiesPanel;
		
		public:
			
			ToolsPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 218,369 ), long style = wxTAB_TRAVERSAL );
			~ToolsPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class TypesPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class TypesPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			
			TypesPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
			~TypesPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ProjectPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class ProjectPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxDataViewCtrl* m_DataViewCtrl;
		
		public:
			
			ProjectPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
			~ProjectPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class DirectoryPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class DirectoryPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			wxNotebook* m_DirectoryNotebook;
			wxPanel* m_HierarchyPanel;
			wxPanel* m_EntitiesPanel;
			wxPanel* m_TypesPanel;
		
		public:
			
			DirectoryPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
			~DirectoryPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class PropertiesPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class PropertiesPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
		
		public:
			
			PropertiesPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
			~PropertiesPanelGenerated();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class ViewPanelGenerated
	///////////////////////////////////////////////////////////////////////////////
	class ViewPanelGenerated : public wxPanel 
	{
		private:
		
		protected:
			enum
			{
				ID_FrameOrigin = 1000,
				ID_FrameSelected,
				ID_PreviousView,
				ID_NextView,
				ID_HighlightMode,
				ID_CameraOrbit,
				ID_CameraFront,
				ID_CameraSide,
				ID_CameraTop,
				ID_ShowAxes,
				ID_ShowGrid,
				ID_ShowBounds,
				ID_ShowStatistics,
				ID_FrustumCull,
				ID_BackfaceCull,
				ID_ShadingWireframe,
				ID_ShadingMaterial,
				ID_ColorModeScene,
				ID_ColorModeLayer,
				ID_ColorModeNodeType,
				ID_ColorModeScale,
				ID_ColorModeScaleGradient,
			};
			
			wxToolBar* m_ToolbarView;
			wxPanel* m_ViewPanel;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnFrameOrigin( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnFrameSelected( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnViewPrevious( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnViewNext( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnHighlightMode( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnCamera( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnShowAxes( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnShowGrid( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnShowBounds( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnShowStatistics( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnCulling( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnShading( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnColorMode( wxCommandEvent& event ) { event.Skip(); }
			
		
		public:
			
			ViewPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 766,617 ), long style = wxTAB_TRAVERSAL );
			~ViewPanelGenerated();
		
	};
	
} // namespace Luna

#endif //__LunaGenerated__
