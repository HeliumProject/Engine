#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace Editor
    {
        class PrimitivePointer : public Editor::PrimitiveTemplate<Position>
        {
        public:
            PrimitivePointer(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}