#pragma once

#include "Editor/API.h"
#include "SceneOutliner.h"
#include "Object.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class Entity;
        class EntityType;
        class EntityAssetSet;
        struct NodeTypeExistenceArgs;
        struct InstanceSetChangeArgs;
        struct InstanceTypeChangeArgs;

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
            EntityAssetOutliner( Editor::SceneManager* sceneManager );
            virtual ~EntityAssetOutliner();
            void AddEntityTypes();

            // Helpers
        private:
            void AddEntityType( Editor::EntityType* entityType );
            void RemoveEntityType( Editor::EntityType* entityType );
            void AddEntityAssetSet( Editor::EntityAssetSet* classSet );
            void RemoveEntityAssetSet( Editor::EntityAssetSet* classSet );
            void AddEntity( Editor::Entity* entity );
            void RemoveEntity( Editor::Entity* entity );

            // Overrides from SceneOutliner
            SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;
            virtual void CurrentSceneChanged( Editor::Scene* oldScene ) HELIUM_OVERRIDE;
            virtual void ConnectSceneListeners() HELIUM_OVERRIDE;
            virtual void DisconnectSceneListeners() HELIUM_OVERRIDE;

            // Application callbacks
        private:
            void SetAdded( const InstanceTypeChangeArgs& args );
            void SetRemoved( const InstanceTypeChangeArgs& args );
            void EntityAdded( const InstanceSetChangeArgs& args );
            void EntityRemoved( const InstanceSetChangeArgs& args );
            void NodeTypeAdded( const NodeTypeExistenceArgs& args );
            void NodeTypeRemoved( const NodeTypeExistenceArgs& args );

        private:
            // GUI callbacks
            void OnBeginLabelEdit( wxTreeEvent& args );
        };
    }
}