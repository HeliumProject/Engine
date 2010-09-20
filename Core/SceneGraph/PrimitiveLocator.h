#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace Core
    {
        class PrimitiveLocator : public Core::PrimitiveTemplate<Position>
        {
        public:
            float m_Length;

        public:
            PrimitiveLocator(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}