#pragma once

#include "Pipeline/SceneGraph/Instance.h"
#include "Pipeline/SceneGraph/Primitive.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Light;
        class PrimitiveLocator;
        class PrimitiveSphere;

        class PIPELINE_API Light : public Instance
        {
        public:
            REFLECT_DECLARE_OBJECT( Light, Instance );
            static void PopulateComposite( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

            Light();
            ~Light();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;
            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

            Color3 GetColor() const;
            void SetColor( Color3 color );

        protected:
            // Reflected
            HDRColor3         m_Color;

            // Non-reflected
            static D3DMATERIAL9     s_Material;
        };
        typedef Helium::SmartPtr<Light> LightPtr;
        typedef std::vector<Light*> V_LightDumbPtr;
    }
}