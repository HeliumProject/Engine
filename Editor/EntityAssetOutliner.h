#pragma once

#include "Editor/API.h"
#include "Editor/SceneOutliner.h"

#include "Core/Scene/Object.h"
#include "Core/Scene/EntityInstanceType.h"
#include "Core/Scene/EntitySet.h"

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
            EntityAssetOutliner( Core::SceneManager* sceneManager );
            virtual ~EntityAssetOutliner();
            void AddEntityTypes();

            // Helpers
        private:
            void AddEntityType( Core::EntityType* entityType );
            void RemoveEntityType( Core::EntityType* entityType );
            void AddEntitySet( Core::EntitySet* classSet );
            void RemoveEntitySet( Core::EntitySet* classSet );
            void AddEntity( Core::Entity* entity );
            void RemoveEntity( Core::Entity* entity );

            // Overrides from SceneOutliner
            SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;
            virtual void CurrentSceneChanged( Core::Scene* oldScene ) HELIUM_OVERRIDE;
            virtual void ConnectSceneListeners() HELIUM_OVERRIDE;
            virtual void DisconnectSceneListeners() HELIUM_OVERRIDE;

            // Application callbacks
        private:
            void SetAdded( const Core::InstanceTypeChangeArgs& args );
            void SetRemoved( const Core::InstanceTypeChangeArgs& args );
            void EntityAdded( const Core::InstanceSetChangeArgs& args );
            void EntityRemoved( const Core::InstanceSetChangeArgs& args );
            void NodeTypeAdded( const Core::NodeTypeExistenceArgs& args );
            void NodeTypeRemoved( const Core::NodeTypeExistenceArgs& args );

        private:
            // GUI callbacks
            void OnBeginLabelEdit( wxTreeEvent& args );
        };
    }
}