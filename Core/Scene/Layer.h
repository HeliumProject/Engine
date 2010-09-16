#pragma once

#include "Core/Content/Nodes/ContentLayer.h"
#include "Core/Content/NodeVisibility.h"

#include "Core/API.h"
#include "Core/Scene/SceneNode.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API Layer : public Core::SceneNode
        {
            //
            // Runtime Type Info
            //

        public:
            REFLECT_DECLARE_ABSTRACT( Core::Layer, Core::SceneNode );
            static void InitializeType();
            static void CleanupType();

        public:
            Layer( Core::Scene* scene, Content::Layer* layer );
            virtual ~Layer();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Initialize() HELIUM_OVERRIDE;
            virtual void Pack() HELIUM_OVERRIDE;

            bool IsVisible() const HELIUM_OVERRIDE;
            void SetVisible( bool visible );

            bool IsSelectable() const;
            void SetSelectable( bool selectable );

            const Math::Color3& GetColor() const;
            void SetColor( const Math::Color3& color );

            OS_PersistentDumbPtr GetMembers();
            bool ContainsMember( Core::SceneNode* node ) const;

            virtual void Prune( V_SceneNodeDumbPtr& prunedNodes ) HELIUM_OVERRIDE;
            virtual void Insert(SceneGraph* g, V_SceneNodeDumbPtr& insertedNodes ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

        private:
            static void CreatePanel( CreatePanelArgs& args );
            static void BuildUnionAndIntersection( PropertiesGenerator* generator, const OS_PersistentDumbPtr& selection, tstring& unionStr, tstring& intersectionStr );

            Content::NodeVisibilityPtr m_VisibilityData; 
        };

        typedef Helium::SmartPtr< Core::Layer > LayerPtr;
        typedef std::vector< Core::Layer* > V_LayerDumbPtr;
        typedef std::map< tstring, Core::Layer* > M_LayerDumbPtr;
    }
}