#pragma once

#include "Primitive.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveFrame : public PrimitiveTemplate< Helium::ScreenVertex >
		{
		public:
			typedef PrimitiveTemplate< Helium::ScreenVertex > Base;

			Point m_Start;
			Point m_End;

			Helium::Color m_InnerColor;
			Helium::Color m_BorderColor;

		public:
			PrimitiveFrame();

			virtual void Update() override;
			virtual void Draw(
				BufferedDrawer* drawInterface,
				Helium::Color materialColor = Colors::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY,
				const bool* solid = NULL,
				const bool* transparent = NULL ) const override;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const override;
		};
	}
}