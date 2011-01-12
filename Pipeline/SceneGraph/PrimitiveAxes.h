#pragma once

#include "Primitive.h"

namespace Helium
{
    class Matrix4;

    namespace SceneGraph
    {
        class PrimitiveAxes : public PrimitiveTemplate<PositionColored>
        {
        public:
            typedef PrimitiveTemplate<PositionColored> Base;

        private:
            uint32_t m_ColorX;
            uint32_t m_ColorY;
            uint32_t m_ColorZ;

        public:
            float m_Length;

        public:
            SceneGraph::PrimitiveAxes(ResourceTracker* tracker);

            void SetColor(AxesFlags axis, uint32_t c);
            void SetColor(uint32_t c);
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