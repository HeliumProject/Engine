#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveFrame : public PrimitiveTemplate< Lunar::ScreenVertex >
        {
        public:
            typedef PrimitiveTemplate< Lunar::ScreenVertex > Base;

            Point m_Start;
            Point m_End;

            Lunar::Color m_InnerColor;
            Lunar::Color m_BorderColor;

        public:
            PrimitiveFrame();

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw(
                Lunar::BufferedDrawer* drawInterface, DrawArgs* args, Lunar::Color materialColor = Color::WHITE,
                const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY, const bool* solid = NULL,
                const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
        };
    }
}