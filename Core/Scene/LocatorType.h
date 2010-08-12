#pragma once

#include "InstanceType.h"

#include "Core/Content/Nodes/Locator.h"

namespace Helium
{
    namespace Core
    {
        class Locator;
        class Primitive;
        class PrimitiveLocator;
        class PrimitiveCube;

        class LocatorType : public Core::InstanceType
        {
            //
            // Members
            //

        private:
            // locator shapes
            Core::PrimitiveLocator* m_Locator;
            Core::PrimitiveCube* m_Cube;


            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::LocatorType, Core::InstanceType );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            LocatorType( Core::Scene* scene, i32 instanceType );

            virtual ~LocatorType();

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            const Core::Primitive* GetShape( Content::LocatorShape shape ) const;
        };
    }
}