#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveFrame : public SceneGraph::PrimitiveTemplate<TransformedColored>
        {
        public:
            Point m_Start;
            Point m_End;

            uint32_t m_InnerColor;
            uint32_t m_BorderColor;

        public:
            PrimitiveFrame(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}