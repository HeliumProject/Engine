#pragma once

#include "Editor/API.h"
#include "Editor/SceneOutliner.h"
#include "Pipeline/SceneGraph/SceneNodeType.h"

namespace Helium
{
    namespace SceneGraph
    {
        class SceneNode;
        struct NodeChangeArgs;
        struct NodeTypeExistenceArgs;
    }

    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Wrapper for a tree control that displays all of the scene nodes, grouped
        // by NodeType.
        // 
        class NodeTypeOutliner : public SceneOutliner
        {
            // Member variables
        private:
            wxTreeItemId m_InvisibleRoot;

            // Public functions
        public:
            NodeTypeOutliner( SceneGraph::SceneManager* sceneManager );
            virtual ~NodeTypeOutliner();
            void AddNodeTypes();

            // Helpers
        private:
            void AddNodeType( SceneGraph::SceneNodeType* nodeType );
            void AddInstance( SceneGraph::SceneNode* instance );
            void RemoveNodeType( SceneGraph::SceneNodeType* nodeType );
            void RemoveInstance( SceneGraph::SceneNode* instance );

            // Overrides from SceneOutliner
            SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;
            virtual void CurrentSceneChanged( SceneGraph::Scene* oldScene ) HELIUM_OVERRIDE;
            virtual void ConnectSceneListeners() HELIUM_OVERRIDE;
            virtual void DisconnectSceneListeners() HELIUM_OVERRIDE;

            // Application callbacks
        private:
            void NodeTypeAdded( const SceneGraph::NodeTypeExistenceArgs& args );
            void NodeTypeRemoved( const SceneGraph::NodeTypeExistenceArgs& args );
            void NodeAddedToType( const SceneGraph::NodeTypeChangeArgs& args );
            void NodeRemovedFromType( const SceneGraph::NodeTypeChangeArgs& args );

        private:
            // GUI callbacks
            void OnBeginLabelEdit( wxTreeEvent& args );
        };
    }
}