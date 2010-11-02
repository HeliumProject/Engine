#pragma once

#include "Pipeline/SceneGraph/InstanceType.h"
#include "Pipeline/SceneGraph/Volume.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Volume;
        class Primitive;
        class PrimitivePointer;
        class PrimitiveCube;
        class PrimitiveCylinder;
        class PrimitiveSphere;
        class PrimitiveCapsule;

        class VolumeType : public InstanceType
        {
        private:
            // volume shapes
            PrimitiveCube* m_Cube;
            PrimitiveCylinder* m_Cylinder;
            PrimitiveSphere* m_Sphere;
            PrimitiveCapsule* m_Capsule;

        public:
            REFLECT_DECLARE_ABSTRACT( VolumeType, InstanceType );
            static void InitializeType();
            static void CleanupType();

        public:
            VolumeType( Scene* scene, int32_t instanceType );
            virtual ~VolumeType();

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            const Primitive* GetShape( VolumeShape shape ) const;
        };
    }
}