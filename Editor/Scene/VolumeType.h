#pragma once

#include "InstanceType.h"

#include "Core/Content/Nodes/Volume.h"

namespace Helium
{
    namespace Editor
    {
        class Volume;
        class Primitive;
        class PrimitivePointer;
        class PrimitiveCube;
        class PrimitiveCylinder;
        class PrimitiveSphere;
        class PrimitiveCapsule;

        class VolumeType : public Editor::InstanceType
        {
            //
            // Members
            //

        private:
            // volume shapes
            Editor::PrimitiveCube* m_Cube;
            Editor::PrimitiveCylinder* m_Cylinder;
            Editor::PrimitiveSphere* m_Sphere;
            Editor::PrimitiveCapsule* m_Capsule;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::VolumeType, Editor::InstanceType );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            VolumeType( Editor::Scene* scene, i32 instanceType );

            virtual ~VolumeType();

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            const Editor::Primitive* GetShape( Content::VolumeShape shape ) const;
        };
    }
}