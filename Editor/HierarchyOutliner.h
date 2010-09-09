#pragma once

// Includes
#include "Editor/API.h"
#include "Editor/SceneOutliner.h"

namespace Helium
{
    namespace Editor
    {

        /////////////////////////////////////////////////////////////////////////////
        // Wrapper for hierarchy data stored in tree items.  Derives from 
        // SceneOutlinerItemData and internally handles casting from Object to
        // Editor::HierarchyNode.  Used in the Hierarchy Outline to track hierarchy nodes
        // for each item in the tree (instead of just the Object base class).
        // 
        class HierarchyOutlinerItemData : public SceneOutlinerItemData
        {
        public:
            HierarchyOutlinerItemData( Core::HierarchyNode* node )
            : SceneOutlinerItemData( node )
            {
            }

            ~HierarchyOutlinerItemData()
            {
            }

            Core::HierarchyNode* GetHierarchyNode() const
            {
              return Reflect::AssertCast< Core::HierarchyNode >( GetObject() );
            }

            void SetHierarchyNode( Core::HierarchyNode* node )
            {
              SetObject( node );
            }
        };


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
            // Member variables
        private:
            // Needed to simulate multiple root items in the tree
            wxTreeItemId m_InvisibleRoot;

        public:
            HierarchyOutliner( Core::SceneManager* sceneManager );
            virtual ~HierarchyOutliner();

        protected:
            HierarchyOutlinerItemData* GetTreeItemData( const wxTreeItemId& item );
            void AddHierarchyNodes();
            void RecurseAddHierarchyNode( Core::HierarchyNode* node, bool root = false );
            void AddHierarchyNode( Core::HierarchyNode* node );

        protected:
            // Overrides from SceneOutliner
            virtual SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;
            virtual void CurrentSceneChanged( Core::Scene* oldScene ) HELIUM_OVERRIDE;
            virtual void ConnectSceneListeners() HELIUM_OVERRIDE;
            virtual void DisconnectSceneListeners() HELIUM_OVERRIDE;

        private:
            // Tree event callbacks
            void OnBeginDrag( wxTreeEvent& args );
            void OnEndDrag( wxTreeEvent& args );

        private:
            // Event callbacks for other systems in Editor - do not call directly
            void ParentChanged( const Core::ParentChangedArgs& args );
            void NodeAdded( const Core::NodeChangeArgs& args );
            void NodeRemoved( const Core::NodeChangeArgs& args );
        };
    }
}