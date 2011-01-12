#pragma once

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/SceneNode.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PIPELINE_API Layer : public SceneNode
        {
        public:
            REFLECT_DECLARE_CLASS( Layer, SceneGraph::SceneNode );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Layer();
            ~Layer();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

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

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

        private:
            static void CreatePanel( CreatePanelArgs& args );
            static void BuildUnionAndIntersection( PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection, tstring& unionStr, tstring& intersectionStr );

        protected:
            // Reflected
            bool                        m_Visible;
            bool                        m_Selectable;
            S_TUID                      m_Members;
            Color3                m_Color;
        };

        typedef Helium::StrongPtr< SceneGraph::Layer > LayerPtr;
        typedef std::vector< SceneGraph::Layer* > V_LayerDumbPtr;
        typedef std::map< tstring, SceneGraph::Layer* > M_LayerDumbPtr;
    }
}