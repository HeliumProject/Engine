#pragma once

#include "LunaGenerated.h"

#include "DirectoryPanel.h"
#include "HelpPanel.h"
#include "ProjectPanel.h"
#include "LayersPanel.h"
#include "PropertiesPanel.h"
#include "ToolsPanel.h"
#include "TypesPanel.h"
#include "ViewPanel.h"

#include "PropertiesManager.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include "Application/UI/MenuMRU.h"

#ifdef UI_REFACTOR
# include "TreeMonitor.h"
# include "TreeSortTimer.h"
#endif

namespace Luna
{
    class MainFrame : public MainFrameGenerated
    {
    protected:
        enum
        {
            ID_MenuOpened = wxID_HIGHEST + 1,
        };

    public:
        MainFrame( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,768 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
        virtual ~MainFrame();

        virtual void SaveWindowState()
        {
        }

        virtual const tstring& GetPreferencePrefix() const
        {
            return s_PreferencesPrefix;
        }

        void SetHelpText( const tchar* text );

        bool DoOpen( const tstring& path );


#ifdef UI_REFACTOR
        TreeMonitor& GetTreeMonitor()
        {
            return m_TreeMonitor;
        }
#endif

    private:
        static tstring s_PreferencesPrefix;

        HelpPanel*       m_HelpPanel;
        ProjectPanel*    m_ProjectPanel;
        LayersPanel*     m_LayersPanel;
        TypesPanel*      m_TypesPanel;
        ViewPanel*       m_ViewPanel;
        ToolsPanel*      m_ToolsPanel;
        DirectoryPanel*  m_DirectoryPanel;
        PropertiesPanel* m_PropertiesPanel;

        SceneManager     m_SceneManager;

        // the attributes for the current selection
        EnumeratorPtr m_SelectionEnumerator;
        PropertiesManagerPtr m_SelectionPropertiesManager;
        Inspect::Canvas m_SelectionProperties;

        // the attributes for the current tool
        EnumeratorPtr m_ToolEnumerator;
        PropertiesManagerPtr m_ToolPropertiesManager;
        Inspect::Canvas m_ToolProperties;

        Nocturnal::MenuMRUPtr m_MRU;

        //context items ordered by name  
        V_HierarchyNodeDumbPtr m_OrderedContextItems;

#ifdef UI_REFACTOR
        TreeMonitor m_TreeMonitor;
        TreeSortTimer m_TreeSortTimer;
#endif
    private:
        void OnMenuOpen( wxMenuEvent& event );

        void OnNewScene( wxCommandEvent& event );
        void OnNewEntity( wxCommandEvent& event );
        void OnOpen( wxCommandEvent& event );
        void OnClose( wxCommandEvent& event );
        void OnSaveAll( wxCommandEvent& event );

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

        void CurrentSceneChanged( const SceneChangeArgs& args );
        void CurrentSceneChanging( const SceneChangeArgs& args );
        void OnPropertiesCreated( const PropertiesCreatedArgs& args );
        void OnToolSelected(wxCommandEvent& event);
        void ViewToolChanged( const ToolChangeArgs& args );
        void SceneStatusChanged( const SceneStatusChangeArgs& args );

        void OnExit( wxCommandEvent& event );
        void OnExiting( wxCloseEvent& args );

        void OnAbout( wxCommandEvent& event );

        void OnManifestContextMenu(wxCommandEvent& event);
        void OnTypeContextMenu(wxCommandEvent& event);

    private:
        bool Copy( Luna::Scene* scene );
        bool Paste( Luna::Scene* scene );
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
        void SetupEntityTypeMenus( const Luna::EntityType* entity, wxMenu* entityMenu, u32& numMenuItems );

        static bool SortContextItemsByName( Luna::SceneNode* lhs, Luna::SceneNode* rhs ); 
        static bool SortTypeItemsByName( Luna::SceneNodeType* lhs, Luna::SceneNodeType* rhs );

    };
}