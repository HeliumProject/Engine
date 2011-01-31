#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveLocator : public PrimitiveTemplate< Lunar::SimpleVertex >
        {
        public:
            typedef PrimitiveTemplate< Lunar::SimpleVertex > Base;

            float m_Length;

        public:
            PrimitiveLocator();

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}