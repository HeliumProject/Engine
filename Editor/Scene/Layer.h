#pragma once

#include "Pipeline/Content/Nodes/Layer.h"
#include "Pipeline/Content/NodeVisibility.h"

#include "Editor/API.h"
#include "Editor/Scene/SceneNode.h"

namespace Helium
{
    namespace Editor
    {
        class Layer : public Editor::SceneNode
        {
            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::Layer, Editor::SceneNode );
            static void InitializeType();
            static void CleanupType();

        public:
            Layer( Editor::Scene* scene, Content::Layer* layer );
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

            OS_SelectableDumbPtr GetMembers();
            bool ContainsMember( Editor::SceneNode* node ) const;

            virtual void Prune( V_SceneNodeDumbPtr& prunedNodes ) HELIUM_OVERRIDE;
            virtual void Insert(SceneGraph* g, V_SceneNodeDumbPtr& insertedNodes ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

        private:
            static void CreatePanel( CreatePanelArgs& args );
            static void BuildUnionAndIntersection( PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection, tstring& unionStr, tstring& intersectionStr );

            Content::NodeVisibilityPtr m_VisibilityData; 
        };

        typedef Helium::SmartPtr< Editor::Layer > LayerPtr;
        typedef std::vector< Editor::Layer* > V_LayerDumbPtr;
        typedef std::map< tstring, Editor::Layer* > M_LayerDumbPtr;
    }
}