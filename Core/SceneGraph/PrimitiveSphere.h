#pragma once

#include "PrimitiveRadius.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveSphere : public SceneGraph::PrimitiveRadius
        {
        private:
            mutable i32 m_WireVertCount;
            mutable i32 m_PolyVertCount;

        public:
            int m_LengthSteps;

        public:
            PrimitiveSphere(ResourceTracker* tracker);

            int GetWireVertCount() const;
            int GetPolyVertCount() const;

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}