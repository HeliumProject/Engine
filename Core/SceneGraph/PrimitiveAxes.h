#pragma once

#include "Primitive.h"

namespace Helium
{
    class Matrix4;

    namespace SceneGraph
    {
        class PrimitiveAxes : public SceneGraph::PrimitiveTemplate<PositionColored>
        {
        private:
            u32 m_ColorX;
            u32 m_ColorY;
            u32 m_ColorZ;

        public:
            float m_Length;

        public:
            SceneGraph::PrimitiveAxes(ResourceTracker* tracker);

            void SetColor(AxesFlags axis, u32 c);
            void SetColor(u32 c);
            void SetRGB();

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual void DrawAxes(DrawArgs* args, AxesFlags axes) const;
            virtual void DrawViewport(DrawArgs* args, const SceneGraph::Camera* camera) const;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
            AxesFlags PickAxis(const Matrix4& transform, Line pick, float err);
        };
    }
}