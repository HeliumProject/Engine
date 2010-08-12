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

#include "Core/Scene/PropertiesManager.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"

#include "Application/UI/MenuMRU.h"
#include "Application/Inspect/DragDrop/DropTarget.h"

#include "Editor/TreeMonitor.h"
#include "Editor/MessageDisplayer.h"

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

            typedef std::map< Editor::Scene*, OutlinerStates > M_OutlinerStates;

        public:
            MainFrame( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1280,1024 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
            virtual ~MainFrame();

            void SetHelpText( const tchar* text );

            bool OpenProject( const Helium::Path& path );

            void SyncPropertyThread();

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

            Helium::Core::ProjectPtr    m_Project;
            MessageDisplayer            m_MessageDisplayer;
            SceneManager                m_SceneManager;

            // the attributes for the current selection
            EnumeratorPtr               m_SelectionEnumerator;
            PropertiesManagerPtr        m_SelectionPropertiesManager;
            Inspect::Canvas             m_SelectionProperties;

            // the attributes for the current tool
            EnumeratorPtr               m_ToolEnumerator;
            PropertiesManagerPtr        m_ToolPropertiesManager;
            Inspect::Canvas             m_ToolProperties;

            Helium::MenuMRUPtr          m_MRU;
            M_IDToColorMode             m_ColorModeLookup;

            //context items ordered by name  
            V_HierarchyNodeDumbPtr      m_OrderedContextItems;

            TreeMonitor                 m_TreeMonitor;

        private:
            bool ValidateDrag( const Inspect::DragArgs& args );
            wxDragResult DragOver( const Inspect::DragArgs& args );
            wxDragResult Drop( const Inspect::DragArgs& args );

            void SceneAdded( const SceneChangeArgs& args );
            void SceneRemoving( const SceneChangeArgs& args );
            void SceneLoadFinished( const LoadArgs& args );

            bool DoOpen( const tstring& path );

        private:

            void OnMRUOpen( const Helium::MRUArgs& args );

            // frame events
            void OnEraseBackground( wxEraseEvent& event );
            void OnSize( wxSizeEvent& event );
            void OnChar( wxKeyEvent& event );
            void OnShow( wxShowEvent& event );
            void OnMenuOpen( wxMenuEvent& event );

            void OnNewScene( wxCommandEvent& event );
            void OnNewEntity( wxCommandEvent& event );
            void OnNewProject( wxCommandEvent& event );
            void OnOpen( wxCommandEvent& event );
            void OnClose( wxCommandEvent& event );
            void OnSaveAll( wxCommandEvent& event );

            void OnViewChange( wxCommandEvent& event );
            void OnViewCameraChange( wxCommandEvent& event );
            void OnViewVisibleChange( wxCommandEvent& event );
            void OnViewColorModeChange( wxCommandEvent& event );

            void OnImport( wxCommandEvent& event );
            void OnExport( wxCommandEvent& event );

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

            void Executed( const ExecuteArgs& args );

            void SelectionChanged( const OS_SelectableDumbPtr& selection );

            void CurrentSceneChanged( const SceneChangeArgs& args );
            void CurrentSceneChanging( const SceneChangeArgs& args );
            void OnPropertiesCreated( const PropertiesCreatedArgs& args );
            void OnToolSelected(wxCommandEvent& event);
            void PickWorld( PickArgs& args );
            void DocumentModified( const DocumentChangedArgs& args );
            void DocumentClosed( const DocumentChangedArgs& args );
            void ViewToolChanged( const ToolChangeArgs& args );
            void SceneStatusChanged( const SceneStatusChangeArgs& args );
            void SceneContextChanged( const SceneContextChangeArgs& args );

            void OnExit( wxCommandEvent& event );
            void OnExiting( wxCloseEvent& args );

            void OnAbout( wxCommandEvent& event );
            void OnPreferences( wxCommandEvent& event );

            void OnManifestContextMenu(wxCommandEvent& event);
            void OnTypeContextMenu(wxCommandEvent& event);

        private:
            bool Copy( Editor::Scene* scene );
            bool Paste( Editor::Scene* scene );
            void Render( RenderVisitor* render );
            void Select( const SelectArgs& args );
            void SetHighlight( const SetHighlightArgs& args );
            void ClearHighlight( const ClearHighlightArgs& args );

        private:
            void SelectItemInScene( wxCommandEvent& event );
            void SelectSimilarItemsInScene( wxCommandEvent& event );

            void OpenManifestContextMenu( const SelectArgs& args );
            void OpenTypeContextMenu( const SelectArgs& args );

            void SetupTypeContextMenu( const HM_StrToSceneNodeTypeSmartPtr& sceneNodeTypes,wxMenu& contextMenu,  u32& numMenuItems );
            void SetupEntityTypeMenus( const Editor::EntityType* entity, wxMenu* entityMenu, u32& numMenuItems );

            static bool SortContextItemsByName( Editor::SceneNode* lhs, Editor::SceneNode* rhs ); 
            static bool SortTypeItemsByName( Editor::SceneNodeType* lhs, Editor::SceneNodeType* rhs );

        };
    }
}