#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveGrid : public PrimitiveTemplate< Lunar::SimpleVertex >
        {
        public:
            typedef PrimitiveTemplate< Lunar::SimpleVertex > Base;

            int32_t m_Width;
            int32_t m_Length;
            Lunar::Color m_AxisColor;
            Lunar::Color m_MajorColor;
            Lunar::Color m_MinorColor;
            float32_t m_MajorStep;
            float32_t m_MinorStep;

            void SetAxisColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
            void SetMajorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
            void SetMinorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

        public:
            PrimitiveGrid();

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}