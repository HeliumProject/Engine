#pragma once

#include "Primitive.h"

namespace Helium
{
    namespace Math
    {
        class Matrix4;
    }

    namespace Editor
    {
        class PrimitiveAxes : public Editor::PrimitiveTemplate<PositionColored>
        {
        private:
            u32 m_ColorX;
            u32 m_ColorY;
            u32 m_ColorZ;

        public:
            float m_Length;

        public:
            Editor::PrimitiveAxes(ResourceTracker* tracker);

            void SetColor(Math::AxesFlags axis, u32 c);
            void SetColor(u32 c);
            void SetRGB();

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual void DrawAxes(DrawArgs* args, Math::AxesFlags axes) const;
            virtual void DrawViewport(DrawArgs* args, const Editor::Camera* camera) const;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
            Math::AxesFlags PickAxis(const Math::Matrix4& transform, Math::Line pick, float err);
        };
    }
}