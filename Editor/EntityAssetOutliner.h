#pragma once

#include "Editor/API.h"
#include "Editor/SceneOutliner.h"

#include "Foundation/Reflect/Object.h"
#include "Core/SceneGraph/EntityInstanceType.h"
#include "Core/SceneGraph/EntitySet.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Provides a tree representation of all the entity classes in the scene, 
        // grouped according to their entity class set.
        // 
        class EntityAssetOutliner : public SceneOutliner
        {
            // Member variables
        private:
            // Needed to simulate multiple root items in the tree
            wxTreeItemId m_InvisibleRoot;

            // Public functions
        public:
            EntityAssetOutliner( SceneGraph::SceneManager* sceneManager );
            virtual ~EntityAssetOutliner();
            void AddEntityTypes();

            // Helpers
        private:
            void AddEntityType( SceneGraph::EntityInstanceType* entityType );
            void RemoveEntityType( SceneGraph::EntityInstanceType* entityType );
            void AddEntitySet( SceneGraph::EntitySet* classSet );
            void RemoveEntitySet( SceneGraph::EntitySet* classSet );
            void AddEntityInstance( SceneGraph::EntityInstance* entityInstance );
            void RemoveEntityInstance( SceneGraph::EntityInstance* entityInstance );

            // Overrides from SceneOutliner
            SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;
            virtual void CurrentSceneChanged( SceneGraph::Scene* oldScene ) HELIUM_OVERRIDE;
            virtual void ConnectSceneListeners() HELIUM_OVERRIDE;
            virtual void DisconnectSceneListeners() HELIUM_OVERRIDE;

            // Application callbacks
        private:
            void SetAdded( const SceneGraph::InstanceTypeChangeArgs& args );
            void SetRemoved( const SceneGraph::InstanceTypeChangeArgs& args );
            void EntityAdded( const SceneGraph::InstanceSetChangeArgs& args );
            void EntityRemoved( const SceneGraph::InstanceSetChangeArgs& args );
            void NodeTypeAdded( const SceneGraph::NodeTypeExistenceArgs& args );
            void NodeTypeRemoved( const SceneGraph::NodeTypeExistenceArgs& args );

        private:
            // GUI callbacks
            void OnBeginLabelEdit( wxTreeEvent& args );
        };
    }
}