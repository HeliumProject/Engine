#pragma once

#include "Instance.h"

namespace Helium
{
    namespace SceneGraph
    {
        class VolumeType;

        class VolumeShape
        {
        public:
            enum Enum
            {
                Cube,
                Cylinder,
                Sphere,
                Capsule,
            };

            REFLECT_DECLARE_ENUMERATION( VolumeShape );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(Cube,       TXT( "Cube" ) );
                info.AddElement(Cylinder,   TXT( "Cylinder" ) );
                info.AddElement(Sphere,     TXT( "Sphere" ) );
                info.AddElement(Capsule,    TXT( "Capsule" ) );
            }
        };

        class Volume : public Instance
        {
        public:
            REFLECT_DECLARE_CLASS( Volume, Instance );
            static void EnumerateClass( Reflect::Compositor<Volume>& comp );
            static void InitializeType();
            static void CleanupType();

            Volume();
            ~Volume();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
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