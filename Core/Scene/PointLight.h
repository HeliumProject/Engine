#pragma once

#include "Light.h"
#include "Core/Content/Nodes/ContentPointLight.h"

namespace Helium
{
    namespace Core
    {
        class PrimitiveSphere;

        class CORE_API PointLight : public Core::Light
        {
            //
            // Members
            //
        protected:
            Core::PrimitiveSphere* m_InnerSphere;
            Core::PrimitiveSphere* m_OuterSphere;

            bool m_FirstEntry;


            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::PointLight, Core::Light );
            static void InitializeType();
            static void CleanupType();


            //
            // Member functions
            //

            PointLight(Core::Scene* scene);
            PointLight(Core::Scene* scene, Content::PointLight* light);
            virtual ~PointLight();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;
            virtual void Evaluate( GraphDirection direction ) HELIUM_OVERRIDE;

            virtual void Initialize() HELIUM_OVERRIDE;
            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;
            virtual void SetScale( const Math::Scale& value );

            Core::PrimitiveSphere* GetInnerSphere() const
            {
                return m_InnerSphere;
            }

            Core::PrimitiveSphere* GetOuterSphere() const
            {
                return m_OuterSphere;
            }

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

            float GetInnerRadius() const;
            void SetInnerRadius( float radius );

            float GetOuterRadius() const;
            void SetOuterRadius( float radius );    

        };
    }
}