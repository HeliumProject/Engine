#pragma once

#include "Primitive.h"

#include "GraphicsTypes/VertexTypes.h"

namespace Helium
{
    class Matrix4;

    namespace SceneGraph
    {
        class PrimitiveAxes : public PrimitiveTemplate< Lunar::SimpleVertex >
        {
        public:
            typedef PrimitiveTemplate< Lunar::SimpleVertex > Base;

        private:
            Lunar::Color m_ColorX;
            Lunar::Color m_ColorY;
            Lunar::Color m_ColorZ;

        public:
            float32_t m_Length;

        public:
            PrimitiveAxes();

            void SetColor( AxesFlags axis, Lunar::Color c );
            void SetColor( Lunar::Color c );
            void SetRGB();

            virtual void Update() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const HELIUM_OVERRIDE;
            virtual void DrawAxes( DrawArgs* args, AxesFlags axes ) const;
            virtual void DrawViewport( DrawArgs* args, const SceneGraph::Camera* camera ) const;
            virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
            AxesFlags PickAxis( const Matrix4& transform, Line pick, float32_t err );
        };
    }
}