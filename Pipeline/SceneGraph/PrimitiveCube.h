#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveCube : public PrimitiveTemplate< Lunar::SimpleVertex >
        {
        public:
            typedef PrimitiveTemplate< Lunar::SimpleVertex > Base;

            PrimitiveCube();

            void SetRadius( float radius )
            {
                m_Bounds.minimum = Vector3 (-radius, -radius, -radius);
                m_Bounds.maximum = Vector3 (radius, radius, radius);
            }

            void ScaleRadius( float scale )
            {
                m_Bounds.minimum *= scale;
                m_Bounds.maximum *= scale;
            }

            void SetBounds( const AlignedBox& box )
            {
                m_Bounds = box;
            }

            void SetBounds( const Vector3& min, const Vector3& max )
            {
                m_Bounds.minimum = min;
                m_Bounds.maximum = max;
            }

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}