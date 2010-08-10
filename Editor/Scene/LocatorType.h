#pragma once

#include "InstanceType.h"

#include "Core/Content/Nodes/Locator.h"

namespace Helium
{
    namespace Editor
    {
        class Locator;
        class Primitive;
        class PrimitiveLocator;
        class PrimitiveCube;

        class LocatorType : public Editor::InstanceType
        {
            //
            // Members
            //

        private:
            // locator shapes
            Editor::PrimitiveLocator* m_Locator;
            Editor::PrimitiveCube* m_Cube;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::LocatorType, Editor::InstanceType );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            LocatorType( Editor::Scene* scene, i32 instanceType );

            virtual ~LocatorType();

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            const Editor::Primitive* GetShape( Content::LocatorShape shape ) const;
        };
    }
}