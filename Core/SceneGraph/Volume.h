#pragma once

#include "Instance.h"

namespace Helium
{
    namespace Core
    {
        class VolumeType;

        namespace VolumeShapes
        {
            enum VolumeShape
            {
                Cube,
                Cylinder,
                Sphere,
                Capsule,
            };
            static void VolumeShapeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(Cube, TXT( "Cube" ) );
                info->AddElement(Cylinder, TXT( "Cylinder" ) );
                info->AddElement(Sphere, TXT( "Sphere" ) );
                info->AddElement(Capsule, TXT( "Capsule" ) );
            }
        }
        typedef VolumeShapes::VolumeShape VolumeShape;

        class Volume : public Instance
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Volume, Instance );
            static void EnumerateClass( Reflect::Compositor<Volume>& comp );
            static void InitializeType();
            static void CleanupType();

            Volume();
            ~Volume();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;
            virtual SceneNodeTypePtr CreateNodeType( Scene* scene ) const HELIUM_OVERRIDE;

            int GetShape() const;
            void SetShape( int shape );

            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

            static void CreatePanel( CreatePanelArgs& args );

            bool IsPointerVisible() const;
            void SetPointerVisible(bool visible);

        protected:
            VolumeShape m_Shape;
            bool        m_ShowPointer;
        };

        typedef Helium::SmartPtr<Volume> VolumePtr;
        typedef std::vector< Volume* > V_VolumeDumbPtr;
    }
}