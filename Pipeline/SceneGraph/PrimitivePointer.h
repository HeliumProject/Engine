#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitivePointer : public PrimitiveTemplate<Position>
        {
        public:
            typedef PrimitiveTemplate<Position> Base;

            PrimitivePointer(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}