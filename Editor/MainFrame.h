#pragma once

#include "EditorScene/SettingsManager.h"
#include "EditorScene/PropertiesManager.h"
#include "EditorScene/Scene.h"
#include "EditorScene/SceneManager.h"

#include "Editor/Vault/VaultPanel.h"
#include "Editor/DragDrop/DropTarget.h"

#include "Editor/MRU/MenuMRU.h"
#include "Editor/TreeMonitor.h"
#include "Editor/MessageDisplayer.h"
#include "Editor/Dialogs/FileDialogDisplayer.h"

#include "Editor/EditorGeneratedWrapper.h"
#include "Editor/HierarchyPanel.h"
#include "Editor/HelpPanel.h"
#include "Editor/ProjectPanel.h"
#include "Editor/LayersPanel.h"
#include "Editor/PropertiesPanel.h"
#include "Editor/ToolbarPanel.h"
#include "Editor/ViewPanel.h"
#include "Editor/Inspect/TreeCanvas.h"
#include "Editor/Inspect/StripCanvas.h"

namespace Helium
{
	namespace Editor
	{
		typedef std::map< int32_t, int32_t > M_IDToColorMode; // Maps wx ID for menu items to our ViewColorMode enum

		class MainFrame : public MainFrameGenerated
		{
		protected:
			enum
			{
				ID_MenuOpened = wxID_HIGHEST + 1,
			};

		private:
			struct OutlinerStates
			{
				SceneOutlinerState m_Hierarchy;
				SceneOutlinerState m_Entities;
				SceneOutlinerState m_Types;
			};

			typedef std::map< Editor::Scene*, OutlinerStates > M_OutlinerStates;

		public:
			MainFrame( SettingsManager* settingsManager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1280,1024 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			virtual ~MainFrame();

			bool Initialize();

			void SetHelpText( const char* text ) override;

			void OpenProject( const Helium::FilePath& path );
			void CloseProject();
			const FilePath& GetProject()
			{
				return m_Project;
			}

			FilePath NewSceneDialog();

			void NewProjectDialog();
			void OpenProjectDialog();

			void OpenScene( SceneDefinition &sceneDefinition );
			void CloseAllScenes();

			Editor::SceneManager& GetSceneManager()
			{
				return m_SceneManager;
			}

			DocumentManager* GetDocumentManager()
			{
				return &m_DocumentManager;
			}

		public:
			void InvertSelection();

			bool SaveAll( std::string& error );

			void SelectionChanged( const Editor::SelectionChangeArgs& selection );


		private:
			// Stores information about the state of each outliner for each scene
			// that is open.  Restores the state when switching between scenes.
			M_OutlinerStates					m_OutlinerStates;

			HelpPanel*							m_HelpPanel;
			ProjectPanel*						m_ProjectPanel;
			LayersPanel*						m_LayersPanel;
			ViewPanel*							m_ViewPanel;
			wxPanel*							m_LogoPanel;
			ToolbarPanel*						m_ToolbarPanel;
			HierarchyPanel*						m_HierarchyPanel;
			PropertiesPanel*					m_PropertiesPanel;
			VaultPanel*							m_VaultPanel;

			FilePath							m_Project;

			MessageDisplayer					m_MessageDisplayer;
			FileDialogDisplayer					m_FileDialogDisplayer;
			DocumentManager						m_DocumentManager;
			SceneManager						m_SceneManager;
			SettingsManager*					m_SettingsManager;

			MenuMRUPtr							m_MenuMRU;
			M_IDToColorMode						m_ColorModeLookup;
			V_HierarchyNodeDumbPtr				m_OrderedContextItems;

			TreeMonitor							m_TreeMonitor;

		private:
			bool ValidateDrag( const Editor::DragArgs& args );
			void DragOver( const Editor::DragArgs& args );
			void Drop( const Editor::DragArgs& args );

			void SceneAdded( const Editor::SceneChangeArgs& args );
			void SceneRemoving( const Editor::SceneChangeArgs& args );
			void SceneLoadFinished( const Editor::LoadArgs& args );
			void SceneExecuted( const Editor::ExecuteArgs& args );

			bool DoOpen( const std::string& path );

		private:
			void OnOpen( wxCommandEvent& event ) override;
			void OnMRUOpen( const MRUArgs& args );

			// frame events
			void OnChar( wxKeyEvent& event );
			void OnMenuOpen( wxMenuEvent& event );

			virtual void OnNewScene( wxCommandEvent& event ) override;
			virtual void OnNewEntity( wxCommandEvent& event ) override;
			virtual void OnNewProject( wxCommandEvent& event ) override;
			virtual void OnClose( wxCommandEvent& event ) override;
			virtual void OnSaveAll( wxCommandEvent& event ) override;

			void OpenVaultPanel();
			void OnSearchGoButtonClick( wxCommandEvent& event );
			void OnSearchTextEnter( wxCommandEvent& event );

			void OnViewChange( wxCommandEvent& event );
			void OnViewCameraChange( wxCommandEvent& event );
			void OnViewVisibleChange( wxCommandEvent& event );
			void OnViewColorModeChange( wxCommandEvent& event );

			void OnImport( wxCommandEvent& event ) override;
			void OnExport( wxCommandEvent& event ) override;

			void OnSceneUndoCommand( const Editor::UndoCommandArgs& command );

			void OnUndo( wxCommandEvent& event ) override;
			void OnRedo( wxCommandEvent& event ) override;
			void OnCut( wxCommandEvent& event ) override;
			void OnCopy( wxCommandEvent& event ) override;
			void OnPaste( wxCommandEvent& event ) override;
			void OnDelete( wxCommandEvent& event ) override;

			void OnSelectAll( wxCommandEvent& event ) override;
			void OnInvertSelection( wxCommandEvent& event ) override;

			void OnParent( wxCommandEvent& event ) override;
			void OnUnparent( wxCommandEvent& event ) override;
			void OnGroup( wxCommandEvent& event ) override;
			void OnUngroup( wxCommandEvent& event ) override;
			void OnCenter( wxCommandEvent& event ) override;
			void OnDuplicate( wxCommandEvent& event ) override;
			void OnSmartDuplicate( wxCommandEvent& event ) override;
			void OnCopyTransform( wxCommandEvent& event ) override;
			void OnPasteTransform( wxCommandEvent& event ) override;
			void OnSnapToCamera( wxCommandEvent& event ) override;
			void OnSnapCameraTo( wxCommandEvent& event ) override;

			void OnPickWalk( wxCommandEvent& event ) override;

			void Executed( const Editor::ExecuteArgs& args );

			void CurrentSceneChanged( const Editor::SceneChangeArgs& args );
			void CurrentSceneChanging( const Editor::SceneChangeArgs& args );
			void OnToolSelected( wxCommandEvent& event );
			void PickWorld( Editor::PickArgs& args );

			void ConnectDocument( Document* document );
			void DisconnectDocument( const Document* document );
			void DocumentChanged( const DocumentEventArgs& args );
			void DocumentClosed( const DocumentEventArgs& args );

			void ViewToolChanged( const Editor::ToolChangeArgs& args );
			void SceneStatusChanged( const Editor::SceneStatusChangeArgs& args );
			void SceneContextChanged( const Editor::SceneContextChangeArgs& args );

			void OnExit( wxCommandEvent& event ) override;
			void OnExiting( wxCloseEvent& args );

			void OnAbout( wxCommandEvent& event ) override;
			void OnSettings( wxCommandEvent& event ) override;

			void OnManifestContextMenu(wxCommandEvent& event);

			void OnSelectTool( wxCommandEvent& event );
			void OnTranslateTool( wxCommandEvent& event );
			void OnRotateTool( wxCommandEvent& event );
			void OnScaleTool( wxCommandEvent& event );
			void OnMovePivotTool( wxCommandEvent& event );
			void OnDuplicateTool( wxCommandEvent& event );

			void AllocateNestedScene( const Editor::ResolveSceneArgs& args );
			void ReleaseNestedScene( const Editor::ReleaseSceneArgs& args );

		private:
			bool Copy( Editor::Scene* scene );
			bool Paste( Editor::Scene* scene );
			void Render( Editor::RenderVisitor* render );
			void Select( const Editor::SelectArgs& args );
			void SetHighlight( const Editor::SetHighlightArgs& args );
			void ClearHighlight( const Editor::ClearHighlightArgs& args );

		private:
			void SelectItemInScene( wxCommandEvent& event );
			void SelectSimilarItemsInScene( wxCommandEvent& event );

			void OpenManifestContextMenu( const Editor::SelectArgs& args );

			static bool SortContextItemsByName( Editor::SceneNode* lhs, Editor::SceneNode* rhs ); 
		};
	}
}
