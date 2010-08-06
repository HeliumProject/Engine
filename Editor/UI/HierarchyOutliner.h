#pragma once

// Includes
#include "Editor/API.h"
#include "Editor/UI/SceneOutliner.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class HierarchyNode;
        class HierarchyOutlinerItemData;
        struct ParentChangedArgs;
        struct NodeChangeArgs;

        /////////////////////////////////////////////////////////////////////////////
        // Class to coordinate GUI events on a tree control, and the underlying 
        // hierarchy data.  Items are not loaded in the tree until they are needed
        // when the user expands parent items.  Call Load() to get at least the first
        // item displayed in the tree.  Call Unload() before calling Load() on any
        // other HierarchyOutliner's that are associated with the same tree control.  To 
        // completely free all data associated with this HierarchyOutliner, call Clear().
        // 
        class HierarchyOutliner : public SceneOutliner
        {
        public:
            HierarchyOutliner( Editor::SceneManager* sceneManager );
            virtual ~HierarchyOutliner();

        protected:
            HierarchyOutlinerItemData* GetTreeItemData( const wxTreeItemId& item );
            void AddHierarchyNodes();
            void RecurseAddHierarchyNode( Editor::HierarchyNode* node );
            void AddHierarchyNode( Editor::HierarchyNode* node );

        protected:
            // Overrides from SceneOutliner
            virtual SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) HELIUM_OVERRIDE;
            virtual void CurrentSceneChanged( Editor::Scene* oldScene ) HELIUM_OVERRIDE;
            virtual void ConnectSceneListeners() HELIUM_OVERRIDE;
            virtual void DisconnectSceneListeners() HELIUM_OVERRIDE;

        private:
            // Tree event callbacks
            void OnBeginDrag( wxTreeEvent& args );
            void OnEndDrag( wxTreeEvent& args );

        private:
            // Event callbacks for other systems in Editor - do not call directly
            void ParentChanged( const ParentChangedArgs& args );
            void NodeAdded( const NodeChangeArgs& args );
            void NodeRemoved( const NodeChangeArgs& args );
        };
    }
}