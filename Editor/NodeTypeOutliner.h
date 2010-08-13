#pragma once

#include "Editor/API.h"
#include "Editor/SceneOutliner.h"
#include "Core/Scene/SceneNodeType.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class SceneNode;
        struct NodeChangeArgs;
        struct NodeTypeExistenceArgs;

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
            NodeTypeOutliner( Core::SceneManager* sceneManager );
            virtual ~NodeTypeOutliner();
            void AddNodeTypes();

            // Helpers
        private:
            void AddNodeType( Core::SceneNodeType* nodeType );
            void AddInstance( Core::SceneNode* instance );
            void RemoveNodeType( Core::SceneNodeType* nodeType );
            void RemoveInstance( Core::SceneNode* instance );

            // Overrides from SceneOutliner
            SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;
            virtual void CurrentSceneChanged( Core::Scene* oldScene ) HELIUM_OVERRIDE;
            virtual void ConnectSceneListeners() HELIUM_OVERRIDE;
            virtual void DisconnectSceneListeners() HELIUM_OVERRIDE;

            // Application callbacks
        private:
            void NodeTypeAdded( const Core::NodeTypeExistenceArgs& args );
            void NodeTypeRemoved( const Core::NodeTypeExistenceArgs& args );
            void NodeAddedToType( const Core::NodeTypeChangeArgs& args );
            void NodeRemovedFromType( const Core::NodeTypeChangeArgs& args );

        private:
            // GUI callbacks
            void OnBeginLabelEdit( wxTreeEvent& args );
        };
    }
}