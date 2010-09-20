#pragma once

#include "Core/SceneGraph/Instance.h"
#include "Core/SceneGraph/Primitive.h"
#include "Core/SceneGraph/PropertiesGenerator.h"

namespace Helium
{
    namespace Core
    {
        class Light;
        class PrimitiveLocator;
        class PrimitiveSphere;

        class CORE_API Light HELIUM_ABSTRACT : public Instance
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Light, Instance );
            static void EnumerateClass( Reflect::Compositor<Light>& comp );
            static void InitializeType();
            static void CleanupType();

            Light();
            ~Light();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;
            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

            Math::Color3 GetColor() const;
            void SetColor( Math::Color3 color );

        protected:
            // Reflected
            Math::HDRColor3         m_Color;

            // Non-reflected
            static D3DMATERIAL9     s_Material;
        };
        typedef Helium::SmartPtr<Light> LightPtr;
        typedef std::vector<Light*> V_LightDumbPtr;
    }
}