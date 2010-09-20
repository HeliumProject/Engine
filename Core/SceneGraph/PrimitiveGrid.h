#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveGrid : public SceneGraph::PrimitiveTemplate<PositionColored>
        {
        public:
            int m_Width;
            int m_Length;
            int m_AxisColor;
            int m_MajorColor;
            int m_MinorColor;
            float m_MajorStep;
            float m_MinorStep;

            void SetAxisColor( u8 r, u8 g, u8 b, u8 a );
            void SetMajorColor( u8 r, u8 g, u8 b, u8 a );
            void SetMinorColor( u8 r, u8 g, u8 b, u8 a );

        public:
            PrimitiveGrid(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}