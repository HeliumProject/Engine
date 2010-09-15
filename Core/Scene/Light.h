#pragma once

#include "Instance.h"
#include "Core/Content/Nodes/ContentLight.h"
#include "Core/Scene/PropertiesGenerator.h"
#include "Foundation/Container/BitArray.h"
#include "Primitive.h"

namespace Helium
{
    namespace Core
    {
        class Light;
        class PrimitiveLocator;
        class PrimitiveSphere;

        class CORE_API Light HELIUM_ABSTRACT : public Core::Instance
        {
        public:
            static D3DMATERIAL9 s_Material;

            //
            // Runtime Type Info
            //

        public:
            REFLECT_DECLARE_ABSTRACT( Core::Light, Core::Instance );
            static void InitializeType();
            static void CleanupType();

            //
            // Member functions
            //

            Light(Core::Scene* scene, Content::Light* light);

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
        };
        typedef Helium::SmartPtr<Core::Light> LightPtr;
        typedef std::vector<Core::Light*> V_LightDumbPtr;
    }
}