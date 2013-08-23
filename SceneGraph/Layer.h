#pragma once

#include "SceneGraph/API.h"
#include "SceneGraph/SceneNode.h"
#include "SceneGraph/PropertiesGenerator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HELIUM_SCENE_GRAPH_API Layer : public SceneNode
        {
        public:
            HELIUM_DECLARE_CLASS( Layer, SceneGraph::SceneNode );
            static void PopulateMetaType( Reflect::MetaStruct& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Layer();
            ~Layer();

            virtual void Initialize() HELIUM_OVERRIDE;

            bool IsVisible() const HELIUM_OVERRIDE;
            void SetVisible( bool visible );

            bool IsSelectable() const;
            void SetSelectable( bool selectable );

            const Color3& GetColor() const;
            void SetColor( const Color3& color );

            OS_SceneNodeDumbPtr GetMembers();
            bool ContainsMember( SceneGraph::SceneNode* node ) const;

            virtual void ConnectDescendant( SceneGraph::SceneNode* descendant ) HELIUM_OVERRIDE;
            virtual void DisconnectDescendant( SceneGraph::SceneNode* descendant ) HELIUM_OVERRIDE;

            virtual void Insert(Graph* g, V_SceneNodeDumbPtr& insertedNodes ) HELIUM_OVERRIDE;
            virtual void Prune( V_SceneNodeDumbPtr& prunedNodes ) HELIUM_OVERRIDE;

        protected:
            // Reflected
            bool                        m_Visible;
            bool                        m_Selectable;
            std::set<TUID>                      m_Members;
            Color3						m_Color;
        };

        typedef Helium::StrongPtr< SceneGraph::Layer > LayerPtr;
        typedef std::vector< SceneGraph::Layer* > V_LayerDumbPtr;
        typedef std::map< std::string, SceneGraph::Layer* > M_LayerDumbPtr;
    }
}