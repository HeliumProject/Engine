#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveCone : public SceneGraph::PrimitiveTemplate<Position>
        {
        private:
            mutable int32_t m_WireVertCount;
            mutable int32_t m_PolyVertCount;

        public:
            float32_t m_Length;
            float32_t m_Radius;
            int32_t m_Steps;

        public:
            PrimitiveCone(ResourceTracker* tracker);

            uint32_t GetWireVertCount() const;
            uint32_t GetPolyVertCount() const;

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}