#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveGrid : public PrimitiveTemplate< Helium::SimpleVertex >
        {
        public:
            typedef PrimitiveTemplate< Helium::SimpleVertex > Base;

            int32_t m_Width;
            int32_t m_Length;
            Helium::Color m_AxisColor;
            Helium::Color m_MajorColor;
            Helium::Color m_MinorColor;
            float32_t m_MajorStep;
            float32_t m_MinorStep;

            void SetAxisColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
            void SetMajorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
            void SetMinorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a );

        public:
            PrimitiveGrid();

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw(
                Helium::BufferedDrawer* drawInterface, DrawArgs* args, Helium::Color materialColor = Color::WHITE,
                const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY, const bool* solid = NULL,
                const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}