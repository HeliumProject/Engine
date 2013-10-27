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

			virtual void Update() HELIUM_OVERRIDE;
			virtual void Draw(
				Helium::BufferedDrawer* drawInterface, DrawArgs* args, Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY, const bool* solid = NULL,
				const bool* transparent = NULL ) const HELIUM_OVERRIDE;
			virtual void DrawAxes(
				Helium::BufferedDrawer* drawInterface, DrawArgs* args, AxesFlags axes,
				Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY ) const;
			virtual void DrawViewport(
				Helium::BufferedDrawer* drawInterface, DrawArgs* args, const Editor::Camera* camera,
				Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY ) const;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
			AxesFlags PickAxis( const Matrix4& transform, Line pick, float32_t err );
		};
	}
}