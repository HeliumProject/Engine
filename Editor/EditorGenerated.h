///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EditorGenerated__
#define __EditorGenerated__

#include <wx/intl.h>

class wxBitmapToggleButton;
namespace Helium{ namespace Editor{ class Frame; } }

#include "Editor/Frame.h"
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/scrolwin.h>
#include <wx/dataview.h>
#include <wx/notebook.h>
#include <wx/radiobut.h>

///////////////////////////////////////////////////////////////////////////

namespace Helium
{
	namespace Editor
	{
		///////////////////////////////////////////////////////////////////////////////
		/// Class MainFrameGenerated
		///////////////////////////////////////////////////////////////////////////////
		class MainFrameGenerated : public Helium::Editor::Frame
		{
			private:
			
			protected:
				enum
				{
					ID_NewScene = 1000,
					ID_NewEntity,
					ID_NewProject,
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
					ID_Settings,
					ID_About,
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
				wxStatusBar* m_MainStatusBar;
				
				// Virtual event handlers, overide them in your derived class
				virtual void OnNewScene( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnNewEntity( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnNewProject( wxCommandEvent& event ) { event.Skip(); }
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
				virtual void OnSettings( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
				
			
			public:
				
				MainFrameGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1280,1024 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
				~MainFrameGenerated();
			
		};
		
		///////////////////////////////////////////////////////////////////////////////
		/// Class ToolbarPanelGenerated
		///////////////////////////////////////////////////////////////////////////////
		class ToolbarPanelGenerated : public wxPanel 
		{
			private:
			
			protected:
				wxPanel* m_MainPanel;
				wxBitmapButton* m_NewSceneButton;
				wxBitmapButton* m_OpenButton;
				wxBitmapButton* m_SaveAllButton;
				wxStaticLine* m_staticline1;
				wxBitmapButton* m_CutButton;
				wxBitmapButton* m_CopyButton;
				wxBitmapButton* m_PasteButton;
				wxBitmapButton* m_DeleteButton;
				wxStaticLine* m_staticline2;
				wxBitmapButton* m_UndoButton;
				wxBitmapButton* m_RedoButton;
				
				wxPanel* m_ToolsPanel;
				wxBitmapButton* m_PlayButton;
				wxBitmapButton* m_PauseButton;
				wxBitmapButton* m_StopButton;
				
				wxPanel* m_ToolsPropertiesPanel;
				
				// Virtual event handlers, overide them in your derived class
				virtual void OnNewScene( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnSaveAll( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnCut( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnCopy( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnPaste( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnDelete( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnUndo( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnRedo( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnSearchButtonClick( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnSearchTextEnter( wxCommandEvent& event ) { event.Skip(); }
				
			
			public:
				wxSearchCtrl* m_VaultSearchBox;
				
				ToolbarPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 939,70 ), long style = wxTAB_TRAVERSAL );
				~ToolbarPanelGenerated();
			
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
				wxPanel* m_LayerManagementPanel;
				wxBitmapButton* m_CreateNewLayerFromSelectionButton;
				wxBitmapButton* m_CreateNewLayerButton;
				wxBitmapButton* m_DeleteSelectedLayersButton;
				
				wxBitmapButton* m_AddSelectionToLayerButton;
				wxBitmapButton* m_RemoveSelectionFromLayerButton;
				
				wxBitmapButton* m_SelectLayerMembersButton;
				wxBitmapButton* m_SelectLayersButton;
				
				// Virtual event handlers, overide them in your derived class
				virtual void OnNewLayerFromSelection( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnNewLayer( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnDeleteLayer( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnAddSelectionToLayer( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnRemoveSelectionFromLayer( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnSelectLayerMembers( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnSelectLayer( wxCommandEvent& event ) { event.Skip(); }
				
			
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
				wxSearchCtrl* m_SearchCtrl;
				wxBitmapButton* m_OptionsButton;
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
				wxPanel* m_ProjectManagementPanel;
				wxBitmapButton* m_AddFile;
				wxBitmapButton* m_CreateFolder;
				wxBitmapButton* m_Delete;
				wxDataViewCtrl* m_DataViewCtrl;
				
				// Virtual event handlers, overide them in your derived class
				virtual void OnAddFile( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnCreateFolder( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnDelete( wxCommandEvent& event ) { event.Skip(); }
				
			
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
				wxPanel* m_ControlsPanel;
				wxRadioButton* m_CommonButton;
				wxRadioButton* m_AllButton;
				wxStaticLine* m_staticline5;
				wxButton* m_ExpandAllButton;
				wxButton* m_CollapseAllButton;
				wxPanel* m_PropertiesPanel;
				
				// Virtual event handlers, overide them in your derived class
				virtual void OnIntersection( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnUnion( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnExpandAll( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnCollapseAll( wxCommandEvent& event ) { event.Skip(); }
				
			
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
				wxPanel* m_ToolPanel;
				wxBitmapButton* m_FrameOriginButton;
				wxBitmapButton* m_FrameSelectedButton;
				
				wxBitmapButton* m_PreviousViewButton;
				wxBitmapButton* m_NextViewButton;
				
				wxBitmapToggleButton* m_HighlightModeToggleButton;
				
				wxBitmapToggleButton* m_OrbitCameraToggleButton;
				wxBitmapToggleButton* m_FrontCameraToggleButton;
				wxBitmapToggleButton* m_SideCameraToggleButton;
				wxBitmapToggleButton* m_TopCameraToggleButton;
				
				wxBitmapToggleButton* m_ShowAxesToggleButton;
				wxBitmapToggleButton* m_ShowGridToggleButton;
				wxBitmapToggleButton* m_ShowBoundsToggleButton;
				wxBitmapToggleButton* m_ShowStatisticsToggleButton;
				
				wxBitmapToggleButton* m_FrustumCullingToggleButton;
				wxBitmapToggleButton* m_BackfaceCullingToggleButton;
				
				wxBitmapToggleButton* m_WireframeShadingToggleButton;
				wxBitmapToggleButton* m_MaterialShadingToggleButton;
				
				wxBitmapToggleButton* m_ColorModeSceneToggleButton;
				wxBitmapToggleButton* m_ColorModeLayerToggleButton;
				wxBitmapToggleButton* m_ColorModeNodeTypeToggleButton;
				wxBitmapToggleButton* m_ColorModeScaleToggleButton;
				wxBitmapToggleButton* m_ColorModeScaleGradientToggleButton;
				wxPanel* m_ViewContainerPanel;
				
				// Virtual event handlers, overide them in your derived class
				virtual void OnFrameOrigin( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnFrameSelected( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnPreviousView( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnNextView( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnHighlightMode( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnViewCameraChange( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnViewChange( wxCommandEvent& event ) { event.Skip(); }
				virtual void OnColorMode( wxCommandEvent& event ) { event.Skip(); }
				
			
			public:
				
				ViewPanelGenerated( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 826,617 ), long style = wxTAB_TRAVERSAL );
				~ViewPanelGenerated();
			
		};
		
	} // namespace Editor
} // namespace Helium

#endif //__EditorGenerated__
