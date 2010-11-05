#pragma once

#include "Pipeline/SceneGraph/InstanceType.h"
#include "Pipeline/SceneGraph/Locator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Locator;
        class Primitive;
        class PrimitiveLocator;
        class PrimitiveCube;

        class LocatorType : public InstanceType
        {
        private:
            // locator shapes
            PrimitiveLocator* m_Locator;
            PrimitiveCube* m_Cube;

        public:
            REFLECT_DECLARE_ABSTRACT( LocatorType, InstanceType );
            static void InitializeType();
            static void CleanupType();

        public:
            LocatorType( Scene* scene, int32_t instanceType );

            virtual ~LocatorType();

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            const Primitive* GetShape( LocatorShape shape ) const;
        };
    }
}