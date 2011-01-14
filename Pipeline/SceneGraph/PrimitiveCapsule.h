#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveCapsule : public PrimitiveTemplate<Position>
        {
        private:
            typedef PrimitiveTemplate<Position> Base;

            mutable int32_t m_WireVertCount;
            mutable int32_t m_PolyVertCount;
            mutable int32_t m_CapVertCount;
            mutable int32_t m_ShaftVertCount;

        public:
            float32_t m_Radius;
            int32_t m_RadiusSteps;

            float32_t m_Length;
            int32_t m_LengthSteps;

        public:
            SceneGraph::PrimitiveCapsule(ResourceTracker* tracker);

            int GetWireVertCount() const;
            int GetPolyVertCount() const;

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}