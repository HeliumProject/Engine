#pragma once

#include "PrimitiveRadius.h"

namespace Helium
{
    namespace Editor
    {
        class PrimitiveCylinder : public Editor::PrimitiveRadius
        {
        public:
            float m_Length;
            int m_LengthSteps;

            bool m_VerticalOrientation;

        public:
            PrimitiveCylinder(ResourceTracker* tracker);

            int GetWireVertCount() const;
            int GetPolyVertCount() const;

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}