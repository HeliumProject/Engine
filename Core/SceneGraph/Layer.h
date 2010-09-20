#pragma once

#include "Core/API.h"
#include "Core/SceneGraph/SceneNode.h"
#include "Core/SceneGraph/PropertiesGenerator.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API Layer : public Core::SceneNode
        {
        public:
            REFLECT_DECLARE_CLASS( Layer, Core::SceneNode );
            static void EnumerateClass( Reflect::Compositor<Layer>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Layer();
            ~Layer();

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

            OS_SceneNodeDumbPtr GetMembers();
            bool ContainsMember( Core::SceneNode* node ) const;

            virtual void Prune( V_SceneNodeDumbPtr& prunedNodes ) HELIUM_OVERRIDE;
            virtual void Insert(Graph* g, V_SceneNodeDumbPtr& insertedNodes ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

        private:
            static void CreatePanel( CreatePanelArgs& args );
            static void BuildUnionAndIntersection( PropertiesGenerator* generator, const OS_SceneNodeDumbPtr& selection, tstring& unionStr, tstring& intersectionStr );

        protected:
            // Reflected
            bool                        m_Visible;
            bool                        m_Selectable;
            V_TUID                      m_Members;
            Math::Color3                m_Color;
        };

        typedef Helium::SmartPtr< Core::Layer > LayerPtr;
        typedef std::vector< Core::Layer* > V_LayerDumbPtr;
        typedef std::map< tstring, Core::Layer* > M_LayerDumbPtr;
    }
}