#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveRadius : public SceneGraph::PrimitiveTemplate<Position>
        {
        public:
            float m_Radius;
            int m_RadiusSteps;

        public:
            PrimitiveRadius(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE = 0;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE = 0;
        };
    }
}