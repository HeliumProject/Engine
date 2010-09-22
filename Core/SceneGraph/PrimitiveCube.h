#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveCube : public SceneGraph::PrimitiveTemplate<Position>
        {
        public:
            PrimitiveCube(ResourceTracker* tracker);

            void SetRadius( float radius )
            {
                m_Bounds.minimum = Math::Vector3 (-radius, -radius, -radius);
                m_Bounds.maximum = Math::Vector3 (radius, radius, radius);
            }

            void ScaleRadius( float scale )
            {
                m_Bounds.minimum *= scale;
                m_Bounds.maximum *= scale;
            }

            void SetBounds( const Math::AlignedBox& box )
            {
                m_Bounds = box;
            }

            void SetBounds( const Math::Vector3& min, const Math::Vector3& max )
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