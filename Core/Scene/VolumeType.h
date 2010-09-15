#pragma once

#include "InstanceType.h"

#include "Core/Content/Nodes/ContentVolume.h"

namespace Helium
{
    namespace Core
    {
        class Volume;
        class Primitive;
        class PrimitivePointer;
        class PrimitiveCube;
        class PrimitiveCylinder;
        class PrimitiveSphere;
        class PrimitiveCapsule;

        class VolumeType : public Core::InstanceType
        {
            //
            // Members
            //

        private:
            // volume shapes
            Core::PrimitiveCube* m_Cube;
            Core::PrimitiveCylinder* m_Cylinder;
            Core::PrimitiveSphere* m_Sphere;
            Core::PrimitiveCapsule* m_Capsule;


            //
            // Runtime Type Info
            //

        public:
            REFLECT_DECLARE_ABSTRACT( Core::VolumeType, Core::InstanceType );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            VolumeType( Core::Scene* scene, i32 instanceType );

            virtual ~VolumeType();

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            const Core::Primitive* GetShape( Content::VolumeShape shape ) const;
        };
    }
}