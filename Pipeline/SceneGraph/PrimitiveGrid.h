#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveGrid : public PrimitiveTemplate<PositionColored>
        {
        public:
            typedef PrimitiveTemplate<PositionColored> Base;

            int m_Width;
            int m_Length;
            int m_AxisColor;
            int m_MajorColor;
            int m_MinorColor;
            float m_MajorStep;
            float m_MinorStep;

            void SetAxisColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
            void SetMajorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
            void SetMinorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

        public:
            PrimitiveGrid(ResourceTracker* tracker);

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}