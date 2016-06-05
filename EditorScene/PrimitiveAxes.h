#pragma once

#include "Primitive.h"

#include "GraphicsTypes/VertexTypes.h"

namespace Helium
{
	struct Matrix4;

	namespace Editor
	{
		class PrimitiveAxes : public PrimitiveTemplate< Helium::SimpleVertex >
		{
		public:
			typedef PrimitiveTemplate< Helium::SimpleVertex > Base;

		private:
			Helium::Color m_ColorX;
			Helium::Color m_ColorY;
			Helium::Color m_ColorZ;

		public:
			float32_t m_Length;

		public:
			PrimitiveAxes();

			void SetColor( AxesFlags axis, Helium::Color c );
			void SetColor( Helium::Color c );
			void SetRGB();

			virtual void Update() override;
			virtual void Draw(
				BufferedDrawer* drawInterface,
				Helium::Color materialColor = Colors::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY,
				const bool* solid = NULL,
				const bool* transparent = NULL ) const override;
			virtual void DrawAxes(
				BufferedDrawer* drawInterface,
				AxesFlags axes,
				Helium::Color materialColor = Colors::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY ) const;
			virtual void DrawViewport(
				BufferedDrawer* drawInterface,
				const Editor::Camera* camera,
				Helium::Color materialColor = Colors::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY ) const;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const override;
			AxesFlags PickAxis( const Matrix4& transform, Line pick, float32_t err );
		};
	}
}