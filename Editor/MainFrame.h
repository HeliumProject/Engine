#pragma once

#include "Editor/EditorGenerated.h"

#include "DirectoryPanel.h"
#include "HelpPanel.h"
#include "ProjectPanel.h"
#include "LayersPanel.h"
#include "PropertiesPanel.h"
#include "ToolbarPanel.h"
#include "TypesPanel.h"
#include "ViewPanel.h"

#include "Editor/Vault/VaultPanel.h"

#include "Core/SettingsManager.h"

#include "Core/SceneGraph/PropertiesManager.h"
#include "Core/SceneGraph/Scene.h"
#include "Core/SceneGraph/SceneManager.h"

#include "Editor/DragDrop/DropTarget.h"

#include "Editor/MRU/MenuMRU.h"
#include "Editor/TreeMonitor.h"
#include "Editor/MessageDisplayer.h"
#include "Editor/FileDialogDisplayer.h"

#include "Editor/Inspect/TreeCanvas.h"
#include "Editor/Inspect/StripCanvas.h"

#include "Core/Project.h"

namespace Helium
{
    namespace Editor
    {
        typedef std::map< i32, i32 > M_IDToColorMode; // Maps wx ID for menu items to our ViewColorMode enum

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

            typedef std::map< SceneGraph::Scene*, OutlinerStates > M_OutlinerStates;

        public:
            MainFrame( SettingsManager* settingsManager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1280,1024 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
            virtual ~MainFrame();

            void SetHelpText( const tchar* text );

            bool OpenProject( const Helium::Path& path );

            SceneGraph::SceneManager& GetSceneManager()
            {
                return m_SceneManager;
            }

        private:
            // Stores information about the state of each outliner for each scene
            // that is open.  Restores the state when switching between scenes.
            M_OutlinerStates            m_OutlinerStates;

            HelpPanel*                  m_HelpPanel;
            ProjectPanel*               m_ProjectPanel;
            LayersPanel*                m_LayersPanel;
            TypesPanel*                 m_TypesPanel;
            ViewPanel*                  m_ViewPanel;
            ToolbarPanel*               m_ToolbarPanel;
            DirectoryPanel*             m_DirectoryPanel;
            PropertiesPanel*            m_PropertiesPanel;

            VaultPanel*                 m_VaultPanel;

            ProjectPtr                  m_Project;
            MessageDisplayer            m_MessageDisplayer;
            FileDialogDisplayer         m_FileDialogDisplayer;
            SceneGraph::SceneManager    m_SceneManager;

            SettingsManager*            m_SettingsManager;

            MenuMRUPtr                  m_MRU;
            M_IDToColorMode             m_ColorModeLookup;

            //context items ordered by name  
            SceneGraph::V_HierarchyNodeDumbPtr m_OrderedContextItems;

            TreeMonitor                 m_TreeMonitor;

        private:
            bool ValidateDrag( const Editor::DragArgs& args );
            void DragOver( const Editor::DragArgs& args );
            void Drop( const Editor::DragArgs& args );

            void SceneAdded( const SceneGraph::SceneChangeArgs& args );
            void SceneRemoving( const SceneGraph::SceneChangeArgs& args );
            void SceneLoadFinished( const SceneGraph::LoadArgs& args );
            void SceneExecuted( const SceneGraph::ExecuteArgs& args );

            bool DoOpen( const tstring& path );

        private:
            void OnOpen( wxCommandEvent& event ) HELIUM_OVERRIDE;
            void OnMRUOpen( const MRUArgs& args );

            // frame events
            void OnChar( wxKeyEvent& event );
            void OnMenuOpen( wxMenuEvent& event );

            virtual void OnNewScene( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnNewEntity( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnNewProject( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnClose( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnSaveAll( wxCommandEvent& event ) HELIUM_OVERRIDE;

            void OpenVaultPanel();
            void OnSearchGoButtonClick( wxCommandEvent& event );
            void OnSearchTextEnter( wxCommandEvent& event );

            void OnViewChange( wxCommandEvent& event );
            void OnViewCameraChange( wxCommandEvent& event );
            void OnViewVisibleChange( wxCommandEvent& event );
            void OnViewColorModeChange( wxCommandEvent& event );

            void OnImport( wxCommandEvent& event );
            void OnExport( wxCommandEvent& event );

            void OnSceneUndoCommand( const SceneGraph::UndoCommandArgs& command );

            void OnUndo( wxCommandEvent& event );
            void OnRedo( wxCommandEvent& event );
            void OnCut( wxCommandEvent& event );
            void OnCopy( wxCommandEvent& event );
            void OnPaste( wxCommandEvent& event );
            void OnDelete( wxCommandEvent& event );

            void OnSelectAll( wxCommandEvent& event );
            void OnInvertSelection( wxCommandEvent& event );

            void OnParent( wxCommandEvent& event );
            void OnUnparent( wxCommandEvent& event );
            void OnGroup( wxCommandEvent& event );
            void OnUngroup( wxCommandEvent& event );
            void OnCenter( wxCommandEvent& event );
            void OnDuplicate( wxCommandEvent& event );
            void OnSmartDuplicate( wxCommandEvent& event );
            void OnCopyTransform( wxCommandEvent& event );
            void OnPasteTransform( wxCommandEvent& event );
            void OnSnapToCamera( wxCommandEvent& event );
            void OnSnapCameraTo( wxCommandEvent& event );

            void OnPickWalk( wxCommandEvent& event );

            void Executed( const SceneGraph::ExecuteArgs& args );

            void SelectionChanged( const SceneGraph::SelectionChangeArgs& selection );

            void CurrentSceneChanged( const SceneGraph::SceneChangeArgs& args );
            void CurrentSceneChanging( const SceneGraph::SceneChangeArgs& args );
            void OnToolSelected(wxCommandEvent& event);
            void PickWorld( SceneGraph::PickArgs& args );
            void DocumentChanged( const DocumentEventArgs& args );
            void DocumentClosed( const DocumentEventArgs& args );
            void ViewToolChanged( const SceneGraph::ToolChangeArgs& args );
            void SceneStatusChanged( const SceneGraph::SceneStatusChangeArgs& args );
            void SceneContextChanged( const SceneGraph::SceneContextChangeArgs& args );

            void OnExit( wxCommandEvent& event );
            void OnExiting( wxCloseEvent& args );

            void OnAbout( wxCommandEvent& event );
            void OnSettings( wxCommandEvent& event );

            void OnManifestContextMenu(wxCommandEvent& event);
            void OnTypeContextMenu(wxCommandEvent& event);

            void OnSelectTool( wxCommandEvent& event );
            void OnTranslateTool( wxCommandEvent& event );
            void OnRotateTool( wxCommandEvent& event );
            void OnScaleTool( wxCommandEvent& event );
            void OnMovePivotTool( wxCommandEvent& event );
            void OnDuplicateTool( wxCommandEvent& event );

        private:
            bool Copy( SceneGraph::Scene* scene );
            bool Paste( SceneGraph::Scene* scene );
            void Render( SceneGraph::RenderVisitor* render );
            void Select( const SceneGraph::SelectArgs& args );
            void SetHighlight( const SceneGraph::SetHighlightArgs& args );
            void ClearHighlight( const SceneGraph::ClearHighlightArgs& args );

        private:
            void SelectItemInScene( wxCommandEvent& event );
            void SelectSimilarItemsInScene( wxCommandEvent& event );

            void OpenManifestContextMenu( const SceneGraph::SelectArgs& args );
            void OpenTypeContextMenu( const SceneGraph::SelectArgs& args );

            void SetupTypeContextMenu( const SceneGraph::HM_StrToSceneNodeTypeSmartPtr& sceneNodeTypes,wxMenu& contextMenu,  u32& numMenuItems );
            void SetupEntityTypeMenus( const SceneGraph::EntityInstanceType* entity, wxMenu* entityMenu, u32& numMenuItems );

            static bool SortContextItemsByName( SceneGraph::SceneNode* lhs, SceneGraph::SceneNode* rhs ); 
            static bool SortTypeItemsByName( SceneGraph::SceneNodeType* lhs, SceneGraph::SceneNodeType* rhs );
        };
    }
}